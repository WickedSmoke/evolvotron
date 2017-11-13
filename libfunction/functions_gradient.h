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

#ifndef _functions_gradient_h_
#define _functions_gradient_h_

#include "common.h" 

#include "function_boilerplate.h"

//------------------------------------------------------------------------------------------

FUNCTION_BEGIN(FunctionDerivative,3,1,false,0)
     
  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      const XYZ d(epsilon()*XYZ(param(0),param(1),param(2)).normalised());
      
      const XYZ v0(arg(0)(p-d));
      const XYZ v1(arg(0)(p+d));

      return (v1-v0)*inv_epsilon2();
    }
  
FUNCTION_END(FunctionDerivative)

//------------------------------------------------------------------------------------------

FUNCTION_BEGIN(FunctionDerivativeGeneralised,0,2,false,0)
     
  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      const XYZ d(epsilon()*(arg(1)(p)).normalised());
      
      const XYZ v0(arg(0)(p-d));
      const XYZ v1(arg(0)(p+d));

      return (v1-v0)*inv_epsilon2();
    }
  
FUNCTION_END(FunctionDerivativeGeneralised)

//------------------------------------------------------------------------------------------

FUNCTION_BEGIN(FunctionGradient,3,1,false,0)
     
  //! Evaluate function.
  /*! Gradient converts scalar to vector, so need a scalar to work on.
   */
  virtual const XYZ evaluate(const XYZ& p) const
    {
      const XYZ k(param(0),param(1),param(2));

      const real vx0=k%arg(0)(p-XYZ(epsilon(),0.0,0.0));
      const real vy0=k%arg(0)(p-XYZ(0.0,epsilon(),0.0));
      const real vz0=k%arg(0)(p-XYZ(0.0,0.0,epsilon()));

      const real vx1=k%arg(0)(p+XYZ(epsilon(),0.0,0.0));
      const real vy1=k%arg(0)(p+XYZ(0.0,epsilon(),0.0));
      const real vz1=k%arg(0)(p+XYZ(0.0,0.0,epsilon()));

      return XYZ(vx1-vx0,vy1-vy0,vz1-vz0)*inv_epsilon2();
    }
  
FUNCTION_END(FunctionGradient)

//------------------------------------------------------------------------------------------

FUNCTION_BEGIN(FunctionGradientGeneralised,0,2,false,0)
     
  //! Evaluate function.
  /*! Gradient converts scalar to vector, so need a scalar to work on.
   */
  virtual const XYZ evaluate(const XYZ& p) const
    {
      const XYZ k(arg(1)(p));

      const real vx0=k%arg(0)(p-XYZ(epsilon(),0.0,0.0));
      const real vy0=k%arg(0)(p-XYZ(0.0,epsilon(),0.0));
      const real vz0=k%arg(0)(p-XYZ(0.0,0.0,epsilon()));

      const real vx1=k%arg(0)(p+XYZ(epsilon(),0.0,0.0));
      const real vy1=k%arg(0)(p+XYZ(0.0,epsilon(),0.0));
      const real vz1=k%arg(0)(p+XYZ(0.0,0.0,epsilon()));

      return XYZ(vx1-vx0,vy1-vy0,vz1-vz0)*inv_epsilon2();
    }
  
FUNCTION_END(FunctionGradientGeneralised)

//------------------------------------------------------------------------------------------

FUNCTION_BEGIN(FunctionDivergence,0,1,false,0)

  //! Evaluate function.
  /*! Divergence maps scalar to a scalar, so no problem doing vector->vector.
   */
  virtual const XYZ evaluate(const XYZ& p) const
    {      
      const XYZ vx0(arg(0)(p-XYZ(epsilon(),0.0,0.0)));
      const XYZ vy0(arg(0)(p-XYZ(0.0,epsilon(),0.0)));
      const XYZ vz0(arg(0)(p-XYZ(0.0,0.0,epsilon())));

      const XYZ vx1(arg(0)(p+XYZ(epsilon(),0.0,0.0)));
      const XYZ vy1(arg(0)(p+XYZ(0.0,epsilon(),0.0)));
      const XYZ vz1(arg(0)(p+XYZ(0.0,0.0,epsilon())));

      return (vx1-vx0+vy1-vy0+vz1-vz0)*inv_epsilon2();
    }
  
FUNCTION_END(FunctionDivergence)

//------------------------------------------------------------------------------------------

FUNCTION_BEGIN(FunctionCurl,0,1,false,0)

  //! Evaluate function.
  /*! Curl maps vector to vector, which is what we want.
   */
  virtual const XYZ evaluate(const XYZ& p) const
    {
      const XYZ vx0(arg(0)(p-XYZ(epsilon(),0.0,0.0)));
      const XYZ vy0(arg(0)(p-XYZ(0.0,epsilon(),0.0)));
      const XYZ vz0(arg(0)(p-XYZ(0.0,0.0,epsilon())));

      const XYZ vx1(arg(0)(p+XYZ(epsilon(),0.0,0.0)));
      const XYZ vy1(arg(0)(p+XYZ(0.0,epsilon(),0.0)));
      const XYZ vz1(arg(0)(p+XYZ(0.0,0.0,epsilon())));

      const XYZ d_dx((vx1-vx0)*inv_epsilon2());
      const XYZ d_dy((vy1-vy0)*inv_epsilon2());
      const XYZ d_dz((vz1-vz0)*inv_epsilon2());

      const real dzdy=d_dy.z();
      const real dydz=d_dz.y();

      const real dxdz=d_dz.x();
      const real dzdx=d_dx.z();

      const real dydx=d_dx.y();
      const real dxdy=d_dy.x();

      return XYZ
	(
	 dzdy-dydz,
	 dxdz-dzdx,
	 dydx-dxdy
	 );
    }
  
FUNCTION_END(FunctionCurl)

//------------------------------------------------------------------------------------------

FUNCTION_BEGIN(FunctionScalarLaplacian,0,1,false,0)
     
  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      // Need to use a bigger baseline to avoid noise being amplified
      const XYZ vx0(arg(0)(p-XYZ(big_epsilon(),0.0,0.0)));
      const XYZ vy0(arg(0)(p-XYZ(0.0,big_epsilon(),0.0)));
      const XYZ vz0(arg(0)(p-XYZ(0.0,0.0,big_epsilon())));

      const XYZ v(arg(0)(p));

      const XYZ vx1(arg(0)(p+XYZ(big_epsilon(),0.0,0.0)));
      const XYZ vy1(arg(0)(p+XYZ(0.0,big_epsilon(),0.0)));
      const XYZ vz1(arg(0)(p+XYZ(0.0,0.0,big_epsilon())));

      const XYZ dx0(v-vx0);
      const XYZ dy0(v-vy0);
      const XYZ dz0(v-vz0);

      const XYZ dx1(vx1-v);
      const XYZ dy1(vy1-v);
      const XYZ dz1(vz1-v);

      return XYZ(dx1-dx0+dy1-dy0+dz1-dz0)/(big_epsilon()*big_epsilon());
    }
  
FUNCTION_END(FunctionScalarLaplacian)

//------------------------------------------------------------------------------------------

#endif
