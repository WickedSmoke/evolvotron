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
  \brief Implementation for class FunctionRegistry and associated classes.
*/



#include "function_registry.h"

#include "register_all_functions.h"

FunctionRegistry::FunctionRegistry()
{
  register_all_functions(*this);
  std::clog << "FunctionRegistry created\n";
}

FunctionRegistry::~FunctionRegistry()
{
  _registry_by_name.clear();
  std::clog << "FunctionRegistry destroyed\n";
}

//! Return the registration for the function named (returns 0 if unknown)
const FunctionRegistration* FunctionRegistry::lookup(const std::string& f) const
{
  Registrations::const_iterator it=_registry_by_name.find(f);
  if (it==_registry_by_name.end())
    return 0;
  else
    {
#if BOOST_VERSION >= 103400
      return it->second;
#else
      return &*it;
#endif
    }
}

std::ostream& FunctionRegistry::status(std::ostream& out) const
{
  out << "Registered functions:\n";
  for (Registrations::const_iterator it=_registry_by_name.begin();it!=_registry_by_name.end();it++)
    {
      out 
	<< "  " 
#if BOOST_VERSION >= 103400
	<< it->first
#else
	<< it.key() 
#endif
	<< "\n";
    }
  return out;
}

void FunctionRegistry::register_function(const FunctionRegistration& r)
{
  if (_registry_by_name.find(r.name())==_registry_by_name.end())
    {
      _registry_by_name[r.name()]=r;
    }
}
