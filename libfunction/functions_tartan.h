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
  \brief Functions inspired by tartan patterns.
*/

#ifndef _functions_tartan_h_
#define _functions_tartan_h_

#include "common.h"

#include "function_boilerplate.h"

//------------------------------------------------------------------------------------------

FUNCTION_BEGIN(FunctionTartanSelectFree,10,6,false,FnStructure)

  //! Evaluate function.
  /*! Sign of one 1D function's dot product determines one bit, ditto for another bit.  
      2 bits used to select from 4 possibilities.
      There's no guarantee of a repetitive pattern unless the generator functions are.
   */
  virtual const XYZ evaluate(const XYZ& p) const
    {
      const XYZ p0(p.x(),param(0),param(1));
      const XYZ p1(param(2),p.y(),param(3));
      const XYZ d0(param(4),param(5),param(6));
      const XYZ d1(param(7),param(8),param(9));
      const int b0=(arg(0)(p0)%XYZ(d0)>0.0);
      const int b1=(arg(1)(p1)%XYZ(d1)>0.0);
      const int which=2+b0+2*b1;
      assert(2<=which && which<6);
      return arg(which)(p);
    }
  
FUNCTION_END(FunctionTartanSelectFree)

//------------------------------------------------------------------------------------------

FUNCTION_BEGIN(FunctionTartanSelect,14,6,false,FnStructure)

  //! Evaluate function.
  /*! Similar to function free except the generators repeat.
   */
  virtual const XYZ evaluate(const XYZ& p) const
    {
      const real x=(param(0)>0.0 ? modulusf(p.x(),param(1)) : trianglef(p.x(),param(1)));
      const real y=(param(2)>0.0 ? modulusf(p.y(),param(3)) : trianglef(p.y(),param(3)));
      const XYZ p0(x,param(4),param(5));
      const XYZ p1(param(6),y,param(7));
      const XYZ d0(param(8),param(9),param(10));
      const XYZ d1(param(11),param(12),param(13));
      const int b0=(arg(0)(p0)%XYZ(d0)>0.0);
      const int b1=(arg(1)(p1)%XYZ(d1)>0.0);
      const int which=2+b0+2*b1;
      assert(2<=which && which<6);
      return arg(which)(p);
    }
  
FUNCTION_END(FunctionTartanSelect)

//------------------------------------------------------------------------------------------

FUNCTION_BEGIN(FunctionTartanSelectRepeat,14,6,false,FnStructure)

  //! Evaluate function.
  /*! Similar to above function except the invoked functions repeat too.
   */
  virtual const XYZ evaluate(const XYZ& p) const
    {
      const real x=(param(0)>0.0 ? modulusf(p.x(),param(1)) : trianglef(p.x(),param(1)));
      const real y=(param(2)>0.0 ? modulusf(p.y(),param(3)) : trianglef(p.y(),param(3)));
      const XYZ p0(x,param(4),param(5));
      const XYZ p1(param(6),y,param(7));
      const XYZ d0(param(8),param(9),param(10));
      const XYZ d1(param(11),param(12),param(13));
      const int b0=(arg(0)(p0)%XYZ(d0)>0.0);
      const int b1=(arg(1)(p1)%XYZ(d1)>0.0);
      const int which=2+b0+2*b1;
      assert(2<=which && which<6);
      return arg(which)(XYZ(x,y,p.z()));
    }
  
FUNCTION_END(FunctionTartanSelectRepeat)

//------------------------------------------------------------------------------------------

FUNCTION_BEGIN(FunctionTartanMixFree,4,2,false,0)

  //! Evaluate function.
  /*! As above, but mix 2 functions.
   */
  virtual const XYZ evaluate(const XYZ& p) const
    {
      const XYZ p0(p.x(),param(0),param(1));
      const XYZ p1(param(2),p.y(),param(3));
      const XYZ warp(arg(0)(p0));
      const XYZ weft(arg(1)(p1));
      return 0.5*(warp+weft);
    }
  
FUNCTION_END(FunctionTartanMixFree)

//------------------------------------------------------------------------------------------

FUNCTION_BEGIN(FunctionTartanMixRepeat,8,2,false,0)

  //! Evaluate function.
  /*! As above, but mix 2 functions.
   */
  virtual const XYZ evaluate(const XYZ& p) const
    {
      const real x=(param(0)>0.0 ? modulusf(p.x(),param(1)) : trianglef(p.x(),param(1)));
      const real y=(param(2)>0.0 ? modulusf(p.y(),param(3)) : trianglef(p.y(),param(3)));
      const XYZ p0(x,param(4),param(5));
      const XYZ p1(param(6),y,param(7));
      const XYZ warp(arg(0)(p0));
      const XYZ weft(arg(1)(p1));
      return 0.5*(warp+weft);
    }
  
FUNCTION_END(FunctionTartanMixRepeat)

//------------------------------------------------------------------------------------------

#endif
