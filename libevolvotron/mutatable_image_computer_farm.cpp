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
  \brief Implementation of class MutatableImageComputerFarm.
*/

#include "mutatable_image_computer_farm.h"
#include <algorithm>

/*! Creates the specified number of threads and store pointers to them.
 */
MutatableImageComputerFarm::MutatableImageComputerFarm(uint n_threads)
{
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

  // Kill all the computers
  for (std::vector<MutatableImageComputer*>::iterator it=_computers.begin();it!=_computers.end();it++)
    {
      delete (*it);
    }

  // Clear all the tasks in queues
  _mutex.lock();

  for (std::multiset<MutatableImageComputerTask*,CompareTaskPriorityLoResFirst>::iterator it=_todo.begin();it!=_todo.end();it++)
    {
      delete (*it);
    }
  _todo.clear();

  for (std::multiset<MutatableImageComputerTask*,CompareTaskPriorityHiResFirst>::iterator it=_done.begin();it!=_done.end();it++)
    {
      delete (*it);
    }
  _done.clear();

  _mutex.unlock();

  std::clog << "...completed compute farm shut down\n";
}

//! Predicate function to test whether a task has been aborted
static const bool predicate_aborted(const MutatableImageComputerTask*const t) 
{
  return t->aborted();
}

void MutatableImageComputerFarm::fasttrack_aborted()
{
  _mutex.lock();
  
  // \todo: Inefficient starting search again each time.  Some problem with erase otherwise though, but might have been task abort mem leak.
  std::multiset<MutatableImageComputerTask*,CompareTaskPriorityLoResFirst>::iterator it;
  while (
	 (
	  it=std::find_if(_todo.begin(),_todo.end(),predicate_aborted)
	  )
	 !=
	 _todo.end()
	 )
    {
      _done.insert(*it);
      _todo.erase(it);
    }
  
  _mutex.unlock();
}

void MutatableImageComputerFarm::push_todo(MutatableImageComputerTask* task)
{
  _mutex.lock();
  _todo.insert(task);
  
  // Check if any of the computers are executing lower priority tasks and if so defer least important one.
  for (std::vector<MutatableImageComputer*>::iterator it=_computers.begin();it!=_computers.end();it++)
    {
      ((*it)->defer_if_less_important_than(task->priority()));
    }

  _mutex.unlock();
}

MutatableImageComputerTask*const MutatableImageComputerFarm::pop_todo()
{
  MutatableImageComputerTask* ret=0;
  _mutex.lock();
  
  std::multiset<MutatableImageComputerTask*,CompareTaskPriorityLoResFirst>::iterator it=_todo.begin();
  if (it!=_todo.end())
    {
      ret=(*it);
      _todo.erase(it);
    }

  _mutex.unlock();
  return ret;
}

void MutatableImageComputerFarm::push_done(MutatableImageComputerTask* task)
{
  _mutex.lock();
  _done.insert(task);
  _mutex.unlock();
}

MutatableImageComputerTask* MutatableImageComputerFarm::pop_done()
{
  MutatableImageComputerTask* ret=0;
  _mutex.lock();
  
  std::multiset<MutatableImageComputerTask*,CompareTaskPriorityHiResFirst>::iterator it=_done.begin();
  if (it!=_done.end())
    {
      ret=(*it);
      _done.erase(it);
    }

  _mutex.unlock();
  return ret;
}



void MutatableImageComputerFarm::abort_all()
{
  _mutex.lock();

  for (std::multiset<MutatableImageComputerTask*,CompareTaskPriorityLoResFirst>::iterator it=_todo.begin();it!=_todo.end();it++)
    {
      (*it)->abort();
      delete (*it);
      _todo.erase(it);
    }

  for (std::vector<MutatableImageComputer*>::iterator it=_computers.begin();it!=_computers.end();it++)
    {
      (*it)->abort();
    }

  for (std::multiset<MutatableImageComputerTask*,CompareTaskPriorityHiResFirst>::iterator it=_done.begin();it!=_done.end();it++)
    {
      (*it)->abort();
      delete (*it);
      _done.erase(it);
    }
  
  _mutex.unlock();  
}

void MutatableImageComputerFarm::abort_for(const MutatableImageDisplay* disp)
{
  _mutex.lock();

  for (std::multiset<MutatableImageComputerTask*,CompareTaskPriorityLoResFirst>::iterator it=_todo.begin();it!=_todo.end();it++)
    {
      if ((*it)->display()==disp)
	{
	  (*it)->abort();
	  delete (*it);
	  _todo.erase(it);
	}
    }

  for (std::vector<MutatableImageComputer*>::iterator it=_computers.begin();it!=_computers.end();it++)
    {      
	(*it)->abort_for(disp);
    }

  for (std::multiset<MutatableImageComputerTask*,CompareTaskPriorityHiResFirst>::iterator it=_done.begin();it!=_done.end();it++)
    {
      if ((*it)->display()==disp)
	{
	  (*it)->abort();
	  delete (*it);
	  _done.erase(it);
	}
    }
  
  _mutex.unlock();  
}

std::ostream& MutatableImageComputerFarm::write_info(std::ostream& out) const
{
  uint active_computers=0;

  for (std::vector<MutatableImageComputer*>::const_iterator it=_computers.begin();it!=_computers.end();it++)
    {      
      if ((*it)->active())
	{
	  active_computers++;
	}
    }

  out << "[" << _todo.size() << "/" << active_computers << "/" << _done.size() << "]";
  return out;
}

const uint MutatableImageComputerFarm::tasks() const
{
  uint ret=0;
  
  for (std::vector<MutatableImageComputer*>::const_iterator it=_computers.begin();it!=_computers.end();it++)
    {      
      if ((*it)->active())
	{
	  ret++;
	}
    }

  _mutex.lock();
  ret+=_todo.size();
  ret+=_done.size();
  _mutex.unlock();

  return ret;
}
