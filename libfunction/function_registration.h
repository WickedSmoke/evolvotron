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
  \brief Interfaces for class FunctionRegistration
*/

#ifndef _function_registration_h_
#define _function_registration_h_

#include "common.h"

class FunctionNode;
class FunctionRegistry;
class MutationParameters;
class FunctionNodeInfo;

//! Enum for classification bits
enum
  {
    FnCore=1,           // Constant, Identity or Transform: the 3 zero-child diluting functions
    FnStructure=2,      // Functions which give rise to a lot of structure e.g spirals and grids
    FnRender=4,         // Functions which use rendering algorithms
    FnIterative=8,      // Iterative functions
    FnFractal=16,       // Fractal functions
    FnClassifications=5 // The number of function classifications defined.
  };

extern const char*const function_classification_name[FnClassifications];

//! Define FunctionNodeStubNewFnPtr for convenience.
typedef std::unique_ptr<FunctionNode> (*FunctionNodeStubNewFnPtr)(const MutationParameters&,bool);
typedef std::unique_ptr<FunctionNode> (*FunctionNodeCreateFnPtr)(const FunctionRegistry&,const FunctionNodeInfo&,std::string&);

//! Class for meta information about functions.
class FunctionRegistration
{
 public:
  
  //! Constructor.
  FunctionRegistration(const std::string& n,FunctionNodeStubNewFnPtr fs,FunctionNodeCreateFnPtr fc,uint np,uint na,bool i,uint fnc)
    :_name(n)
    ,_stubnew_fn(fs)
    ,_create_fn(fc)
    ,_params(np)
    ,_args(na)
    ,_iterative(i)
    ,_classification(fnc)
    {}

  //! Void constructor
  FunctionRegistration()
    :_name()
    ,_stubnew_fn(0)
    ,_create_fn(0)
    ,_params(0)
    ,_args(0)
    ,_iterative(false)
    ,_classification(0)
    {}
  
  //! Constructor (copy)
  FunctionRegistration(const FunctionRegistration& f)
    :_name(f._name)
    ,_stubnew_fn(f._stubnew_fn)
    ,_create_fn(f._create_fn)
    ,_params(f._params)
    ,_args(f._args)
    ,_iterative(f._iterative)
    ,_classification(f._classification)
    {}

  //! Accessor.
  const std::string& name() const
    {
       return _name;
    }

  //! Accessor.
  FunctionNodeStubNewFnPtr stubnew_fn() const
    {
      return _stubnew_fn;
    }

  //! Accessor.
  FunctionNodeCreateFnPtr create_fn() const
    {
      return _create_fn;
    }

  //! Accessor.
  uint params() const
    {
      return _params;
    }

  //! Accessor.
  uint args() const
    {
      return _args;
    }

  //! Accessor.
  bool iterative() const
    {
      return _iterative;
    }

  //! Accessor.
  uint classification() const
    {
      return _classification;
    }

 protected:

  //! Name of the function.
  std::string _name;

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
