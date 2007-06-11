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
  \brief Implementation of class MutatableImageComputerFarm.
*/

#include "mutatable_image_computer_farm.h"
#include "mutatable_image_computer.h"
#include <algorithm>

/*! Creates the specified number of threads and store pointers to them.
 */
MutatableImageComputerFarm::MutatableImageComputerFarm(uint n_threads)
{
  _done_position=_done.end();

  for (uint i=0;i<n_threads;i++)
    {
      // The computer's constructor includes a start()
      _computers.push_back(new MutatableImageComputer(this));
    }
}

/*! Destructor kills off all compute threads and frees their resources.  
  NB The MutatableImageComputer destructor signals the thread to stop and waits for it.
 */
MutatableImageComputerFarm::~MutatableImageComputerFarm()
{
  std::clog << "Compute farm shut down begun...\n";

  // Kill all the computers (care needed to wake any waiting ones).
  for (boost::ptr_vector<MutatableImageComputer>::iterator it=_computers.begin();it!=_computers.end();it++) (*it).kill();
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
static const bool predicate_aborted(const boost::shared_ptr<const MutatableImageComputerTask> t) 
{
  return t->aborted();
}

void MutatableImageComputerFarm::fasttrack_aborted()
{
  QMutexLocker lock(&_mutex);
  
  // \todo: Inefficient starting search again each time.  Some problem with erase otherwise though, but might have been task abort mem leak.
  TodoQueue::iterator it;
  while (
	 (
	  it=std::find_if(_todo.begin(),_todo.end(),predicate_aborted)
	  )
	 !=
	 _todo.end()
	 )
    {
      _done[(*it)->display()].insert(*it);
      _todo.erase(it);
    }  
}

void MutatableImageComputerFarm::push_todo(const boost::shared_ptr<MutatableImageComputerTask>& task)
{
  QMutexLocker lock(&_mutex);
  _todo.insert(task);
  _wait_condition.wakeOne();

  // \todo: Check if any of the computers are executing lower priority tasks and if so defer least important one (however, we're currently deferring all).
  for (boost::ptr_vector<MutatableImageComputer>::iterator it=_computers.begin();it!=_computers.end();it++)
    {
      (*it).defer_if_less_important_than(task->priority());
    }
}

const boost::shared_ptr<MutatableImageComputerTask> MutatableImageComputerFarm::pop_todo(MutatableImageComputer& requester)
{
  QMutexLocker lock(&_mutex);

  boost::shared_ptr<MutatableImageComputerTask> ret;
  while (!ret)
    {
      TodoQueue::iterator it=_todo.begin();
      if (it!=_todo.end())
	{
	  ret=(*it);
	  _todo.erase(it);
	}
      else
	{
	  std::clog << "Thread waiting\n";
	  _wait_condition.wait(&_mutex);
	  std::clog << "Thread woken\n";
	  if (requester.killed()) break;
	}
    }
  return ret;
}

void MutatableImageComputerFarm::push_done(const boost::shared_ptr<MutatableImageComputerTask>& task)
{
  QMutexLocker lock(&_mutex);
  _done[task->display()].insert(task);
}

const boost::shared_ptr<MutatableImageComputerTask> MutatableImageComputerFarm::pop_done()
{
  QMutexLocker lock(&_mutex); 

  boost::shared_ptr<MutatableImageComputerTask> ret;  
  if (_done_position==_done.end())
    {
      _done_position=_done.begin();
    }

  if (_done_position!=_done.end())
    {
      DoneQueue& q=(*_done_position).second;
      DoneQueue::iterator it=q.begin();
      if (it!=q.end())
	{
	  ret=(*it);
	  q.erase(it);
	}

      if (q.empty())
	{
	  DoneQueueByDisplay::iterator advanced_done_position=_done_position;
	  advanced_done_position++;	  
	  _done.erase(_done_position);
	  _done_position=advanced_done_position;
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

  for (TodoQueue::iterator it=_todo.begin();it!=_todo.end();it++)
    {
      (*it)->abort();
    }
  _todo.clear();

  for (boost::ptr_vector<MutatableImageComputer>::iterator it=_computers.begin();it!=_computers.end();it++)
    {
      (*it).abort();
    }

  for (DoneQueueByDisplay::iterator it0=_done.begin();it0!=_done.end();it0++)
    {
      DoneQueue& q=(*it0).second;
      for (DoneQueue::iterator it1=q.begin();it1!=q.end();it1++)
	{
	  (*it1)->abort();
	}
    }
  _done.clear();
}

void MutatableImageComputerFarm::abort_for(const MutatableImageDisplay* disp)
{
  QMutexLocker lock(&_mutex); 

  for (TodoQueue::iterator it=_todo.begin();it!=_todo.end();it++)
    {
      if ((*it)->display()==disp)
	{
	  (*it)->abort();
	  _todo.erase(it);
	}
    }
  
  for (boost::ptr_vector<MutatableImageComputer>::iterator it=_computers.begin();it!=_computers.end();it++)
    {      
      (*it).abort_for(disp);
    }
  
  DoneQueueByDisplay::iterator it0=_done.find(disp);
  if (it0!=_done.end())
    {
      DoneQueue& q=(*it0).second;

      //! \todo It would be pretty odd if display didn't match the queue bin: change to assert
      for (DoneQueue::iterator it1=q.begin();it1!=q.end();it1++)
	{
	  if ((*it1)->display()==disp)
	    {
	      (*it1)->abort();
	      q.erase(it1);
	    }
	}
    }
}

std::ostream& MutatableImageComputerFarm::write_info(std::ostream& out) const
{
  uint active_computers=0;

  for (boost::ptr_vector<MutatableImageComputer>::const_iterator it=_computers.begin();it!=_computers.end();it++)
    {      
      if ((*it).active())
	{
	  active_computers++;
	}
    }

  //! /todo _done.size() no longer returns number of tasks to be delivered to, but number of displays with pending deliveries.  Fix ?
  out << "[" << _todo.size() << "/" << active_computers << "/" << _done.size() << "]";
  return out;
}

const uint MutatableImageComputerFarm::tasks() const
{
  uint ret=0;
  
  for (boost::ptr_vector<MutatableImageComputer>::const_iterator it=_computers.begin();it!=_computers.end();it++)
    {      
      if ((*it).active())
	{
	  ret++;
	}
    }

  QMutexLocker lock(&_mutex); 

  ret+=_todo.size();

  for (DoneQueueByDisplay::const_iterator it=_done.begin();it!=_done.end();it++)
    ret+=(*it).second.size();

  return ret;
}
