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
  \brief Interfaces and implementation for certain core Function classes.
  Split out from functions.h as included in mutation_parameters.cpp
*/

#ifndef _function_core_h_
#define _function_core_h_

#include "useful.h"
#include "function_boilerplate.h"

//------------------------------------------------------------------------------------------

//! Function class representing a constant value.
FUNCTION_BEGIN(FunctionConstant,3,0,false,FnCore)
  
  //! Returns the constant value
  virtual const XYZ evaluate(const XYZ&) const
    {
      return XYZ(param(0),param(1),param(2));
    }

  //! Returns true, obviously.
  virtual const bool is_constant() const
    {
      return true;
    }

FUNCTION_END(FunctionConstant)

//------------------------------------------------------------------------------------------

//! Function class simply returning the position argument.
FUNCTION_BEGIN(FunctionIdentity,0,0,false,FnCore)

  //! Simply return the position argument.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      return p;
    }

  //! Is definitely not constant.
  virtual const bool is_constant() const
    {
      return false;
    }

FUNCTION_END(FunctionIdentity)

//------------------------------------------------------------------------------------------

//! Function class returning position transfomed by a 12-component linear transform.
FUNCTION_BEGIN(FunctionTransform,12,0,false,FnCore)

  //! Return the transformed position argument.
  virtual const XYZ evaluate(const XYZ& p) const
  {
    const Transform transform(params());
    return transform.transformed(p);
  }

  //! Is definitely not constant.
  virtual const bool is_constant() const
    {
      return false;
    }

FUNCTION_END(FunctionTransform)

//------------------------------------------------------------------------------------------


#endif
