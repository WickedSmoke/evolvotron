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
  \brief Implementation for class FunctionRegistry and associated classes.
*/

#include "function_registry.h"

//! Return the registration for the function named (returns 0 if unknown)
const FunctionRegistration*const FunctionRegistry::lookup(const std::string& f) const
{
  std::map<std::string,const FunctionRegistration*>::const_iterator it=_registry_by_name.find(f);
  if (it==_registry_by_name.end())
    return 0;
  else
    return (*it).second;
}

const FunctionRegistration*const FunctionRegistry::lookup(uint n) const
{
  return _registry_by_series[n];
}

const FunctionRegistration*const FunctionRegistry::lookup(float f) const
{
  return lookup(static_cast<uint>(f*_registry_by_series.size()));
}

FunctionRegistry*const FunctionRegistry::get()
{
  //! Can't declare this as a static class member because of C++ static initializer fiasco.
  static FunctionRegistry _instance;
  return &_instance;
}

std::ostream& FunctionRegistry::status(std::ostream& out) const
{
  out << "Registered functions:\n";
  for (std::map<std::string,const FunctionRegistration*>::const_iterator it=_registry_by_name.begin();it!=_registry_by_name.end();it++)
    {
      out << "  " << (*it).first << "\n";
    }
  return out;
}

FunctionRegistration*const FunctionRegistry::reg(const char* n,FunctionRegistration* r)
{
  r->name(n);
  _registry_by_name[n]=r;
  _registry_by_series.push_back(r);
  return r;
}




