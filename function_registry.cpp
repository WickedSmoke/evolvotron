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
  \brief Implementation for class FunctionRegistry and associated classes.
*/

#include "function_registry.h"


/*! If there are any entries queued up, register them now.
  This is to avoid any headaches with std::string during static initialisation.
*/
FunctionRegistry& FunctionRegistry::get()
{
  if (!_instance)
    _instance=new FunctionRegistry();
  
  while (!_preregister.empty())
    {
      FunctionRegistration* r=_preregister.front();
      _instance->_registry[r->name()]=r;
      _preregister.pop_front();
    }
  
  return *_instance;
}

std::ostream& FunctionRegistry::status(std::ostream& out) const
{
  out << "Registered functions:\n";
  for (std::map<std::string,const FunctionRegistration*>::const_iterator it=_registry.begin();it!=_registry.end();it++)
    {
      out << "  " << (*it).first << "\n";
    }
  return out;
}

FunctionRegistration*const FunctionRegistry::reg(const char* n,FunctionRegistration* r)
{
  r->name(n);
  _registry[n]=r;
  return r;
}

const FunctionRegistration*const FunctionRegistry::pre_reg(const char* n,FunctionRegistration* r)
{
  if (_instance)
    {
      get().reg(n,r);
    }
  else
    {
      r->name(n);
      _preregister.push_back(r);
    }
  return r;
}

FunctionRegistry* FunctionRegistry::_instance=0;

std::deque<FunctionRegistration*> FunctionRegistry::_preregister;

