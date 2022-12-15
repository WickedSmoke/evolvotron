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

//! Holds meta information about functions.
struct FunctionRegistration
{
  //! Constructor.
  FunctionRegistration(const std::string& n,FunctionNodeStubNewFnPtr fs,FunctionNodeCreateFnPtr fc,uint np,uint na,bool i,uint fnc)
    : name(n)
    ,stubnew_fn(fs)
    ,create_fn(fc)
    ,params(np)
    ,args(na)
    ,iterative(i)
    ,classification(fnc)
    {}

  //! Name of the function.
  std::string name;

  //! The FunctionNodeUsing's stubnew function.
  FunctionNodeStubNewFnPtr stubnew_fn;

  //! The FunctionNodeUsing's create function.
  FunctionNodeCreateFnPtr create_fn;

  //! Number of parameters
  uint params;

  //! Number of arguments
  uint args;

  //! Whether iterative
  bool iterative;

  //! Classification bits
  uint classification;
};

#endif
