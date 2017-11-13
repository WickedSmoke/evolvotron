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

/*! \file 
  \brief Interfaces for class FunctionRegistry.
*/

#ifndef _function_registry_h_
#define _function_registry_h_

#include "common.h"

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
  const FunctionRegistration* lookup(const std::string& f) const;

  //! typedefed for convenience
  typedef boost::ptr_map<std::string,FunctionRegistration> Registrations;
  
  //! Just get the collection of registrations.
  const Registrations& registrations() const
    {
      return _registry_by_name;
    }

  //! Dump list of registered functions
  std::ostream& status(std::ostream& out) const;

  //! Register a function.  Handles duplicates gracefully. 
  void register_function(const FunctionRegistration& r);

 protected:
  //! Dictionary from names to Registration objects
  Registrations _registry_by_name;
};

#endif
