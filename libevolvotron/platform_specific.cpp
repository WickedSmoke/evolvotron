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

#include "platform_specific.h"

#include <boost/optional.hpp>

#ifdef PLATFORM_LINUX
#include <sched.h>           // for CPU count
#include <sys/resource.h>    // for getpriority/setprioirty
#endif

#ifdef PLATFORM_BSD
#include <sys/param.h>       // for CPU count
#include <sys/sysctl.h>      // for CPU count
#include <sys/resource.h>    // for getpriority/setprioirty
#endif

uint get_number_of_processors()
{
  boost::optional<uint> num_processors;

#ifdef PLATFORM_LINUX
 {
   cpu_set_t cpus;
   if (sched_getaffinity(0,sizeof(cpu_set_t),&cpus)==0)
     {
       uint bits=0;
       for (int i=0;i<CPU_SETSIZE;i++)
	 {
	   if (CPU_ISSET(i,&cpus)) bits++;
	 }
       num_processors=bits;
     }
 }
#endif

#ifdef PLATFORM_BSD
 {
   // Code contributed by Linc Davis
   int count;
   size_t size=sizeof(count);
   if (!sysctlbyname("hw.ncpu",&count,&size,NULL,0))
     num_processors=count;
 }
#endif

#if !defined(PLATFORM_LINUX) && !defined(PLATFORM_BSD)
#warning "No platform-specific implementation of get_number_of_processors selected"
#endif

 if (!num_processors)
   {
     // If we can't find out... well, 2 is not uncommon these days.
     num_processors=2;
     std::cerr
       << "Could not determine number of CPUs; guessing "
       << num_processors.get()
       << "\n";
     //! todo For real fun, test the system and see what it scales to.
   }

 return num_processors.get();
}

void add_thread_niceness(uint n)
{
#if defined(PLATFORM_LINUX) || defined(PLATFORM_BSD)
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
