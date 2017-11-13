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

#ifndef _functions_arithmetic_h_
#define _functions_arithmetic_h_

#include "common.h" 

#include "function_boilerplate.h"

//------------------------------------------------------------------------------------------

FUNCTION_BEGIN(FunctionAdd,0,2,false,0)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      return arg(0)(p)+arg(1)(p);
    }
  
FUNCTION_END(FunctionAdd)

//------------------------------------------------------------------------------------------

FUNCTION_BEGIN(FunctionMultiply,0,2,false,0)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      const XYZ v0(arg(0)(p));
      const XYZ v1(arg(1)(p));
      // NB Don't use v0*v1 as it would be cross-product.
      return XYZ(v0.x()*v1.x(),v0.y()*v1.y(),v0.z()*v1.z());
    }
  
FUNCTION_END(FunctionMultiply)

//------------------------------------------------------------------------------------------

FUNCTION_BEGIN(FunctionDivide,0,2,false,0)

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
  
FUNCTION_END(FunctionDivide)

//------------------------------------------------------------------------------------------

FUNCTION_BEGIN(FunctionMax,0,2,false,0)

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
  
FUNCTION_END(FunctionMax)

//------------------------------------------------------------------------------------------

FUNCTION_BEGIN(FunctionMin,0,2,false,0)

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
  
FUNCTION_END(FunctionMin)

//------------------------------------------------------------------------------------------

//! Function returning components of one function modulus thos of another.
/*! Sane always-positive modulus used to avoid funny business at zero.
 */
FUNCTION_BEGIN(FunctionModulus,0,2,false,0)

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
  
FUNCTION_END(FunctionModulus)

//------------------------------------------------------------------------------------------

FUNCTION_BEGIN(FunctionExp,0,0,false,0)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      return XYZ(exp(p.x()),exp(p.y()),exp(p.z()));
    }
  
FUNCTION_END(FunctionExp)

//------------------------------------------------------------------------------------------

FUNCTION_BEGIN(FunctionSin,0,0,false,0)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      return XYZ(sin(p.x()),sin(p.y()),sin(p.z()));
    }
  
FUNCTION_END(FunctionSin)

//------------------------------------------------------------------------------------------

FUNCTION_BEGIN(FunctionCos,0,0,false,0)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      return XYZ(cos(p.x()),cos(p.y()),cos(p.z()));
    }
  
FUNCTION_END(FunctionCos)

//------------------------------------------------------------------------------------------

#endif
