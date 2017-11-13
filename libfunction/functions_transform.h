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

#ifndef _functions_transform_h_
#define _functions_transform_h_

#include "common.h"

#include "function_boilerplate.h"

#include "transform.h"

//------------------------------------------------------

//! Function class returning simply scaled position
FUNCTION_BEGIN(FunctionIsotropicScale,1,0,false,0)

  //! Return the evaluation of arg(0) at the transformed position argument.
  virtual const XYZ evaluate(const XYZ& p) const
  {
    return param(0)*p;
  }

FUNCTION_END(FunctionIsotropicScale)

//------------------------------------------------------------------------------------------

//! Function class returning leaf node evaluated at position transfomed by a 12-component linear transform.
/*! Unlike FunctionPreTransform, the basis vectors for the transform are not fixed but determined from leaf functions
 */
FUNCTION_BEGIN(FunctionPreTransformGeneralised,0,5,false,0)

  //! Return the evaluation of arg(0) at the transformed position argument.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      const Transform transform(arg(1)(p),arg(2)(p),arg(3)(p),arg(4)(p));
      return arg(0)(transform.transformed(p));
    }

FUNCTION_END(FunctionPreTransformGeneralised)

//------------------------------------------------------------------------------------------

//! Function class returning leaf node evaluated at given position; result is then transfomed by a 12-component linear transform.
/*! Unlike FunctionPostTransform, the basis vectors for the transform are not fixed but determined from leaf functions
 */
FUNCTION_BEGIN(FunctionPostTransformGeneralised,0,5,false,0)

  //! Return the evaluation of arg(0) at the transformed position argument.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      const Transform transform(arg(1)(p),arg(2)(p),arg(3)(p),arg(4)(p));
      return transform.transformed(arg(0)(p));
    }

FUNCTION_END(FunctionPostTransformGeneralised)

//------------------------------------------------------------------------------------------

//! Transforms position transformed by a 30 paramter quadratic transform.
/*! This used to be a core function but it doesn't look that great.
 */
FUNCTION_BEGIN(FunctionTransformQuadratic,30,0,false,0)

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

FUNCTION_END(FunctionTransformQuadratic)

//------------------------------------------------------------------------------------------

FUNCTION_BEGIN(FunctionRotate,0,1,false,0)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      const XYZ a(arg(0)(p)*M_PI);
      
      const TransformRotateX rx(a.x());
      const TransformRotateY ry(a.y());
      const TransformRotateZ rz(a.z());
      
      return rx*(ry*(rz*p));
    }
  
FUNCTION_END(FunctionRotate)

//------------------------------------------------------------------------------------------

#endif
