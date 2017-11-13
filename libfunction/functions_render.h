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

#ifndef _functions_render_h_
#define _functions_render_h_

#include "common.h" 

#include "function_boilerplate.h"

//------------------------------------------------------------------------------------------

//! Rays intersecting a textured unit sphere
/*! arg(0) is background
    arg(1) is 3D texture for sphere
  param(0,1,2) is light source direction
  p.x, p.y is the 2D position of a ray from infinity travelling in direction (0 0 1)
*/
FUNCTION_BEGIN(FunctionOrthoSphereShaded,3,2,false,FnRender)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      const real pr2=p.x()*p.x()+p.y()*p.y();
      if (pr2<1.0)
	{
	  const real z=-sqrt(1.0-pr2);
	  const XYZ n(p.x(),p.y(),z);

	  const XYZ lu(param(0),param(1),param(2));
	  const XYZ l(lu.normalised());

	  const real i=0.5*(1.0+l%n); // In range 0-1
	  return i*arg(1)(n);
	}
      else
	{
	  return arg(0)(p);
	}
    }
  
FUNCTION_END(FunctionOrthoSphereShaded)

//------------------------------------------------------------------------------------------

//! Rays intersecting a textured unit sphere, with bump mapping
/*! arg(0) is background
    arg(1) is 3D texture for sphere
    arg(2) is bump-map for sphere (take magnitude2)
  param(0,1,2) is light source direction
  p.x, p.y is the 2D position of a ray from infinity travelling in direction (0 0 1)
*/
FUNCTION_BEGIN(FunctionOrthoSphereShadedBumpMapped,3,3,false,FnRender)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      const real pr2=p.x()*p.x()+p.y()*p.y();
      if (pr2<1.0)
	{
	  const real z=-sqrt(1.0-pr2);
	  const XYZ n(p.x(),p.y(),z);

	  // Tangent vectors
	  const XYZ east((XYZ(0.0,1.0,0.0)*n).normalised());
	  const XYZ north(n*east);

	  const real e0=(arg(2)(n-epsilon()*east)).magnitude2();
	  const real e1=(arg(2)(n+epsilon()*east)).magnitude2();
	  const real n0=(arg(2)(n-epsilon()*north)).magnitude2();
	  const real n1=(arg(2)(n+epsilon()*north)).magnitude2();

	  const real de=(e1-e0)*inv_epsilon2();
	  const real dn=(n1-n0)*inv_epsilon2();

	  const XYZ perturbed_n((n-east*de-north*dn).normalised());

	  const XYZ lu(param(0),param(1),param(2));
	  const XYZ l(lu.normalised());

	  const real i=0.5*(1.0+l%perturbed_n); // In range 0-1
	  return i*arg(1)(n);
	}
      else
	{
	  return arg(0)(p);
	}
    }
  
FUNCTION_END(FunctionOrthoSphereShadedBumpMapped)


//------------------------------------------------------------------------------------------

//! Rays reflecting off a unit sphere
/*! arg(0) is background
    arg(1) sampled using a normalised vector defines an environment for reflected rays
  p.x, p.y is the 2D position of a ray from infinity travelling in direction (0 0 1)
*/
FUNCTION_BEGIN(FunctionOrthoSphereReflect,0,2,false,FnRender)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      const real pr2=p.x()*p.x()+p.y()*p.y();
      if (pr2<1.0)
	{
	  const real z=-sqrt(1.0-pr2);

	  // This is on surface of unit radius sphere - no need to normalise
	  XYZ n(p.x(),p.y(),z);

	  // The ray _towards_ the viewer v is (0 0 -1)
	  const XYZ v(0.0,0.0,-1.0);

	  // The reflected ray is (2n.v)n-v
	  const XYZ reflected((2.0*(n%v))*n-v);

	  return arg(1)(reflected);
	}
      else
	{
	  return arg(0)(p);
	}
    }
  
FUNCTION_END(FunctionOrthoSphereReflect)

//------------------------------------------------------------------------------------------

//! Rays reflecting off a bump mapped unit sphere
/*! arg(0) is background
    arg(1) sampled using a normalised vector defines an environment for reflected rays
    arg(2) is bump map
  p.x, p.y is the 2D position of a ray from infinity travelling in direction (0 0 1)
*/
FUNCTION_BEGIN(FunctionOrthoSphereReflectBumpMapped,0,3,false,FnRender)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      const real pr2=p.x()*p.x()+p.y()*p.y();
      if (pr2<1.0)
	{
	  const real z=-sqrt(1.0-pr2);

	  // This is on surface of unit radius sphere - no need to normalise
	  XYZ n(p.x(),p.y(),z);

	  // Tangent vectors
	  const XYZ east((XYZ(0.0,1.0,0.0)*n).normalised());
	  const XYZ north(n*east);

	  const real e0=(arg(2)(n-epsilon()*east)).magnitude2();
	  const real e1=(arg(2)(n+epsilon()*east)).magnitude2();
	  const real n0=(arg(2)(n-epsilon()*north)).magnitude2();
	  const real n1=(arg(2)(n+epsilon()*north)).magnitude2();

	  const real de=(e1-e0)*inv_epsilon2();
	  const real dn=(n1-n0)*inv_epsilon2();

	  const XYZ perturbed_n((n-east*de-north*dn).normalised());

	  // The ray _towards_ the viewer is (0 0 -1)
	  const XYZ v(0.0,0.0,-1.0);

	  // The reflected ray is (2n.v)n-v
	  const XYZ reflected((2.0*(perturbed_n%v))*perturbed_n-v);

	  return arg(1)(reflected);
	}
      else
	{
	  return arg(0)(p);
	}
    }
  
FUNCTION_END(FunctionOrthoSphereReflectBumpMapped)

//------------------------------------------------------------------------------------------

#endif
