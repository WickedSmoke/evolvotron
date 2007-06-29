// Source file for evolvotron
// Copyright (C) 2002,2003,2004,2007 Tim Day
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

#include "libevolvotron_precompiled.h"

#include "evolvotron_main.h"

#include <qapplication.h>
#include <qmessagebox.h>
#include <qtooltip.h>
#include <qregexp.h>

#include "args.h"
#include "dialog_about.h"
#include "dialog_help.h"
#include "dialog_mutation_parameters.h"
#include "dialog_functions.h"
#include "dialog_favourite.h"
#include "function_node.h"
#include "function_post_transform.h"
#include "function_pre_transform.h"
#include "function_top.h"

void EvolvotronMain::History::purge()
{
  if (_archive.size()>0) _archive.pop_back();
}

EvolvotronMain::History::History(EvolvotronMain* m)
:_main(m)
 ,max_slots(32)
{
  // Don't call _main->set_undoable because menus probably haven't been constructed yet.
}

EvolvotronMain::History::~History()
{}

void EvolvotronMain::History::log_status() const
{
  std::clog << "[History: " << _archive.size() << " records (";
  for (Archive::const_iterator it=_archive.begin();it!=_archive.end();it++)
    std::clog << (it!=_archive.begin() ? "," : "") << (*it).second.size();
  std::clog << ")]\n";
}

void EvolvotronMain::History::goodbye(MutatableImageDisplay* display)
{
  // First pass to delete any individual items for that display.
  for (Archive::iterator it=_archive.begin();it!=_archive.end();it++)
    (*it).second.erase(display);

  // Second pass to delete any undo items which are now empty
  Archive::iterator it=_archive.begin();
  while (it!=_archive.end())
    {
      if ((*it).second.empty()) it=_archive.erase(it);
      it++;
    }

  // Set menu label again in case we've changed the topmost item
  const std::string action_name(_archive.empty() ? "" : _archive.front().first);
  _main->set_undoable(undoable(),action_name);
}

void EvolvotronMain::History::replacing(MutatableImageDisplay* display)
{
  if (_archive.size()==0)
    {
      begin_action("");
    }
  
  const boost::shared_ptr<const MutatableImage> image=display->image();

  if (image.get())
    {
      const boost::shared_ptr<const MutatableImage> saved_image(image->deepclone(image->locked()));
      _archive.front().second.insert(std::make_pair(display,saved_image));
    }
}

/*! Only creates a new slot for display-image pairs if the current top one (if any) isn't empty.
 */
void EvolvotronMain::History::begin_action(const std::string& action_name)
{
  if (_archive.size()==0 || _archive.front().second.size()!=0)
    {
      _archive.push_front(ArchiveRecord());

      assert(_archive.front().second.size()==0);
    }
  
  _archive.front().first=action_name;

  while (_archive.size()>max_slots)
    {
      purge();
    }
}

void EvolvotronMain::History::end_action()
{
  const std::string action_name(_archive.empty() ? "" : _archive.front().first);
  _main->set_undoable(undoable(),action_name);

  log_status();
}

bool EvolvotronMain::History::undoable()
{
  if (_archive.size()==0)
    {
      return false;
    }
  else if (_archive.front().second.size()==0)
    {
      _archive.pop_front();
      return undoable();
    }
  else
    {
      return true;
    }
}

void EvolvotronMain::History::undo()
{
  if (_archive.size()==0)
    {
      // Shouldn't ever see this if Undo menu item is correctly greyed out.
      QMessageBox::warning(_main,"Evolvotron","Sorry, cannot undo any further");
    }
  else if (_archive.front().second.size()==0)
    {
      _archive.pop_front();
      undo();
    }
  else
    {
      for (ArchiveRecordEntries::iterator it=_archive.front().second.begin();
	   it!=_archive.front().second.end();
	   it++
	   )
	{
	  _main->restore((*it).first,(*it).second);
	}
      _archive.pop_front();
    }

  const std::string action_name(_archive.empty() ? "" : _archive.front().first);
  _main->set_undoable(undoable(),action_name);
}

void EvolvotronMain::last_spawned_image(const boost::shared_ptr<const MutatableImage>& image,SpawnMemberFn method)
{
  _last_spawned_image=image;
  _last_spawn_method=method;
}

/*! Constructor sets up GUI components and fires up QTimer.
  Initialises mutation parameters using time, so different every time.
 */
EvolvotronMain::EvolvotronMain(QWidget* parent,const QSize& grid_size,uint frames,uint framerate,uint n_threads,bool start_fullscreen,bool start_menuhidden)
  :QMainWindow(parent,0,Qt::WType_TopLevel|Qt::WDestructiveClose)
   ,_history(new EvolvotronMain::History(this))
   ,_mutation_parameters(time(0),this)
   ,_statusbar_tasks(0)
   ,_last_spawn_method(&EvolvotronMain::spawn_normal)
{
  setMinimumSize(600,400);

  // Need to create this first or DialogMutationParameters will cause one to be created too.
  _statusbar=new QStatusBar(this);
  _statusbar->setSizeGripEnabled(false);
  _statusbar->message("Ready");

  _statusbar_tasks_label=new QLabel("Ready",_statusbar);
  _statusbar->addWidget(_statusbar_tasks_label,0,true);

  _dialog_about=new DialogAbout(this,n_threads);
  _dialog_help_short=new DialogHelp(this,false);
  _dialog_help_long=new DialogHelp(this,true);

  _dialog_mutation_parameters=new DialogMutationParameters(this,&_mutation_parameters);

  _dialog_functions=new DialogFunctions(this,&_mutation_parameters);

  _dialog_favourite=new DialogFavourite(this);

  _menubar=new QMenuBar(this);

  _popupmenu_file=new QPopupMenu;
  _popupmenu_file->insertItem("Re&set (Reset mutation parameters, clear locks)",this,SLOT(reset_cold()));
  _popupmenu_file->insertItem("&Restart (Preserve mutation parameters and locks)",this,SLOT(reset_warm()));
  _popupmenu_file->insertItem("Remi&x (Randomize function weights and restart)",this,SLOT(reset_randomized()));
  _popupmenu_file->insertSeparator();
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

  _popupmenu_settings->insertItem("&Mutation parameters...",_dialog_mutation_parameters,SLOT(show()));
  _popupmenu_settings->insertItem("&Function weightings...",_dialog_functions,SLOT(show()));
  _popupmenu_settings->insertItem("Fa&vourite function...",_dialog_favourite,SLOT(show()));

  _popupmenu_settings->insertSeparator();

  _menu_item_number_fullscreen=_popupmenu_settings->insertItem("Full&screen",this,SLOT(toggle_fullscreen()));
  _menu_item_number_hide_menu=_popupmenu_settings->insertItem("Hide &menu and statusbar",this,SLOT(toggle_hide_menu()));  

  _popupmenu_settings->setItemChecked(_menu_item_number_fullscreen,start_fullscreen);
  _popupmenu_settings->setItemChecked(_menu_item_number_hide_menu,start_menuhidden);

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

  _farm=std::auto_ptr<MutatableImageComputerFarm>(new MutatableImageComputerFarm(n_threads));

  //! \todo frames and framerate should be retained and modifiable from the GUI
  for (int r=0;r<grid_size.height();r++)
    for (int c=0;c<grid_size.width();c++)
      {
	displays().push_back(new MutatableImageDisplay(_grid,this,true,false,QSize(0,0),frames,framerate));
      }

  // Run tick() at 100Hz
  _timer->start(10);

  if (start_fullscreen)
    {
      showFullScreen();
    }

  if (start_menuhidden)
    {
      menuBar()->hide();
      statusBar()->hide();
    }
}

/*! If this is being destroyed then the whole application is going down.
  Could be ordering issues with the display destructors though.
 */
EvolvotronMain::~EvolvotronMain()
{
  std::clog << "Evolvotron shut down begun...\n";

  // Orphan any displays which outlived us (and clear their images)  (look out: shutdown order is Qt-determined)
  for (std::set<MutatableImageDisplay*>::const_iterator it=_known_displays.begin();it!=_known_displays.end();it++)
    {
      (*it)->image(boost::shared_ptr<const MutatableImage>());
      (*it)->main(0);
    }

  std::clog << "...cleared displays, deleting farm...\n";

  // Shut down the compute farm
  _farm.reset();;

  std::clog << "...deleted farm, deleting history...\n";

  // Clean up records.
  _last_spawned_image.reset();
  _history.reset();

  std::clog << "...deleted history\n";

#ifndef NDEBUG
  assert(InstanceCounted::is_clear());
#endif

  std::clog << "...completed Evolvotron shutdown\n";  
}

const bool EvolvotronMain::favourite_function(const std::string& f)
{
  return _dialog_favourite->favourite_function(f);
}

void EvolvotronMain::favourite_function_unwrapped(bool v)
{
  _dialog_favourite->favourite_function_unwrapped(v);
}

void EvolvotronMain::spawn_normal(const boost::shared_ptr<const MutatableImage>& image,MutatableImageDisplay* display)
{
  boost::shared_ptr<const MutatableImage> new_image;

  do
    {
      new_image=image->mutated(mutation_parameters());
    }
  while (new_image->is_constant());

  history().replacing(display);
  display->image(new_image);
}

void EvolvotronMain::spawn_recoloured(const boost::shared_ptr<const MutatableImage>& image,MutatableImageDisplay* display)
{  
  std::auto_ptr<FunctionTop> new_root(image->top().typed_deepclone());
  
  new_root->reset_posttransform_parameters(mutation_parameters());
  history().replacing(display);
  boost::shared_ptr<const MutatableImage> it(new MutatableImage(new_root,image->sinusoidal_z(),image->spheremap(),false));
  display->image(it);
}

void EvolvotronMain::spawn_warped(const boost::shared_ptr<const MutatableImage>& image,MutatableImageDisplay* display)
{
  std::auto_ptr<FunctionTop> new_root=std::auto_ptr<FunctionTop>(image->top().typed_deepclone());

  // Get the transform from whatever factory is currently set
  const Transform transform(transform_factory()(mutation_parameters().rng01()));
      
  new_root->concatenate_pretransform_on_right(transform);  
  history().replacing(display);
  boost::shared_ptr<const MutatableImage> it(new MutatableImage(new_root,image->sinusoidal_z(),image->spheremap(),false));
  display->image(it);
}

void EvolvotronMain::restore(MutatableImageDisplay* display,const boost::shared_ptr<const MutatableImage>& image)
{
  if (is_known(display)) display->image(image);
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
  const boost::shared_ptr<const MutatableImage> spawning_image(spawning_display->image());

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
  _history->goodbye(disp);
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
  const uint tasks=farm().tasks();
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

  boost::shared_ptr<MutatableImageComputerTask> task;

  // If there are aborted jobs in the todo queue 
  // shift them straight over to done queue so the compute threads don't have to worry about them.
  farm().fasttrack_aborted();

  QTime watchdog;
  watchdog.start();

  while ((task=farm().pop_done())!=0)
    {
      if (is_known(task->display()))
	{
	  task->display()->deliver(task);
	}
      else
	{
	  // If we don't know who owns it we just have to trash it 
	  // (probably a top level window which was closed with incomplete tasks).
	  task.reset();
	}

      // Timeout in case we're being swamped by incoming tasks (maintain app responsiveness).
      if (watchdog.elapsed()>20)
	break;
    }
}    

void EvolvotronMain::keyPressEvent(QKeyEvent* e)
{
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
  else if (e->key()==Qt::Key_R && !(e->state()^Qt::ControlButton))
      {
	//Ctrl-R does a restart mainly because that's most useful in full-screen mode
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
}

void EvolvotronMain::toggle_hide_menu()
{
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
}

/*! Set up an initial random image in the specified display. 
  If a favourite function was specified then we use that as the top level node.
 */
void EvolvotronMain::reset(MutatableImageDisplay* display)
{
  std::auto_ptr<FunctionTop> root;
  if (_dialog_favourite->favourite_function().empty())
    {
      root=std::auto_ptr<FunctionTop>(FunctionTop::initial(mutation_parameters()));
    }
  else
    {
      root=std::auto_ptr<FunctionTop>
	(
	 FunctionTop::initial
	 (
	  mutation_parameters(),
	  mutation_parameters().function_registry().lookup(_dialog_favourite->favourite_function()),
	  _dialog_favourite->favourite_function_unwrapped()
	  )
	 );
    }

  history().replacing(display);
  //! \todo sinz and spheremap should be obtained from mutation parameters
  const boost::shared_ptr<const MutatableImage> image(new MutatableImage(root,!Args::global().option("-linz"),Args::global().option("-spheremap"),false));
  display->image(image);
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

  if (reset_mutation_parameters)
    {
      // Invoking reset on the 1st dialog actually resets the parameters 
      _dialog_mutation_parameters->reset();
      // This one just serves to setup the dialog from the now reset parameters
      _dialog_functions->setup_from_mutation_parameters();
    }

  for (std::vector<MutatableImageDisplay*>::iterator it=displays().begin();it!=displays().end();it++)
    {
      if (clear_locks)
	(*it)->lock(false,false);  // lock method mustn't make it's own history recording

      if (!(*it)->locked())
	reset(*it);
    }

  last_spawned_image(boost::shared_ptr<const MutatableImage>(),&EvolvotronMain::spawn_normal);

  history().end_action();
}

void EvolvotronMain::reset_randomized()
{
  _mutation_parameters.randomize_function_weightings_for_classifications(static_cast<uint>(-1));
  reset(false,false);
}

void EvolvotronMain::reset_warm()
{
  reset(false,false);
}

void EvolvotronMain::reset_cold()
{
  reset(true,true);
}

