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
  \brief Interfaces and implementation for specific Function classes.
  As much as possible of the implementation should be pushed into the FunctionBoilerplate template.
  \warning This file should ONLY be included in function_node.cpp, which instantiates everything.
  The two functions allowed to escape "into the wild" are FunctionPreTransform and FunctionPostTransform 
  which have their own header file.
*/

#ifndef _functions_h_
#define _functions_h_

#include <cmath>

#include "useful.h"
#include "function_boilerplate.h"

//! Sane modulus function always returning a number in the range [0,y)
inline float modulusf(float x,float y)
{
  if (y<0.0f) y=-y;
  float r=fmod(x,y);
  if (r<0.0f) r+=y;
  return r;
}

//! Sane modulus function always returning a number in the range [0,y-1]
inline uint modulusi(int x,int y)
{
  if (y<0) y=-y;
  int r=x%y;
  if (r<0) r+=y;
  assert(r>=0);
  return r;
}

//! Triangle function: like modulus, but ramps down instead of discontinuity at y
inline float trianglef(float x,float y)
{
  if (y<0.0f) y=-y;
  if (x<0.0f) x=-x;
  float r=fmod(x,2.0f*y);
  if (r>y) r=2.0f*y-r;
  return r;
}


//------------------------------------------------------------------------------------------

//! Function class representing a constant value.
FUNCTION_BEGIN(FunctionConstant,3,0,false)
  
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
FUNCTION_BEGIN(FunctionIdentity,0,0,false)

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

#include "function_null.h"

REGISTER(FunctionNull);

//------------------------------------------------------------------------------------------

//! Function class returning position transfomed by a 12-component linear transform.
FUNCTION_BEGIN(FunctionTransform,12,0,false)

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

//! Function class returning position transfomed by a 12-component linear transform.
FUNCTION_BEGIN(FunctionTransformGeneralised,0,4,false)

  //! Return the transformed position argument.
  virtual const XYZ evaluate(const XYZ& p) const
  {
    const Transform transform(arg(0)(p),arg(1)(p),arg(2)(p),arg(3)(p));
    return transform.transformed(p);
  }

  //! Is almost certainly not constant.
  virtual const bool is_constant() const
  {
    return false;
  }

FUNCTION_END(FunctionTransformGeneralised)

//------------------------------------------------------------------------------------------

#include "function_pre_transform.h"

REGISTER(FunctionPreTransform);

//------------------------------------------------------------------------------------------

//! Function class returning leaf node evaluated at position transfomed by a 12-component linear transform.
/*! Unlike FunctionPreTransform, the basis vectors for the transform are not fixed but determined from leaf functions
 */
FUNCTION_BEGIN(FunctionPreTransformGeneralised,0,5,false)

  //! Return the evaluation of arg(0) at the transformed position argument.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      const Transform transform(arg(1)(p),arg(2)(p),arg(3)(p),arg(4)(p));
      return arg(0)(transform.transformed(p));
    }

  //! Has the same const-ness as arg(0)
  virtual const bool is_constant() const
    {
      return arg(0).is_constant();
    }

FUNCTION_END(FunctionPreTransformGeneralised)

//------------------------------------------------------------------------------------------

#include "function_post_transform.h"

REGISTER(FunctionPostTransform);

//------------------------------------------------------------------------------------------

//! Function class returning leaf node evaluated at given position; result is then transfomed by a 12-component linear transform.
/*! Unlike FunctionPostTransform, the basis vectors for the transform are not fixed but determined from leaf functions
 */
FUNCTION_BEGIN(FunctionPostTransformGeneralised,0,5,false)

  //! Return the evaluation of arg(0) at the transformed position argument.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      const Transform transform(arg(1)(p),arg(2)(p),arg(3)(p),arg(4)(p));
      return transform.transformed(arg(0)(p));
    }

  //! Only constant if all the leaf functions are constant.
  virtual const bool is_constant() const
    {
      return (
	      arg(0).is_constant()
	      && arg(1).is_constant()
	      && arg(2).is_constant()
	      && arg(3).is_constant()
	      && arg(4).is_constant()
	      );
    }

FUNCTION_END(FunctionPostTransformGeneralised)

//------------------------------------------------------------------------------------------

//! Transforms position transformed by a 30 paramter quadratic transform.
FUNCTION_BEGIN(FunctionTransformQuadratic,30,0,false)

  //! Return p transformed.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      const XYZ translate(param( 0),param( 1),param( 2));
      const XYZ basis_x  (param( 3),param( 4),param( 5));
      const XYZ basis_y  (param( 6),param( 7),param( 8));
      const XYZ basis_z  (param( 9),param(10),param(11));
      const XYZ basis_xy (param(12),param(13),param(14));
      const XYZ basis_xz (param(15),param(16),param(17));
      const XYZ basis_yz (param(18),param(19),param(20));
      const XYZ basis_xx (param(21),param(22),param(23));
      const XYZ basis_yy (param(24),param(25),param(26));
      const XYZ basis_zz (param(27),param(28),param(29));

      return 
	translate
	+basis_x*p.x()+basis_y*p.y()+basis_z*p.z()
	+basis_xy*(p.x()*p.y())+basis_xz*(p.x()*p.z())+basis_yz*(p.y()*p.z())
	+basis_xx*(p.x()*p.x())+basis_yy*(p.y()*p.y())+basis_zz*(p.z()*p.z());
    }

  //! Unlikely to ever be constant (requires all parameters zero).
  virtual const bool is_constant() const
    {
      return false;
    }

FUNCTION_END(FunctionTransformQuadratic)

//------------------------------------------------------------------------------------------

//! Transforms cartesian coordinates to spherical
FUNCTION_BEGIN(FunctionCartesianToSpherical,0,0,false)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
  {
    const float r=p.magnitude();
    
    // Angles are normalised (-1 to +1) over their usual possible range.
    const float theta=atan2(p.y(),p.x())*(1.0f/M_PI);
    const float phi=(r== 0.0f ? 0.0f : asin(p.z()/r)*(1.0f/(0.5f*M_PI)));
  
    return XYZ(r,theta,phi);
  }

  //! Not constant.
  virtual const bool is_constant() const
    {
      return false;
    }

FUNCTION_END(FunctionCartesianToSpherical)

//------------------------------------------------------------------------------------------

//! Transforms spherical coordinates to cartesian 
FUNCTION_BEGIN(FunctionSphericalToCartesian,0,0,false)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
  {
    const float r=p.x();
    const float theta=M_PI*p.y();
    const float phi=0.5*M_PI*p.z();
    
    const float x=r*cos(theta)*sin(phi);
    const float y=r*sin(theta)*sin(phi);
    const float z=r*cos(phi);
    
    return XYZ(x,y,z);
  }

  //! Not constant.
  virtual const bool is_constant() const
  {
    return false;
  }

FUNCTION_END(FunctionSphericalToCartesian)

//------------------------------------------------------------------------------------------

// Converts the position argument to spherical coords, pass these through the leaf node, and convert the result back to cartesian.
FUNCTION_BEGIN(FunctionEvaluateInSpherical,0,1,false)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      const float in_r=p.magnitude();
      const float in_theta=atan2(p.y(),p.x())*(1.0f/M_PI);
      const float in_phi=(in_r== 0.0f ? 0.0f : asin(p.z()/in_r)*(1.0f/(0.5f*M_PI)));
      
      const XYZ v(arg(0)(XYZ(in_r,in_theta,in_phi)));
      
      const float out_r=v.x();
      const float out_theta=M_PI*v.y();
      const float out_phi=0.5*M_PI*v.z();
      
      const float x=out_r*cos(out_theta)*sin(out_phi);
      const float y=out_r*sin(out_theta)*sin(out_phi);
      const float z=out_r*cos(out_phi);
      
      return XYZ(x,y,z);
    }
  
  //! Is constant if leaf node is.
  virtual const bool is_constant() const
  {
    return arg(0).is_constant();
  }

FUNCTION_END(FunctionEvaluateInSpherical)

//------------------------------------------------------------------------------------------

FUNCTION_BEGIN(FunctionRotate,0,1,false)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      const XYZ a(arg(0)(p)*M_PI);
  
      Matrix33RotateX rx(a.x());
      Matrix33RotateY ry(a.y());
      Matrix33RotateZ rz(a.z());
      
      return XYZ((rx*ry*rz)*p);
    }
  
  //! Is constant if leaf node is.
  virtual const bool is_constant() const
  {
    return arg(0).is_constant();
  }

FUNCTION_END(FunctionRotate)

//------------------------------------------------------------------------------------------

FUNCTION_BEGIN(FunctionSin,0,0,false)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      return XYZ(sin(p.x()),sin(p.y()),sin(p.z()));
    }
  
  //! Isn't constant
  virtual const bool is_constant() const
  {
    return false;
  }

FUNCTION_END(FunctionSin)

//------------------------------------------------------------------------------------------

FUNCTION_BEGIN(FunctionCos,0,0,false)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      return XYZ(cos(p.x()),cos(p.y()),cos(p.z()));
    }
  
  //! Isn't constant
  virtual const bool is_constant() const
  {
    return false;
  }

FUNCTION_END(FunctionCos)

//------------------------------------------------------------------------------------------

FUNCTION_BEGIN(FunctionSpiralLinear,0,1,false)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      const float r=p.magnitude();
      float theta=atan2(p.y(),p.x());
      if (theta<0.0f) theta+=2.0f*M_PI;
      const float winding=floorf(r-theta/(2.0*M_PI));
      
      const float x=2.0f*winding+theta/M_PI;
      const float y=2.0f*r-x;
      
      return arg(0)(XYZ(x,y,p.z()));
    }
  
  //! Is constant if the function being spiral-ed is.
  virtual const bool is_constant() const
    {
      return arg(0).is_constant();
    }

FUNCTION_END(FunctionSpiralLinear)

//------------------------------------------------------------------------------------------

FUNCTION_BEGIN(FunctionSpiralLogarithmic,0,1,false)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      const float r=p.magnitude();
      float theta=atan2(p.y(),p.x());
      if (theta<0.0f) theta+=2.0f*M_PI;
      const float lnr=log(r);
      const float winding=floorf(lnr-theta/(2.0*M_PI));
      
      const float x=2.0f*winding+theta/M_PI;
      const float y=2.0f*lnr-x;
      
      return arg(0)(XYZ(x,y,p.z()));
    }
  
  //! Is constant if the function being spiral-ed is.
  virtual const bool is_constant() const
    {
      return arg(0).is_constant();
    }

FUNCTION_END(FunctionSpiralLogarithmic)

//------------------------------------------------------------------------------------------

FUNCTION_BEGIN(FunctionGradient,0,1,false)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      const float epsilon=1e-4;
      const XYZ vepsilon(epsilon,epsilon,epsilon);
      
      const XYZ v0(arg(0)(p-vepsilon));
      const XYZ v1(arg(0)(p+vepsilon));
      return (v1-v0)/(2.0*epsilon);
    }
  
  //! Is constant if the function being gradient-ed is.
  virtual const bool is_constant() const
    {
      return arg(0).is_constant();
    }

FUNCTION_END(FunctionGradient)

//------------------------------------------------------------------------------------------

FUNCTION_BEGIN(FunctionComposePair,0,2,false)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      return arg(1)(arg(0)(p));
    }
  
  //! Is constant if either function is.
  virtual const bool is_constant() const
    {
      return (arg(0).is_constant() || arg(1).is_constant());
    }

FUNCTION_END(FunctionComposePair)

//------------------------------------------------------------------------------------------

FUNCTION_BEGIN(FunctionComposeTriple,0,3,false)
  
  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      return arg(2)(arg(1)(arg(0)(p)));
    }
  
  //! Is constant if any function is.
  virtual const bool is_constant() const
    {
      return (arg(0).is_constant() || arg(1).is_constant() || arg(2).is_constant());
    }

FUNCTION_END(FunctionComposeTriple)

//------------------------------------------------------------------------------------------

FUNCTION_BEGIN(FunctionAdd,0,2,false)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      return arg(0)(p)+arg(1)(p);
    }
  
  //! Is constant if both leaf functions are.
  virtual const bool is_constant() const
    {
      return (arg(0).is_constant() && arg(1).is_constant());
    }

FUNCTION_END(FunctionAdd)

//------------------------------------------------------------------------------------------

FUNCTION_BEGIN(FunctionMultiply,0,2,false)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      const XYZ v0(arg(0)(p));
      const XYZ v1(arg(1)(p));
      return XYZ(v0.x()*v1.x(),v0.y()*v1.y(),v0.z()*v1.z());
    }
  
  //! Is constant if both leaf functions are.
  virtual const bool is_constant() const
    {
      return (arg(0).is_constant() && arg(1).is_constant());
    }

FUNCTION_END(FunctionMultiply)

//------------------------------------------------------------------------------------------

FUNCTION_BEGIN(FunctionDivide,0,2,false)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      const XYZ v0(arg(0)(p));
      const XYZ v1(arg(1)(p));

      return XYZ(
		 (v1.x()==0.0 ? 0.0 : v0.x()/v1.x()),
		 (v1.y()==0.0 ? 0.0 : v0.y()/v1.y()),
		 (v1.z()==0.0 ? 0.0 : v0.z()/v1.z())
		 );

    }
  
  //! Is constant if both leaf functions are.
  virtual const bool is_constant() const
    {
      return (arg(0).is_constant() && arg(1).is_constant());
    }

FUNCTION_END(FunctionDivide)

//------------------------------------------------------------------------------------------

FUNCTION_BEGIN(FunctionCross,0,2,false)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      const XYZ v0(arg(0)(p));
      const XYZ v1(arg(1)(p));
      return v0*v1;
    }
  
  //! Is constant if both leaf functions are.
  virtual const bool is_constant() const
    {
      return (arg(0).is_constant() && arg(1).is_constant());
    }

FUNCTION_END(FunctionCross)

//------------------------------------------------------------------------------------------

FUNCTION_BEGIN(FunctionMax,0,2,false)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      const XYZ v0(arg(0)(p));
      const XYZ v1(arg(1)(p));
      return XYZ(
		 std::max(v0.x(),v1.x()),
		 std::max(v0.y(),v1.y()),
		 std::max(v0.z(),v1.z())
		 );
    }
  
  //! Is constant if both leaf functions are.
  virtual const bool is_constant() const
    {
      return (arg(0).is_constant() && arg(1).is_constant());
    }

FUNCTION_END(FunctionMax)

//------------------------------------------------------------------------------------------

FUNCTION_BEGIN(FunctionMin,0,2,false)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      const XYZ v0(arg(0)(p));
      const XYZ v1(arg(1)(p));
      return XYZ(
		 std::min(v0.x(),v1.x()),
		 std::min(v0.y(),v1.y()),
		 std::min(v0.z(),v1.z())
		 );
    }
  
  //! Is constant if both leaf functions are.
  virtual const bool is_constant() const
    {
      return (arg(0).is_constant() && arg(1).is_constant());
    }

FUNCTION_END(FunctionMin)

//------------------------------------------------------------------------------------------

//! Function returning components of one function modulus thos of another.
/*! Sane always-positive modulus used to avoid funny business at zero.
 */
FUNCTION_BEGIN(FunctionModulus,0,2,false)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      const XYZ v0(arg(0)(p));
      const XYZ v1(arg(1)(p));
      return XYZ(
		 modulusf(v0.x(),fabs(v1.x())),
		 modulusf(v0.y(),fabs(v1.y())),
		 modulusf(v0.z(),fabs(v1.z()))
		 );
    }
  
  //! Is constant if both leaf functions are.
  virtual const bool is_constant() const
    {
      return (arg(0).is_constant() && arg(1).is_constant());
    }

FUNCTION_END(FunctionModulus)

//------------------------------------------------------------------------------------------

FUNCTION_BEGIN(FunctionExp,0,0,false)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      return XYZ(exp(p.x()),exp(p.y()),exp(p.z()));
    }
  
  //! Is not constant.
  virtual const bool is_constant() const
    {
      return false;
    }

FUNCTION_END(FunctionExp)

//------------------------------------------------------------------------------------------

//! Invert the leaf function using a radius-one origin centred sphere.
FUNCTION_BEGIN(FunctionGeometricInversion,0,1,false)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      const float radius2=p.magnitude2();
      const XYZ ip(p/radius2);

      return arg(0)(ip);
    }
  
  //! Is constant if leaf function is.
  virtual const bool is_constant() const
    {
      return arg(0).is_constant();
    }

FUNCTION_END(FunctionGeometricInversion)

//------------------------------------------------------------------------------------------

//! Implements reflection of sampling point about a plane
FUNCTION_BEGIN(FunctionReflect,0,3,false)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      const XYZ pt_in_plane(arg(0)(p));
      const XYZ normal(arg(1)(p).normalised());
      
      XYZ pos(arg(2)(p));
      
      const float distance_from_plane=(pos-pt_in_plane)%normal;
      
      // If pos is on the wrong side of the plane, reflect it over
      // Check: normal (0,0,1), pos (0,0,-1) => distance -1, pos-=(2*-1)*(0,0,1) => pos-=(0,0,-2)
      if (distance_from_plane<0.0)
	{
	  pos-=(2.0*distance_from_plane)*normal;
	}
      
      return pos;
    }
  
  //! Is constant if sampled function is
  virtual const bool is_constant() const
    {
      return arg(2).is_constant();
    }

FUNCTION_END(FunctionReflect)

//------------------------------------------------------------------------------------------

//! Implements reflection of sampling point about multiple planes
FUNCTION_BEGIN(FunctionKaleidoscope,1,1,false)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      const uint n=2+static_cast<uint>(floor(8.0f*fabs(param(0))));

      const float a=atan2(p.x(),p.y());
      const float r=sqrt(p.x()*p.x()+p.y()*p.y());
      
      const float sa=trianglef(a,M_PI/n);

      const XYZ s(r*sin(sa),r*cos(sa),p.z());
      return arg(0)(s);
    }
  
  //! Is constant if sampled function is
  virtual const bool is_constant() const
    {
      return arg(0).is_constant();
    }

FUNCTION_END(FunctionKaleidoscope)

//------------------------------------------------------------------------------------------

//! Implements reflection of sampling point about multiple planes
FUNCTION_BEGIN(FunctionWindmill,1,1,false)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      const uint n=1+static_cast<uint>(floor(8.0f*fabs(param(0))));

      const float a=atan2(p.x(),p.y());
      const float r=sqrt(p.x()*p.x()+p.y()*p.y());
      
      const float sa=modulusf(a,M_PI/n);

      const XYZ s(r*sin(sa),r*cos(sa),p.z());
      return arg(0)(s);
    }
  
  //! Is constant if sampled function is
  virtual const bool is_constant() const
    {
      return arg(0).is_constant();
    }

FUNCTION_END(FunctionWindmill)

//------------------------------------------------------------------------------------------

//! Function returns a value comprising the magnitude of three leaf functions.
FUNCTION_BEGIN(FunctionMagnitudes,0,3,false)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      return XYZ(
		 arg(0)(p).magnitude(),
		 arg(1)(p).magnitude(),
		 arg(2)(p).magnitude()
		 );
    }
  
  //! Is constant if all leaf functions are
  virtual const bool is_constant() const
    {
      return (arg(0).is_constant() && arg(1).is_constant() && arg(2).is_constant());
    }

FUNCTION_END(FunctionMagnitudes)

//------------------------------------------------------------------------------------------

//! Function returns position magnitude.
FUNCTION_BEGIN(FunctionMagnitude,0,0,false)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      const float m=p.magnitude();
      return XYZ(m,m,m);
    }
  
  //! Is not constant.
  virtual const bool is_constant() const
    {
      return false;
    }

FUNCTION_END(FunctionMagnitude)

//------------------------------------------------------------------------------------------

//! Function implements selection between 2 functions based on the relative magnitudes of 2 other functions
FUNCTION_BEGIN(FunctionChooseSphere,0,4,false)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      if ((arg(0)(p)).magnitude2()<(arg(1)(p)).magnitude2())
	return arg(2)(p);
      else
	return arg(3)(p);
    }
  
  //! Is constant if all leaf functions are
  virtual const bool is_constant() const
    {
      return (arg(0).is_constant() && arg(1).is_constant() && arg(2).is_constant() && arg(3).is_constant());
    }

FUNCTION_END(FunctionChooseSphere)

//------------------------------------------------------------------------------------------

//! Function implements selection between 2 functions based on whether a rectangle contains a point
FUNCTION_BEGIN(FunctionChooseRect,0,4,false)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      const XYZ p0(arg(0)(p));
      const XYZ p1(arg(1)(p));
      
      if (p1.origin_centred_rect_contains(p0))
	return arg(2)(p);
      else
	return arg(3)(p);
    }
  
  //! Is constant if all leaf functions are
  virtual const bool is_constant() const
    {
      return (arg(0).is_constant() && arg(1).is_constant() && arg(2).is_constant() && arg(3).is_constant());
    }

FUNCTION_END(FunctionChooseRect)

//------------------------------------------------------------------------------------------

//! Function implements selection between 2 functions based on position in 3d mesh
FUNCTION_BEGIN(FunctionChooseFrom2InCubeMesh,0,2,false)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      const int x=static_cast<int>(floorf(p.x()));
      const int y=static_cast<int>(floorf(p.y()));
      const int z=static_cast<int>(floorf(p.z()));

      if ((x+y+z)&1)
	return arg(0)(p);
      else
	return arg(1)(p);
    }
  
  //! Isn't constant (unless leaf functions constant and return same result - unlikely)
  virtual const bool is_constant() const
    {
      return false;
    }

FUNCTION_END(FunctionChooseFrom2InCubeMesh);

//------------------------------------------------------------------------------------------

//! Function implements selection between 2 functions based on position in 3d mesh
FUNCTION_BEGIN(FunctionChooseFrom3InCubeMesh,0,3,false)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      const int x=static_cast<int>(floorf(p.x()));
      const int y=static_cast<int>(floorf(p.y()));
      const int z=static_cast<int>(floorf(p.z()));

      return arg(modulusi(x+y+z,3))(p);
    }
  
  //! Isn't constant (unless leaf functions constant and return same result - unlikely)
  virtual const bool is_constant() const
    {
      return false;
    }

FUNCTION_END(FunctionChooseFrom3InCubeMesh)

//------------------------------------------------------------------------------------------

//! Function implements selection between 2 functions based on position in 2d grid
FUNCTION_BEGIN(FunctionChooseFrom2InSquareGrid,0,2,false)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      const int x=static_cast<int>(floorf(p.x()));
      const int y=static_cast<int>(floorf(p.y()));

      if ((x+y)&1)
	return arg(0)(p);
      else
	return arg(1)(p);
    }
  
  //! Isn't constant (unless leaf functions constant and return same result - unlikely)
  virtual const bool is_constant() const
    {
      return false;
    }

FUNCTION_END(FunctionChooseFrom2InSquareGrid)

//------------------------------------------------------------------------------------------

//! Function implements selection between 3 functions based on position in 2d grid
FUNCTION_BEGIN(FunctionChooseFrom3InSquareGrid,0,3,false)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      const int x=static_cast<int>(floorf(p.x()));
      const int y=static_cast<int>(floorf(p.y()));

      return arg(modulusi(x+y,3))(p);
    }
  
  //! Isn't constant (unless leaf functions constant and return same result - unlikely)
  virtual const bool is_constant() const
    {
      return false;
    }

FUNCTION_END(FunctionChooseFrom3InSquareGrid)

//------------------------------------------------------------------------------------------

//! Function implements selection between 2 functions based on position in grid of triangles 
FUNCTION_BEGIN(FunctionChooseFrom2InTriangleGrid,0,2,false)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      static const XYZ d0(1.0f         ,0.0f         ,0.0f);
      static const XYZ d1(cos(  M_PI/3),sin(  M_PI/3),0.0f);
      static const XYZ d2(cos(2*M_PI/3),sin(2*M_PI/3),0.0f);
      
      const int a=static_cast<int>(floorf(p%d0));
      const int b=static_cast<int>(floorf(p%d1));
      const int c=static_cast<int>(floorf(p%d2));

      if ((a+b+c)&1)
	return arg(0)(p);
      else
	return arg(1)(p);
    }
  
  //! Isn't constant (unless leaf functions constant and return same result - unlikely)
  virtual const bool is_constant() const
    {
      return false;
    }

FUNCTION_END(FunctionChooseFrom2InTriangleGrid)

//------------------------------------------------------------------------------------------

//! Function implements selection between 2 functions based on position in grid of triangles 
/*! Not entirely sure this one produces a sensible pattern.  Needs explicitly testing.
 */
FUNCTION_BEGIN(FunctionChooseFrom3InTriangleGrid,0,3,false)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      static const XYZ d0(1.0f         ,0.0f         ,0.0f);
      static const XYZ d1(cos(  M_PI/3),sin(  M_PI/3),0.0f);
      static const XYZ d2(cos(2*M_PI/3),sin(2*M_PI/3),0.0f);
      
      const int a=static_cast<int>(floorf(p%d0));
      const int b=static_cast<int>(floorf(p%d1));
      const int c=static_cast<int>(floorf(p%d2));

      return arg(modulusi(a+b+c,3))(p);
    }
  
  //! Isn't constant (unless leaf functions constant and return same result - unlikely)
  virtual const bool is_constant() const
    {
      return false;
    }

FUNCTION_END(FunctionChooseFrom3InTriangleGrid)

//------------------------------------------------------------------------------------------

//! Function implements selection between 3 functions based on position in grid of hexagons
/*! Don't entirely understand how this works, but it looks nice.
 */
FUNCTION_BEGIN(FunctionChooseFrom3InDiamondGrid,0,3,false)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      // Basis vectors for hex grid
      static const XYZ d0(1.0f         ,0.0f         ,0.0f);
      static const XYZ d1(cos(  M_PI/3),sin(  M_PI/3),0.0f);
      static const XYZ d2(cos(2*M_PI/3),sin(2*M_PI/3),0.0f);
      
      // Dot with basis
      const float p0=p%d0;
      const float p1=p%d1;
      const float p2=p%d2;

      // Find nearest on-grid point
      const int i0=(int)floorf(p0+0.5f);
      const int i1=(int)floorf(p1+0.5f);
      const int i2=(int)floorf(p2+0.5f);

      // Measure distance
      const float m0=fabsf(p0-i0);
      const float m1=fabsf(p1-i1);
      const float m2=fabsf(p2-i2);

      // Closest one decides which function
      if (m0<=m1 && m0<=m2)
	return arg(0)(p);
      else if (m1<=m0 && m1<=m2)
	return arg(1)(p);
      else 
	return arg(2)(p);
    }
  
  //! Isn't constant (unless leaf functions constant and return same result - unlikely)
  virtual const bool is_constant() const
    {
      return false;
    }

FUNCTION_END(FunctionChooseFrom3InDiamondGrid)

//------------------------------------------------------------------------------------------

//! Function implements selection between 3 functions based on position in grid of hexagons
FUNCTION_BEGIN(FunctionChooseFrom3InHexagonGrid,0,3,false)

  //! Co-ordinates of hexagon with given hex-grid coords
  static const XYZ hex(int x,int y)
    {
      const float k=sqrt(3.0f)/2.0f;
      return XYZ(
		 x*k,
		 y+((x&1) ? 0.5f : 0.0f),
		 0.0f
		 );
    }

  //! Finds hex-grid coordinates of hex containing cartesian px,py
  static void nearest_hex(float px,float py,int &hx,int& hy)
    {
      // Initial guess at which hex we're in:
      const float k=sqrt(3.0f)/2.0f;

      const int nx=(int)rintf(px/k);
      const int ny=(int)(
			 (nx&1) 
			 ? 
			 rintf(py-0.5f) 
			 : 
			 rintf(py)
			 );

      hx=nx;
      hy=ny;
      const XYZ ph=hex(nx,ny);
      float m2b=(XYZ(px,py,0.0f)-ph).magnitude2();
      
      for (int dy=-1;dy<=1;dy++)
	for (int dx=-1;dx<=1;dx++)
	  if (!(dy==0 && dx==0))
	    {
	      const float m2=(XYZ(px,py,0.0f)-hex(nx+dx,ny+dy)).magnitude2();
	      if (m2<m2b)
		{
		  hx=nx+dx;
		  hy=ny+dy;
		  m2b=m2;
		}
	    }
    } 
  
  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      int hx;
      int hy;
      nearest_hex(p.x(),p.y(),hx,hy);
      const uint which=hy+((hx&1)? 2 : 0);
      return arg(modulusi(which,3))(p);
    }
    
  //! Isn't constant (unless leaf functions constant and return same result - unlikely)
  virtual const bool is_constant() const
    {
      return false;
    }

FUNCTION_END(FunctionChooseFrom3InHexagonGrid)

//------------------------------------------------------------------------------------------

//! Function implements selection between 2 functions based on position in grid of hexagons
FUNCTION_BEGIN(FunctionChooseFrom2InBorderedHexagonGrid,1,2,false)

  //! Co-ordinates of hexagon with given hex-grid coords
  static const XYZ hex(int x,int y)
    {
      const float k=sqrt(3.0f)/2.0f;
      return XYZ(
		 x*k,
		 y+((x&1) ? 0.5f : 0.0f),
		 0.0f
		 );
    }

  //! Finds hex-grid coordinates of hex containing cartesian px,py
  static void nearest_hex(float px,float py,int &hx,int& hy)
    {
      // Initial guess at which hex we're in:
      const float k=sqrt(3.0f)/2.0f;

      const int nx=(int)rintf(px/k);
      const int ny=(int)(
			 (nx&1) 
			 ? 
			 rintf(py-0.5f) 
			 : 
			 rintf(py)
			 );

      hx=nx;
      hy=ny;
      const XYZ ph=hex(nx,ny);
      float m2b=(XYZ(px,py,0.0f)-ph).magnitude2();
      
      for (int dy=-1;dy<=1;dy++)
	for (int dx=-1;dx<=1;dx++)
	  if (!(dy==0 && dx==0))
	    {
	      const float m2=(XYZ(px,py,0.0f)-hex(nx+dx,ny+dy)).magnitude2();
	      if (m2<m2b)
		{
		  hx=nx+dx;
		  hy=ny+dy;
		  m2b=m2;
		}
	    }
    } 
  
  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      int hx;
      int hy;
      nearest_hex(p.x(),p.y(),hx,hy);

      bool in_border=false;

      // Hex centres are separated by 1.0 so limit border size
      const float b=modulusf(param(0),0.5f);

      // Step along grid co-ordinates in various directions.  If there's a nearer point, we're in the border.
      for (uint a=0;a<6;a++)
	{
	  const float dx=b*sin(a*M_PI/3.0f);
	  const float dy=b*cos(a*M_PI/3.0f);
	  
	  int ax;
	  int ay;
	  nearest_hex(p.x()+dx,p.y()+dy,ax,ay);

	  if (hx!=ax || hy!=ay)
	    {
	      in_border=true;
	      break;
	    }
	}

      return arg(in_border)(p);
    }
  
  //! Isn't constant (unless leaf functions constant and return same result - unlikely)
  virtual const bool is_constant() const
    {
      return false;
    }

FUNCTION_END(FunctionChooseFrom2InBorderedHexagonGrid)

//------------------------------------------------------------------------------------------

//! Rays intersecting a textured unit sphere
/*! arg(0) is background
    arg(1) is 3D texture for sphere
  param(0,1,2) is light source direction
  p.x, p.y is the 2D position of a ray from infinity travelling in direction (0 0 1)
*/
FUNCTION_BEGIN(FunctionOrthoSphereShaded,3,2,false)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      const float pr2=p.x()*p.x()+p.y()*p.y();
      if (pr2<1.0f)
	{
	  const float z=-sqrt(1.0f-pr2);
	  const XYZ n(p.x(),p.y(),z);

	  const XYZ lu(param(0),param(1),param(2));
	  const XYZ l(lu.normalised());

	  const float i=0.5*(1.0+l%n); // In range 0-1
	  return i*arg(1)(n);
	}
      else
	{
	  return arg(0)(p);
	}
    }
  
  //! Can't be constant unless args are constant and equal (unlikely).
  virtual const bool is_constant() const
    {
      return false;
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
FUNCTION_BEGIN(FunctionOrthoSphereShadedBumpMapped,3,3,false)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      const float pr2=p.x()*p.x()+p.y()*p.y();
      if (pr2<1.0f)
	{
	  const float z=-sqrt(1.0f-pr2);
	  const XYZ n(p.x(),p.y(),z);

	  // Tangent vectors
	  const XYZ east((XYZ(0.0f,1.0f,0.0f)*n).normalised());
	  const XYZ north(n*east);

	  const float epsilon=1e-4;

	  const float e0=(arg(2)(n-epsilon*east)).magnitude2();
	  const float e1=(arg(2)(n+epsilon*east)).magnitude2();
	  const float n0=(arg(2)(n-epsilon*north)).magnitude2();
	  const float n1=(arg(2)(n+epsilon*north)).magnitude2();

	  const float de=(e1-e0)/(2.0f*epsilon);
	  const float dn=(n1-n0)/(2.0f*epsilon);

	  const XYZ perturbed_n((n-east*de-north*dn).normalised());

	  const XYZ lu(param(0),param(1),param(2));
	  const XYZ l(lu.normalised());

	  const float i=0.5*(1.0+l%perturbed_n); // In range 0-1
	  return i*arg(1)(n);
	}
      else
	{
	  return arg(0)(p);
	}
    }
  
  //! Unlikely to be constant.
  virtual const bool is_constant() const
    {
      return false;
    }

FUNCTION_END(FunctionOrthoSphereShadedBumpMapped)


//------------------------------------------------------------------------------------------

//! Rays reflecting off a unit sphere
/*! arg(0) is background
    arg(1) sampled using a normalised vector defines an environment for reflected rays
  p.x, p.y is the 2D position of a ray from infinity travelling in direction (0 0 1)
*/
FUNCTION_BEGIN(FunctionOrthoSphereReflect,0,2,false)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      const float pr2=p.x()*p.x()+p.y()*p.y();
      if (pr2<1.0f)
	{
	  const float z=-sqrt(1.0f-pr2);

	  // This is on surface of unit radius sphere - no need to normalise
	  XYZ n(p.x(),p.y(),z);

	  // The ray _towards_ the viewer v is (0 0 -1)
	  const XYZ v(0.0f,0.0f,-1.0f);

	  // The reflected ray is (2n.v)n-v
	  const XYZ reflected((2.0f*(n%v))*n-v);

	  return arg(1)(reflected);
	}
      else
	{
	  return arg(0)(p);
	}
    }
  
  //! Can't be constant unless args are constant and equal (unlikely).
  virtual const bool is_constant() const
    {
      return false;
    }

FUNCTION_END(FunctionOrthoSphereReflect)

//------------------------------------------------------------------------------------------

//! Rays reflecting off a bump mapped unit sphere
/*! arg(0) is background
    arg(1) sampled using a normalised vector defines an environment for reflected rays
    arg(2) is bump map
  p.x, p.y is the 2D position of a ray from infinity travelling in direction (0 0 1)
*/
FUNCTION_BEGIN(FunctionOrthoSphereReflectBumpMapped,0,3,false)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      const float pr2=p.x()*p.x()+p.y()*p.y();
      if (pr2<1.0f)
	{
	  const float z=-sqrt(1.0f-pr2);

	  // This is on surface of unit radius sphere - no need to normalise
	  XYZ n(p.x(),p.y(),z);

	  // Tangent vectors
	  const XYZ east((XYZ(0.0f,1.0f,0.0f)*n).normalised());
	  const XYZ north(n*east);

	  const float epsilon=1e-4;

	  const float e0=(arg(2)(n-epsilon*east)).magnitude2();
	  const float e1=(arg(2)(n+epsilon*east)).magnitude2();
	  const float n0=(arg(2)(n-epsilon*north)).magnitude2();
	  const float n1=(arg(2)(n+epsilon*north)).magnitude2();

	  const float de=(e1-e0)/(2.0f*epsilon);
	  const float dn=(n1-n0)/(2.0f*epsilon);

	  const XYZ perturbed_n((n-east*de-north*dn).normalised());

	  // The ray _towards_ the viewer is (0 0 -1)
	  const XYZ v(0.0f,0.0f,-1.0f);

	  // The reflected ray is (2n.v)n-v
	  const XYZ reflected((2.0f*(perturbed_n%v))*perturbed_n-v);

	  return arg(1)(reflected);
	}
      else
	{
	  return arg(0)(p);
	}
    }
  
  //! Can't be constant unless args are constant and equal (unlikely).
  virtual const bool is_constant() const
    {
      return false;
    }

FUNCTION_END(FunctionOrthoSphereReflectBumpMapped)

//------------------------------------------------------------------------------------------

FUNCTION_BEGIN(FunctionFilter2D,2,1,false)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      return
	arg(0)(p)
	-(
	  arg(0)(p+XYZ(param(0),0.0f,0.0f))
	  +arg(0)(p+XYZ(-param(0),0.0f,0.0f))
	  +arg(0)(p+XYZ(0.0f,param(1),0.0f))
	  +arg(0)(p+XYZ(0.0f,-param(1),0.0f))
	  )/4.0f;
    }
  
  //! Is constant if arg is
  virtual const bool is_constant() const
    {
      return arg(0).is_constant();
    }

FUNCTION_END(FunctionFilter2D)

//------------------------------------------------------------------------------------------

FUNCTION_BEGIN(FunctionFilter3D,3,1,false)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      return
	arg(0)(p)
	-(
	  arg(0)(p+XYZ(param(0),0.0f,0.0f))
	  +arg(0)(p+XYZ(-param(0),0.0f,0.0f))
	  +arg(0)(p+XYZ(0.0f,param(1),0.0f))
	  +arg(0)(p+XYZ(0.0f,-param(1),0.0f))
	  +arg(0)(p+XYZ(0.0f,0.0f,param(1)))
	  +arg(0)(p+XYZ(0.0f,0.0f,-param(1)))
	  )/6.0f;
    }
  
  //! Is constant if arg is
  virtual const bool is_constant() const
    {
      return arg(0).is_constant();
    }

FUNCTION_END(FunctionFilter3D)

//------------------------------------------------------------------------------------------

//! Sum of two evaluations of a function, one sampled at an offset and weighted.
FUNCTION_BEGIN(FunctionShadow,4,1,false)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      return
	arg(0)(p)+param(3)*arg(0)(p+XYZ(param(0),param(1),param(2)));
    }
  
  //! Is constant if arg is
  virtual const bool is_constant() const
    {
      return arg(0).is_constant();
    }

FUNCTION_END(FunctionShadow)


//------------------------------------------------------------------------------------------

//! Like FunctionShadow but the offset is obtained from a function.
FUNCTION_BEGIN(FunctionShadowGeneralised,1,2,false)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      return
	arg(0)(p)+param(0)*arg(0)(p+arg(1)(p));
    }
  
  //! Is constant if arg(0) is
  virtual const bool is_constant() const
    {
      return arg(0).is_constant();
    }

FUNCTION_END(FunctionShadowGeneralised)

//------------------------------------------------------------------------------------------

//! Multiply x and y by z
FUNCTION_BEGIN(FunctionCone,0,0,false)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      return XYZ(p.x()*p.z(),p.y()*p.z(),p.z());
    }
  
  //! Not constant
  virtual const bool is_constant() const
    {
      return false;
    }

FUNCTION_END(FunctionCone)

//------------------------------------------------------------------------------------------

//! Multiply x and y by exp(z)
FUNCTION_BEGIN(FunctionExpCone,0,0,false)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      const float k=exp(p.z());
      return XYZ(p.x()*k,p.y()*k,p.z());
    }
  
  //! Not constant
  virtual const bool is_constant() const
    {
      return false;
    }

FUNCTION_END(FunctionExpCone)

//------------------------------------------------------------------------------------------

//! Separate influence of z co-ordinate.
/*! Interesting as a top level node for animations as structure will tend to be fixed, with only colour map changing
 */
FUNCTION_BEGIN(FunctionSeparateZ,3,2,false)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      const XYZ v=arg(0)(XYZ(p.x(),p.y(),0.0f));
      return arg(1)(v+p.z()*XYZ(param(0),param(1),param(2)));
    }
  
  //! Constant if arg(1) is
  virtual const bool is_constant() const
    {
      return arg(1).is_constant();
    }

FUNCTION_END(FunctionSeparateZ)

//------------------------------------------------------------------------------------------

#include "noise.h"

//! Perlin noise function.
/*! Returns a single value replicated into all three channels
*/
FUNCTION_BEGIN(FunctionNoiseOneChannel,0,0,false)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      // Crank up the frequency a bit otherwise don't see much variation in base case
      const float v=_noise(2.0f*p);
      return XYZ(v,v,v);
    }
  
  //! Not constant
  virtual const bool is_constant() const
    {
      return false;
    }

 protected:
  static Noise _noise;

FUNCTION_END(FunctionNoiseOneChannel)

//------------------------------------------------------------------------------------------


//! Multiscale noise function.
/*! Returns a single value replicated into all three channels
*/
FUNCTION_BEGIN(FunctionMultiscaleNoiseOneChannel,0,0,false)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      float t=0.0f;
      float tm=0.0f;
      for (uint i=0;i<8;i++)
	{
	  const float k=(1<<i);
	  const float ik=1.0f/k;
	  t+=ik*_noise(k*p);
	  tm+=ik;
	}
      const float v=t/tm;
      return XYZ(v,v,v);
    }
  
  //! Not constant
  virtual const bool is_constant() const
    {
      return false;
    }

 protected:
  static Noise _noise;

FUNCTION_END(FunctionMultiscaleNoiseOneChannel)

//------------------------------------------------------------------------------------------

//! Perlin noise function.
/*! Returns three independent channels
*/
FUNCTION_BEGIN(FunctionNoiseThreeChannel,0,0,false)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      return XYZ(_noise0(p),_noise1(p),_noise2(p));
    }
  
  //! Not constant
  virtual const bool is_constant() const
    {
      return false;
    }

 protected:
  static Noise _noise0;
  static Noise _noise1;
  static Noise _noise2;

FUNCTION_END(FunctionNoiseThreeChannel)

//------------------------------------------------------------------------------------------

//! Perlin multiscale noise function.
/*! Returns three independent channels
*/
FUNCTION_BEGIN(FunctionMultiscaleNoiseThreeChannel,0,0,false)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      XYZ t(0.0f,0.0f,0.0f);
      float tm=0.0f;
      for (uint i=0;i<8;i++)
	{
	  const float k=(1<<i);
	  const float ik=1.0f/k;
	  const XYZ kp(k*p);
	  t+=ik*XYZ(_noise0(kp),_noise1(kp),_noise2(kp));
	  tm+=ik;
	}
      return t/tm;
    }
  
  //! Not constant
  virtual const bool is_constant() const
    {
      return false;
    }

 protected:
  static Noise _noise0;
  static Noise _noise1;
  static Noise _noise2;

FUNCTION_END(FunctionMultiscaleNoiseThreeChannel)


//------------------------------------------------------------------------------------------

//! Function repeatedly applying it's leaf function to the argument
FUNCTION_BEGIN(FunctionIterate,0,1,true)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      XYZ ret(p);
      for (uint i=0;i<iterations();i++)
	ret=arg(0)(ret);
      return ret;
    }
  
  //! Is constant if leaf function is.
  virtual const bool is_constant() const
    {
      return arg(0).is_constant();
    }

FUNCTION_END(FunctionIterate)

//------------------------------------------------------------------------------------------

//! Function returning average value of evenly spaced samples between two points
FUNCTION_BEGIN(FunctionAverageSamples,3,1,true)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      const XYZ baseline(param(0),param(1),param(2));
     
      XYZ p0;
      XYZ p1;
      XYZ delta;
      
      if (iterations()==1)
	{
	  p0=p;
	  p1=p;
	  delta=XYZ(0.0f,0.0f,0.0f);
	}
      else
	{
	  // In the case of two iterations the samples will be at p0 and p1
	  p0=p-baseline;
	  p1=p+baseline;
	  delta=(p1-p0)/(iterations()-1);
	}

      XYZ ret(0.0f,0.0f,0.0f);
      XYZ ps=p0;

      for (uint i=0;i<iterations();i++)
	{
	  ret+=arg(0)(ps);
	  ps+=delta;
	}
      ret/=iterations();
      return ret;
    }
  
  //! Is constant if sampled leaf function is.
  virtual const bool is_constant() const
    {
      return arg(0).is_constant();
    }

FUNCTION_END(FunctionAverageSamples)

//------------------------------------------------------------------------------------------

//! Similar to average samples except one end has a higher weighting
FUNCTION_BEGIN(FunctionStreak,3,1,true)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      const XYZ baseline(param(0),param(1),param(2));
     
      XYZ p0;
      XYZ p1;
      XYZ delta;
      
      if (iterations()==1)
	{
	  p0=p;
	  p1=p;
	  delta=XYZ(0.0f,0.0f,0.0f);
	}
      else
	{
	  p0=p;
	  p1=p+baseline;
	  delta=(p1-p0)/(iterations()-1);
	}

      XYZ ret(0.0f,0.0f,0.0f);
      XYZ ps=p0;
      float w=0.0f;

      for (uint i=0;i<iterations();i++)
	{
	  const float k=1.0f-static_cast<float>(i)/iterations();
	  ret+=k*arg(0)(ps);
	  w+=k;
	  ps+=delta;
	}
      ret/=w;
      return ret;
    }
  
  //! Is constant if sampled leaf function is.
  virtual const bool is_constant() const
    {
      return arg(0).is_constant();
    }

FUNCTION_END(FunctionStreak)

//------------------------------------------------------------------------------------------

//! Function similar to FunctionAverageSamples but doing convolution
FUNCTION_BEGIN(FunctionConvolveSamples,3,2,true)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      const XYZ baseline(param(0),param(1),param(2));
     
      XYZ p0;
      XYZ p1;
      XYZ delta;
      
      if (iterations()==1)
	{
	  p0=p;
	  p1=p;
	  delta=XYZ(0.0f,0.0f,0.0f);
	}
      else
	{
	  p0=p-baseline;
	  p1=p+baseline;
	  delta=(p1-p0)/(iterations()-1);
	}

      XYZ ret(0.0f,0.0f,0.0f);
      XYZ pd(0.0f,0.0f,0.0f);

      for (uint i=0;i<iterations();i++)
	{
	  ret+=(arg(0)(p+pd)*arg(1)(pd));
	  pd+=delta;
	}
      ret/=iterations();
      return ret;
    }
  
  //! Is constant if arg(0) is
  virtual const bool is_constant() const
    {
      return arg(0).is_constant();
    }

FUNCTION_END(FunctionConvolveSamples)

//------------------------------------------------------------------------------------------

//! Function summing decreasing amounts of higher frequency versions of image
FUNCTION_BEGIN(FunctionAccumulateOctaves,0,1,true)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      XYZ ret(0.0f,0.0f,0.0f);
      float k=0.0f;
      for (uint i=0;i<iterations();i++)
	{
	  const float scale=(1<<i);
	  const float iscale=1.0f/scale;
	  ret+=iscale*(arg(0)(scale*p));
	  k+=iscale;
	}
      ret/=k;
      return ret;
    }
  
  //! Is constant if sampled leaf function is.
  virtual const bool is_constant() const
    {
      return arg(0).is_constant();
    }

FUNCTION_END(FunctionAccumulateOctaves)


//------------------------------------------------------------------------------------------

//! Generalised (3D) Mandelbrot iterator for fractal functions.
/*! Returns i in 0 to _iterations inclusive.  i==_iterations implies "in" set.
 */
inline const uint brot(const XYZ& z0,const XYZ& c,const uint iterations)
{
  XYZ z(z0);
  uint i;
  for (i=0;i<iterations;i++)
    {
      const float x2=z.x()*z.x();
      const float y2=z.y()*z.y();
      const float z2=z.z()*z.z();
      
      if (x2+y2+z2>4.0f)
	break;
      
      XYZ nz;
      nz.x(x2-y2-z2+c.x());
      nz.y(2.0f*z.x()*z.y()+c.y());
      nz.z(2.0f*z.x()*z.z()+c.z());
      
      z=nz;
    }

  return i;
}

//------------------------------------------------------------------------------------------

//! Function selects arg to evaluate based on test for point in Mandelbrot set.
FUNCTION_BEGIN(FunctionMandelbrotChoose,0,2,false)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      return (brot(XYZ(0.0f,0.0f,0.0f),p,iterations())==iterations() ? arg(0)(p) : arg(1)(p));
    }
  
  //! Can't be constant unless functions are the same.
  virtual const bool is_constant() const
    {
      return false;
    }

FUNCTION_END(FunctionMandelbrotChoose)

//-----------------------------------------------------------------------------------------

//! Function returns -1 for points in set, 0-1 for escaped points
FUNCTION_BEGIN(FunctionMandelbrotContour,0,0,true)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      const uint i=brot(XYZ(0.0f,0.0f,0.0f),p,iterations());
      return (i==iterations() ? XYZ::fill(-1.0f) : XYZ::fill(static_cast<float>(i)/iterations()));
    }
  
  //! Not constant.
  virtual const bool is_constant() const
    {
      return false;
    }

FUNCTION_END(FunctionMandelbrotContour)

//------------------------------------------------------------------------------------------

//! Function selects arg to evaluate based on test for point in Julia set.
FUNCTION_BEGIN(FunctionJuliaChoose,0,3,true)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      return (brot(p,arg(2)(p),iterations())==iterations() ? arg(0)(p) : arg(1)(p));
    }
  
  //! Can't be constant unless functions are the same.
  virtual const bool is_constant() const
    {
      return false;
    }

FUNCTION_END(FunctionJuliaChoose)

//------------------------------------------------------------------------------------------

//! Function returns -1 for points in set, 0-1 for escaped points
FUNCTION_BEGIN(FunctionJuliaContour,0,1,true)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      const uint i=brot(p,arg(0)(p),iterations());
      return (i==iterations() ? XYZ::fill(-1.0f) : XYZ::fill(static_cast<float>(i)/iterations()));
    }
  
  //! Not constant.
  virtual const bool is_constant() const
    {
      return false;
    }

FUNCTION_END(FunctionJuliaContour)

//------------------------------------------------------------------------------------------

//! \todo Something like Conway's Game of Life or a reaction-diffusion system, seeded from an initial function.
/*! Probably operate on an iteration()xiterations()xiterations() grid.
  Need ability to cache state at a particular time by identity (address-of?) of "our".
  WILL cause slow start-up due to precompute, although compute-on-demand should help a bit.
  Maybe the whole static thing was a mistake.
  Should derive Function classes from FunctionBoilerplate<T> to provide Clone etc.
 */
FUNCTION_BEGIN(FunctionCellular,0,1,true)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p)
    {
      return XYZ(0.0f,0.0f,0.0f);
    }
  
  //! Not constant.
  virtual const bool is_constant()
    {
      return false;
    }

 protected:

};
// Not done yet.
//REGISTER(FunctionCellular);

//------------------------------------------------------------------------------------------

#endif
