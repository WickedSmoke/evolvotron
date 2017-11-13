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
  \brief Interfaces and implementation for FunctionNoise
*/

#ifndef _functions_noise_h_
#define _functions_noise_h_

#include "common.h"

#include "function_boilerplate.h"

#include "noise.h"

//------------------------------------------------------------------------------------------


//! Perlin noise function.
/*! Returns a single value replicated into all three channels
*/
FUNCTION_BEGIN(FunctionNoiseOneChannel,0,0,false,0)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      // Crank up the frequency a bit otherwise don't see much variation in base case
      const real v=_noise(2.0*p);
      return XYZ(v,v,v);
    }
  
 protected:
  static Noise _noise;

FUNCTION_END(FunctionNoiseOneChannel)

//------------------------------------------------------------------------------------------

//! Multiscale noise function.
/*! Returns a single value replicated into all three channels
*/
FUNCTION_BEGIN(FunctionMultiscaleNoiseOneChannel,0,0,false,0)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      real t=0.0;
      real tm=0.0;
      for (uint i=0;i<8;i++)
	{
	  const real k=(1<<i);
	  const real ik=1.0/k;
	  t+=ik*_noise(k*p);
	  tm+=ik;
	}
      const real v=t/tm;
      return XYZ(v,v,v);
    }
  
 protected:
  static Noise _noise;

FUNCTION_END(FunctionMultiscaleNoiseOneChannel)

//------------------------------------------------------------------------------------------

//! Perlin noise function.
/*! Returns three independent channels
*/
FUNCTION_BEGIN(FunctionNoiseThreeChannel,0,0,false,0)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      return XYZ(_noise0(p),_noise1(p),_noise2(p));
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
FUNCTION_BEGIN(FunctionMultiscaleNoiseThreeChannel,0,0,false,0)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      XYZ t(0.0,0.0,0.0);
      real tm=0.0;
      for (uint i=0;i<8;i++)
	{
	  const real k=(1<<i);
	  const real ik=1.0/k;
	  const XYZ kp(k*p);
	  t+=ik*XYZ(_noise0(kp),_noise1(kp),_noise2(kp));
	  tm+=ik;
	}
      return t/tm;
    }
  
 protected:
  static Noise _noise0;
  static Noise _noise1;
  static Noise _noise2;

FUNCTION_END(FunctionMultiscaleNoiseThreeChannel)

#endif
