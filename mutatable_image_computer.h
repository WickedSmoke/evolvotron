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
  \brief Interface for class MutatableImageComputer.
*/

#ifndef _mutatable_image_computer_h_
#define _mutatable_image_computer_h_

#include <qthread.h>
#include <qmutex.h>

#include "useful.h"
#include "mutatable_image.h"

class MutatableImageDisplay;
class MutatableImageComputerFarm;
class MutatableImageComputerTask;

//! Class to handle computing of MutatableImages in a separate thread.
/*! A compute task starts up and fetches work from it's parent farm.
  The parent farm thread can communicate when necessary using the public methods of the class.
 */
class MutatableImageComputer : public QThread
{
 protected:
  //! Pointer to compute farm of which this thread is part.
  MutatableImageComputerFarm*const _farm;

  //! The current task.
  MutatableImageComputerTask* _task;

  //! Class encapsulating mutex-protected flags used for communicating between farm and worker
  class Communications
    {
    protected:
      //! Mutex protecting access to members (mutable to enable const-ness of accessors).
      mutable QMutex _mutex;
      
      //! Flag to indicate we should put our current task back on the todo queue and take another one.
      /*! volatile because used for inter-thread communication.
       */
      volatile bool _defer;
      
      //! Flag to indicate we should abort the current compute.
      /*! volatile because used for inter-thread communication.
       */
      volatile bool _abort;
      
      //! Flag to indicate the thread should shut down and exit.
      /*! volatile because used for inter-thread communication.
       */
      volatile bool _kill;

    public:
      //! Constructor.
      /*! Mutex is recursive to allow nesting.
       */
      Communications()
	:_mutex(true)
	,_defer(false)
	,_abort(false)
	,_kill(false)
	{}

      //! Mutex-protected accessor.
      void defer(bool v)
	{
	  _mutex.lock();
	  _defer=v;
	  _mutex.unlock();
	}
      //! Mutex-protected accessor.
      const bool defer() const
	{
	  _mutex.lock();
	  const bool ret=_defer;
	  _mutex.unlock();
	  return ret;
	}
      //! Mutex-protected accessor.
      void abort(bool v)
	{
	  _mutex.lock();
	  _abort=v;
	  _mutex.unlock();	  
	}
      //! Mutex-protected accessor.
      const bool abort() const
	{
	  _mutex.lock();
	  const bool ret=_abort;
	  _mutex.unlock();
	  return ret;
	}
      //! Mutex-protected accessor.
      void kill(bool v)
	{
	  _mutex.lock();
	  _kill=v;
	  _mutex.unlock();
	}
      //! Mutex-protected accessor.
      const bool kill() const
	{
	  _mutex.lock();
	  const bool ret=_kill;
	  _mutex.unlock();
	  return ret;
	}
    };

  // Instance of communications
  Communications _communications;
  
  //! The actual compute code, launched by invoking start() in the constructor
  virtual void run();

  //! Accessor 
  Communications& communications()
    {
      return _communications;
    }
  //! Accessor 
  const Communications& communications() const
    {
      return _communications;
    }
  
  //! Accessor.
  MutatableImageComputerTask*const task() const
    {
      return _task;
    }

  //! Accessor.
  MutatableImageComputerFarm*const farm() const
    {
      return _farm;
    }

 public:

  //! Constructor
  MutatableImageComputer(MutatableImageComputerFarm* frm);

  //! Destructor
  ~MutatableImageComputer();

  //! Defer the current task if it's priority is less important than specified.  Returns true if deferrment occurred.
  bool defer_if_less_important_than(uint pri);

  //! This method called by an external threads to shut down the current task
  void abort();

  //! This method called by an external threads to shut down the current task if it's for a particular display
  void abort_for(const MutatableImageDisplay* disp);

  //! This method called by external thread to kill the thread.
  void kill();

  //! Returns whether the computer has a task currently
  /*! \warning Access to _task not mutex locked.
    \warning Only intended for debugging & status reporting.
   */
  bool active() const
    {
      return (task()!=0);
    }
};

#endif
