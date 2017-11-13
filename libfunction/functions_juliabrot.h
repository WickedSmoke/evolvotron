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

#ifndef _functions_juliabrot_h_
#define _functions_juliabrot_h_

#include "common.h"

#include "function_boilerplate.h"

//------------------------------------------------------------------------------------------

//! Mandelbrot/Julia iterator for fractal functions.
/*! Returns i in 0 to iterations inclusive.  i==iterations implies "in" set.
 */
inline uint brot(const real z0r,const real z0i,const real cr,const real ci,const uint iterations)
{
  real zr=z0r;
  real zi=z0i;
  uint i;
  for (i=0;i<iterations;i++)
    {
      const real zr2=zr*zr;
      const real zi2=zi*zi;
      
      if (zr2+zi2>4.0)
	break;
      
      const real nzr=zr2-zi2+cr;
      const real nzi=2.0*zr*zi+ci;

      zr=nzr;
      zi=nzi;
    }

  return i;
}

//------------------------------------------------------------------------------------------

//! Function selects arg to evaluate based on test for point in Mandelbrot set.
FUNCTION_BEGIN(FunctionMandelbrotChoose,0,2,true,FnIterative|FnFractal)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      return (brot(0.0,0.0,p.x(),p.y(),iterations())==iterations() ? arg(0)(p) : arg(1)(p));
    }
  
FUNCTION_END(FunctionMandelbrotChoose)

//-----------------------------------------------------------------------------------------

//! Function returns -1 for points in set, 0-1 for escaped points
FUNCTION_BEGIN(FunctionMandelbrotContour,0,0,true,FnIterative|FnFractal)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      const uint i=brot(0.0,0.0,p.x(),p.y(),iterations());
      return (i==iterations() ? XYZ::fill(-1.0) : XYZ::fill(static_cast<real>(i)/iterations()));
    }
  
FUNCTION_END(FunctionMandelbrotContour)

//------------------------------------------------------------------------------------------

//! Function selects arg to evaluate based on test for point in Julia set.
FUNCTION_BEGIN(FunctionJuliaChoose,2,2,true,FnIterative|FnFractal)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      return (brot(p.x(),p.y(),param(0),param(1),iterations())==iterations() ? arg(0)(p) : arg(1)(p));
    }
  
FUNCTION_END(FunctionJuliaChoose)

//------------------------------------------------------------------------------------------

//! Function returns -1 for points in set, 0-1 for escaped points
FUNCTION_BEGIN(FunctionJuliaContour,2,0,true,FnIterative|FnFractal)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      const uint i=brot(p.x(),p.y(),param(0),param(1),iterations());
      return (i==iterations() ? XYZ::fill(-1.0) : XYZ::fill(static_cast<real>(i)/iterations()));
    }
  
FUNCTION_END(FunctionJuliaContour)

//------------------------------------------------------------------------------------------

//! Function selects arg to evaluate based on test for point in Juliabrot set.
/*! Juliabrot is 4 dimensional, but we only have 3 incoming parameters,
  so have 4 4d-basis vector parameters.
 */
FUNCTION_BEGIN(FunctionJuliabrotChoose,16,2,true,FnIterative|FnFractal)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      const real zr=p.x()*param( 0)+p.y()*param( 1)+p.z()*param( 2)+param( 3);
      const real zi=p.x()*param( 4)+p.y()*param( 5)+p.z()*param( 6)+param( 7);
      const real cr=p.x()*param( 8)+p.y()*param( 9)+p.z()*param(10)+param(11);
      const real ci=p.x()*param(12)+p.y()*param(13)+p.z()*param(14)+param(15);
      return (brot(zr,zi,cr,ci,iterations())==iterations() ? arg(0)(p) : arg(1)(p));
    }
  
FUNCTION_END(FunctionJuliabrotChoose)

//------------------------------------------------------------------------------------------

//! Function returns -1 for points in set, 0-1 for escaped points
/*! Juliabrot is 4 dimensional, but we only have 3 incoming parameters,
  so have 4 4d-basis vector parameters.
 */
FUNCTION_BEGIN(FunctionJuliabrotContour,16,0,true,FnIterative|FnFractal)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      const real zr=p.x()*param( 0)+p.y()*param( 1)+p.z()*param( 2)+param( 3);
      const real zi=p.x()*param( 4)+p.y()*param( 5)+p.z()*param( 6)+param( 7);
      const real cr=p.x()*param( 8)+p.y()*param( 9)+p.z()*param(10)+param(11);
      const real ci=p.x()*param(12)+p.y()*param(13)+p.z()*param(14)+param(15);
      const uint i=brot(zr,zi,cr,ci,iterations());
      return (i==iterations() ? XYZ::fill(-1.0) : XYZ::fill(static_cast<real>(i)/iterations()));
    }
  
FUNCTION_END(FunctionJuliabrotContour)

//------------------------------------------------------------------------------------------

#endif
