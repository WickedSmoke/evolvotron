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
  \brief Implementation of platform specific functions.
*/

#include <QThread>

#ifdef __unix__
#include <sys/resource.h>    // for getpriority/setprioirty
#endif

uint get_number_of_processors()
{
    return QThread::idealThreadCount();
}

void add_thread_niceness(uint n)
{
#ifdef __unix__
  /*! \todo: People porting to non-Linux (BSD, MacOS, Fink etc) please send
    a suitable #ifdef-able patch if you need something different here.
    Note that this code relies on Linux NPTL's non-Posix-compliant
    thread-specific nice value (although without a suitable replacement
    per-thread priority mechanism it's just as well it's that way).
    \todo: Should check some error codes, but it's probably pretty harmless if it doesn't work.
  */
  const int current_priority=getpriority(PRIO_PROCESS,0);
  setpriority(PRIO_PROCESS,0,std::min(19u,current_priority+n));
#else
#warning "No platform-specific implementation of add_thread_niceness available"
#endif
}
