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

#ifndef _functions_spherical_h_
#define _functions_spherical_h_

#include "common.h" 

#include "function_boilerplate.h"

//------------------------------------------------------

//! Transforms cartesian coordinates to spherical
FUNCTION_BEGIN(FunctionCartesianToSpherical,0,0,false,0)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
  {
    const real r=p.magnitude();
    
    // Angles are normalised (-1 to +1) over their usual possible range.
    const real theta=atan2(p.y(),p.x())*(1.0/M_PI);
    const real phi=(r== 0.0 ? 0.0 : asin(p.z()/r)*(1.0/(0.5*M_PI)));
  
    return XYZ(r,theta,phi);
  }

FUNCTION_END(FunctionCartesianToSpherical)

//------------------------------------------------------------------------------------------

//! Transforms spherical coordinates to cartesian 
FUNCTION_BEGIN(FunctionSphericalToCartesian,0,0,false,0)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
  {
    const real r=p.x();
    const real theta=M_PI*p.y();
    const real phi=0.5*M_PI*p.z();
    
    const real x=r*cos(theta)*sin(phi);
    const real y=r*sin(theta)*sin(phi);
    const real z=r*cos(phi);
    
    return XYZ(x,y,z);
  }

FUNCTION_END(FunctionSphericalToCartesian)

//------------------------------------------------------------------------------------------

// Converts the position argument to spherical coords, pass these through the leaf node, and convert the result back to cartesian.
FUNCTION_BEGIN(FunctionEvaluateInSpherical,0,1,false,0)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      const real in_r=p.magnitude();
      const real in_theta=atan2(p.y(),p.x())*(1.0/M_PI);
      const real in_phi=(in_r== 0.0 ? 0.0 : asin(p.z()/in_r)*(1.0/(0.5*M_PI)));
      
      const XYZ v(arg(0)(XYZ(in_r,in_theta,in_phi)));
      
      const real out_r=v.x();
      const real out_theta=M_PI*v.y();
      const real out_phi=0.5*M_PI*v.z();
      
      const real x=out_r*cos(out_theta)*sin(out_phi);
      const real y=out_r*sin(out_theta)*sin(out_phi);
      const real z=out_r*cos(out_phi);
      
      return XYZ(x,y,z);
    }
  
FUNCTION_END(FunctionEvaluateInSpherical)

//------------------------------------------------------------------------------------------

#endif
