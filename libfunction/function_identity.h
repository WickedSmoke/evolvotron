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
  \brief Interfaces and implementation for certain core Function classes.
  Split out from functions.h as included in mutation_parameters.cpp
*/

#ifndef _function_identity_h_
#define _function_identity_h_

#include "common.h"

#include "function_boilerplate.h"

//------------------------------------------------------------------------------------------

//! Function class simply returning the position argument.
FUNCTION_BEGIN(FunctionIdentity,0,0,false,FnCore)

  //! Simply return the position argument.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      return p;
    }

FUNCTION_END(FunctionIdentity)

//------------------------------------------------------------------------------------------

#endif
