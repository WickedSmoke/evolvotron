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
  \brief Interfaces for class FunctionRegistration
*/

#ifndef _function_registration_h_
#define _function_registration_h_

#include "useful.h"

#include <string>

class FunctionNode;
class MutationParameters;
class FunctionNodeInfo;

//! Define FunctionNodeStubNewFnPtr for convenience.
typedef FunctionNode*const (*FunctionNodeStubNewFnPtr)(const MutationParameters&,bool);
typedef FunctionNode*const (*FunctionNodeCreateFnPtr)(const FunctionNodeInfo*,std::string&);

//! Class for meta information about functions.
/*! We use char*'s for the name 'cos they aren't dynamic so might as well use the strings from the object code.
  (Also it avoids headaches with std::string in static initialisers).
 */
class FunctionRegistration
{
 public:
  
  //! Constructor.
  FunctionRegistration(const char* n,FunctionNodeStubNewFnPtr fs,FunctionNodeCreateFnPtr fc,uint np,uint na,bool i,uint fnc)
    :_name(n)
    ,_stubnew_fn(fs)
    ,_create_fn(fc)
    ,_params(np)
    ,_args(na)
    ,_iterative(i)
    ,_classification(fnc)
    {}

  //! Constructor (no name version)
  FunctionRegistration(FunctionNodeStubNewFnPtr s,FunctionNodeCreateFnPtr fc,uint np,uint na,bool i,uint fnc)
    :_name(0)
    ,_stubnew_fn(s)
    ,_create_fn(fc)
    ,_params(np)
    ,_args(na)
    ,_iterative(i)
    ,_classification(fnc)
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

  //! Accessor.
  const FunctionNodeStubNewFnPtr stubnew_fn() const
    {
      return _stubnew_fn;
    }

  //! Accessor.
  const FunctionNodeCreateFnPtr create_fn() const
    {
      return _create_fn;
    }

  //! Accessor.
  const uint params() const
    {
      return _params;
    }

  //! Accessor.
  const uint args() const
    {
      return _args;
    }

  //! Accessor.
  const bool iterative() const
    {
      return _iterative;
    }

  //! Accessor.
  const uint classification() const
    {
      return _classification;
    }

 protected:

  //! Name of the function.
  const char* _name;

  //! The FunctionNodeUsing's stubnew function.
  FunctionNodeStubNewFnPtr _stubnew_fn;

  //! The FunctionNodeUsing's create function.
  FunctionNodeCreateFnPtr _create_fn;

  //! Number of parameters
  uint _params;

  //! Number of arguments
  uint _args;

  //! Whether iterative
  bool _iterative;

  //! Classification bits
  uint _classification;
};

#endif
