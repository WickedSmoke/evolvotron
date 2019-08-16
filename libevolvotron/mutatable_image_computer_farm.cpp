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
  \brief Implementation of class MutatableImageComputerFarm.
*/

#include "mutatable_image_computer_farm.h"

#include "mutatable_image_computer.h"

/*! Creates the specified number of threads and store pointers to them.
 */
MutatableImageComputerFarm::MutatableImageComputerFarm(uint n_threads, int niceness)
{
  _done_position = _done.end();

  for (uint i = 0; i < n_threads; i++)
  {
    // The computer's constructor includes a start()
    _computers.push_back(new MutatableImageComputer(this, niceness));
  }
}

/*! Destructor kills off all compute threads and frees their resources.  
  NB The MutatableImageComputer destructor signals the thread to stop and waits for it.
 */
MutatableImageComputerFarm::~MutatableImageComputerFarm()
{
  std::clog << "Compute farm shut down begun...\n";

  // Kill all the computers (care needed to wake any waiting ones).
  for (boost::ptr_vector<MutatableImageComputer>::iterator it = _computers.begin(); it != _computers.end(); it++)
    (*it).kill();
  _wait_condition.wakeAll();
  _computers.clear();

  // Clear all the tasks in queues
  {
    QMutexLocker lock(&_mutex);
    _todo.clear();
    _done.clear();
  }

  std::clog << "...completed compute farm shut down\n";
}

//! Predicate function to test whether a task has been aborted
static bool predicate_aborted(const boost::shared_ptr<const MutatableImageComputerTask> t)
{
  return t->aborted();
}

void MutatableImageComputerFarm::fasttrack_aborted()
{
  QMutexLocker lock(&_mutex);

  // == PATCH START ==

  // -- code modified --
  TodoQueue::iterator it = _todo.begin();

  while (it != _todo.end())
  {
    if ((*it)->aborted())
    {
      _done[(*it)->display()].insert(*it);
      it = _todo.erase(it);
    }
    else
      it++;
  }

  // == PATCH END ==
}

void MutatableImageComputerFarm::push_todo(const boost::shared_ptr<MutatableImageComputerTask> &task)
{
  {
    QMutexLocker lock(&_mutex);

    // We could be in a situation where there are tasks with lower priority which should be defered in favour of this one.
    // Currently we simply defer everything with a lower priority and let the queue sort them out.
    //! \todo: It would be better to just defer the lowest priority task if there's any less than the queued task.
    /*
    bool any_deferred=false;
    for (boost::ptr_vector<MutatableImageComputer>::iterator it=_computers.begin();it!=_computers.end();it++)
      {
	if ((*it).defer_if_less_important_than(task->priority()))
	  {
	    any_deferred=true;
	  }
      }
    */

    _todo.insert(task);
  }

  // If there any threads waiting, we should wake one up.
  _wait_condition.wakeOne();
}

const boost::shared_ptr<MutatableImageComputerTask> MutatableImageComputerFarm::pop_todo(MutatableImageComputer &requester)
{
  _mutex.lock();
  boost::shared_ptr<MutatableImageComputerTask> ret;
  while (!ret)
  {
    TodoQueue::iterator it = _todo.begin();
    if (it != _todo.end())
    {
      ret = (*it);
      _todo.erase(it);
    }
    else
    {
      std::clog << "Thread waiting\n";
      _wait_condition.wait(&_mutex);
      std::clog << "Thread woken\n";
      if (requester.killed())
        break;
    }
  }
  _mutex.unlock();
  return ret;
}

void MutatableImageComputerFarm::push_done(const boost::shared_ptr<MutatableImageComputerTask> &task)
{
  QMutexLocker lock(&_mutex);
  _done[task->display()].insert(task);
}

const boost::shared_ptr<MutatableImageComputerTask> MutatableImageComputerFarm::pop_done()
{
  QMutexLocker lock(&_mutex);

  boost::shared_ptr<MutatableImageComputerTask> ret;
  if (_done_position == _done.end())
  {
    _done_position = _done.begin();
  }

  if (_done_position != _done.end())
  {
    DoneQueue &q = (*_done_position).second;
    DoneQueue::iterator it = q.begin();
    if (it != q.end())
    {
      ret = (*it);
      q.erase(it);
    }

    if (q.empty())
    {
      DoneQueueByDisplay::iterator advanced_done_position = _done_position;
      advanced_done_position++;
      _done.erase(_done_position);
      _done_position = advanced_done_position;
    }
    else
    {
      _done_position++;
    }
  }

  return ret;
}

void MutatableImageComputerFarm::abort_all()
{
  QMutexLocker lock(&_mutex);

  for (TodoQueue::iterator it = _todo.begin(); it != _todo.end(); it++)
  {
    (*it)->abort();
  }
  _todo.clear();

  for (boost::ptr_vector<MutatableImageComputer>::iterator it = _computers.begin(); it != _computers.end(); it++)
  {
    (*it).abort();
  }

  for (DoneQueueByDisplay::iterator it0 = _done.begin(); it0 != _done.end(); it0++)
  {
    DoneQueue &q = (*it0).second;
    for (DoneQueue::iterator it1 = q.begin(); it1 != q.end(); it1++)
    {
      (*it1)->abort();
    }
  }
  _done.clear();
}

void MutatableImageComputerFarm::abort_for(const MutatableImageDisplay *disp)
{
  QMutexLocker lock(&_mutex);

  for (TodoQueue::iterator it = _todo.begin(); it != _todo.end(); it++)
  {
    if ((*it)->display() == disp)
    {
      (*it)->abort();

      // == PATCH START ==
      // -- code modified --
      it = _todo.erase(it);
      if (it == _todo.end())
        break;
      // == PATCH END ==
    }
  }

  for (boost::ptr_vector<MutatableImageComputer>::iterator it = _computers.begin(); it != _computers.end(); it++)
  {
    (*it).abort_for(disp);
  }

  DoneQueueByDisplay::iterator it0 = _done.find(disp);
  if (it0 != _done.end())
  {
    DoneQueue &q = (*it0).second;

    //! \todo It would be pretty odd if display didn't match the queue bin: change to assert
    for (DoneQueue::iterator it1 = q.begin(); it1 != q.end(); it1++)
    {
      if ((*it1)->display() == disp)
      {
        (*it1)->abort();

        // == PATCH START ==
        // -- code modified --
        it1 = q.erase(it1);
        if (it1 == q.end())
          break;
        // == PATCH END ==
      }
    }
  }
}

uint MutatableImageComputerFarm::tasks() const
{
  uint ret = 0;

  for (boost::ptr_vector<MutatableImageComputer>::const_iterator it = _computers.begin(); it != _computers.end(); it++)
  {
    if ((*it).active())
    {
      ret++;
    }
  }

  QMutexLocker lock(&_mutex);

  ret += _todo.size();

  for (DoneQueueByDisplay::const_iterator it = _done.begin(); it != _done.end(); it++)
    ret += (*it).second.size();

  return ret;
}
