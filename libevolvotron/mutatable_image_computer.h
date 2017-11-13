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
  \brief Interface for class MutatableImageComputer.
*/

#ifndef _mutatable_image_computer_h_
#define _mutatable_image_computer_h_

#include "common.h"

#include "mutatable_image.h"
#include "random.h"

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

  //! Priority offset applied to compute threads.
  const int _niceness;

  //! The current task.  Can't be a const MutatableImageComputerTask because the task holds the calculated result.
  boost::shared_ptr<MutatableImageComputerTask> _task;

  //! Randomness for sampling jitter
  Random01 _r01;

  //! Class encapsulating mutex-protected flags used for communicating between farm and worker.
  /*! The Mutex is of dubious value (could certainly be eliminated for reads).
   */
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
	:_mutex()
	,_defer(false)
	,_abort(false)
	,_kill(false)
	{}

      //! Mutex-protected accessor.
      void defer(bool v)
	{
	  QMutexLocker lock(&_mutex);
	  _defer=v;
	}
      //! Mutex-protected accessor.
      bool defer() const
	{
	  QMutexLocker lock(&_mutex);
	  const bool ret=_defer;
	  return ret;
	}
      //! Mutex-protected accessor.
      void abort(bool v)
	{
	  QMutexLocker lock(&_mutex);
	  _abort=v;
	}
      //! Mutex-protected accessor.
      bool abort() const
	{
	  QMutexLocker lock(&_mutex);
	  const bool ret=_abort;
	  return ret;
	}
      //! Mutex-protected accessor.
      void kill(bool v)
	{
	  QMutexLocker lock(&_mutex);
	  _kill=v;
	}
      //! Mutex-protected accessor.
      bool kill() const
	{
	  QMutexLocker lock(&_mutex);
	  const bool ret=_kill;
	  return ret;
	}
      //! Check union of all flags with only one mutex lock.
      bool kill_or_abort_or_defer() const
	{
	  QMutexLocker lock(&_mutex);
	  const bool ret=(_kill || _abort || _defer);
	  return ret;
	}
    };

  //! Instance of communications flags.
  Communications _communications;
  
  //! The actual compute code, launched by invoking start() in the constructor.
  virtual void run();

  //! Accessor.
  Communications& communications()
    {
      return _communications;
    }
  //! Accessor.
  const Communications& communications() const
    {
      return _communications;
    }
  
  //! Accessor.
  const boost::shared_ptr<MutatableImageComputerTask>& task() const
    {
      return _task;
    }

  //! Accessor.
  MutatableImageComputerFarm* farm() const
    {
      return _farm;
    }

 public:

  //! Constructor
  MutatableImageComputer(MutatableImageComputerFarm* frm,int niceness);

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

  //! Return kill state.
  /*! Needs external visibility for deciding what to do when woken from wait for task.
   */
  bool killed() const;

  //! Indicate whether computation us taking place (only intended for counting outstanding threads).
  bool active() const
    {
      return (_task!=0);
    }
};

#endif
