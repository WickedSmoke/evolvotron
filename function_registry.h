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
  \brief Interfaces for class FunctionRegistry.
*/

#ifndef _function_registry_h_
#define _function_registry_h_

#include <string>
#include <map>
#include <deque>
#include <iostream>
#include "useful.h"
#include "function_registration.h"

//! Macro to force instantiation of static registration members, and register them with Registry.
#define REGISTER(F) static const FunctionRegistration* force_ ## F = FunctionRegistry::pre_reg(#F,&FunctionNodeUsing<F>::_registration)

//! Class acting as a dictionary from function name to registration info.
/*! Singleton pattern.  Instance is obtained using get() method.
 */
class FunctionRegistry
{
 public:
  
  //! Return the singleton instance of the FunctionRegistry
  static FunctionRegistry& get();

  //! Dump list of registered functions
  std::ostream& status(std::ostream& out) const;

  //! Probably not used as everything should be pre_registered by static initialisers
  FunctionRegistration*const reg(const char* n,FunctionRegistration* r);

  //! Register the function type (but change it's name first because the default one is probably obtained from typeid and seems to be slightly mangled).
  static const FunctionRegistration*const pre_reg(const char* n,FunctionRegistration* r);

 protected:
  //! Dictionary from names to Registration objects
  std::map<std::string,const FunctionRegistration*> _registry;

  //! Singleton instance, created on first call to static FunctionRegistry::get() method
  static FunctionRegistry* _instance;

  //! Queue of registrations awaiting registration on creation of the singleton instance.
  /*! This exists because we want to use std::string in the registration std::map,
    but std::string seems to have real problems when used dusing static initialisation.
  */
  static std::deque<FunctionRegistration*> _preregister;
};

#endif
