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
  \brief Interfaces for class FunctionRegistration
*/

#ifndef _function_registration_h_
#define _function_registration_h_

class FunctionNode;
class MutationParameters;

//! Define FunctionNodeStubNewFnPtr for convenience.
typedef FunctionNode*const (*FunctionNodeStubNewFnPtr)(const MutationParameters&);

//! Class for meta information about functions.
/*! We use char*'s for the name 'cos they aren't dynamic so might as well use the strings from the object code.
  (Also it avoids headaches with std::string in static initialisers).
 */
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

#endif
