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
  \brief Interfaces for class FunctionRegistry and associated classes.
*/

#ifndef _function_registry_h_
#define _function_registry_h_

#include <string>
#include <map>
#include <deque>
#include <iostream>
#include "useful.h"

class FunctionNode;
class MutationParameters;

//! Macro to force instantiation of static registration members, and register them with Registry.
#define REGISTER(F) static const FunctionRegistration* force_ ## F = FunctionRegistry::pre_reg(#F,&FunctionNodeUsing<F>::registration)



//! Define FunctionNodeStubNewFnPtr to be type of a pointer to 
typedef FunctionNode*const (*FunctionNodeStubNewFnPtr)(const MutationParameters&);

//! Class for meta information about functions.
class FunctionRegistration
{
 public:
  
  //! Constructor.
  FunctionRegistration(const char* n,FunctionNodeStubNewFnPtr s)
    :_name(n)
    ,_stubnew(s)
    {}

  //! Constructor (no name version)
  FunctionRegistration(FunctionNodeStubNewFnPtr s)
    :_name(0)
    ,_stubnew(s)
    {}

  //! Accessor.
  const char*const name() const
    {
       return _name;
    }

  //! Accessor.
  void name(const char* name)
    {
      _name=name;
    }

 protected:

  //! Name of the function.
  const char* _name;

  //! The function's FunctionNodeUsing's stubnew function.
  FunctionNodeStubNewFnPtr _stubnew;
};

class FunctionRegistry
{
 public:
  static FunctionRegistry& get()
    {
      if (!_instance)
	_instance=new FunctionRegistry();
      
      while (!_preregister.empty())
	{
	  _instance->_registry[_preregister.front()->name()]=_preregister.front();
	  _preregister.pop_front();
	}

      return *_instance;
    }

  std::ostream& status(std::ostream& out) const
    {
      out << "Registered functions:\n";
      for (std::map<std::string,const FunctionRegistration*>::const_iterator it=_registry.begin();it!=_registry.end();it++)
	{
	  out << "  " << (*it).first << "\n";
	}
      return out;
    }

  //! Probably not used as everything should be pre_registered by static initialisers
  FunctionRegistration* reg(const char* n,FunctionRegistration* r)
    {
      r->name(n);
      _registry[n]=r;
      return r;
    }

  //! Register the function type (but change it's name first because the default one is obtained from typeid and seems to be slightly mangled).
  static const FunctionRegistration*const pre_reg(const char* n,FunctionRegistration* r)
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

 protected:
      std::map<std::string,const FunctionRegistration*> _registry;

  static FunctionRegistry* _instance;

  static std::deque<const FunctionRegistration*> _preregister;
};

#endif
