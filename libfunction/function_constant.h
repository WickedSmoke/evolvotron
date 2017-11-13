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
  \brief Interfaces and implementation for specific Function class.
*/

#ifndef _function_constant_h_
#define _function_constant_h_

#include "common.h"

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
  /*! One of the few cases this method is overriden; most (all?) other no-argument functions should return false
   */
  virtual bool is_constant() const
    {
      return true;
    }

FUNCTION_END(FunctionConstant)

//------------------------------------------------------------------------------------------

#endif
