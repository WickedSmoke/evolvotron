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

#include "friezegroup.h"

//! Jump (Conway p1m1): horizontal reflection only
/*! Just cycle x range and reflect in y.
\verbatim
     o    o    o
   ---  ---  ---
   ---  ---  ---
     o    o    o
\endverbatim
*/
struct Jump
{
  const XY operator()(const XY& p) const
    {
      return XY
	(
	 modulusf(p.x(),1.0),
	 fabs(p.y())
	 );
    }
};

struct JumpInvariant
{
  JumpInvariant(const Function& f,const XYZ& k)
    :_f(f)
    ,_k(k)
  {}
  const std::pair<XY,XY> operator()(const XY& p) const
  {
    // A distortion dependent only on y
    XY d(_f(_k*fabs(p.y())).xy());
    // Reflect the distortion across symmetry line
    if (p.y()<0.0) d.y(-d.y());
    // This is where the distortion will be mapped to by symmetry
    const real intermediate_y=fabs(p.y()+d.y());
    // This is what would take us back to the original position
    const real back_y=fabs(p.y())-intermediate_y;
    return std::make_pair(d,XY(-d.x(),back_y));
  }
private:
  const Function& _f;
  const XYZ& _k;
};

//! Constructs two points and a blending weight which will behave sensibly for Hop
struct JumpBlend
{
  const boost::tuple<float,XY,XY> operator()(const XY& p) const
  {
    return boost::tuple<float,XY,XY>
      (
       2.0*trianglef(p.x(),0.5),
       Jump()(p),
       Jump()(p+XY(0.5,0.0))
       );
  }
};

//------------------------------------------------------------------------------------------

FUNCTION_BEGIN(FunctionFriezeGroupJumpFreeZ,0,1,false,FnStructure)

  virtual const XYZ evaluate(const XYZ& p) const
    {
      return Friezegroup(arg(0),p,Jump(),FreeZ());
    }
  
FUNCTION_END(FunctionFriezeGroupJumpFreeZ)

//------------------------------------------------------------------------------------------

FUNCTION_BEGIN(FunctionFriezeGroupJumpClampZ,1,1,false,FnStructure)

  virtual const XYZ evaluate(const XYZ& p) const
    {
      return Friezegroup(arg(0),p,Jump(),ClampZ(param(0)));
    }
  
FUNCTION_END(FunctionFriezeGroupJumpClampZ)

//------------------------------------------------------------------------------------------

FUNCTION_BEGIN(FunctionFriezeGroupJumpWarpFreeZ,3,2,false,FnStructure)

  virtual const XYZ evaluate(const XYZ& p) const
    {
      return FriezegroupWarp(arg(0),p,Jump(),JumpInvariant(arg(1),XYZ(param(0),param(1),param(2))),FreeZ());
    }
  
FUNCTION_END(FunctionFriezeGroupJumpWarpFreeZ)

//------------------------------------------------------------------------------------------

FUNCTION_BEGIN(FunctionFriezeGroupJumpWarpClampZ,4,2,false,FnStructure)

  virtual const XYZ evaluate(const XYZ& p) const
    {
      return FriezegroupWarp(arg(0),p,Jump(),JumpInvariant(arg(1),XYZ(param(0),param(1),param(2))),ClampZ(param(3)));
    }
  
FUNCTION_END(FunctionFriezeGroupJumpWarpClampZ)

//------------------------------------------------------------------------------------------

FUNCTION_BEGIN(FunctionFriezeGroupJumpBlendFreeZ,0,1,false,FnStructure)

  virtual const XYZ evaluate(const XYZ& p) const
    {
      return FriezegroupBlend(arg(0),p,Jump(),JumpBlend(),FreeZ());
    }
  
FUNCTION_END(FunctionFriezeGroupJumpBlendFreeZ)

//------------------------------------------------------------------------------------------

FUNCTION_BEGIN(FunctionFriezeGroupJumpBlendClampZ,1,1,false,FnStructure)

  virtual const XYZ evaluate(const XYZ& p) const
    {
      return FriezegroupBlend(arg(0),p,Jump(),JumpBlend(),ClampZ(param(0)));
    }
  
FUNCTION_END(FunctionFriezeGroupJumpBlendClampZ)

//------------------------------------------------------------------------------------------

FUNCTION_BEGIN(FunctionFriezeGroupJumpBlendWarpFreeZ,3,2,false,FnStructure)

  virtual const XYZ evaluate(const XYZ& p) const
    {
      return FriezegroupBlendWarp(arg(0),p,Jump(),JumpBlend(),JumpInvariant(arg(1),XYZ(param(0),param(1),param(2))),FreeZ());
    }
  
FUNCTION_END(FunctionFriezeGroupJumpBlendWarpFreeZ)

//------------------------------------------------------------------------------------------

FUNCTION_BEGIN(FunctionFriezeGroupJumpBlendWarpClampZ,4,2,false,FnStructure)

  virtual const XYZ evaluate(const XYZ& p) const
    {
      return FriezegroupBlendWarp(arg(0),p,Jump(),JumpBlend(),JumpInvariant(arg(1),XYZ(param(0),param(1),param(2))),ClampZ(param(3)));
    }
  
FUNCTION_END(FunctionFriezeGroupJumpBlendWarpClampZ)

//------------------------------------------------------------------------------------------

#endif
