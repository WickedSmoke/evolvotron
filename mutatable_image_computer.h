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
class MutatableImageComputer : public QThread
{
 protected:
  //! Pointer to compute farm of which this thread is part.
  MutatableImageComputerFarm*const _farm;

  //! The current task.
  MutatableImageComputerTask* _task;

  //! Flag to indicate we should put our current task back on the todo queue and take another one.
  bool _defer;

  //! Flag to indicate we should abort the current compute.
  bool _abort;

  //! Flag to indicate the thread should shut down and exit.
  bool _kill;

  //! The actual compute code, launched by invoking start() in the constructor
  virtual void run();

 public:

  //! Constructor
  MutatableImageComputer(MutatableImageComputerFarm* frm);

  //! Destructor
  ~MutatableImageComputer();

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

  //! Defer the current task if it's priority is less important than specified.  Returns true if deferrment occurred.
  bool defer_if_less_important_than(uint pri);

  //! This method called by an external threads to shut down the current task
  void abort();

  //! This method called by an external threads to shut down the current task if it's for a particular display
  void abort_for(const MutatableImageDisplay* disp);

  //! This method called by external thread to kill the thread.
  void kill();
};

#endif
