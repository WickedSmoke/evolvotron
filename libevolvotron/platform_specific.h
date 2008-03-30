// Source file for evolvotron
// Copyright (C) 2008 Tim Day
/*! \page License License

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
  \brief Try to isolate platform specific code here (or rather, in the .cpp implementations)
*/

#ifndef _platform_specific_h_
#define _platform_specific_h_

//! Return the number of processors on the system
extern const uint get_number_of_processors();

//! Lower the priority of the calling thread by increasing its "niceness" (unix 0-19 'nice' scale used)
extern void add_thread_niceness(uint);

#endif
