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

#include "evolvotron_main.h"
#include "xyz.h"

/*! Constructor sets up GUI components and fires up QTimer.
 */
EvolvotronMain::EvolvotronMain(QWidget* parent,const QSize& grid_size,uint n_threads)
  :QVBox(parent)
   ,_statusbar_tasks(0)
   ,_mutation_parameters(time(0),0.5,0.05,0.05)
{
  _dialog_about=new DialogAbout(this);

  _menubar=new QMenuBar(this);

  _popupmenu_file=new QPopupMenu;
  _popupmenu_file->insertItem("&Restart",this,SLOT(reset()));
  _popupmenu_file->insertItem("&Quit",qApp,SLOT(quit()));

  _popupmenu_help=new QPopupMenu;
  _popupmenu_help->insertItem("&About",_dialog_about,SLOT(show()));

  _menubar->insertItem("&File",_popupmenu_file);
  _menubar->insertSeparator();
  _menubar->insertItem("&Help",_popupmenu_help);

  _grid=new QGrid(grid_size.width(),this);

  _statusbar=new QStatusBar(this);
  _statusbar->setSizeGripEnabled(false);
  _statusbar->message("Ready");

  // We need to make sure the display grid gets all the space it can
  setStretchFactor(_grid,1);
  setStretchFactor(_statusbar,0);

  _timer=new QTimer(this);

  connect(
	  _timer,SIGNAL(timeout()),
	  this, SLOT(tick()) 
	  );

  _farm=new MutatableImageComputerFarm(n_threads);

  for (int r=0;r<grid_size.height();r++)
    for (int c=0;c<grid_size.width();c++)
      {
	display().push_back(new MutatableImageDisplay(_grid,this,true));
      }

  // Run tick() at 100Hz
  _timer->start(10);
}

/*! If one of our sub displays has spawned, distribute a mutated copy of its image to the other non-locked images
  in the mutation grid.
 */
void EvolvotronMain::spawn(MutatableImageDisplay* spawning_display)
{
  // Spawn can be a bit sluggish so set the hourglass cursor.
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

      args_toplevel.push_back(new MutatableImageNodeTransform(args0));

      std::vector<MutatableImageNode*> args1;
      args1.push_back(new MutatableImageNodeConstant(RandomXYZInSphere(mutation_parameters().rng01(),1.0)));
      args1.push_back(new MutatableImageNodeConstant(RandomXYZInSphere(mutation_parameters().rng01(),1.0)));
      args1.push_back(new MutatableImageNodeConstant(RandomXYZInSphere(mutation_parameters().rng01(),1.0)));
      args1.push_back(new MutatableImageNodeConstant(RandomXYZInSphere(mutation_parameters().rng01(),1.0)));
      args1.push_back(new MutatableImageNodePosition());
      
      args_toplevel.push_back(new MutatableImageNodeTransform(args1));

      std::vector<MutatableImageNode*> args2;
      args2.push_back(new MutatableImageNodeConstant(RandomXYZInSphere(mutation_parameters().rng01(),1.0)));
      args2.push_back(new MutatableImageNodeConstant(RandomXYZInSphere(mutation_parameters().rng01(),1.0)));
      args2.push_back(new MutatableImageNodeConstant(RandomXYZInSphere(mutation_parameters().rng01(),1.0)));
      args2.push_back(new MutatableImageNodeConstant(RandomXYZInSphere(mutation_parameters().rng01(),1.0)));
      args2.push_back(new MutatableImageNodePosition());

      args_toplevel.push_back(new MutatableImageNodeTransform(args2));
      
      (*it)->image(new MutatableImageNodeConcatenateTriple(args_toplevel));
    }
}
