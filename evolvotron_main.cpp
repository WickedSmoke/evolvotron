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

#include "evolvotron_main.h"
#include "xyz.h"

/*! Constructor sets up GUI components and fires up QTimer.
 */
EvolvotronMain::EvolvotronMain(QWidget* parent,const QSize& grid_size,uint n_threads)
  :QMainWindow(parent,0,Qt::WType_TopLevel|Qt::WDestructiveClose)
   ,_statusbar_tasks(0)
   ,_mutation_parameters(time(0))
{
  setMinimumSize(640,480);

  _dialog_about=new DialogAbout(this);

  _dialog_mutation_parameters=new DialogMutationParameters(this,&_mutation_parameters);

  _menubar=new QMenuBar(this);

  _popupmenu_file=new QPopupMenu;
  _popupmenu_file->insertItem("&Restart",this,SLOT(reset()));
  _popupmenu_file->insertItem("&Quit",qApp,SLOT(quit()));
  _menubar->insertItem("&File",_popupmenu_file);

  _popupmenu_mutate=new QPopupMenu;
  _popupmenu_mutate->insertItem("&Parameters...",_dialog_mutation_parameters,SLOT(show()));
  _menubar->insertItem("&Mutate",_popupmenu_mutate);

  _menubar->insertSeparator();

  _popupmenu_help=new QPopupMenu;
  _popupmenu_help->insertItem("&About",_dialog_about,SLOT(show()));

  _menubar->insertItem("&Help",_popupmenu_help);

  _grid=new QGrid(grid_size.width(),this);

  _statusbar=new QStatusBar(this);
  _statusbar->setSizeGripEnabled(false);
  _statusbar->message("Ready");

  //! \todo These might work better as QToolButton
  _button_cool=new QPushButton("&Cool",_statusbar);
  _button_heat=new QPushButton("&Heat",_statusbar);
  _button_shield=new QPushButton("&Shield",_statusbar);
  _button_irradiate=new QPushButton("&Irradiate",_statusbar);

  connect(_button_cool,     SIGNAL(clicked()),this,SLOT(cool()));
  connect(_button_heat,     SIGNAL(clicked()),this,SLOT(heat()));
  connect(_button_shield,   SIGNAL(clicked()),this,SLOT(shield()));
  connect(_button_irradiate,SIGNAL(clicked()),this,SLOT(irradiate()));

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
	display().push_back(new MutatableImageDisplay(_grid,this,true,false,QSize(0,0)));
      }

  // Run tick() at 100Hz
  _timer->start(10);
}

/*! If one of our sub displays has spawned, distribute a mutated copy of its image to the other non-locked images
  in the mutation grid.
 */
void EvolvotronMain::spawn(MutatableImageDisplay* spawning_display)
{
  // Spawn potentially a bit sluggish so set the hourglass cursor.
  QApplication::setOverrideCursor(Qt::WaitCursor);
  
  // Issue new images (except to locked displays and to originator)
  // This will cause them to abort any running tasks
  const MutatableImageNode*const spawning_image=spawning_display->image();
  
  for (std::vector<MutatableImageDisplay*>::iterator it=display().begin();it!=display().end();it++)
    {
      if ((*it)!=spawning_display && !(*it)->locked())
	{
	  MutatableImageNode*const new_image=spawning_image->deepclone();
	  new_image->mutate(mutation_parameters());
	  (*it)->image(new_image);
	}
    }

  QApplication::restoreOverrideCursor();
}

/*! This is the similar to spawn, except images ARE NOT MUTATED after deepclone and have a final transform applied to change their colour.
 */
void EvolvotronMain::spawn_recoloured(MutatableImageDisplay* spawning_display)
{
  // Spawn potentially a bit sluggish so set the hourglass cursor.
  QApplication::setOverrideCursor(Qt::WaitCursor);
  
  // Issue new images (except to locked displays and to originator)
  // This will cause them to abort any running tasks
  const MutatableImageNode*const spawning_image=spawning_display->image();
  
  for (std::vector<MutatableImageDisplay*>::iterator it=display().begin();it!=display().end();it++)
    {
      if ((*it)!=spawning_display && !(*it)->locked())
	{
	  std::vector<MutatableImageNode*> args;
	  args.push_back(new MutatableImageNodeConstant(RandomXYZInSphere(mutation_parameters().rng01(),1.0)));
	  args.push_back(new MutatableImageNodeConstant(RandomXYZInSphere(mutation_parameters().rng01(),1.0)));
	  args.push_back(new MutatableImageNodeConstant(RandomXYZInSphere(mutation_parameters().rng01(),1.0)));
	  args.push_back(new MutatableImageNodeConstant(RandomXYZInSphere(mutation_parameters().rng01(),1.0)));
	  args.push_back(spawning_image->deepclone());
	  
	  (*it)->image(new MutatableImageNodePostTransform(args));
	}
    }

  QApplication::restoreOverrideCursor();
}

/*! This is the similar to spawn, except images ARE NOT MUTATED after deepclone and have an initial transform applied to spatially warp them.
 */
void EvolvotronMain::spawn_warped(MutatableImageDisplay* spawning_display)
{
  // Spawn potentially a bit sluggish so set the hourglass cursor.
  QApplication::setOverrideCursor(Qt::WaitCursor);
  
  // Issue new images (except to locked displays and to originator)
  // This will cause them to abort any running tasks
  const MutatableImageNode*const spawning_image=spawning_display->image();
  
  for (std::vector<MutatableImageDisplay*>::iterator it=display().begin();it!=display().end();it++)
    {
      if ((*it)!=spawning_display && !(*it)->locked())
	{
	  std::vector<MutatableImageNode*> args;

	  // NB We don't generate any z co-ordinates so the random transform remains in the image plane.
	  // (completely random transforms mostly look too dissimilar; they are different slices of the 3D image volume)
	  args.push_back(new MutatableImageNodeConstant(RandomXYZInXYDisc(mutation_parameters().rng01(),1.0)));
	  args.push_back(new MutatableImageNodeConstant(RandomXYZInXYDisc(mutation_parameters().rng01(),2.0)));
	  args.push_back(new MutatableImageNodeConstant(RandomXYZInXYDisc(mutation_parameters().rng01(),2.0)));
	  args.push_back(new MutatableImageNodeConstant(RandomXYZInXYDisc(mutation_parameters().rng01(),2.0)));
	  args.push_back(spawning_image->deepclone());
	  
	  (*it)->image(new MutatableImageNodePreTransform(args));
	}
    }

  QApplication::restoreOverrideCursor();
}


void EvolvotronMain::hello(MutatableImageDisplay* disp)
{
  _known_displays.insert(disp);
}

void EvolvotronMain::goodbye(MutatableImageDisplay* disp)
{
  _known_displays.erase(disp);  
}

/*! Periodically report number of remaining compute tasks and check farm's done queue for completed tasks.
 */
void EvolvotronMain::tick()
{
  const uint tasks=farm()->tasks();
  if (tasks!=_statusbar_tasks)
    {
      if (tasks==0)
	_statusbar->message("Ready");
      else
	{
	  _statusbar->message(QString("%1 tasks remaining").arg(tasks));
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
      if (_known_displays.find(task->display())!=_known_displays.end())
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

/*! Set up a different image in each display. 

  The choice of initial structure to start from is quite crucial to giving a good user experience.
  We concatenate 3 transforms, each seeded with random basis vectors and offsets.
  You can think of the first function as a co-ordinate transform,
  the second function as being the "actual" image,
  and the final function as being a colour-space transform.
  Basically the idea is to give lots of opportunities for stuff to happen.

  \todo Try something more interesting for the middle function.  (But can't just use a random stub because the chances are it will just be a constant.)
 */
void EvolvotronMain::reset()
{
  for (std::vector<MutatableImageDisplay*>::iterator it=display().begin();it!=display().end();it++)
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
      
      (*it)->image(new MutatableImageNodeConcatenateTriple(args_toplevel));
    }

  _mutation_parameters.reset();
}
