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
#include <iostream>
#include "useful.h"

class FunctionNode;
class MutationParameters;

//! Define FunctionNodeStubNewFnPtr to be type of a pointer to 
typedef FunctionNode*const (*FunctionNodeStubNewFnPtr)(const MutationParameters&);

//! Class for meta information about functions.
class FunctionRegistration
{
 public:
  
  //! Constructor.
  FunctionRegistration(const std::string& n,FunctionNodeStubNewFnPtr s)
    :_name(n)
    ,_stubnew(s)
    {}

  //! Constructor (no name version)
  FunctionRegistration(FunctionNodeStubNewFnPtr s)
    :_name()
    ,_stubnew(s)
    {}

  //! Accessor.
  const std::string& name() const
    {
      return _name;
    }

  //! Accessor.
  void name(const std::string& name)
    {
      _name=name;
    }

 protected:

  //! Name of the function.
  std::string _name;

  //! The function's FunctionNodeUsing's stubnew function.
  FunctionNodeStubNewFnPtr _stubnew;
};

class FunctionRegistry
{
 public:
  FunctionRegistry*const get()
    {
      if (!_instance)
	_instance=new FunctionRegistry();
      return _instance;
    }
  
  //! Register the function type (but change it's name first because the default one is obtained from typeid and seems to be slightly mangled).
  /*! \todo There is a BIG problem here: any attempt to use std::string in static initialiser code seems to bomb.
    Workround using Less->strcmp and use char* instead.
   */
  static const FunctionRegistration*const add(const char* n,FunctionRegistration* reg)
  {
    //reg->name(n);
    std::clog << "Registry : registered " << n << "\n";
    //_registry[reg->name()]=reg;
    return reg;
  }

 protected:
  std::map<std::string,const FunctionRegistration*> _registry;
  static FunctionRegistry* _instance;
};

#endif
