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
  \brief Interfaces and implementation for specific Function classes.
  As much as possible of the implementation should be pushed into the FunctionBoilerplate template.
*/

#ifndef _functions_magnitude_h_
#define _functions_magnitude_h_

#include "common.h" 

#include "function_boilerplate.h"

//------------------------------------------------------------------------------------------

//! Function returns a value comprising the magnitude of three leaf functions.
FUNCTION_BEGIN(FunctionMagnitudes,0,3,false,0)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      return XYZ(
		 arg(0)(p).magnitude(),
		 arg(1)(p).magnitude(),
		 arg(2)(p).magnitude()
		 );
    }
  
FUNCTION_END(FunctionMagnitudes)

//------------------------------------------------------------------------------------------

//! Function returns leaf function value magnitude scaled by position magnitude.
FUNCTION_BEGIN(FunctionMagnitude,3,1,false,0)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      return arg(0)(p).magnitude()*XYZ(param(0),param(1),param(2));
    }
  
FUNCTION_END(FunctionMagnitude)

//------------------------------------------------------------------------------------------

#endif
