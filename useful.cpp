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

#include "useful.h"

#include <iostream>

/*! \file
  \brief Code for useful little helper functions..
*/

void fatal_error(const char* msg)
{
  std::cerr
    << "\n*** Fatal error: "
    << msg
    << " ***\n";
  exit(1);
}

void fatal_internal_error(const char* src_file,uint src_line)
{
  std::cerr 
    << "\n*** Fatal internal error in "
    << src_file
    << " at line "
    << src_line
    << " ***\n";
  exit(1);
}

void constraint_violation(const char* test,const char* src_file,uint src_line)
{
  std::cerr 
    << "\n*** Constraint \""
    << test
    << "\" violated in file"
    << src_file
    << " at line "
    << src_line
    << " ***\n";
  exit(1);
}
