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
#define REGISTER(FN) static const FunctionRegistration*const force_ ## FN = FunctionRegistry::get()->reg(#FN,FN::get_registration())

//! Class acting as a dictionary from function name to registration info.
/*! Singleton pattern.  Instance is obtained using get() method (NB workround for static initializer fiasco)
 */
class FunctionRegistry
{
 public:
  //! Return the singleton instance of the FunctionRegistry
  static FunctionRegistry*const get();

  //! Return the registration for the function named (returns 0 if unknown)
  const FunctionRegistration*const lookup(const std::string& f) const;
  
  //! Dump list of registered functions
  std::ostream& status(std::ostream& out) const;

  //! Probably not used as everything should be pre_registered by static initialisers
  FunctionRegistration*const reg(const char* n,FunctionRegistration* r);

 protected:
  //! Dictionary from names to Registration objects
  std::map<std::string,const FunctionRegistration*> _registry;
};

#endif
