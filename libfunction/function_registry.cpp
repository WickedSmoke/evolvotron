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
    for (iterator it = begin(); it != end(); it++)
        delete it->second;
    std::clog << "FunctionRegistry destroyed\n";
}

//! Return the registration for the function named (returns 0 if unknown)
const FunctionRegistration* FunctionRegistry::lookup(const std::string& f) const
{
    const_iterator it = find(f);
    if (it == end())
        return 0;
    else
        return it->second;
}

std::ostream& FunctionRegistry::status(std::ostream& out) const
{
    out << "Registered functions:\n";
    for (const_iterator it = begin(); it != end(); it++)
        out << "  " << it->first << "\n";
    return out;
}

bool FunctionRegistry::register_function(FunctionRegistration* r)
{
    if (find(r->name) == end())
    {
        (*this)[r->name] = r;
        return true;
    }
    return false;
}
