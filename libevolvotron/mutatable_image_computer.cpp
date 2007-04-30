// Source file for evolvotron
// Copyright (C) 2002,2003,2007 Tim Day
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
  \brief Implementation of class MutatableImageComputer.
*/

#include <iostream>

#include "mutatable_image_computer.h"
#include "mutatable_image.h"
#include "mutatable_image_computer_farm.h"
#include "mutatable_image_computer_task.h"

// This is needed for tanh
#include <cmath>

MutatableImageComputer::MutatableImageComputer(MutatableImageComputerFarm* frm)
  :
#ifndef NDEBUG
  InstanceCounted(typeid(this).name(),false),
#endif
  _farm(frm)
{
  start();
}

MutatableImageComputer::~MutatableImageComputer()
{
  std::clog << "Deleting a computer...\n";

  kill();
  wait();

  _task.reset();

  std::clog << "...deleted a computer\n";
}

/*! Compute threads run this method untill killed (probably by the destructor being invoked by the original spawning thread.
 */
void MutatableImageComputer::run()
{
  std::clog << "Thread starting\n";

  // Run until something sets the kill flag 
  while(!communications().kill())
    {
      // If we don't have a task try and get one
      if (task()==0)
	{
	  _task=farm()->pop_todo();
	}
      
      // If we still don't have one might as well wait a bit before retrying
      if (task()==0)
	{
	  msleep(10);
	}
      else
	{
	  // Careful, we could be given an already aborted task
	  if (!task()->aborted())
	    {
	      const uint frames=task()->frames();
	      const uint height=task()->size().height();
	      const uint width=task()->size().width();

	      while (!communications().kill_or_abort_or_defer() && !task()->completed())
		{
		  // xyz co-ords vary over -1.0 to 1.0
		  // In the one frame case z will be 0
		  const XYZ p(task()->image()->sampling_coordinate
			      (
			       task()->current_col(),task()->current_row(),task()->current_frame(),
			       width,height,frames
			       )
			      );

		  
		  uint c[3];
		  task()->image()->get_rgb(p,c);

		  task()->image_data()[task()->current_pixel()]=((c[0]<<16)|(c[1]<<8)|(c[2]));

		  task()->pixel_advance();
		}
	    }
	  
	  // Maybe should capture copies of the flags for use here
	  if (!communications().kill())
	    {
	      if (communications().defer() && !communications().abort())
		{
		  // \todo Optimisation: Add a progress tracker to Task class so deferred tasks can continue from where they left off.
		  farm()->push_todo(task());
		  communications().defer(false);
		  _task.reset();
		}
	      else
		{
		  if (communications().abort())
		    {
		      task()->abort();
		    }
	      
		  communications().defer(false);
		  communications().abort(false);

		  farm()->push_done(task());	  
		  _task.reset();
		}
	    }
	}
    }
  std::clog << "Thread shutting down\n";
}

bool MutatableImageComputer::defer_if_less_important_than(uint pri)
{
  const boost::shared_ptr<const MutatableImageComputerTask> task_tmp=_task;
  if (task_tmp && task_tmp->priority()>pri)
    {
      communications().defer(true);
      return true;
    }
  else
    {
      return false;
    }
}

void MutatableImageComputer::abort()
{
  communications().abort(true);
}

void MutatableImageComputer::abort_for(const MutatableImageDisplay* disp)
{
  if (task()!=0 && task()->display()==disp)
    {
      communications().abort(true);
    }
}

void MutatableImageComputer::kill()
{
  communications().kill(true);
}
