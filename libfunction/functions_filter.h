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

#ifndef _functions_filter_h_
#define _functions_filter_h_

#include "common.h" 

#include "function_boilerplate.h"

//------------------------------------------------------------------------------------------

FUNCTION_BEGIN(FunctionFilter2D,2,1,false,0)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      return
	arg(0)(p)
	-(
	  arg(0)(p+XYZ(param(0),0.0,0.0))
	  +arg(0)(p+XYZ(-param(0),0.0,0.0))
	  +arg(0)(p+XYZ(0.0,param(1),0.0))
	  +arg(0)(p+XYZ(0.0,-param(1),0.0))
	  )/4.0;
    }
  
FUNCTION_END(FunctionFilter2D)

//------------------------------------------------------------------------------------------

FUNCTION_BEGIN(FunctionFilter3D,3,1,false,0)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      return
	arg(0)(p)
	-(
	  arg(0)(p+XYZ(param(0),0.0,0.0))
	  +arg(0)(p+XYZ(-param(0),0.0,0.0))
	  +arg(0)(p+XYZ(0.0,param(1),0.0))
	  +arg(0)(p+XYZ(0.0,-param(1),0.0))
	  +arg(0)(p+XYZ(0.0,0.0,param(2)))
	  +arg(0)(p+XYZ(0.0,0.0,-param(2)))
	  )/6.0;
    }
  
FUNCTION_END(FunctionFilter3D)

//------------------------------------------------------------------------------------------

//! Function returning average value of evenly spaced samples between two points
FUNCTION_BEGIN(FunctionAverageSamples,3,1,true,FnIterative)

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
	  delta=XYZ(0.0,0.0,0.0);
	}
      else
	{
	  // In the case of two iterations the samples will be at p0 and p1
	  p0=p-baseline;
	  p1=p+baseline;
	  delta=(p1-p0)/(iterations()-1);
	}

      XYZ ret(0.0,0.0,0.0);
      XYZ ps=p0;

      for (uint i=0;i<iterations();i++)
	{
	  ret+=arg(0)(ps);
	  ps+=delta;
	}
      ret/=iterations();
      return ret;
    }
  
FUNCTION_END(FunctionAverageSamples)

//------------------------------------------------------------------------------------------

//! Similar to average samples except one end has a higher weighting
FUNCTION_BEGIN(FunctionStreak,3,1,true,FnIterative)

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
	  delta=XYZ(0.0,0.0,0.0);
	}
      else
	{
	  p0=p;
	  p1=p+baseline;
	  delta=(p1-p0)/(iterations()-1);
	}

      XYZ ret(0.0,0.0,0.0);
      XYZ ps=p0;
      real w=0.0;

      for (uint i=0;i<iterations();i++)
	{
	  const real k=1.0-static_cast<real>(i)/iterations();
	  ret+=k*arg(0)(ps);
	  w+=k;
	  ps+=delta;
	}
      ret/=w;
      return ret;
    }
  
FUNCTION_END(FunctionStreak)

//------------------------------------------------------------------------------------------

//! Average of samples around a ring
FUNCTION_BEGIN(FunctionAverageRing,1,1,true,FnIterative)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      if (iterations()==1) return arg(0)(p);

      const real da=2.0*M_PI/iterations();
      XYZ ret(0.0,0.0,0.0);
      for (uint i=0;i<iterations();i++)
	{
	  const real a=i*da;
	  const XYZ delta(param(0)*cos(a),param(0)*sin(a),0.0);
	  ret+=arg(0)(p+delta);
	}
      return ret/iterations();
    }
  
FUNCTION_END(FunctionAverageRing)

//------------------------------------------------------------------------------------------

//! Like FunctionAverageRing but subtract off the centre value
FUNCTION_BEGIN(FunctionFilterRing,1,1,true,FnIterative)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      if (iterations()==1) return XYZ(0.0,0.0,0.0);

      const real da=2.0*M_PI/iterations();
      XYZ ret(0.0,0.0,0.0);
      for (uint i=0;i<iterations();i++)
	{
	  const real a=i*da;
	  const XYZ delta(param(0)*cos(a),param(0)*sin(a),0.0);
	  ret+=arg(0)(p+delta);
	}
      return ret/iterations()-arg(0)(p);
    }
  
FUNCTION_END(FunctionFilterRing)

//------------------------------------------------------------------------------------------

//! Function similar to FunctionAverageSamples but doing convolution
FUNCTION_BEGIN(FunctionConvolveSamples,3,2,true,FnIterative)

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
	  delta=XYZ(0.0,0.0,0.0);
	}
      else
	{
	  p0=p-baseline;
	  p1=p+baseline;
	  delta=(p1-p0)/(iterations()-1);
	}

      XYZ ret(0.0,0.0,0.0);
      XYZ pd(0.0,0.0,0.0);

      for (uint i=0;i<iterations();i++)
	{
	  //! \todo Hmmm.. this is cross product, not inner product
	  ret+=(arg(0)(p+pd)*arg(1)(pd));
	  pd+=delta;
	}
      ret/=iterations();
      return ret;
    }
  
FUNCTION_END(FunctionConvolveSamples)

//------------------------------------------------------------------------------------------

//! Function summing decreasing amounts of higher frequency versions of image
FUNCTION_BEGIN(FunctionAccumulateOctaves,0,1,true,FnIterative)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      XYZ ret(0.0,0.0,0.0);
      real k=0.0;
      for (uint i=0;i<iterations();i++)
	{
	  const real scale=(1<<i);
	  const real iscale=1.0/scale;
	  ret+=iscale*(arg(0)(scale*p));
	  k+=iscale;
	}
      ret/=k;
      return ret;
    }
  
FUNCTION_END(FunctionAccumulateOctaves)

//------------------------------------------------------------------------------------------

#endif

