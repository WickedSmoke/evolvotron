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

#include "libfunction_precompiled.h"

#include "useful.h"

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

std::ofstream sink_ostream("/dev/null");

#ifndef NDEBUG

InstanceCounted::InstanceCounted(const std::string& what,bool verbose)
:_what(what)
,_verbose(verbose)
{
  if (!_instance_counts.get()) _instance_counts=std::auto_ptr<std::map<std::string,uint> >(new std::map<std::string,uint>());
  (*_instance_counts)[_what]++;
  if (_verbose) std::clog << "[+" << _what << ":" << (*_instance_counts)[_what] << "]";
}

InstanceCounted::~InstanceCounted()
{
  assert(_instance_counts.get());
  assert((*_instance_counts)[_what]>0);
  (*_instance_counts)[_what]--;
  if (_verbose) std::clog << "[-" << _what << ":" << (*_instance_counts)[_what] << "]" << ( (*_instance_counts)[_what]==0 ? "\n" : "");
}

bool InstanceCounted::is_clear()
{
  bool ok=true;
  for (std::map<std::string,uint>::const_iterator it=(*_instance_counts).begin();it!=(*_instance_counts).end();it++)
    {
      if ((*it).second>0) {
	std::clog << "InstanceCounted::is_clear() violated by " << (*it).first << " (" << (*it).second << " instances)\n";
	ok=false;
      }
    }
  if (ok) std::clog << "InstanceCounted::is_clear() - no remaining instances\n";	
  return ok;
}

std::auto_ptr<std::map<std::string,uint> > InstanceCounted::_instance_counts;

#endif
