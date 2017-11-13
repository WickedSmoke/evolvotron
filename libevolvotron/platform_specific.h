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
  \brief Try to isolate platform specific code here (or rather, in the .cpp implementations)
*/

#ifndef _platform_specific_h_
#define _platform_specific_h_

#include "../libfunction/useful.h"

// qt3 for Darwin appears to define this, so use it to definitely select the PLATFORM_BSD option
#ifdef __DARWIN_X11__
#define PLATFORM_BSD
#undef PLATFORM_LINUX
#endif

//! Return the number of processors on the system
extern uint get_number_of_processors();

//! Lower the priority of the calling thread by increasing its "niceness" (unix 0-19 'nice' scale used)
extern void add_thread_niceness(uint);

#endif
