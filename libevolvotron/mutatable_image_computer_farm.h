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
  \brief Interface for class MutatableImageComputerFarm.
*/

#ifndef _mutatable_image_computer_farm_h_
#define _mutatable_image_computer_farm_h_

#include "common.h"
#include "useful.h"

#include "mutatable_image_computer.h"
#include "mutatable_image_computer_task.h"

class MutatableImageComputer;
class MutatableImageDisplay;

//! Class encapsulating some compute threads and queues of tasks to be done and tasks completed.
/*! Priority queues are implemented using multiset becase we want to be able to iterate over all members.
 */
class MutatableImageComputerFarm
{
 protected:
  
  //! Comparison class for STL template.
  class CompareTaskPriorityLoResFirst : public std::binary_function<boost::shared_ptr<const MutatableImageComputerTask>,boost::shared_ptr<const MutatableImageComputerTask>,bool> 
    {
    public:
      //! Compare task priorities.
      bool operator()(const boost::shared_ptr<const MutatableImageComputerTask>& t0,const boost::shared_ptr<const MutatableImageComputerTask>& t1) 
	{ 
	  return (t0->priority() < t1->priority());
	}
    };

  //! Comparison class for STL template.
  class CompareTaskPriorityHiResFirst : public std::binary_function<boost::shared_ptr<const MutatableImageComputerTask>,boost::shared_ptr<const MutatableImageComputerTask>,bool> 
    {
    public:
      //! Compare task priorities.
      bool operator()(const boost::shared_ptr<const MutatableImageComputerTask>& t0,const boost::shared_ptr<const MutatableImageComputerTask>& t1) 
	{ 
	  return (t0->priority() > t1->priority());
	}
    };

  //! Mutex for locking.  This is the ONLY thing the compute threads should ever block on.
  mutable QMutex _mutex;

  //! Wait condition for threads waiting for a new task.
  QWaitCondition _wait_condition;

  //! The compute threads
  boost::ptr_vector<MutatableImageComputer> _computers;

  //! Convenience typedef.
  typedef std::multiset<boost::shared_ptr<MutatableImageComputerTask>,CompareTaskPriorityLoResFirst> TodoQueue;

  //! Queue of tasks to be performed, lowest resolution first
  TodoQueue _todo;

  //! Conveniencetypedef.
  typedef std::multiset<boost::shared_ptr<MutatableImageComputerTask>,CompareTaskPriorityHiResFirst> DoneQueue;

  //! Convenience typedef.  
  /*! const because never needs to do anything other than compare pointers
   */
  typedef std::map<const MutatableImageDisplay*,DoneQueue> DoneQueueByDisplay;

  //! Queue of tasks completed awaiting display.
  /*! We reverse the compute priority so that highest resolution images get displayed first.
      Lower resolution ones arriving later should be discarded by the displays.
      This mainly makes a difference for animation where enlarging multiple low resolution 
      images to screen res takes a lot of time.  May help low-bandwidth X11 connections
      by minimising redraws too.
      We now also sort by display and do round-robin delivery (ithout this one display can run way ahead of the others)
   */
  DoneQueueByDisplay _done;

  //! Points to the next display queue to be returned (could be .end())
  DoneQueueByDisplay::iterator _done_position;

 public:

  //! Constructor.
  MutatableImageComputerFarm(uint n_threads,int niceness);

  //! Destructor cleans up threads.
  ~MutatableImageComputerFarm();

  //! Accessor.
  uint num_threads() const
    {
      return _computers.size();
    }

  //! Move aborted tasks from todo queue to done queue.
  void fasttrack_aborted();

  //! Enqueue a task for computing.
  void push_todo(const boost::shared_ptr<MutatableImageComputerTask>&);

  //! Remove a task from the head of the todo queue (returns null if none).
  const boost::shared_ptr<MutatableImageComputerTask> pop_todo(MutatableImageComputer& requester);

  //! Enqueue a task for display.
  void push_done(const boost::shared_ptr<MutatableImageComputerTask>&);

  //! Remove a task from the head of the display queue (returns null if none).
  const boost::shared_ptr<MutatableImageComputerTask> pop_done();

  //! Flags all tasks in all queues as aborted, and signals the compute threads to abort their current task.
  void abort_all();

  //! Flags all tasks for a particular display as aborted (including compute threads)
  void abort_for(const MutatableImageDisplay* disp);

  //! Number of tasks in queues
  uint tasks() const;
};

#endif
