// Source file for evolvotron
// Copyright (C) 2002 Tim Day
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
*/

#include <time.h>
#include <qapplication.h>
#include <qcursor.h>
#include <qdatetime.h>
#include <qmessagebox.h>

#include "evolvotron_main.h"
#include "xyz.h"

void EvolvotronMain::History::purge()
{
  if (_history.size()>0)
    {
      // Clean up any images.
      for (std::vector<std::pair<MutatableImageDisplay*,MutatableImageNode*> >::iterator it=_history.back().begin();it!=_history.back().end();it++)
	{
	  delete (*it).second;
	}
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

void EvolvotronMain::History::replacing(MutatableImageDisplay* display)
{
  if (_history.size()==0)
    {
      begin_action();
    }
  
  MutatableImageNode*const image=display->image();

  if (image!=0)
    {
      _history.front().push_back(std::pair<MutatableImageDisplay*,MutatableImageNode*>(display,image->deepclone()));
    }
}

/*! Only creates a new slot for display-image pairs if the current top one (if any) isn't empty.
 */
void EvolvotronMain::History::begin_action()
{
  if (_history.size()==0 || _history.front().size()!=0)
    {
      _history.push_front(std::vector<std::pair<MutatableImageDisplay*,MutatableImageNode*> >());
    }

  while (_history.size()>max_slots)
    {
      purge();
    }
}

void EvolvotronMain::History::end_action()
{
  _main->set_undoable(undoable());
}

bool EvolvotronMain::History::undoable()
{
  if (_history.size()==0)
    {
      return false;
    }
  else if (_history.front().size()==0)
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
      QMessageBox::warning(_main,"Evolvotron","Cannot undo further");
    }
  else if (_history.front().size()==0)
    {
      _history.pop_front();
      undo();
    }
  else
    {
      for (std::vector<std::pair<MutatableImageDisplay*,MutatableImageNode*> >::iterator it=_history.front().begin();it!=_history.front().end();it++)
	{
	  _main->restore((*it).first,(*it).second);
	}
      _history.pop_front();
      begin_action();
    }

  _main->set_undoable(undoable());
}

void EvolvotronMain::last_spawned(const MutatableImageNode* image,SpawnMemberFn method)
{
  delete _last_spawned;
  _last_spawned=(image==0 ? 0 : image->deepclone());
  _last_spawn_method=method;
}

    
/*! Constructor sets up GUI components and fires up QTimer.
 */
EvolvotronMain::EvolvotronMain(QWidget* parent,const QSize& grid_size,uint n_threads)
  :QMainWindow(parent,0,Qt::WType_TopLevel|Qt::WDestructiveClose)
   ,_history(this)
   ,_statusbar_tasks(0)
   ,_last_spawned(0)
   ,_last_spawn_method(&EvolvotronMain::spawn_normal)
{
  setMinimumSize(600,400);

  // Need to create this first or DialogMutationParameters will cause one to be created too.
  _statusbar=new QStatusBar(this);
  _statusbar->setSizeGripEnabled(false);
  _statusbar->message("Ready");

  _statusbar_tasks_label=new QLabel("Ready",_statusbar);
  _statusbar->addWidget(_statusbar_tasks_label,0,true);

  _dialog_about=new DialogAbout(this);

  _dialog_mutation_parameters=new DialogMutationParameters(this);

  _menubar=new QMenuBar(this);

  _popupmenu_file=new QPopupMenu;
  _popupmenu_file->insertItem("&Restart",this,SLOT(reset()));
  _popupmenu_file->insertItem("&Quit",qApp,SLOT(quit()));
  _menubar->insertItem("&File",_popupmenu_file);

  _popupmenu_edit=new QPopupMenu;
  _popupmenu_edit_undo_id=_popupmenu_edit->insertItem("&Undo",this,SLOT(undo()));
  _popupmenu_edit->setItemEnabled(_popupmenu_edit_undo_id,false);
  _popupmenu_edit->insertSeparator();
  _popupmenu_edit->insertItem("&Mutation parameters...",_dialog_mutation_parameters,SLOT(show()));
  _menubar->insertItem("&Edit",_popupmenu_edit);

  _menubar->insertSeparator();

  _popupmenu_help=new QPopupMenu;
  _popupmenu_help->insertItem("&About",_dialog_about,SLOT(show()));

  _menubar->insertItem("&Help",_popupmenu_help);

  _grid=new QGrid(grid_size.width(),this);

  //! \todo These might work better as QToolButton
  _button_cool=new QPushButton("&Cool",_statusbar);
  _button_heat=new QPushButton("&Heat",_statusbar);
  _button_shield=new QPushButton("&Shield",_statusbar);
  _button_irradiate=new QPushButton("&Irradiate",_statusbar);

  connect(_button_cool,     SIGNAL(clicked()),_dialog_mutation_parameters,SLOT(cool()));
  connect(_button_heat,     SIGNAL(clicked()),_dialog_mutation_parameters,SLOT(heat()));
  connect(_button_shield,   SIGNAL(clicked()),_dialog_mutation_parameters,SLOT(shield()));
  connect(_button_irradiate,SIGNAL(clicked()),_dialog_mutation_parameters,SLOT(irradiate()));

  _statusbar->addWidget(_button_cool,0,true);
  _statusbar->addWidget(_button_heat,0,true);
  _statusbar->addWidget(_button_shield,0,true);
  _statusbar->addWidget(_button_irradiate,0,true);
  
  // We need to make sure the display grid gets all the space it can
  setCentralWidget(_grid);

  _timer=new QTimer(this);

  connect(
	  _timer,SIGNAL(timeout()),
	  this, SLOT(tick()) 
	  );

  _farm=new MutatableImageComputerFarm(n_threads);

  for (int r=0;r<grid_size.height();r++)
    for (int c=0;c<grid_size.width();c++)
      {
	displays().push_back(new MutatableImageDisplay(_grid,this,true,false,QSize(0,0)));
      }

  // Run tick() at 100Hz
  _timer->start(10);
}

void EvolvotronMain::spawn_normal(const MutatableImageNode* image,MutatableImageDisplay* display)
{
  MutatableImageNode*const new_image=image->deepclone();
  new_image->mutate(mutation_parameters());
  history().replacing(display);
  display->image(new_image);
}

void EvolvotronMain::spawn_recoloured(const MutatableImageNode* image,MutatableImageDisplay* display)
{
  std::vector<MutatableImageNode*> args;
  args.push_back(new MutatableImageNodeConstant(RandomXYZInSphere(mutation_parameters().rng01(),1.0)));
  args.push_back(new MutatableImageNodeConstant(RandomXYZInSphere(mutation_parameters().rng01(),1.0)));
  args.push_back(new MutatableImageNodeConstant(RandomXYZInSphere(mutation_parameters().rng01(),1.0)));
  args.push_back(new MutatableImageNodeConstant(RandomXYZInSphere(mutation_parameters().rng01(),1.0)));
  args.push_back(image->deepclone());
  
  history().replacing(display);
  display->image(new MutatableImageNodePostTransform(args));
}

void EvolvotronMain::spawn_warped(const MutatableImageNode* image,MutatableImageDisplay* display)
{
  std::vector<MutatableImageNode*> args;
  
  // NB We don't generate any z co-ordinates (except z identity) so the random transform remains in the image plane.
  // (completely random transforms mostly look too dissimilar; they are different slices of the 3D image volume)
  args.push_back(new MutatableImageNodeConstant(RandomXYZInXYDisc(mutation_parameters().rng01(),1.0)));
  args.push_back(new MutatableImageNodeConstant(RandomXYZInXYDisc(mutation_parameters().rng01(),2.0)));
  args.push_back(new MutatableImageNodeConstant(RandomXYZInXYDisc(mutation_parameters().rng01(),2.0)));
  args.push_back(new MutatableImageNodeConstant(XYZ(0.0,0.0,1.0f)));
  args.push_back(image->deepclone());
  
  history().replacing(display);
  display->image(new MutatableImageNodePreTransform(args));
}

void EvolvotronMain::restore(MutatableImageDisplay* display,MutatableImageNode* image)
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

void EvolvotronMain::set_undoable(bool v)
{
  _popupmenu_edit->setItemEnabled(_popupmenu_edit_undo_id,v);
}

void EvolvotronMain::respawn(MutatableImageDisplay* display)
{
  if (display->locked())
    {
      QMessageBox::warning(this,"Evolvotron","Cannot respawn a locked image.\nUnlock and try again.");
    }
  else if (last_spawned()==0)
    {
      reset(display);
    }
  else
    {
      (this->*last_spawn_method())(last_spawned(),display);
    }
}

void EvolvotronMain::spawn_all(MutatableImageDisplay* display,SpawnMemberFn method)
{
  // Spawn potentially a bit sluggish so set the hourglass cursor.
  QApplication::setOverrideCursor(Qt::WaitCursor);
  
  history().begin_action();

  // Issue new images (except to locked displays and to originator)
  // This will cause them to abort any running tasks
  const MutatableImageNode*const spawning_image=display->image();

  last_spawned(spawning_image,method);
  
  for (std::vector<MutatableImageDisplay*>::iterator it=displays().begin();it!=displays().end();it++)
    {
      if ((*it)!=display && !(*it)->locked())
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
	    &EvolvotronMain::spawn_normal
	    );
}

/*! This is the similar to spawn_normal, except images ARE NOT MUTATED after deepclone and have a final transform applied to change their colour.
 */
void EvolvotronMain::spawn_recoloured(MutatableImageDisplay* spawning_display)
{
  spawn_all(
	    spawning_display,
	    &EvolvotronMain::spawn_recoloured
	    );
}

/*! This is the similar to spawn_normal, except images ARE NOT MUTATED after deepclone and have an initial transform applied to spatially warp them.
 */
void EvolvotronMain::spawn_warped(MutatableImageDisplay* spawning_display)
{
  spawn_all(
	    spawning_display,
	    &EvolvotronMain::spawn_warped
	    );
}

void EvolvotronMain::hello(MutatableImageDisplay* disp)
{
  _known_displays.insert(disp);
}

void EvolvotronMain::goodbye(MutatableImageDisplay* disp)
{
  _known_displays.erase(disp);  
}

bool EvolvotronMain::is_known(MutatableImageDisplay* disp) const
{
  return (_known_displays.find(disp)!=_known_displays.end());
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
	  // If we don't know who owns it we just have to trash it (probably a top level window which was closed with incomplete tasks).
	  delete task;
	}

      // Timeout in case we're being swamped by incoming tasks (maintain app responsiveness).
      if (watchdog.elapsed()>20)
	break;
    }
}    

/*! Set up an initial random image in the specified display. 

  The choice of initial structure to start from is quite crucial to giving a good user experience.
  We concatenate 3 transforms, each seeded with random basis vectors and offsets.
  You can think of the first function as a co-ordinate transform,
  the second function as being the "actual" image,
  and the final function as being a colour-space transform.
  Basically the idea is to give lots of opportunities for stuff to happen.

  \todo Try something more interesting for the middle function.  (But can't just use a random stub because the chances are it will just be a constant.)
 */
void EvolvotronMain::reset(MutatableImageDisplay* display)
{
  std::vector<MutatableImageNode*> args_toplevel;
  
  std::vector<MutatableImageNode*> args0;
  args0.push_back(new MutatableImageNodeConstant(RandomXYZInSphere(mutation_parameters().rng01(),1.0)));
  args0.push_back(new MutatableImageNodeConstant(RandomXYZInSphere(mutation_parameters().rng01(),1.0)));
  args0.push_back(new MutatableImageNodeConstant(RandomXYZInSphere(mutation_parameters().rng01(),1.0)));
  args0.push_back(new MutatableImageNodeConstant(RandomXYZInSphere(mutation_parameters().rng01(),1.0)));
  args0.push_back(new MutatableImageNodePosition());
  
  args_toplevel.push_back(new MutatableImageNodePostTransform(args0));
  
  std::vector<MutatableImageNode*> args1;
  args1.push_back(new MutatableImageNodeConstant(RandomXYZInSphere(mutation_parameters().rng01(),1.0)));
  args1.push_back(new MutatableImageNodeConstant(RandomXYZInSphere(mutation_parameters().rng01(),1.0)));
  args1.push_back(new MutatableImageNodeConstant(RandomXYZInSphere(mutation_parameters().rng01(),1.0)));
  args1.push_back(new MutatableImageNodeConstant(RandomXYZInSphere(mutation_parameters().rng01(),1.0)));
  args1.push_back(new MutatableImageNodePosition());
  
  args_toplevel.push_back(new MutatableImageNodePostTransform(args1));
  
  std::vector<MutatableImageNode*> args2;
  args2.push_back(new MutatableImageNodeConstant(RandomXYZInSphere(mutation_parameters().rng01(),1.0)));
  args2.push_back(new MutatableImageNodeConstant(RandomXYZInSphere(mutation_parameters().rng01(),1.0)));
  args2.push_back(new MutatableImageNodeConstant(RandomXYZInSphere(mutation_parameters().rng01(),1.0)));
  args2.push_back(new MutatableImageNodeConstant(RandomXYZInSphere(mutation_parameters().rng01(),1.0)));
  args2.push_back(new MutatableImageNodePosition());
      
  args_toplevel.push_back(new MutatableImageNodePostTransform(args2));
      
  history().replacing(display);
  display->image(new MutatableImageNodeConcatenateTriple(args_toplevel));
}

void EvolvotronMain::undo()
{
  history().undo();
}

/*! Reset each image in the grid, and the mutation parameters.
 */
void EvolvotronMain::reset()
{
  history().begin_action();

  for (std::vector<MutatableImageDisplay*>::iterator it=displays().begin();it!=displays().end();it++)
    {
      reset(*it);
    }

  _dialog_mutation_parameters->reset();

  last_spawned(0,&EvolvotronMain::spawn_normal);

  history().end_action();
}
