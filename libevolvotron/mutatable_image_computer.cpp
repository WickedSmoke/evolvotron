// Source file for evolvotron
// Copyright (C) 2007 Tim Day
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

#include "libevolvotron_precompiled.h"

#include "mutatable_image_computer.h"

// Needed for getpriority/setprioirty
#include <sys/resource.h>

#include "mutatable_image.h"
#include "mutatable_image_computer_farm.h"
#include "mutatable_image_computer_task.h"

MutatableImageComputer::MutatableImageComputer(MutatableImageComputerFarm* frm,int niceness)
  :
#ifndef NDEBUG
  InstanceCounted(typeid(this).name(),false),
#endif
  _farm(frm),
  _niceness(niceness),
  _r01(23)  // Seed pretty unimportant; only used for sample jitter
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

  // Lower compute thread priority slightly;
  // computing yet more stuff is less important than displaying the results we've got.
  /*! \todo: People porting to non-Linux (BSD, MacOS, Fink etc) please send
    a suitable #ifdef-able patch if you need something different here.
    Note that this code relies on Linux NPTL's non-Posix-compliant
    thread-specific nice value (although without a suitable replacement
    per-thread priority mechanism it's just as well it's that way).
    \todo: Should check some error codes, but it's pretty harmless if it doesn't work.
  */
  const int current_priority=getpriority(PRIO_PROCESS,0);
  setpriority(PRIO_PROCESS,0,std::min(19,current_priority+_niceness));

  // Run until something sets the kill flag 
  while(!communications().kill())
    {
      // If we don't have a task try and get one.  This will block or return null.
      if (task()==0)
	{
	  _task=farm()->pop_todo(*this);
	}
      
      if (task()!=0)
	{
	  // DEBUG
	  std::clog << "[" << task()->multisample_level() << "]";

	  // Careful, we could be given an already aborted task
	  if (!task()->aborted())
	    {
	      const uint frames=task()->frames();
	      const uint height=task()->size().height();
	      const uint width=task()->size().width();

	      while (!communications().kill_or_abort_or_defer() && !task()->completed())
		{
		  XYZ accumulated_colour(0.0,0.0,0.0);
		  const uint multisample=task()->multisample_level();
		  for (uint sy=0;sy<multisample;sy++)
		    for (uint sx=0;sx<multisample;sx++)
		      {
			//! \todo: Multisampling in z would be a motion blur/exposure length sort of effect (but not implemented).
			// xyz co-ords vary over -1.0 to 1.0
			// In the one frame case z will be 0
			const real jx=(task()->jittered_samples() ? _r01() : 0.5);
			const real jy=(task()->jittered_samples() ? _r01() : 0.5);
			const XYZ p
			  (
			   task()->image()->sampling_coordinate
			   (
			    task()->current_col()+(sx+jx)/multisample,
			    task()->current_row()+(sy+jy)/multisample,
			    task()->current_frame(),
			    width,
			    height,
			    frames
			    )
			   );
			
			accumulated_colour+=task()->image()->get_rgb(p);
		      }

		  accumulated_colour/=(multisample*multisample);
		  
		  // Clamp out of range values
		  accumulated_colour.x(clamped(accumulated_colour.x(),0.0,255.0));
		  accumulated_colour.y(clamped(accumulated_colour.y(),0.0,255.0));
		  accumulated_colour.z(clamped(accumulated_colour.z(),0.0,255.0));
		  
		  const uint col0=lrint(accumulated_colour.x());
		  const uint col1=lrint(accumulated_colour.y());
		  const uint col2=lrint(accumulated_colour.z());

		  task()->image_data()[task()->current_pixel()]=((col0<<16)|(col1<<8)|(col2));

		  task()->pixel_advance();
		}
	    }
	  
	  // Maybe should capture copies of the flags for use here
	  if (!communications().kill())
	    {
	      if (communications().defer() && !communications().abort())
		{
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

bool MutatableImageComputer::killed() const
{
  return communications().kill();
}
