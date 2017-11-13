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
Use this for functions under development, but they should really be split
out into separate files eventually.
*/

#ifndef _functions_misc_h_
#define _functions_misc_h_

#include "common.h"

#include "function_boilerplate.h"

//------------------------------------------------------------------------------------------

//! Multiply x and y by z.
FUNCTION_BEGIN(FunctionCone,0,0,false,0)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      return XYZ(p.x()*p.z(),p.y()*p.z(),p.z());
    }
  
FUNCTION_END(FunctionCone)

//------------------------------------------------------------------------------------------

//! Multiply x and y by exp(z).
FUNCTION_BEGIN(FunctionExpCone,0,0,false,0)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      const real k=exp(p.z());
      return XYZ(p.x()*k,p.y()*k,p.z());
    }
  
FUNCTION_END(FunctionExpCone)

//------------------------------------------------------------------------------------------

//! Separate influence of z co-ordinate.
/*! Interesting as a top level node for animations as structure will tend to be fixed, with only colour map changing
 */
FUNCTION_BEGIN(FunctionSeparateZ,3,2,false,0)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      const XYZ v=arg(0)(XYZ(p.x(),p.y(),0.0));
      return arg(1)(v+p.z()*XYZ(param(0),param(1),param(2)));
    }
  
FUNCTION_END(FunctionSeparateZ)

//------------------------------------------------------------------------------------------

//! Function repeatedly applying it's leaf function to the argument
FUNCTION_BEGIN(FunctionIterate,0,1,true,FnIterative)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      XYZ ret(p);
      for (uint i=0;i<iterations();i++)
	ret=arg(0)(ret);
      return ret;
    }
  
FUNCTION_END(FunctionIterate)

//------------------------------------------------------------------------------------------


//! \todo Something like Conway's Game of Life or a reaction-diffusion system, seeded from an initial function.
/*! Probably operate on an iterations()xiterations()xiterations() grid.
  Need ability to cache state at a particular time by identity (address-of?) of "our".
  WILL cause slow start-up due to precompute, although compute-on-demand should help a bit.
  Locking method to check it is valid before compute starts.  Read during main evaluation should be lock-free.
 */
/*
FUNCTION_BEGIN(FunctionCellular,0,1,true,FnIterative)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p)
    {
      return XYZ(0.0,0.0,0.0);
    }
  
 protected:

};
*/
// Not done yet.
//REGISTER(FunctionCellular);

//------------------------------------------------------------------------------------------

#endif
