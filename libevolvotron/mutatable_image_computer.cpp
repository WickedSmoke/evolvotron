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
  \brief Implementation of class MutatableImageComputer.
*/

#include "mutatable_image_computer.h"

#include "mutatable_image.h"
#include "mutatable_image_computer_farm.h"
#include "mutatable_image_computer_task.h"

#include "platform_specific.h"

MutatableImageComputer::MutatableImageComputer(MutatableImageComputerFarm* frm,int niceness)
  :_farm(frm)
  ,_niceness(niceness)
  ,_r01(23)  // Seed pretty unimportant; only used for sample jitter
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

/*! Compute threads run this method until killed (probably by the destructor being invoked by the original spawning thread.
 */
void MutatableImageComputer::run()
{
  std::clog << "Thread starting\n";

  // Lower compute thread priority slightly; computing yet more stuff
  // is less important than displaying the results we've got so far.
  add_thread_niceness(_niceness);

  // Run until something sets the kill flag 
  while(!communications().kill())
    {
      // If we don't have a task try and get one.  This will block or return null.
      if (task()==0)
	{
	  _task=farm()->pop_todo(*this);
	}
      
      if (task())
	{
	  // Careful, we could be given an already aborted task
	  if (!task()->aborted())
	    {
	      while (!communications().kill_or_abort_or_defer() && !task()->completed())
		{
		  XYZ accumulated_colour=task()->image_function()->get_rgb
		    (
		     task()->fragment_origin().width()+task()->current_col(),
		     task()->fragment_origin().height()+task()->current_row(),
		     task()->current_frame(),
		     task()->whole_image_size().width(),
		     task()->whole_image_size().height(),
		     task()->frames(),
		     (task()->jittered_samples() ? &_r01 : 0),
		     task()->multisample_grid()
		     );

		  const uint col0=lrint(accumulated_colour.x());
		  const uint col1=lrint(accumulated_colour.y());
		  const uint col2=lrint(accumulated_colour.z());

		  task()->images()[task()->current_frame()].setPixel(task()->current_col(),task()->current_row(),((col0<<16)|(col1<<8)|(col2)));

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
