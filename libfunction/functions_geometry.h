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

#ifndef _functions_geometry_h_
#define _functions_geometry_h_

#include "common.h" 

#include "function_boilerplate.h"

//------------------------------------------------------------------------------------------

FUNCTION_BEGIN(FunctionCross,0,2,false,0)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      const XYZ v0(arg(0)(p));
      const XYZ v1(arg(1)(p));
      return v0*v1;
    }
  
FUNCTION_END(FunctionCross)

//------------------------------------------------------------------------------------------

//! Invert the leaf function using a radius-one origin centred sphere.
FUNCTION_BEGIN(FunctionGeometricInversion,0,1,false,0)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      const real radius2=p.magnitude2();
      const XYZ ip(p/radius2);

      return arg(0)(ip);
    }
  
FUNCTION_END(FunctionGeometricInversion)

//------------------------------------------------------------------------------------------

//! Implements reflection of sampling point about a plane
FUNCTION_BEGIN(FunctionReflect,0,3,false,0)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      const XYZ pt_in_plane(arg(0)(p));
      const XYZ normal(arg(1)(p).normalised());
      
      XYZ pos(arg(2)(p));
      
      const real distance_from_plane=(pos-pt_in_plane)%normal;
      
      // If pos is on the wrong side of the plane, reflect it over
      // Check: normal (0,0,1), pos (0,0,-1) => distance -1, pos-=(2*-1)*(0,0,1) => pos-=(0,0,-2)
      if (distance_from_plane<0.0)
	{
	  pos-=(2.0*distance_from_plane)*normal;
	}
      
      return pos;
    }
  
FUNCTION_END(FunctionReflect)

//------------------------------------------------------------------------------------------

#endif
