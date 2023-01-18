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

#include "useful.h"
#include "function_registration.h"

//! Class acting as a dictionary from function name to registration info.
/*! Intended to be used as singleton; get() obtains instance.
  This holds the "definitive" collection of registrations.  FunctionRegistrations can be compared using pointer identiy.
 */
struct FunctionRegistry : public std::map<std::string,FunctionRegistration*>
{
    FunctionRegistry();
    ~FunctionRegistry();

    //! Return the registration for the function named (returns 0 if unknown)
    const FunctionRegistration* lookup(const std::string& f) const;

    //! Dump list of registered functions
    std::ostream& status(std::ostream& out) const;

    //! Register a function.  Handles duplicates gracefully.
    bool register_function(FunctionRegistration* r);
};

#endif
