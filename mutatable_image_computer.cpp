// Source file for evolvotron
// Copyright (C) 2002,2003 Tim Day
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

MutatableImageComputer::MutatableImageComputer(MutatableImageComputerFarm* frm)
  :_farm(frm)
  ,_task(0)
{
  start();
}

MutatableImageComputer::~MutatableImageComputer()
{
  kill();
  wait();

  delete _task;
}

/*! Compute threads run this method untill killed (probably by the destructor being invoked by the original spawning thread.
 */
void MutatableImageComputer::run()
{
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
	      const uint height=task()->size().height();
	      const uint width=task()->size().width();
	      const uint pixels=height*width;

	      while (task()->pixel()<pixels)
		{
		  if (communications().kill()) exit();
		  if (communications().abort()) break;
		  if (communications().defer()) break;
		  
		  const uint r=task()->pixel()/width;
		  const uint c=task()->pixel()%width;
		
		  // xy co-ords vary over -1.0 to 1.0
		  const XYZ p(
			      -1.0+2.0*(c+0.5)/width,
			       1.0-2.0*(r+0.5)/height,
			      0.0
			      );
		  
		  const MutatableImage*const image=task()->image();

		  // Actually calculate a pixel value from the image.
		  const XYZ pv((*image)(p));

		  // Scale nominal -1.0 to 1.0 range to 0-255
		  XYZ v(127.5*(pv+XYZ(1.0,1.0,1.0)));
		  
		  // Clamp out of range values.
		  v.x(clamped(v.x(),0.0f,255.0f));
		  v.y(clamped(v.y(),0.0f,255.0f));
		  v.z(clamped(v.z(),0.0f,255.0f));
		  
		  const uint red  =(uint)floorf(v.x());
		  const uint blue =(uint)floorf(v.y());
		  const uint green=(uint)floorf(v.z());
		  
		  task()->image_data()[task()->pixel()]=((red<<16)|(green<<8)|(blue));

		  task()->pixel_advance();
		}
	    }
	  
	  if (communications().defer() && !communications().abort())
	    {
	      // \todo Optimisation: Add a progress tracker to Task class so deferred tasks can continue from where they left off.
	      farm()->push_todo(task());
	      communications().defer(false);
	      _task=0;
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
	      _task=0;
	    }
	}
    }
}

bool MutatableImageComputer::defer_if_less_important_than(uint pri)
{
  //! \todo Need to mutex-lock task here, because it could be changing under us.
  const MutatableImageComputerTask*const task_tmp=_task;
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
