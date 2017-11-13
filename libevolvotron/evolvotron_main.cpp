/**************************************************************************/
/*  Copyright 2012 Tim Day                                                */
/*                                                                        */
/*  This file is part of Evolvotron                                       */
/*                                                                        */
/*  Evolvotron is free software: you can redistribute it and/or modify    */
/*  it under the terms of the GNU General Public License as published by  */
/*  the Free Software Foundation, either version 3 of the License, or     */
/*  (at your option) any later version.                                   */
/*                                                                        */
/*  Evolvotron is distributed in the hope that it will be useful,         */
/*  but WITHOUT ANY WARRANTY; without even the implied warranty of        */
/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         */
/*  GNU General Public License for more details.                          */
/*                                                                        */
/*  You should have received a copy of the GNU General Public License     */
/*  along with Evolvotron.  If not, see <http://www.gnu.org/licenses/>.   */
/**************************************************************************/

/*! \file
  \brief Implementation of class EvolvotronMain.
  \todo Eliminate need to include function.h (and instantiate lots of stuff) by moving more into function_node.h/.cpp
*/

#include "evolvotron_main.h"

#include "dialog_about.h"
#include "dialog_help.h"
#include "dialog_mutation_parameters.h"
#include "dialog_render_parameters.h"
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
  
  const boost::shared_ptr<const MutatableImage> image_function=display->image_function();

  if (image_function.get())
    {
      const boost::shared_ptr<const MutatableImage> saved_image_function(image_function->deepclone(image_function->locked()));
      _archive.front().second.insert(std::make_pair(display,saved_image_function));
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
	  _main->restore((*it).first,(*it).second,_archive.size()>1);
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
EvolvotronMain::EvolvotronMain
(
 QWidget* parent,
 const QSize& grid_size,
 uint frames,
 uint framerate,
 uint n_threads,
 bool separate_farm_for_enlargements,
 int niceness_grid,
 int niceness_enlargements,
 bool start_fullscreen,
 bool start_menuhidden,
 bool autocool,
 bool jitter,
 uint multisample_level,
 bool function_debug_mode,
 bool linear_zsweep,
 bool spheremap,
 const std::vector<std::string>& startup_filenames,
 bool startup_shuffle
 )
  :QMainWindow(parent)
  ,_history(new EvolvotronMain::History(this))
  ,_linear_zsweep(linear_zsweep)
  ,_spheremap(spheremap)
  ,_startup_filenames(startup_filenames)
  ,_startup_shuffle(startup_shuffle)
  ,_mutation_parameters(time(0),autocool,function_debug_mode,this)
  ,_render_parameters(jitter,multisample_level,this)
  ,_statusbar_tasks_main(0)
  ,_statusbar_tasks_enlargement(0)
  ,_last_spawn_method(&EvolvotronMain::spawn_normal)
{
  setAttribute(Qt::WA_DeleteOnClose,true);
  setAttribute(Qt::WA_QuitOnClose,true);

  setMinimumSize(640,480);

  // Need to create this first or DialogMutationParameters might cause one to be created too.
  _statusbar=new QStatusBar;
  _statusbar->setSizeGripEnabled(true);
  setStatusBar(_statusbar);

  _statusbar->addWidget(_statusbar_tasks_label=new QLabel("Ready"));

  _dialog_about=new DialogAbout(this,n_threads,separate_farm_for_enlargements);
  _dialog_help_short=new DialogHelp(this,false);
  _dialog_help_long=new DialogHelp(this,true);

  _dialog_mutation_parameters=new DialogMutationParameters(this,&_mutation_parameters);

  _dialog_render_parameters=new DialogRenderParameters(this,&_render_parameters);

  _dialog_functions=new DialogFunctions(this,&_mutation_parameters);

  _dialog_favourite=new DialogFavourite(this);

  _popupmenu_file=menuBar()->addMenu("&File");
  _popupmenu_file->addAction("Reset (Reset mutation parameters, clear locks)",this,SLOT(reset_cold()),QKeySequence("r"));
  _popupmenu_file->addAction("Restart (Preserve mutation parameters and locks)",this,SLOT(reset_warm()),QKeySequence("t"));
  _popupmenu_file->addAction("Remix (Randomize function weights and restart)",this,SLOT(reset_randomized()),QKeySequence("x"));
  _popupmenu_file->addSeparator();
  _popupmenu_file->addAction("Quit",qApp,SLOT(quit()),QKeySequence("q"));

  _popupmenu_edit=menuBar()->addMenu("&Edit");
  _popupmenu_edit_undo_action=_popupmenu_edit->addAction("Undo",this,SLOT(undo()),QKeySequence("u"));
  _popupmenu_edit_undo_action->setEnabled(false);
  _popupmenu_edit->addSeparator();
  _popupmenu_edit->addAction("Simplify all functions",this,SLOT(simplify_constants()));
  
  _popupmenu_settings=menuBar()->addMenu("Se&ttings");
  _popupmenu_settings->addAction("Mutation parameters...",_dialog_mutation_parameters,SLOT(show()));
  _popupmenu_settings->addAction("Function weightings...",_dialog_functions,SLOT(show()));
  _popupmenu_settings->addAction("Favourite function...",_dialog_favourite,SLOT(show()));

  _popupmenu_settings->addSeparator();

  _popupmenu_settings->addAction("Render parameters...",_dialog_render_parameters,SLOT(show()));  

  _popupmenu_settings->addSeparator();

  _menu_action_fullscreen=_popupmenu_settings->addAction("Fullscreen",this,SLOT(toggle_fullscreen()),QKeySequence("f"));
  _menu_action_fullscreen->setCheckable(true);
  _menu_action_fullscreen->setChecked(start_fullscreen);
  _menu_action_hide_menu=_popupmenu_settings->addAction("Hide menu and statusbar",this,SLOT(toggle_hide_menu()),QKeySequence("m"));  
  _menu_action_hide_menu->setCheckable(true);
  _menu_action_hide_menu->setChecked(start_menuhidden);

  //! This doesn't seem to do anything (supposed to push help menu over to far end ?)
  menuBar()->addSeparator();

  _popupmenu_help=menuBar()->addMenu("&Help");
  _popupmenu_help->addAction("Quick Reference",_dialog_help_short,SLOT(show()));
  _popupmenu_help->addAction("User Manual",_dialog_help_long,SLOT(show()));
  _popupmenu_help->addSeparator();
  _popupmenu_help->addAction("About",_dialog_about,SLOT(show()));

  _checkbox_autocool_enable=new QCheckBox("Autocool");
  _checkbox_autocool_enable->setToolTip("Autocooling gradually reduces the chance and magnitude of mutations with time.");
  _label_autocool_enable=new QLabel("");  // Used to display generation count
  _button_autocool_reheat=new QPushButton("Reheat");
  _button_autocool_reheat->setToolTip("Reheat restarts the autocooling generation count, restoring the full strength of mutations.");
  
  connect(_checkbox_autocool_enable,SIGNAL(stateChanged(int)),_dialog_mutation_parameters,SLOT(changed_autocool_enable(int)));
  connect(_button_autocool_reheat,SIGNAL(clicked()),_dialog_mutation_parameters,SLOT(reheat()));

  _statusbar->addPermanentWidget(_checkbox_autocool_enable);
  _statusbar->addPermanentWidget(_label_autocool_enable);
  _statusbar->addPermanentWidget(_button_autocool_reheat);

  connect(
	  &_render_parameters,SIGNAL(changed()),
	  this,SLOT(render_parameters_changed())
	  );

  connect(
	  &_mutation_parameters,SIGNAL(changed()),
	  this,SLOT(mutation_parameters_changed())
	  );
  

  _farm[0]=std::unique_ptr<MutatableImageComputerFarm>(new MutatableImageComputerFarm(n_threads,niceness_grid));
  if (separate_farm_for_enlargements)
    {
      _farm[1]=std::unique_ptr<MutatableImageComputerFarm>(new MutatableImageComputerFarm(n_threads,niceness_enlargements));
    }

  _grid=new QWidget;
  QGridLayout*const grid_layout=new QGridLayout;
  _grid->setLayout(grid_layout);
  setCentralWidget(_grid);

  //! \todo frames and framerate should be retained and modifiable from the GUI
  for (int r=0;r<grid_size.height();r++)
    for (int c=0;c<grid_size.width();c++)
      {
	MutatableImageDisplay*const d=new MutatableImageDisplay(this,true,false,QSize(0,0),frames,framerate);
	grid_layout->addWidget(d,r,c);
	displays().push_back(d);
      }

  _timer=new QTimer(this);
  connect(
	  _timer,SIGNAL(timeout()),
	  this, SLOT(tick()) 
	  );
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
  std::clog << "(There are " << _known_displays.size() << " displays remaining)\n";
  for (std::set<MutatableImageDisplay*>::const_iterator it=_known_displays.begin();it!=_known_displays.end();it++)
    {
      (*it)->image_function(boost::shared_ptr<const MutatableImage>(),true);
      (*it)->main(0);
    }

  std::clog << "...cleared displays, deleting farm...\n";

  // Shut down the compute farms
  _farm[0].reset();
  _farm[1].reset();

  std::clog << "...deleted farm, deleting history...\n";

  // Clean up records.
  _last_spawned_image.reset();
  _history.reset();

  std::clog << "...deleted history\n";

  std::clog << "...completed Evolvotron shutdown\n";  
}

bool EvolvotronMain::favourite_function(const std::string& f)
{
  return _dialog_favourite->favourite_function(f);
}

void EvolvotronMain::favourite_function_unwrapped(bool v)
{
  _dialog_favourite->favourite_function_unwrapped(v);
}

void EvolvotronMain::spawn_normal(const boost::shared_ptr<const MutatableImage>& image_function,MutatableImageDisplay* display,bool one_of_many)
{
  boost::shared_ptr<const MutatableImage> new_image_function;

  do
    {
      new_image_function=image_function->mutated(mutation_parameters());
    }
  while (new_image_function->is_constant());
  
  history().replacing(display);
  display->image_function(new_image_function,one_of_many);
}

void EvolvotronMain::spawn_recoloured(const boost::shared_ptr<const MutatableImage>& image_function,MutatableImageDisplay* display,bool one_of_many)
{  
  std::unique_ptr<FunctionTop> new_root(image_function->top().typed_deepclone());
  
  new_root->reset_posttransform_parameters(mutation_parameters());
  history().replacing(display);
  boost::shared_ptr<const MutatableImage> it(new MutatableImage(new_root,image_function->sinusoidal_z(),image_function->spheremap(),false));
  display->image_function(it,one_of_many);
}

void EvolvotronMain::spawn_warped(const boost::shared_ptr<const MutatableImage>& image_function,MutatableImageDisplay* display,bool one_of_many)
{
  std::unique_ptr<FunctionTop> new_root=std::unique_ptr<FunctionTop>(image_function->top().typed_deepclone());

  // Get the transform from whatever factory is currently set
  const Transform transform(transform_factory()(mutation_parameters().rng01()));
      
  new_root->concatenate_pretransform_on_right(transform);
  history().replacing(display);
  boost::shared_ptr<const MutatableImage> it(new MutatableImage(new_root,image_function->sinusoidal_z(),image_function->spheremap(),false));
  display->image_function(it,one_of_many);
}

void EvolvotronMain::restore(MutatableImageDisplay* display,const boost::shared_ptr<const MutatableImage>& image_function,bool one_of_many)
{
  if (is_known(display)) display->image_function(image_function,one_of_many);
}

void EvolvotronMain::set_undoable(bool v,const std::string& action_name)
{
  _popupmenu_edit_undo_action->setText(QString(("Undo "+action_name).c_str()));
  _popupmenu_edit_undo_action->setEnabled(v);
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
	  (this->*last_spawn_method())(last_spawned_image(),display,false);
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
  const boost::shared_ptr<const MutatableImage> spawning_image_function(spawning_display->image_function());

  last_spawned_image(spawning_image_function,method);
  
  for (std::vector<MutatableImageDisplay*>::iterator it=displays().begin();it!=displays().end();it++)
    {
      if ((*it)!=spawning_display && !(*it)->locked())
	{
	  (this->*method)(spawning_image_function,(*it),true);
	}
    }

  history().end_action();

  _mutation_parameters.autocool_generations_increment();
    
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
  const uint tasks_main=_farm[0]->tasks();
  const uint tasks_enlargement=(_farm[1].get() ? _farm[1]->tasks() : 0);
  if (tasks_main!=_statusbar_tasks_main || tasks_enlargement!=_statusbar_tasks_enlargement)
    {
      std::ostringstream msg;
      msg << "";

      if (tasks_main+tasks_enlargement==0)
	{
	  msg << "Ready";
	}
      else 
	{
	  msg << tasks_main;
	  if (tasks_enlargement)
	    {
	      msg << "+" << tasks_enlargement;
	    }
	  msg << " tasks remaining";
	}

      _statusbar_tasks_label->setText(msg.str().c_str());
      _statusbar_tasks_main=tasks_main;
      _statusbar_tasks_enlargement=tasks_enlargement;
    }

  boost::shared_ptr<MutatableImageComputerTask> task;

  // If there are aborted jobs in the todo queue 
  // shift them straight over to done queue so the compute threads don't have to worry about them.
  _farm[0]->fasttrack_aborted();
  if (_farm[1].get()) _farm[1]->fasttrack_aborted();

  QTime watchdog;
  watchdog.start();

  for (int which_farm=0;which_farm<(_farm[1].get() ? 2 : 1);which_farm++)
    {
      while ((task=_farm[which_farm]->pop_done())!=0)
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
      _menu_action_fullscreen->setChecked(false);
      _menu_action_hide_menu->setChecked(false);
    }
  else if (e->key()==Qt::Key_Z && !(e->modifiers()^Qt::ControlModifier))
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
      _menu_action_fullscreen->setChecked(false);
    }
  else 
    {
      showFullScreen();
      _menu_action_fullscreen->setChecked(true);
    }
}

void EvolvotronMain::toggle_hide_menu()
{
  if (menuBar()->isHidden())
    {
      menuBar()->show();
      _menu_action_hide_menu->setChecked(false);
    }
  else if (menuBar()->isVisible())
    {
      menuBar()->hide();
      _menu_action_hide_menu->setChecked(true);
    }
  
  if (statusBar()->isHidden())
    statusBar()->show();
  else if (statusBar()->isVisible())
    statusBar()->hide();
}

/*! Set up an initial random image in the specified display. 
  If a favourite function was specified then we use that as the top level node.
 */
void EvolvotronMain::reset(MutatableImageDisplay* display)
{
  std::unique_ptr<FunctionTop> root;
  if (_dialog_favourite->favourite_function().empty())
    {
      root=std::unique_ptr<FunctionTop>(FunctionTop::initial(mutation_parameters()));
    }
  else
    {
      root=std::unique_ptr<FunctionTop>
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
  const boost::shared_ptr<const MutatableImage> image_function(new MutatableImage(root,!_linear_zsweep,_spheremap,false));
  display->image_function(image_function,true);
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
      // This one just serves to setup the function dialog from the now reset parameters
      _dialog_functions->setup_from_mutation_parameters();
    }
  else
    {
      // Seems odd if we don't restart the count.
      _mutation_parameters.autocool_generations(0);
    }

  for (size_t i=0;i<displays().size();++i)
  {
    if (clear_locks)
      displays()[i]->lock(false,false);  // lock method mustn't make it's own history recording
  }

  if (_startup_shuffle) {
    std::random_shuffle(_startup_filenames.begin(),_startup_filenames.end());
  }

  for (size_t i=0;i<displays().size();++i) {
    if (!displays()[i]->locked()) {
      if (i<_startup_filenames.size())
	displays()[i]->load_function_file(_startup_filenames[i].c_str());
      else
	reset(displays()[i]);
    }
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

void EvolvotronMain::mutation_parameters_changed()
{
  _checkbox_autocool_enable->setChecked(_mutation_parameters.autocool_enable());
  if (_mutation_parameters.autocool_enable())
    {
      _label_autocool_enable->setText(QString("Generations:")+QString::number(_mutation_parameters.autocool_generations()));
      _label_autocool_enable->show();

      if (_mutation_parameters.autocool_generations()>0) _button_autocool_reheat->show();
      else _button_autocool_reheat->hide();

    }
  else
    {
      _label_autocool_enable->hide();
      _button_autocool_reheat->hide();
    }
}

void EvolvotronMain::render_parameters_changed()
{
  for (std::set<MutatableImageDisplay*>::iterator it=_known_displays.begin();it!=_known_displays.end();it++)
    (*it)->image_function((*it)->image_function(),true);
}
