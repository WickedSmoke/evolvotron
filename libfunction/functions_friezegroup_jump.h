// Source file for evolvotron
// Copyright (C) 2007 Tim Day 
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
*/

#ifndef _functions_friezegroup_jump_h_
#define _functions_friezegroup_jump_h_

/* Jump (Conway p1m1): horizontal reflection only
   Just cycle x range and reflect in y.

     o    o    o
   ---  ---  ---
   ---  ---  ---
     o    o    o

*/

//------------------------------------------------------------------------------------------

FUNCTION_BEGIN(FunctionFriezeGroupJump,1,1,false,FnStructure)

  virtual const XYZ evaluate(const XYZ& p) const
    {
      const real x=modulusf(p.x(),1.0);
      const real y=fabs(p.y());
      return arg(0)(XYZ(x,y,param(0)));
    }
  
FUNCTION_END(FunctionFriezeGroupJump)

//------------------------------------------------------------------------------------------

FUNCTION_BEGIN(FunctionFriezeGroupJumpWarped,4,2,false,FnStructure)

  virtual const XYZ evaluate(const XYZ& p) const
    {
      const real dx=XYZ(param(0),param(1),param(2))%arg(1)(XYZ(0.0,fabs(p.y()),0.0));
      const real x=modulusf(p.x()-dx,1.0)+dx;
      const real y=fabs(p.y());
      return arg(0)(XYZ(x,y,param(3)));
    }

  virtual const bool is_constant() const
    {
      return arg(0).is_constant();  // Warp function makes no difference if primary argument is constant
    }
  
FUNCTION_END(FunctionFriezeGroupJumpWarped)

//------------------------------------------------------------------------------------------

FUNCTION_BEGIN(FunctionFriezeGroupJumpFreeZ,0,1,false,FnStructure)

  virtual const XYZ evaluate(const XYZ& p) const
    {
      const real x=modulusf(p.x(),1.0);
      const real y=fabs(p.y());
      return arg(0)(XYZ(x,y,p.z()));
    }
  
FUNCTION_END(FunctionFriezeGroupJumpFreeZ)

//------------------------------------------------------------------------------------------

FUNCTION_BEGIN(FunctionFriezeGroupJumpWarpedFreeZ,3,2,false,FnStructure)

  virtual const XYZ evaluate(const XYZ& p) const
    {
      const real dx=XYZ(param(0),param(1),param(2))%arg(1)(XYZ(0.0,fabs(p.y()),0.0));
      const real x=modulusf(p.x()-dx,1.0)+dx;
      const real y=fabs(p.y());
      return arg(0)(XYZ(x,y,p.z()));
    }
  
  virtual const bool is_constant() const
    {
      return arg(0).is_constant();  // Warp function makes no difference if primary argument is constant
    }

FUNCTION_END(FunctionFriezeGroupJumpWarpedFreeZ)

//------------------------------------------------------------------------------------------

#endif
