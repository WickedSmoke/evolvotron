// Source file for evolvotron
// Copyright (C) 2002,2003,2004 Tim Day
/*
This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

/*! \file
  \brief Implementation of class EvolvotronMain.
  \todo Eliminate need to include function.h (and instantiate lots of stuff) by moving more into function_node.h/.cpp
*/

#include <time.h>
#include <qapplication.h>
#include <qcursor.h>
#include <qdatetime.h>
#include <qmessagebox.h>
#include <qtooltip.h>

#include "args.h"
#include "evolvotron_main.h"
#include "xyz.h"
#include "function_node.h"
#include "function_pre_transform.h"
#include "function_post_transform.h"

void EvolvotronMain::History::purge()
{
  if (_history.size()>0)
    {
      // Clean up any images at back end of queue
      for (
	   std::vector<std::pair<MutatableImageDisplay*,MutatableImage*> >::iterator it=_history.back().second.begin();
	   it!=_history.back().second.end();
	   it++
	   )
	{
	  delete (*it).second;
	}

      // Get rid of the entry
      _history.pop_back();
    }
}

EvolvotronMain::History::History(EvolvotronMain* m)
:_main(m)
 ,max_slots(32)
{
  // Don't call _main->set_undoable because menus probably haven't been constructed yet.
}

EvolvotronMain::History::~History()
{
  while (_history.size()>0)
    {
      purge();
    }  
}

void EvolvotronMain::History::goodbye(const MutatableImageDisplay* display)
{
  // First pass to delete any individual items 
  for (
       std::deque<std::pair<std::string,std::vector<std::pair<MutatableImageDisplay*,MutatableImage*> > > >::iterator it_out=_history.begin();
       it_out!=_history.end();
       it_out++
       )
    {
      std::vector<std::pair<MutatableImageDisplay*,MutatableImage*> >::iterator it_in=(*it_out).second.begin();
      while (it_in!=(*it_out).second.end())
	{
	  if ((*it_in).first==display)
	    {
	      delete (*it_in).second;
	      it_in=(*it_out).second.erase(it_in);
	    }
	  else
	    {
	      it_in++;
	    }
	}
    }

  // Second pass to delete any undo items which are now empty
  std::deque<std::pair<std::string,std::vector<std::pair<MutatableImageDisplay*,MutatableImage*> > > >::iterator it=_history.begin();
  while (it!=_history.end())
    {
      if ((*it).second.empty())
	{
	  it=_history.erase(it);
	}
      else
	{
	  it++;
	}
    }

  // Set menu label again in case we've changed the topmost item
  const std::string action_name(_history.empty() ? "" : _history.front().first);
  _main->set_undoable(undoable(),action_name);
}

void EvolvotronMain::History::replacing(MutatableImageDisplay* display)
{
  if (_history.size()==0)
    {
      begin_action("");
    }
  
  MutatableImage*const image=display->image();

  if (image!=0)
    {
      _history.front().second.push_back(std::pair<MutatableImageDisplay*,MutatableImage*>(display,image->deepclone()));
    }
}

/*! Only creates a new slot for display-image pairs if the current top one (if any) isn't empty.
 */
void EvolvotronMain::History::begin_action(const std::string& action_name)
{
  if (_history.size()==0 || _history.front().second.size()!=0)
    {
      _history.push_front(std::pair<std::string,std::vector<std::pair<MutatableImageDisplay*,MutatableImage*> > >());

      assert(_history.front().second.size()==0);
    }
  
  _history.front().first=action_name;

  while (_history.size()>max_slots)
    {
      purge();
    }
}

void EvolvotronMain::History::end_action()
{
  const std::string action_name(_history.empty() ? "" : _history.front().first);
  _main->set_undoable(undoable(),action_name);
}

bool EvolvotronMain::History::undoable()
{
  if (_history.size()==0)
    {
      return false;
    }
  else if (_history.front().second.size()==0)
    {
      _history.pop_front();
      return undoable();
    }
  else
    {
      return true;
    }
}

void EvolvotronMain::History::undo()
{
  if (_history.size()==0)
    {
      // Shouldn't ever see this if Undo menu item is correctly greyed out.
      QMessageBox::warning(_main,"Evolvotron","Sorry, cannot undo any further");
    }
  else if (_history.front().second.size()==0)
    {
      _history.pop_front();
      undo();
    }
  else
    {
      for (
	   std::vector<std::pair<MutatableImageDisplay*,MutatableImage*> >::iterator it=_history.front().second.begin();
	   it!=_history.front().second.end();
	   it++
	   )
	{
	  _main->restore((*it).first,(*it).second);
	}
      _history.pop_front();
    }

  const std::string action_name(_history.empty() ? "" : _history.front().first);
  _main->set_undoable(undoable(),action_name);
}

void EvolvotronMain::last_spawned_image(const MutatableImage* image,SpawnMemberFn method)
{
  delete _last_spawned_image;
  _last_spawned_image=(image==0 ? 0 : image->deepclone());
  _last_spawn_method=method;
}

/*! Constructor sets up GUI components and fires up QTimer.
  Initialises mutation parameters using time, so different every time.
 */
EvolvotronMain::EvolvotronMain(QWidget* parent,const QSize& grid_size,uint frames,uint framerate,uint n_threads,bool start_fullscreen,bool start_menuhidden)
  :QMainWindow(parent,0,Qt::WType_TopLevel|Qt::WDestructiveClose)
   ,_history(this)
   ,_mutation_parameters(time(0))
   ,_statusbar_tasks(0)
   ,_last_spawned_image(0)
   ,_last_spawn_method(&EvolvotronMain::spawn_normal)
   ,_transform_factory(0)
   ,_test_function_unwrapped(false)
{
  setMinimumSize(600,400);

  // Need to create this first or DialogMutationParameters will cause one to be created too.
  _statusbar=new QStatusBar(this);
  _statusbar->setSizeGripEnabled(false);
  _statusbar->message("Ready");

  _statusbar_tasks_label=new QLabel("Ready",_statusbar);
  _statusbar->addWidget(_statusbar_tasks_label,0,true);

  _dialog_about=new DialogAbout(this);
  _dialog_help_short=new DialogHelp(this,false);
  _dialog_help_long=new DialogHelp(this,true);

  _dialog_mutation_parameters=new DialogMutationParameters(this,&_mutation_parameters);

  _dialog_functions=new DialogFunctions(this,&_mutation_parameters);

  _menubar=new QMenuBar(this);

  _popupmenu_file=new QPopupMenu;
  _popupmenu_file->insertItem("&Restart",this,SLOT(reset_warm()));
  _popupmenu_file->insertItem("Re&set",this,SLOT(reset_cold()));
  _popupmenu_file->insertItem("&Quit",qApp,SLOT(quit()));
  _menubar->insertItem("&File",_popupmenu_file);

  _popupmenu_edit=new QPopupMenu;
  _popupmenu_edit_undo_id=_popupmenu_edit->insertItem("&Undo",this,SLOT(undo()));
  _popupmenu_edit->setItemEnabled(_popupmenu_edit_undo_id,false);
  _popupmenu_edit->insertSeparator();
  _popupmenu_edit->insertItem("&Simplify all functions",this,SLOT(simplify_constants()));
  _menubar->insertItem("&Edit",_popupmenu_edit);
  
  _popupmenu_settings=new QPopupMenu;

  // We want to use a checkmark on some items
  _popupmenu_settings->setCheckable(true);

  _popupmenu_settings->insertItem("&Mutations...",_dialog_mutation_parameters,SLOT(show()));
  _popupmenu_settings->insertItem("&Functions...",_dialog_functions,SLOT(show()));

#ifdef FULLSCREEN
  _popupmenu_settings->insertSeparator();

  _menu_item_number_fullscreen=_popupmenu_settings->insertItem("Full&screen",this,SLOT(toggle_fullscreen()));
  _menu_item_number_hide_menu=_popupmenu_settings->insertItem("Hide &menu and statusbar",this,SLOT(toggle_hide_menu()));  

  _popupmenu_settings->setItemChecked(_menu_item_number_fullscreen,start_fullscreen);
  _popupmenu_settings->setItemChecked(_menu_item_number_hide_menu,start_menuhidden);
#endif //FULLSCREEN

  _menubar->insertItem("Se&ttings",_popupmenu_settings);

  //! This doesn't seem to do anything (supposed to push help menu over to far end ?)
  _menubar->insertSeparator();

  _popupmenu_help=new QPopupMenu;
  _popupmenu_help->insertItem("Quick &Reference",_dialog_help_short,SLOT(show()));
  _popupmenu_help->insertItem("User &Manual",_dialog_help_long,SLOT(show()));
  _popupmenu_help->insertSeparator();
  _popupmenu_help->insertItem("&About",_dialog_about,SLOT(show()));

  _menubar->insertItem("&Help",_popupmenu_help);

  _grid=new QGrid(grid_size.width(),this);

  //! \todo These might work better as QToolButton
  _button_cool=new QPushButton("&Cool",_statusbar);
  _button_shield=new QPushButton("&Shield",_statusbar);
  _button_heat=new QPushButton("&Heat",_statusbar);
  _button_irradiate=new QPushButton("&Irradiate",_statusbar);

  QToolTip::add(_button_cool,"Decrease size of constant perturbations during mutation");
  QToolTip::add(_button_heat,"Increase size of constant perturbations during mutation");
  QToolTip::add(_button_shield,"Decrease probability of function tree structural mutations");
  QToolTip::add(_button_irradiate,"Increase probability of function tree structural mutations");
  
  connect(_button_cool,     SIGNAL(clicked()),_dialog_mutation_parameters,SLOT(cool()));
  connect(_button_heat,     SIGNAL(clicked()),_dialog_mutation_parameters,SLOT(heat()));
  connect(_button_shield,   SIGNAL(clicked()),_dialog_mutation_parameters,SLOT(shield()));
  connect(_button_irradiate,SIGNAL(clicked()),_dialog_mutation_parameters,SLOT(irradiate()));

  _statusbar->addWidget(_button_cool,0,true);
  _statusbar->addWidget(_button_shield,0,true);
  _statusbar->addWidget(_button_heat,0,true);
  _statusbar->addWidget(_button_irradiate,0,true);
  
  // We need to make sure the display grid gets all the space it can
  setCentralWidget(_grid);

  _timer=new QTimer(this);

  connect(
	  _timer,SIGNAL(timeout()),
	  this, SLOT(tick()) 
	  );

  _farm=new MutatableImageComputerFarm(n_threads);

  //! \todo frames and framerate should be retained and modifiable from the GUI
  for (int r=0;r<grid_size.height();r++)
    for (int c=0;c<grid_size.width();c++)
      {
	displays().push_back(new MutatableImageDisplay(_grid,this,true,false,QSize(0,0),frames,framerate));
      }

  // Run tick() at 100Hz
  _timer->start(10);

#ifdef FULLSCREEN
  if (start_fullscreen)
    {
      showFullScreen();
    }

  if (start_menuhidden)
    {
      menuBar()->hide();
      statusBar()->hide();
    }
#endif
}

/*! If this is being destroyed then the whole application is going down.
  Could be ordering issues with the display destructors though.
 */
EvolvotronMain::~EvolvotronMain()
{
  std::clog << "Evolvotron shut down begun...\n";

  // Orphan any displays which outlived us (look out: shutdown order is Qt-determined)
  for (std::set<MutatableImageDisplay*>::const_iterator it=_known_displays.begin();it!=_known_displays.end();it++)
    {
      (*it)->main(0);
    }

  // Shut down the compute farm
  delete _farm;

  std::clog << "...completed Evolvotron shutdown\n";  
}

const bool EvolvotronMain::test_function(const std::string& f)
{
  if (FunctionRegistry::get()->lookup(f))
    {
      _test_function=f;
      return true;
    }
  else
    return false;
}

void EvolvotronMain::spawn_normal(const MutatableImage* image,MutatableImageDisplay* display)
{
  MutatableImage* new_image;
  bool new_image_is_constant;

  do
    {
      new_image=image->mutated(mutation_parameters());

      new_image_is_constant=new_image->is_constant();

      if (new_image_is_constant)
	{
	  delete new_image;
	}
    }
  while (new_image_is_constant);

  history().replacing(display);
  display->image(new_image);
}

void EvolvotronMain::spawn_recoloured(const MutatableImage* image,MutatableImageDisplay* display)
{  
  history().replacing(display);

  std::vector<FunctionNode*> args;
  args.push_back(image->root()->deepclone());

  //! \todo Option to use a FunctionPostTransformQuadratic might be interesting. How about a FunctionPostTransform*Generalised ?
  display->image
    (
     new MutatableImage
     (
      new FunctionPostTransform
      (
       FunctionNode::stubparams(mutation_parameters(),12),
       args,
       0
       ),
      image->sinusoidal_z(),
      image->spheremap()
      )
     );
}

void EvolvotronMain::spawn_warped(const MutatableImage* image,MutatableImageDisplay* display)
{
  // Get the transform from whatever factory is currently set
  const Transform transform(transform_factory()(mutation_parameters().rng01()));

  FunctionNode* new_root;
  if (image->root()->is_a_FunctionPreTransform())
    {
      // If the image root is a transform wrapper then we can just concatentate transforms.
      new_root=image->root()->deepclone();

      FunctionPreTransform*const new_root_as_transform=new_root->is_a_FunctionPreTransform();
      assert(new_root_as_transform!=0);

      // Have to access params() through this to avoid protected scope of non-const version creating an error.
      const FunctionPreTransform*const const_new_root_as_transform=new_root_as_transform;

      Transform current_transform(const_new_root_as_transform->params());
      current_transform.concatenate_on_right(transform);
      new_root_as_transform->params(current_transform.get_columns());
    }
  else
    {
      // Otherwise have to create a new wrapper for the transform
      std::vector<FunctionNode*> args;
      args.push_back(image->root()->deepclone());
      new_root=new FunctionPreTransform(transform.get_columns(),args,0);
    }
  
  history().replacing(display);
  display->image(new MutatableImage(new_root,image->sinusoidal_z(),image->spheremap()));
}

void EvolvotronMain::restore(MutatableImageDisplay* display,MutatableImage* image)
{
  if (is_known(display))
    {
      display->image(image);
    }
  else
    {
      delete image;
    }
}

void EvolvotronMain::set_undoable(bool v,const std::string& action_name)
{
  _popupmenu_edit->changeItem(_popupmenu_edit_undo_id,QString(("&Undo "+action_name).c_str()));
  _popupmenu_edit->setItemEnabled(_popupmenu_edit_undo_id,v);
}

void EvolvotronMain::respawn(MutatableImageDisplay* display)
{
  if (display->locked())
    {
      QMessageBox::warning(this,"Evolvotron","Cannot respawn a locked image.\nUnlock and try again.");
    }
  else
    {
      history().begin_action("respawn");
      
      if (last_spawned_image()==0)
	{
	  reset(display);
	}
      else
	{
	  (this->*last_spawn_method())(last_spawned_image(),display);
	}

      history().end_action();
    }
}

void EvolvotronMain::spawn_all(MutatableImageDisplay* spawning_display,SpawnMemberFn method,const std::string& action_name)
{
  // Spawn potentially a bit sluggish so set the hourglass cursor.
  QApplication::setOverrideCursor(Qt::WaitCursor);
    
  history().begin_action(action_name);

  // Issue new images (except to locked displays and to originator)
  // This will cause them to abort any running tasks
  const MutatableImage*const spawning_image=spawning_display->image();

  last_spawned_image(spawning_image,method);
  
  for (std::vector<MutatableImageDisplay*>::iterator it=displays().begin();it!=displays().end();it++)
    {
      if ((*it)!=spawning_display && !(*it)->locked())
	{
	  (this->*method)(spawning_image,(*it));
	}
    }

  history().end_action();

  QApplication::restoreOverrideCursor();
}


/*! If one of our sub displays has spawned, distribute a mutated copy of its image to the other non-locked images
  in the mutation grid.
 */
void EvolvotronMain::spawn_normal(MutatableImageDisplay* spawning_display)
{
  spawn_all(
	    spawning_display,
	    &EvolvotronMain::spawn_normal,
	    "spawn"
	    );
}

/*! This is the similar to spawn_normal, except images ARE NOT MUTATED after deepclone and have a final transform applied to change their colour.
 */
void EvolvotronMain::spawn_recoloured(MutatableImageDisplay* spawning_display)
{
  spawn_all(
	    spawning_display,
	    &EvolvotronMain::spawn_recoloured,
	    "spawn recoloured"
	    );
}

/*! This is the similar to spawn_normal, except images ARE NOT MUTATED after deepclone 
  and have an initial transform (obtained from the supplied TransformFactory) applied to spatially warp them.
 */
void EvolvotronMain::spawn_warped(MutatableImageDisplay* spawning_display,const TransformFactory& tfactory)
{
  transform_factory(tfactory);
  spawn_all(
	    spawning_display,
	    &EvolvotronMain::spawn_warped,
	    "spawn warped"
	    );
}

void EvolvotronMain::hello(MutatableImageDisplay* disp)
{
  _known_displays.insert(disp);
}

void EvolvotronMain::goodbye(MutatableImageDisplay* disp)
{
  _history.goodbye(disp);
  _known_displays.erase(disp);  
}

bool EvolvotronMain::is_known(MutatableImageDisplay* disp) const
{
  return (_known_displays.find(disp)!=_known_displays.end());
}

void EvolvotronMain::list_known(std::ostream& out) const
{
  for (std::set<MutatableImageDisplay*>::const_iterator it=_known_displays.begin();it!=_known_displays.end();it++)
    {
      out << (*it) << " ";
    }
  out << "\n";
}

/*! Periodically report number of remaining compute tasks and check farm's done queue for completed tasks.
 */
void EvolvotronMain::tick()
{
  const uint tasks=farm()->tasks();
  if (tasks!=_statusbar_tasks)
    {
      if (tasks==0)
	_statusbar_tasks_label->setText("Ready");
      else
	{
	  _statusbar_tasks_label->setText(QString("%1 tasks remaining").arg(tasks));
	}
      _statusbar_tasks=tasks;
    }

  MutatableImageComputerTask* task;

  // If there are aborted jobs in the todo queue 
  // shift them straight over to done queue so the compute threads don't have to worry about them.
  farm()->fasttrack_aborted();

  QTime watchdog;
  watchdog.start();

  while ((task=farm()->pop_done())!=0)
    {
      if (is_known(task->display()))
	{
	  task->display()->deliver(task);
	}
      else
	{
	  // If we don't know who owns it we just have to trash it 
	  // (probably a top level window which was closed with incomplete tasks).
	  delete task;
	}

      // Timeout in case we're being swamped by incoming tasks (maintain app responsiveness).
      if (watchdog.elapsed()>20)
	break;
    }
}    

void EvolvotronMain::keyPressEvent(QKeyEvent* e)
{
#ifdef FULLSCREEN
  if (e->key()==Qt::Key_Escape)
    {
      // Esc key used to back out of menu hide and full screen mode
      // Might rescue a few users who have got into those states accidentally
      showNormal();
      menuBar()->show();
      statusBar()->show();
      _popupmenu_settings->setItemChecked(_menu_item_number_fullscreen,false);
      _popupmenu_settings->setItemChecked(_menu_item_number_hide_menu,false);
    }
  else if (e->key()==Qt::Key_F && !(e->state()^Qt::ControlButton))
    {
      //Ctrl-F toggles fullscreen mode
      toggle_fullscreen();
    }
  else if (e->key()==Qt::Key_M && !(e->state()^Qt::ControlButton))
    {
      //Ctrl-M toggles menu and status-bar display
      toggle_hide_menu();
    }
  else
#endif // FULLSCREEN
    if (e->key()==Qt::Key_R && !(e->state()^Qt::ControlButton))
      {
	//Ctrl-R does a reset mainly because that's most useful in full-screen mode
	reset_warm();
      }
    else if (e->key()==Qt::Key_Z && !(e->state()^Qt::ControlButton))
      {
	//Ctrl-Z does an undo
	undo();
      }
    else
      {
	// Perhaps it's for someone else
	e->ignore();
      }
}


void EvolvotronMain::toggle_fullscreen()
{
#ifdef FULLSCREEN
  if (isFullScreen()) 
    {
      showNormal();
      _popupmenu_settings->setItemChecked(_menu_item_number_fullscreen,false);
    }
  else 
    {
      showFullScreen();
      _popupmenu_settings->setItemChecked(_menu_item_number_fullscreen,true);
    }
#endif
}

void EvolvotronMain::toggle_hide_menu()
{
#ifdef FULLSCREEN
  if (menuBar()->isHidden())
    {
      menuBar()->show();
      _popupmenu_settings->setItemChecked(_menu_item_number_hide_menu,false);
    }
  else if (menuBar()->isShown())
    {
      menuBar()->hide();
      _popupmenu_settings->setItemChecked(_menu_item_number_hide_menu,true);
    }
  
  if (statusBar()->isHidden())
    statusBar()->show();
  else if (statusBar()->isShown())
    statusBar()->hide();
#endif
}

/*! Set up an initial random image in the specified display. 

  The choice of initial structure to start from is quite crucial to giving a good user experience.
  We concatenate 3 functions.  The outer 2 are transforms.
  You can think of the first function as a co-ordinate transform,
  the second function as being the "actual" image (we use an "exciting" stub to avoid boring constants or identities),
  and the final function as being a colour-space transform.
  Basically the idea is to give lots of opportunities for stuff to happen.

  If a test function was specified then we use that as the top level node.
 */
void EvolvotronMain::reset(MutatableImageDisplay* display)
{
  FunctionNode* root;
  if (test_function().empty())
    {
      root=FunctionNode::initial(mutation_parameters());
    }
  else if (test_function_unwrapped())
    {
      FunctionNodeStubNewFnPtr rootfn=FunctionRegistry::get()->lookup(test_function())->stubnew_fn();
      root=(*rootfn)(mutation_parameters(),true);
    }
  else
    {
      root=FunctionNode::initial(mutation_parameters(),FunctionRegistry::get()->lookup(test_function()));
    }

  history().replacing(display);
  //! \todo sinz and spheremap should be obtained from mutation parameters
  display->image(new MutatableImage(root,!Args::global().option("-linz"),Args::global().option("-spheremap")));
}

void EvolvotronMain::undo()
{
  history().undo();
}

void EvolvotronMain::simplify_constants()
{
  history().begin_action("simplify all");
  uint nodes_eliminated=0;
  for (std::vector<MutatableImageDisplay*>::iterator it=_displays.begin();it!=_displays.end();it++)
    {
      nodes_eliminated+=(*it)->simplify_constants(false);
    }
  history().end_action();
  std::stringstream msg;
  msg << "Eliminated " << nodes_eliminated << " redundant function nodes\n";
  QMessageBox::information(this,"Evolvotron",msg.str().c_str(),QMessageBox::Ok);
}

/*! Reset each image in the grid, and the mutation parameters.
 */
void EvolvotronMain::reset(bool reset_mutation_parameters,bool clear_locks)
{
  history().begin_action("reset/restart");

  for (std::vector<MutatableImageDisplay*>::iterator it=displays().begin();it!=displays().end();it++)
    {
      if (clear_locks)
	(*it)->lock(false);

      if (!(*it)->locked())
	reset(*it);
    }

  if (reset_mutation_parameters)
    {
      // Invoking reset on the 1st dialog actually resets the parameters 
      _dialog_mutation_parameters->reset();
      // This one just serves to setup the dialog from the now reset parameters
      _dialog_functions->setup_from_mutation_parameters();
    }

  last_spawned_image(0,&EvolvotronMain::spawn_normal);

  history().end_action();
}

void EvolvotronMain::reset_warm()
{
  reset(false,false);
}

void EvolvotronMain::reset_cold()
{
  reset(true,true);
}

