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
  \brief Interfaces and implementation for specific (non-base) Function classes
  NB There is no proper class heirarchy here as all virtualisation and boilerplate
  services are supplied when the functions are plugged into the FunctionNode template
  (rather than being inherited).
  \warning This file should ONLY be included in function_node.cpp, which instantiates everything.
  The two functions allowed to escape "into the wild" are FunctionPreTransform and FunctionPostTransform 
  which have their own header file.
*/


#ifndef _functions_h_
#define _functions_h_

#include <cmath>

#include "useful.h"

//! Sane modulus function always returning a number in the range [0,y)
inline float modulusf(float x,float y)
{
  if (y<0) y=-y;
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

#include "function.h"
//------------------------------------------------------------------------------------------

//! Function class representing a constant value.
class FunctionConstant : public Function
{
 public:
  
  // 3 parameters: one for each component of XYZ
  static const uint parameters()
    {
      return 3;
    }

  // No leaf arguments
  static const uint arguments()
    {
      return 0;
    }

  //! Returns the constant value
  static const XYZ evaluate(const FunctionNode& our,const XYZ&)
    {
      return XYZ(our.param(0),our.param(1),our.param(2));
    }

  //! Returns true, obviously.
  static const bool is_constant(const FunctionNode&)
    {
      return true;
    }
};

REGISTER(FunctionConstant);

//------------------------------------------------------------------------------------------

//! Function class simply returning the position argument.
class FunctionIdentity : public Function
{
 public:

  //! No parameters
  static const uint parameters()
    {
      return 0;
    }

  //! No leaf arguments
  static const uint arguments()
    {
      return 0;
    }

  //! Simply return the position argument.
  static const XYZ evaluate(const FunctionNode&,const XYZ& p)
    {
      return p;
    }

  //! Is definitely not constant.
  static const bool is_constant(const FunctionNode&)
    {
      return false;
    }
};

REGISTER(FunctionIdentity);

//------------------------------------------------------------------------------------------

//! Function class returning position transfomed by a 12-component linear transform.
class FunctionTransform : public Function
{
 public:

  //! 12 parameters
  static const uint parameters()
    {
      return 12;
    }

  //! No leaf arguments
  static const uint arguments()
    {
      return 0;
    }

  //! Return the transformed position argument.
  static const XYZ evaluate(const FunctionNode& our,const XYZ& p)
  {
    const Transform transform(our.params());
    return transform.transformed(p);
  }

  //! Is definitely not constant.
  static const bool is_constant(const FunctionNode&)
    {
      return false;
    }
};

REGISTER(FunctionTransform);

//------------------------------------------------------------------------------------------

//! Function class returning position transfomed by a 12-component linear transform.
class FunctionTransformGeneralised : public Function
{
 public:

  //! 0 parameters
  static const uint parameters()
    {
      return 0;
    }

  //! 4 leaf arguments providing transform components.
  static const uint arguments()
    {
      return 4;
    }

  //! Return the transformed position argument.
  static const XYZ evaluate(const FunctionNode& our,const XYZ& p)
  {
    const Transform transform(our.arg(0)(p),our.arg(1)(p),our.arg(2)(p),our.arg(3)(p));
    return transform.transformed(p);
  }

  //! Is almost certainly not constant.
  static const bool is_constant(const FunctionNode&)
  {
    return false;
  }

};

REGISTER(FunctionTransformGeneralised);

//------------------------------------------------------------------------------------------

#include "function_pre_transform.h"

REGISTER(FunctionPreTransform);

//------------------------------------------------------------------------------------------

//! Function class returning leaf node evaluated at position transfomed by a 12-component linear transform.
/*! Unlike FunctionPreTransform, the basis vectors for the transform are not fixed but determined from leaf functions
 */
class FunctionPreTransformGeneralised : public Function
{
 public:

  //! 0 parameters
  static const uint parameters()
    {
      return 0;
    }

  //! 5 leaf arguments
  static const uint arguments()
    {
      return 5;
    }

  //! Return the evaluation of arg(0) at the transformed position argument.
  static const XYZ evaluate(const FunctionNode& our,const XYZ& p)
    {
      const Transform transform(our.arg(1)(p),our.arg(2)(p),our.arg(3)(p),our.arg(4)(p));
      return our.arg(0)(transform.transformed(p));
    }

  //! Has the same const-ness as arg(0)
  static const bool is_constant(const FunctionNode& our)
    {
      return our.arg(0).is_constant();
    }
};

REGISTER(FunctionPreTransformGeneralised);

//------------------------------------------------------------------------------------------

#include "function_post_transform.h"

REGISTER(FunctionPostTransform);

//------------------------------------------------------------------------------------------

//! Function class returning leaf node evaluated at given position; result is then transfomed by a 12-component linear transform.
/*! Unlike FunctionPostTransform, the basis vectors for the transform are not fixed but determined from leaf functions
 */
class FunctionPostTransformGeneralised : public Function
{
 public:

  //! 0 parameters
  static const uint parameters()
    {
      return 0;
    }

  //! 1 function leaf argument, and 4 for the transform.
  static const uint arguments()
    {
      return 5;
    }

  //! Return the evaluation of arg(0) at the transformed position argument.
  static const XYZ evaluate(const FunctionNode& our,const XYZ& p)
    {
      const Transform transform(our.arg(1)(p),our.arg(2)(p),our.arg(3)(p),our.arg(4)(p));
      return transform.transformed(our.arg(0)(p));
    }

  //! Only constant if all the leaf functions are constant.
  static const bool is_constant(const FunctionNode& our)
    {
      return (
	      our.arg(0).is_constant()
	      && our.arg(1).is_constant()
	      && our.arg(2).is_constant()
	      && our.arg(3).is_constant()
	      && our.arg(4).is_constant()
	      );
    }
};

REGISTER(FunctionPostTransformGeneralised);

//------------------------------------------------------------------------------------------

//! Transforms position transformed by a 30 paramter quadratic transform.
class FunctionTransformQuadratic : public Function
{
 public:

  //! 30 parameters: 12 linear plus 9 cross terms plus 9 squared terms.
  static const uint parameters()
    {
      return 30;
    }

  //! 0 leaf arguments.
  static const uint arguments()
    {
      return 0;
    }

  //! Return p transformed.
  static const XYZ evaluate(const FunctionNode& our,const XYZ& p)
    {
      const XYZ translate(our.param( 0),our.param( 1),our.param( 2));
      const XYZ basis_x  (our.param( 3),our.param( 4),our.param( 5));
      const XYZ basis_y  (our.param( 6),our.param( 7),our.param( 8));
      const XYZ basis_z  (our.param( 9),our.param(10),our.param(11));
      const XYZ basis_xy (our.param(12),our.param(13),our.param(14));
      const XYZ basis_xz (our.param(15),our.param(16),our.param(17));
      const XYZ basis_yz (our.param(18),our.param(19),our.param(20));
      const XYZ basis_xx (our.param(21),our.param(22),our.param(23));
      const XYZ basis_yy (our.param(24),our.param(25),our.param(26));
      const XYZ basis_zz (our.param(27),our.param(28),our.param(29));

      return 
	translate
	+basis_x*p.x()+basis_y*p.y()+basis_z*p.z()
	+basis_xy*(p.x()*p.y())+basis_xz*(p.x()*p.z())+basis_yz*(p.y()*p.z())
	+basis_xx*(p.x()*p.x())+basis_yy*(p.y()*p.y())+basis_zz*(p.z()*p.z());
    }

  //! Unlikely to ever be constant (requires all parameters zero).
  static const bool is_constant(const FunctionNode&)
    {
      return false;
    }
};

REGISTER(FunctionTransformQuadratic);

//------------------------------------------------------------------------------------------

//! Transforms cartesian coordinates to spherical
class FunctionCartesianToSpherical : public Function
{
 public:

  //! No parameters.
  static const uint parameters()
    {
      return 0;
    }

  //! No leaf arguments.
  static const uint arguments()
    {
      return 0;
    }

  //! Evaluate function.
  static const XYZ evaluate(const FunctionNode&,const XYZ& p)
  {
    const float r=p.magnitude();
    
    // Angles are normalised (-1 to +1) over their usual possible range.
    const float theta=atan2(p.y(),p.x())*(1.0f/M_PI);
    const float phi=(r== 0.0f ? 0.0f : asin(p.z()/r)*(1.0f/(0.5f*M_PI)));
  
    return XYZ(r,theta,phi);
  }

  //! Not constant.
  static const bool is_constant(const FunctionNode&)
    {
      return false;
    }

};

REGISTER(FunctionCartesianToSpherical);

//------------------------------------------------------------------------------------------

//! Transforms spherical coordinates to cartesian 
class FunctionSphericalToCartesian : public Function
{
 public:

  //! No parameters.
  static const uint parameters()
    {
      return 0;
    }

  //! No leaf arguments.
  static const uint arguments()
    {
      return 0;
    }

  //! Evaluate function.
  static const XYZ evaluate(const FunctionNode&,const XYZ& p)
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
  static const bool is_constant(const FunctionNode&)
  {
    return false;
  }
};

REGISTER(FunctionSphericalToCartesian);

//------------------------------------------------------------------------------------------

// Converts the position argument to spherical coords, pass these through the leaf node, and convert the result back to cartesian.
class FunctionEvaluateInSpherical : public Function
{
 public:
  //! No parameters.
  static const uint parameters()
    {
      return 0;
    }

  //! 1 leaf argument.
  static const uint arguments()
    {
      return 1;
    }

  //! Evaluate function.
  static const XYZ evaluate(const FunctionNode& our,const XYZ& p)
    {
      const float in_r=p.magnitude();
      const float in_theta=atan2(p.y(),p.x())*(1.0f/M_PI);
      const float in_phi=(in_r== 0.0f ? 0.0f : asin(p.z()/in_r)*(1.0f/(0.5f*M_PI)));
      
      const XYZ v(our.arg(0)(XYZ(in_r,in_theta,in_phi)));
      
      const float out_r=v.x();
      const float out_theta=M_PI*v.y();
      const float out_phi=0.5*M_PI*v.z();
      
      const float x=out_r*cos(out_theta)*sin(out_phi);
      const float y=out_r*sin(out_theta)*sin(out_phi);
      const float z=out_r*cos(out_phi);
      
      return XYZ(x,y,z);
    }
  
  //! Is constant if leaf node is.
  static const bool is_constant(const FunctionNode& our)
  {
    return our.arg(0).is_constant();
  }

};

REGISTER(FunctionEvaluateInSpherical);

//------------------------------------------------------------------------------------------

class FunctionRotate : public Function
{
 public:
  //! No parameters.
  static const uint parameters()
    {
      return 0;
    }

  //! 1 leaf argument supplying rotation angles
  static const uint arguments()
    {
      return 1;
    }

  //! Evaluate function.
  static const XYZ evaluate(const FunctionNode& our,const XYZ& p)
    {
      const XYZ a(our.arg(0)(p)*M_PI);
  
      Matrix33RotateX rx(a.x());
      Matrix33RotateY ry(a.y());
      Matrix33RotateZ rz(a.z());
      
      return XYZ((rx*ry*rz)*p);
    }
  
  //! Is constant if leaf node is.
  static const bool is_constant(const FunctionNode& our)
  {
    return our.arg(0).is_constant();
  }

};

REGISTER(FunctionRotate);

//------------------------------------------------------------------------------------------

class FunctionSin : public Function
{
 public:
  //! No parameters.
  static const uint parameters()
    {
      return 0;
    }

  //! No arguments.
  static const uint arguments()
    {
      return 0;
    }

  //! Evaluate function.
  static const XYZ evaluate(const FunctionNode&,const XYZ& p)
    {
      return XYZ(sin(p.x()),sin(p.y()),sin(p.z()));
    }
  
  //! Isn't constant
  static const bool is_constant(const FunctionNode&)
  {
    return false;
  }

};

REGISTER(FunctionSin);

//------------------------------------------------------------------------------------------

class FunctionCos : public Function
{
 public:
  //! No parameters.
  static const uint parameters()
    {
      return 0;
    }

  //! No arguments.
  static const uint arguments()
    {
      return 0;
    }

  //! Evaluate function.
  static const XYZ evaluate(const FunctionNode&,const XYZ& p)
    {
      return XYZ(cos(p.x()),cos(p.y()),cos(p.z()));
    }
  
  //! Isn't constant
  static const bool is_constant(const FunctionNode&)
  {
    return false;
  }
};

REGISTER(FunctionCos);

//------------------------------------------------------------------------------------------

class FunctionSpiralLinear : public Function
{
 public:
  //! No parameters.
  static const uint parameters()
    {
      return 0;
    }

  //! One argument.
  static const uint arguments()
    {
      return 1;
    }

  //! Evaluate function.
  static const XYZ evaluate(const FunctionNode& our,const XYZ& p)
    {
      const float r=p.magnitude();
      float theta=atan2(p.y(),p.x());
      if (theta<0.0f) theta+=2.0f*M_PI;
      const float winding=floorf(r-theta/(2.0*M_PI));
      
      const float x=2.0f*winding+theta/M_PI;
      const float y=2.0f*r-x;
      
      return our.arg(0)(XYZ(x,y,p.z()));
    }
  
  //! Is constant if the function being spiral-ed is.
  static const bool is_constant(const FunctionNode& our)
    {
      return our.arg(0).is_constant();
    }

};

REGISTER(FunctionSpiralLinear);

//------------------------------------------------------------------------------------------

class FunctionSpiralLogarithmic : public Function
{
 public:
  //! No parameters.
  static const uint parameters()
    {
      return 0;
    }

  //! One argument.
  static const uint arguments()
    {
      return 1;
    }

  //! Evaluate function.
  static const XYZ evaluate(const FunctionNode& our,const XYZ& p)
    {
      const float r=p.magnitude();
      float theta=atan2(p.y(),p.x());
      if (theta<0.0f) theta+=2.0f*M_PI;
      const float lnr=log(r);
      const float winding=floorf(lnr-theta/(2.0*M_PI));
      
      const float x=2.0f*winding+theta/M_PI;
      const float y=2.0f*lnr-x;
      
      return our.arg(0)(XYZ(x,y,p.z()));
    }
  
  //! Is constant if the function being spiral-ed is.
  static const bool is_constant(const FunctionNode& our)
    {
      return our.arg(0).is_constant();
    }
};

REGISTER(FunctionSpiralLogarithmic);

//------------------------------------------------------------------------------------------

class FunctionGradient : public Function
{
 public:
  //! No parameters.
  static const uint parameters()
    {
      return 0;
    }

  //! One argument.
  static const uint arguments()
    {
      return 1;
    }

  //! Evaluate function.
  static const XYZ evaluate(const FunctionNode& our,const XYZ& p)
    {
      const float epsilon=1e-6;
      const XYZ vepsilon(epsilon,epsilon,epsilon);
      
      const XYZ v0(our.arg(0)(p-vepsilon));
      const XYZ v1(our.arg(0)(p+vepsilon));
      return (v1-v0)/(2.0*epsilon);
    }
  
  //! Is constant if the function being gradient-ed is.
  static const bool is_constant(const FunctionNode& our)
    {
      return our.arg(0).is_constant();
    }
};

REGISTER(FunctionGradient);

//------------------------------------------------------------------------------------------

class FunctionComposePair : public Function
{
 public:
  //! No parameters.
  static const uint parameters()
    {
      return 0;
    }

  //! Two arguments.
  static const uint arguments()
    {
      return 2;
    }

  //! Evaluate function.
  static const XYZ evaluate(const FunctionNode& our,const XYZ& p)
    {
      return our.arg(1)(our.arg(0)(p));
    }
  
  //! Is constant if either function is.
  static const bool is_constant(const FunctionNode& our)
    {
      return (our.arg(0).is_constant() || our.arg(1).is_constant());
    }
};

REGISTER(FunctionComposePair);

//------------------------------------------------------------------------------------------

class FunctionComposeTriple : public Function
{
 public:
  //! No parameters.
  static const uint parameters()
    {
      return 0;
    }

  //! Three arguments.
  static const uint arguments()
    {
      return 3;
    }

  //! Evaluate function.
  static const XYZ evaluate(const FunctionNode& our,const XYZ& p)
    {
      return our.arg(2)(our.arg(1)(our.arg(0)(p)));
    }
  
  //! Is constant if any function is.
  static const bool is_constant(const FunctionNode& our)
    {
      return (our.arg(0).is_constant() || our.arg(1).is_constant() || our.arg(2).is_constant());
    }
};

REGISTER(FunctionComposeTriple);

//------------------------------------------------------------------------------------------

class FunctionAdd : public Function
{
 public:
  //! No parameters.
  static const uint parameters()
    {
      return 0;
    }

  //! Two arguments.
  static const uint arguments()
    {
      return 2;
    }

  //! Evaluate function.
  static const XYZ evaluate(const FunctionNode& our,const XYZ& p)
    {
      return our.arg(0)(p)+our.arg(1)(p);
    }
  
  //! Is constant if both leaf functions are.
  static const bool is_constant(const FunctionNode& our)
    {
      return (our.arg(0).is_constant() && our.arg(1).is_constant());
    }
};

REGISTER(FunctionAdd);

//------------------------------------------------------------------------------------------

class FunctionMultiply : public Function
{
 public:
  //! No parameters.
  static const uint parameters()
    {
      return 0;
    }

  //! Two arguments.
  static const uint arguments()
    {
      return 2;
    }

  //! Evaluate function.
  static const XYZ evaluate(const FunctionNode& our,const XYZ& p)
    {
      const XYZ v0(our.arg(0)(p));
      const XYZ v1(our.arg(1)(p));
      return XYZ(v0.x()*v1.x(),v0.y()*v1.y(),v0.z()*v1.z());
    }
  
  //! Is constant if both leaf functions are.
  static const bool is_constant(const FunctionNode& our)
    {
      return (our.arg(0).is_constant() && our.arg(1).is_constant());
    }
};

REGISTER(FunctionMultiply);

//------------------------------------------------------------------------------------------

class FunctionDivide : public Function
{
 public:
  //! No parameters.
  static const uint parameters()
    {
      return 0;
    }

  //! Two arguments.
  static const uint arguments()
    {
      return 2;
    }

  //! Evaluate function.
  static const XYZ evaluate(const FunctionNode& our,const XYZ& p)
    {
      const XYZ v0(our.arg(0)(p));
      const XYZ v1(our.arg(1)(p));

      return XYZ(
		 (v1.x()==0.0 ? 0.0 : v0.x()/v1.x()),
		 (v1.y()==0.0 ? 0.0 : v0.y()/v1.y()),
		 (v1.z()==0.0 ? 0.0 : v0.z()/v1.z())
		 );

    }
  
  //! Is constant if both leaf functions are.
  static const bool is_constant(const FunctionNode& our)
    {
      return (our.arg(0).is_constant() && our.arg(1).is_constant());
    }
};

REGISTER(FunctionDivide);

//------------------------------------------------------------------------------------------

class FunctionCross : public Function
{
 public:
  //! No parameters.
  static const uint parameters()
    {
      return 0;
    }

  //! Two arguments.
  static const uint arguments()
    {
      return 2;
    }

  //! Evaluate function.
  static const XYZ evaluate(const FunctionNode& our,const XYZ& p)
    {
      const XYZ v0(our.arg(0)(p));
      const XYZ v1(our.arg(1)(p));
      return v0*v1;
    }
  
  //! Is constant if both leaf functions are.
  static const bool is_constant(const FunctionNode& our)
    {
      return (our.arg(0).is_constant() && our.arg(1).is_constant());
    }
};

REGISTER(FunctionCross);

//------------------------------------------------------------------------------------------

class FunctionMax : public Function
{
 public:
  //! No parameters.
  static const uint parameters()
    {
      return 0;
    }

  //! Two arguments.
  static const uint arguments()
    {
      return 2;
    }

  //! Evaluate function.
  static const XYZ evaluate(const FunctionNode& our,const XYZ& p)
    {
      const XYZ v0(our.arg(0)(p));
      const XYZ v1(our.arg(1)(p));
      return XYZ(
		 std::max(v0.x(),v1.x()),
		 std::max(v0.y(),v1.y()),
		 std::max(v0.z(),v1.z())
		 );
    }
  
  //! Is constant if both leaf functions are.
  static const bool is_constant(const FunctionNode& our)
    {
      return (our.arg(0).is_constant() && our.arg(1).is_constant());
    }
};

REGISTER(FunctionMax);

//------------------------------------------------------------------------------------------

class FunctionMin : public Function
{
 public:
  //! No parameters.
  static const uint parameters()
    {
      return 0;
    }

  //! Two arguments.
  static const uint arguments()
    {
      return 2;
    }

  //! Evaluate function.
  static const XYZ evaluate(const FunctionNode& our,const XYZ& p)
    {
      const XYZ v0(our.arg(0)(p));
      const XYZ v1(our.arg(1)(p));
      return XYZ(
		 std::min(v0.x(),v1.x()),
		 std::min(v0.y(),v1.y()),
		 std::min(v0.z(),v1.z())
		 );
    }
  
  //! Is constant if both leaf functions are.
  static const bool is_constant(const FunctionNode& our)
    {
      return (our.arg(0).is_constant() && our.arg(1).is_constant());
    }
};

REGISTER(FunctionMin);

//------------------------------------------------------------------------------------------

//! Function returning components of one function modulus thos of another.
/*! Sane always-positive modulus used to avoid funny business at zero.
 */
class FunctionModulus : public Function
{
 public:
  //! No parameters.
  static const uint parameters()
    {
      return 0;
    }

  //! Two arguments.
  static const uint arguments()
    {
      return 2;
    }

  //! Evaluate function.
  static const XYZ evaluate(const FunctionNode& our,const XYZ& p)
    {
      const XYZ v0(our.arg(0)(p));
      const XYZ v1(our.arg(1)(p));
      return XYZ(
		 modulusf(v0.x(),fabs(v1.x())),
		 modulusf(v0.y(),fabs(v1.y())),
		 modulusf(v0.z(),fabs(v1.z()))
		 );
    }
  
  //! Is constant if both leaf functions are.
  static const bool is_constant(const FunctionNode& our)
    {
      return (our.arg(0).is_constant() && our.arg(1).is_constant());
    }
};

REGISTER(FunctionModulus);

//------------------------------------------------------------------------------------------

class FunctionExp : public Function
{
 public:
  //! No parameters.
  static const uint parameters()
    {
      return 0;
    }

  //! No arguments.
  static const uint arguments()
    {
      return 0;
    }

  //! Evaluate function.
  static const XYZ evaluate(const FunctionNode&,const XYZ& p)
    {
      return XYZ(exp(p.x()),exp(p.y()),exp(p.z()));
    }
  
  //! Is not constant.
  static const bool is_constant(const FunctionNode&)
    {
      return false;
    }
};

REGISTER(FunctionExp);

//------------------------------------------------------------------------------------------

//! Invert the leaf function using a radius-one origin centred sphere.
class FunctionGeometricInversion : public Function
{
 public:
  //! No parameters.
  static const uint parameters()
    {
      return 0;
    }

  //! Two arguments.
  static const uint arguments()
    {
      return 1;
    }

  //! Evaluate function.
  static const XYZ evaluate(const FunctionNode& our,const XYZ& p)
    {
      const float radius2=p.magnitude2();
      const XYZ ip(p/radius2);

      return our.arg(0)(ip);
    }
  
  //! Is constant if leaf function is.
  static const bool is_constant(const FunctionNode& our)
    {
      return our.arg(0).is_constant();
    }
};

REGISTER(FunctionGeometricInversion);

//------------------------------------------------------------------------------------------

//! Implements reflection of sampling point about a plane
class FunctionReflect : public Function
{
 public:
  //! No parameters.
  static const uint parameters()
    {
      return 0;
    }

  //! Two arguments.
  static const uint arguments()
    {
      return 3;
    }

  //! Evaluate function.
  static const XYZ evaluate(const FunctionNode& our,const XYZ& p)
    {
      const XYZ pt_in_plane(our.arg(0)(p));
      const XYZ normal(our.arg(1)(p).normalised());
      
      XYZ pos(our.arg(2)(p));
      
      const float distance_from_plane=(pos-pt_in_plane)%normal;
      
      // If pos is on the wrong side of the plane, reflect it over
      if (distance_from_plane<0.0)
	{
	  pos-=(2.0*distance_from_plane)*normal;
	}
      
      return pos;
    }
  
  //! Is constant if sampled function is
  static const bool is_constant(const FunctionNode& our)
    {
      return our.arg(2).is_constant();
    }
};

REGISTER(FunctionReflect);

//------------------------------------------------------------------------------------------

//! Function returns a value comprising the magnitude of three leaf functions.
class FunctionMagnitudes : public Function
{
 public:
  //! No parameters.
  static const uint parameters()
    {
      return 0;
    }

  //! Two arguments.
  static const uint arguments()
    {
      return 3;
    }

  //! Evaluate function.
  static const XYZ evaluate(const FunctionNode& our,const XYZ& p)
    {
      return XYZ(
		 our.arg(0)(p).magnitude(),
		 our.arg(1)(p).magnitude(),
		 our.arg(2)(p).magnitude()
		 );
    }
  
  //! Is constant if all leaf functions are
  static const bool is_constant(const FunctionNode& our)
    {
      return (our.arg(0).is_constant() && our.arg(1).is_constant() && our.arg(2).is_constant());
    }
};

REGISTER(FunctionMagnitudes);

//------------------------------------------------------------------------------------------

//! Function returns position magnitude.
class FunctionMagnitude : public Function
{
 public:
  //! No parameters.
  static const uint parameters()
    {
      return 0;
    }

  //! No arguments.
  static const uint arguments()
    {
      return 0;
    }

  //! Evaluate function.
  static const XYZ evaluate(const FunctionNode&,const XYZ& p)
    {
      const float m=p.magnitude();
      return XYZ(m,m,m);
    }
  
  //! Is not constant.
  static const bool is_constant(const FunctionNode&)
    {
      return false;
    }
};

REGISTER(FunctionMagnitude);

//------------------------------------------------------------------------------------------

//! Function implements selection between 2 functions based on the relative magnitudes of 2 other functions
class FunctionChooseSphere : public Function
{
 public:
  //! No parameters.
  static const uint parameters()
    {
      return 0;
    }

  //! Two arguments.
  static const uint arguments()
    {
      return 4;
    }

  //! Evaluate function.
  static const XYZ evaluate(const FunctionNode& our,const XYZ& p)
    {
      if ((our.arg(0)(p)).magnitude2()<(our.arg(1)(p)).magnitude2())
	return our.arg(2)(p);
      else
	return our.arg(3)(p);
    }
  
  //! Is constant if all leaf functions are
  static const bool is_constant(const FunctionNode& our)
    {
      return (our.arg(0).is_constant() && our.arg(1).is_constant() && our.arg(2).is_constant() && our.arg(3).is_constant());
    }
};

REGISTER(FunctionChooseSphere);

//------------------------------------------------------------------------------------------

//! Function implements selection between 2 functions based on whether a rectangle contains a point
class FunctionChooseRect : public Function
{
 public:
  //! No parameters.
  static const uint parameters()
    {
      return 0;
    }

  //! Two arguments.
  static const uint arguments()
    {
      return 4;
    }

  //! Evaluate function.
  static const XYZ evaluate(const FunctionNode& our,const XYZ& p)
    {
      const XYZ p0(our.arg(0)(p));
      const XYZ p1(our.arg(1)(p));
      
      if (p1.origin_centred_rect_contains(p0))
	return our.arg(2)(p);
      else
	return our.arg(3)(p);
    }
  
  //! Is constant if all leaf functions are
  static const bool is_constant(const FunctionNode& our)
    {
      return (our.arg(0).is_constant() && our.arg(1).is_constant() && our.arg(2).is_constant() && our.arg(3).is_constant());
    }
};

REGISTER(FunctionChooseRect);

//------------------------------------------------------------------------------------------

//! Function implements selection between 2 functions based on position in 3d mesh
class FunctionChooseFrom2InCubeMesh : public Function
{
 public:
  //! No parameters.
  static const uint parameters()
    {
      return 0;
    }

  //! Two arguments.
  static const uint arguments()
    {
      return 2;
    }

  //! Evaluate function.
  static const XYZ evaluate(const FunctionNode& our,const XYZ& p)
    {
      const int x=static_cast<int>(floorf(p.x()));
      const int y=static_cast<int>(floorf(p.y()));
      const int z=static_cast<int>(floorf(p.z()));

      if ((x+y+z)&1)
	return our.arg(0)(p);
      else
	return our.arg(1)(p);
    }
  
  //! Isn't constant (unless leaf functions constant and return same result - unlikely)
  static const bool is_constant(const FunctionNode&)
    {
      return false;
    }
};

REGISTER(FunctionChooseFrom2InCubeMesh);

//------------------------------------------------------------------------------------------

//! Function implements selection between 2 functions based on position in 3d mesh
class FunctionChooseFrom3InCubeMesh : public Function
{
 public:
  //! No parameters.
  static const uint parameters()
    {
      return 0;
    }

  //! Three arguments.
  static const uint arguments()
    {
      return 3;
    }

  //! Evaluate function.
  static const XYZ evaluate(const FunctionNode& our,const XYZ& p)
    {
      const int x=static_cast<int>(floorf(p.x()));
      const int y=static_cast<int>(floorf(p.y()));
      const int z=static_cast<int>(floorf(p.z()));

      return our.arg(modulusi(x+y+z,3))(p);
    }
  
  //! Isn't constant (unless leaf functions constant and return same result - unlikely)
  static const bool is_constant(const FunctionNode&)
    {
      return false;
    }
};

REGISTER(FunctionChooseFrom3InCubeMesh);

//------------------------------------------------------------------------------------------

//! Function implements selection between 2 functions based on position in 2d grid
class FunctionChooseFrom2InSquareGrid : public Function
{
 public:
  //! No parameters.
  static const uint parameters()
    {
      return 0;
    }

  //! Two arguments.
  static const uint arguments()
    {
      return 2;
    }

  //! Evaluate function.
  static const XYZ evaluate(const FunctionNode& our,const XYZ& p)
    {
      const int x=static_cast<int>(floorf(p.x()));
      const int y=static_cast<int>(floorf(p.y()));

      if ((x+y)&1)
	return our.arg(0)(p);
      else
	return our.arg(1)(p);
    }
  
  //! Isn't constant (unless leaf functions constant and return same result - unlikely)
  static const bool is_constant(const FunctionNode&)
    {
      return false;
    }
};

REGISTER(FunctionChooseFrom2InSquareGrid);

//------------------------------------------------------------------------------------------

//! Function implements selection between 2 functions based on position in 2d grid
class FunctionChooseFrom3InSquareGrid : public Function
{
 public:
  //! No parameters.
  static const uint parameters()
    {
      return 0;
    }

  //! Three arguments.
  static const uint arguments()
    {
      return 3;
    }

  //! Evaluate function.
  static const XYZ evaluate(const FunctionNode& our,const XYZ& p)
    {
      const int x=static_cast<int>(floorf(p.x()));
      const int y=static_cast<int>(floorf(p.y()));

      return our.arg(modulusi(x+y,3))(p);
    }
  
  //! Isn't constant (unless leaf functions constant and return same result - unlikely)
  static const bool is_constant(const FunctionNode&)
    {
      return false;
    }
};

REGISTER(FunctionChooseFrom3InSquareGrid);

//------------------------------------------------------------------------------------------

//! Function implements selection between 2 functions based on position in grid of triangles 
class FunctionChooseFrom2InTriangleGrid : public Function
{
 public:
  //! No parameters.
  static const uint parameters()
    {
      return 0;
    }

  //! Two arguments.
  static const uint arguments()
    {
      return 2;
    }

  //! Evaluate function.
  static const XYZ evaluate(const FunctionNode& our,const XYZ& p)
    {
      static const XYZ d0(1.0f         ,0.0f         ,0.0f);
      static const XYZ d1(cos(  M_PI/3),sin(  M_PI/3),0.0f);
      static const XYZ d2(cos(2*M_PI/3),sin(2*M_PI/3),0.0f);
      
      const int a=static_cast<int>(floorf(p%d0));
      const int b=static_cast<int>(floorf(p%d1));
      const int c=static_cast<int>(floorf(p%d2));

      if ((a+b+c)&1)
	return our.arg(0)(p);
      else
	return our.arg(1)(p);
    }
  
  //! Isn't constant (unless leaf functions constant and return same result - unlikely)
  static const bool is_constant(const FunctionNode&)
    {
      return false;
    }
};

REGISTER(FunctionChooseFrom2InTriangleGrid);

//------------------------------------------------------------------------------------------

//! Function implements selection between 2 functions based on position in grid of triangles 
/*! Not entirely sure this one produces a sensible pattern.  Needs explicitly testing.
 */
class FunctionChooseFrom3InTriangleGrid : public Function
{
 public:
  //! No parameters.
  static const uint parameters()
    {
      return 0;
    }

  //! Three arguments.
  static const uint arguments()
    {
      return 3;
    }

  //! Evaluate function.
  static const XYZ evaluate(const FunctionNode& our,const XYZ& p)
    {
      static const XYZ d0(1.0f         ,0.0f         ,0.0f);
      static const XYZ d1(cos(  M_PI/3),sin(  M_PI/3),0.0f);
      static const XYZ d2(cos(2*M_PI/3),sin(2*M_PI/3),0.0f);
      
      const int a=static_cast<int>(floorf(p%d0));
      const int b=static_cast<int>(floorf(p%d1));
      const int c=static_cast<int>(floorf(p%d2));

      return our.arg(modulusi(a+b+c,3))(p);
    }
  
  //! Isn't constant (unless leaf functions constant and return same result - unlikely)
  static const bool is_constant(const FunctionNode&)
    {
      return false;
    }
};

REGISTER(FunctionChooseFrom3InTriangleGrid);

//------------------------------------------------------------------------------------------

//! Function implements selection between 3 functions based on position in grid of hexagons
/*! Don't entirely understand how this works, but it looks nice.
 */
class FunctionChooseFrom3InDiamondGrid : public Function
{
 public:
  //! No parameters.
  static const uint parameters()
    {
      return 0;
    }

  //! Two arguments.
  static const uint arguments()
    {
      return 3;
    }

  //! Evaluate function.
  static const XYZ evaluate(const FunctionNode& our,const XYZ& p)
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
	return our.arg(0)(p);
      else if (m1<=m0 && m1<=m2)
	return our.arg(1)(p);
      else 
	return our.arg(2)(p);
    }
  
  //! Isn't constant (unless leaf functions constant and return same result - unlikely)
  static const bool is_constant(const FunctionNode&)
    {
      return false;
    }
};

REGISTER(FunctionChooseFrom3InDiamondGrid);

//------------------------------------------------------------------------------------------

//! Function implements selection between 3 functions based on position in grid of hexagons
class FunctionChooseFrom3InHexagonGrid : public Function
{
 public:
  //! No parameters.
  static const uint parameters()
    {
      return 0;
    }

  //! Three arguments.
  static const uint arguments()
    {
      return 3;
    }

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
  static const XYZ evaluate(const FunctionNode& our,const XYZ& p)
    {
      int hx;
      int hy;
      nearest_hex(p.x(),p.y(),hx,hy);
      const uint which=hy+((hx&1)? 2 : 0);
      return our.arg(modulusi(which,3))(p);
    }
    
  //! Isn't constant (unless leaf functions constant and return same result - unlikely)
  static const bool is_constant(const FunctionNode&)
    {
      return false;
    }
};

REGISTER(FunctionChooseFrom3InHexagonGrid);

//------------------------------------------------------------------------------------------

//! Function implements selection between 2 functions based on position in grid of hexagons
class FunctionChooseFrom2InBorderedHexagonGrid : public Function
{
 public:
  //! One parameter controlling border width
  static const uint parameters()
    {
      return 1;
    }

  //! Two arguments.
  static const uint arguments()
    {
      return 2;
    }

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
  static const XYZ evaluate(const FunctionNode& our,const XYZ& p)
    {
      int hx;
      int hy;
      nearest_hex(p.x(),p.y(),hx,hy);

      bool in_border=false;

      // Hex centres are separated by 1.0 so limit border size
      const float b=modulusf(our.param(0),0.5f);

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

      return our.arg(in_border)(p);
    }
  
  //! Isn't constant (unless leaf functions constant and return same result - unlikely)
  static const bool is_constant(const FunctionNode&)
    {
      return false;
    }
};

REGISTER(FunctionChooseFrom2InBorderedHexagonGrid);

//------------------------------------------------------------------------------------------

//! Rays intersecting a textured unit sphere
/*! arg(0) is background
    arg(1) is 3D texture for sphere
  param(0,1,2) is light source direction
  p.x, p.y is the 2D position of a ray from infinity travelling in direction (0 0 1)
*/
class FunctionOrthoSphereShaded : public Function
{
 public:
  //! Three parameters.
  static const uint parameters()
    {
      return 3;
    }

  //! Two argument.
  static const uint arguments()
    {
      return 2;
    }

  //! Evaluate function.
  static const XYZ evaluate(const FunctionNode& our,const XYZ& p)
    {
      const float pr2=p.x()*p.x()+p.y()*p.y();
      if (pr2<1.0f)
	{
	  const float z=-sqrt(1.0f-pr2);
	  const XYZ n(p.x(),p.y(),z);

	  const XYZ l(XYZ(our.param(0),our.param(1),our.param(2)).normalised());

	  const float i=0.5*(1.0+l%n); // In range 0-1
	  return i*our.arg(1)(n);
	}
      else
	{
	  return our.arg(0)(p);
	}
    }
  
  //! Can't be constant unless args are constant and equal (unlikely).
  static const bool is_constant(const FunctionNode& our)
    {
      return false;
    }
};

REGISTER(FunctionOrthoSphereShaded);

//------------------------------------------------------------------------------------------

//! Rays reflecting off a unit sphere
/*! arg(0) is background
    arg(1) sampled using a normalised vector defines an environment for reflected rays
  p.x, p.y is the 2D position of a ray from infinity travelling in direction (0 0 1)
*/
class FunctionOrthoSphereReflect : public Function
{
 public:
  //! No parameters.
  static const uint parameters()
    {
      return 0;
    }

  //! Two argument.
  static const uint arguments()
    {
      return 2;
    }

  //! Evaluate function.
  static const XYZ evaluate(const FunctionNode& our,const XYZ& p)
    {
      const float pr2=p.x()*p.x()+p.y()*p.y();
      if (pr2<1.0f)
	{
	  const float z=-sqrt(1.0f-pr2);

	  // This is on surface of unit radius sphere - no need to normalise
	  XYZ n(p.x(),p.y(),z);

	  // The ray _towards_ the viewer is (0 0 -1)
	  // The reflected ray is n-(v-n) = 2n-v
	  const XYZ reflected(2*n-XYZ(0.0f,0.0f,-1.0f));
	  return our.arg(1)(reflected);
	}
      else
	{
	  return our.arg(0)(p);
	}
    }
  
  //! Can't be constant unless args are constant and equal (unlikely).
  static const bool is_constant(const FunctionNode& our)
    {
      return false;
    }
};

REGISTER(FunctionOrthoSphereReflect);

//------------------------------------------------------------------------------------------

//! Function repeatedly applying it's leaf function to the argument
class FunctionIterate : public FunctionIterative
{
 public:
  //! No parameters.
  static const uint parameters()
    {
      return 0;
    }

  //! One argument.
  static const uint arguments()
    {
      return 1;
    }

  //! Evaluate function.
  static const XYZ evaluate(const FunctionNode& our,const XYZ& p)
    {
      XYZ ret(p);
      for (uint i=0;i<our.iterations();i++)
	ret=our.arg(0)(ret);
      return ret;
    }
  
  //! Is constant if leaf function is.
  static const bool is_constant(const FunctionNode& our)
    {
      return our.arg(0).is_constant();
    }
};

REGISTER(FunctionIterate);

//------------------------------------------------------------------------------------------

//! Function returning average value of evenly spaced samples between two points
class FunctionAverageSamples : public FunctionIterative
{
 public:
  //! Three parameters.
  static const uint parameters()
    {
      return 3;
    }

  //! One argument.
  static const uint arguments()
    {
      return 1;
    }

  //! Evaluate function.
  static const XYZ evaluate(const FunctionNode& our,const XYZ& p)
    {
      const XYZ baseline(our.param(0),our.param(1),our.param(2));
     
      XYZ p0;
      XYZ p1;
      XYZ delta;
      
      if (our.iterations()==1)
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
	  delta=(p1-p0)/(our.iterations()-1);
	}

      XYZ ret(0.0f,0.0f,0.0f);
      XYZ ps=p0;

      for (uint i=0;i<our.iterations();i++)
	{
	  ret+=our.arg(0)(ps);
	  ps+=delta;
	}
      ret/=our.iterations();
      return ret;
    }
  
  //! Is constant if sampled leaf function is.
  static const bool is_constant(const FunctionNode& our)
    {
      return our.arg(0).is_constant();
    }
};

REGISTER(FunctionAverageSamples);

//------------------------------------------------------------------------------------------

//! Function similar to FunctionAverageSamples but doing convolution
class FunctionConvolveSamples : public FunctionIterative
{
 public:
  //! Three parameters.
  static const uint parameters()
    {
      return 3;
    }

  //! Two arguments.
  static const uint arguments()
    {
      return 2;
    }

  //! Evaluate function.
  static const XYZ evaluate(const FunctionNode& our,const XYZ& p)
    {
      const XYZ baseline(our.param(0),our.param(1),our.param(2));
     
      XYZ p0;
      XYZ p1;
      XYZ delta;
      
      if (our.iterations()==1)
	{
	  p0=p;
	  p1=p;
	  delta=XYZ(0.0f,0.0f,0.0f);
	}
      else
	{
	  p0=p-baseline;
	  p1=p+baseline;
	  delta=(p1-p0)/our.iterations();
	}

      XYZ ret(0.0f,0.0f,0.0f);
      XYZ ps=p0;

      for (uint i=0;i<our.iterations();i++)
	{
	  ret+=(our.arg(0)(ps)*our.arg(1)(ps));
	  ps+=delta;
	}
      ret/=our.iterations();
      return ret;
    }
  
  //! Is constant if sampled leaf function is.
  static const bool is_constant(const FunctionNode& our)
    {
      return our.arg(0).is_constant();
    }
};

REGISTER(FunctionConvolveSamples);

//------------------------------------------------------------------------------------------

//! Function summing decreasing amounts of higher frequency versions of image
class FunctionAccumulateOctaves : public FunctionIterative
{
 public:
  //! No parameters.
  static const uint parameters()
    {
      return 0;
    }

  //! One argument.
  static const uint arguments()
    {
      return 1;
    }

  //! Evaluate function.
  static const XYZ evaluate(const FunctionNode& our,const XYZ& p)
    {
      XYZ ret(0.0f,0.0f,0.0f);
      float k=0.0f;
      for (uint i=0;i<our.iterations();i++)
	{
	  const float scale=(1<<i);
	  const float iscale=1.0f/scale;
	  ret+=iscale*(our.arg(0)(scale*p));
	  k+=iscale;
	}
      ret/=k;
      return ret;
    }
  
  //! Is constant if sampled leaf function is.
  static const bool is_constant(const FunctionNode& our)
    {
      return our.arg(0).is_constant();
    }
};

REGISTER(FunctionAccumulateOctaves);


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
class FunctionMandelbrotChoose : public FunctionIterative
{
 public:
  //! No parameters.
  static const uint parameters()
    {
      return 0;
    }

  //! Two arguments.
  static const uint arguments()
    {
      return 2;
    }

  //! Evaluate function.
  static const XYZ evaluate(const FunctionNode& our,const XYZ& p)
    {
      return (brot(XYZ(0.0f,0.0f,0.0f),p,our.iterations())==our.iterations() ? our.arg(0)(p) : our.arg(1)(p));
    }
  
  //! Can't be constant unless functions are the same.
  static const bool is_constant(const FunctionNode&)
    {
      return false;
    }
};

REGISTER(FunctionMandelbrotChoose);

//-----------------------------------------------------------------------------------------

//! Function returns -1 for points in set, 0-1 for escaped points
class FunctionMandelbrotContour : public FunctionIterative
{
 public:
  //! No parameters.
  static const uint parameters()
    {
      return 0;
    }

  //! No arguments.
  static const uint arguments()
    {
      return 0;
    }

  //! Evaluate function.
  static const XYZ evaluate(const FunctionNode& our,const XYZ& p)
    {
      const uint i=brot(XYZ(0.0f,0.0f,0.0f),p,our.iterations());
      return (i==our.iterations() ? XYZ::fill(-1.0f) : XYZ::fill(static_cast<float>(i)/our.iterations()));
    }
  
  //! Not constant.
  static const bool is_constant(const FunctionNode&)
    {
      return false;
    }
};

REGISTER(FunctionMandelbrotContour);

//------------------------------------------------------------------------------------------

//! Function selects arg to evaluate based on test for point in Julia set.
class FunctionJuliaChoose : public FunctionIterative
{
 public:
  //! No parameters.
  static const uint parameters()
    {
      return 0;
    }

  //! Three arguments.
  static const uint arguments()
    {
      return 3;
    }

  //! Evaluate function.
  static const XYZ evaluate(const FunctionNode& our,const XYZ& p)
    {
      return (brot(p,our.arg(2)(p),our.iterations())==our.iterations() ? our.arg(0)(p) : our.arg(1)(p));
    }
  
  //! Can't be constant unless functions are the same.
  static const bool is_constant(const FunctionNode&)
    {
      return false;
    }
};

REGISTER(FunctionJuliaChoose);

//------------------------------------------------------------------------------------------

//! Function returns -1 for points in set, 0-1 for escaped points
class FunctionJuliaContour : public FunctionIterative
{
 public:
  //! No paramters.
  static const uint parameters()
    {
      return 0;
    }

  //! One argument.
  static const uint arguments()
    {
      return 1;
    }

  //! Evaluate function.
  static const XYZ evaluate(const FunctionNode& our,const XYZ& p)
    {
      const uint i=brot(p,our.arg(0)(p),our.iterations());
      return (i==our.iterations() ? XYZ::fill(-1.0f) : XYZ::fill(static_cast<float>(i)/our.iterations()));
    }
  
  //! Not constant.
  static const bool is_constant(const FunctionNode&)
    {
      return false;
    }
};

REGISTER(FunctionJuliaContour);

//------------------------------------------------------------------------------------------

#endif
