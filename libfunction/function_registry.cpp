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
const FunctionRegistration*const FunctionRegistry::lookup(const std::string& f) const
{
  Registrations::const_iterator it=_registry_by_name.find(f);
  if (it==_registry_by_name.end())
    return 0;
  else
    return *it;
}

std::ostream& FunctionRegistry::status(std::ostream& out) const
{
  out << "Registered functions:\n";
  for (Registrations::const_iterator it=_registry_by_name.begin();it!=_registry_by_name.end();it++)
    {
      out << "  " << it.key() << "\n";
    }
  return out;
}

void FunctionRegistry::name_and_register(const char* n,FunctionRegistration& r)
{
  r.name(n);

  const std::string ns(n);
  if (_registry_by_name.find(ns)==_registry_by_name.end())
    {
      _registry_by_name[ns]=new FunctionRegistration(n,r);
    }
}