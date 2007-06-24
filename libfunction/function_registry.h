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
#include <iostream>

#include "useful.h"

#include "function_registration.h"

//! Class acting as a dictionary from function name to registration info.
/*! Intended to be used as singleton; get() obtains instance.
  This holds the "definitive" collection of registrations.  FunctionRegistrations can be compared using pointer identiy.
 */
class FunctionRegistry
{
 public:
  //! Constuctor public to Singleton boilerplate can new it.
  /*! \todo Figure out how to make this protected/private.
   */
  FunctionRegistry();
  ~FunctionRegistry();

  //! Return the registration for the function named (returns 0 if unknown)
  const FunctionRegistration*const lookup(const std::string& f) const;

  //! typedefed for convenience
  typedef boost::ptr_map<std::string,const FunctionRegistration*> Registrations;
  
  //! Just get the collection of registrations.
  const Registrations& registrations() const
    {
      return _registry_by_name;
    }

  //! Dump list of registered functions
  std::ostream& status(std::ostream& out) const;

  //! Register a function.  Handle duplicates gracefully. 
  /*! A copy is taken, but we fill in the name field of the supplied registration, which will be empty.*/
  void name_and_register(const char* n,FunctionRegistration& r);

 protected:
  //! Dictionary from names to Registration objects
  Registrations _registry_by_name;
};

#endif