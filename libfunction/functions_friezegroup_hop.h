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

#ifndef _functions_friezegroup_hop_h_
#define _functions_friezegroup_hop_h_

#include "friezegroup.h"

//------------------------------------------------------------------------------------------

//! Hop (Conway p111): no reflections or rotation.
/*! Just have to cycle x range.   
\verbatim
    o    o    o
  ---  ---  --- 
\endverbatim
*/
struct Hop
{
  const XY operator()(const XY& p) const
  {
    return XY
      (
       modulusf(p.x(),1.0),
       p.y()
       );
  }
};

//! Something which can be added to friezegroup_hop without breaking symmetry.
/*
  A function f(p) has hop symmetry if f(xmod1,y)=f(x,y) for all x,y.
  This is true for Hop.

  [What do we mean by added ?
  If f has hop symmetry then f(g(p)) and g(f(p)) for any g do too.
  f+g has hop symmetry if f and g have hop symmetry (for mod1 rule anyway]

  Let g(p)=-d(p)+f(p+d(p)) where f has hop symmetry.
  If d(p) depends purely on y then g(p) has hop symmetry because then
  g(xmod1,y)=-d(y)+f(xmod1+dx(y),y+dy(y))
  g(x,y)    =-d(y)+f(x    +dx(y),y+dy(y))
  ...hmmm, not convincing...
 */
struct HopInvariant
{
  HopInvariant(const Function& f,const XYZ& k)
    :_f(f)
    ,_k(k)
  {}
  const std::pair<XY,XY> operator()(const XY& p) const
  {
    const XY d(_f(_k*p.y()).xy());
    return std::make_pair(d,-d);
  }
private:
  const Function& _f;
  const XYZ& _k;
};

//! Constructs two points and a blending weight which will behave sensibly for Hop
struct HopBlend
{
  const boost::tuple<float,XY,XY> operator()(const XY& p) const
  {
    return boost::tuple<float,XY,XY>
      (
       2.0*trianglef(p.x(),0.5),
       Hop()(p),
       Hop()(p+XY(0.5,0.0))
       );
  }
};

//------------------------------------------------------------------------------------------

FUNCTION_BEGIN(FunctionFriezeGroupHopFreeZ,0,1,false,FnStructure)

  virtual const XYZ evaluate(const XYZ& p) const
    {
      return Friezegroup(arg(0),p,Hop(),FreeZ());
    }
  
FUNCTION_END(FunctionFriezeGroupHopFreeZ)

//------------------------------------------------------------------------------------------

FUNCTION_BEGIN(FunctionFriezeGroupHopClampZ,1,1,false,FnStructure)

  virtual const XYZ evaluate(const XYZ& p) const
    {
      return Friezegroup(arg(0),p,Hop(),ClampZ(param(0)));
    }
  
FUNCTION_END(FunctionFriezeGroupHopClampZ)

//------------------------------------------------------------------------------------------

FUNCTION_BEGIN(FunctionFriezeGroupHopWarpFreeZ,3,2,false,FnStructure)

  virtual const XYZ evaluate(const XYZ& p) const
    {
      return FriezegroupWarp(arg(0),p,Hop(),HopInvariant(arg(1),XYZ(param(0),param(1),param(2))),FreeZ());
    }
  
FUNCTION_END(FunctionFriezeGroupHopWarpFreeZ)

//------------------------------------------------------------------------------------------

FUNCTION_BEGIN(FunctionFriezeGroupHopWarpClampZ,4,2,false,FnStructure)

  virtual const XYZ evaluate(const XYZ& p) const
    {
      return FriezegroupWarp(arg(0),p,Hop(),HopInvariant(arg(1),XYZ(param(0),param(1),param(2))),ClampZ(param(3)));
    }
  
FUNCTION_END(FunctionFriezeGroupHopWarpClampZ)

//------------------------------------------------------------------------------------------

FUNCTION_BEGIN(FunctionFriezeGroupHopBlendFreeZ,0,1,false,FnStructure)

  virtual const XYZ evaluate(const XYZ& p) const
    {
      return FriezegroupBlend(arg(0),p,Hop(),HopBlend(),FreeZ());
    }
  
FUNCTION_END(FunctionFriezeGroupHopBlendFreeZ)

//------------------------------------------------------------------------------------------

FUNCTION_BEGIN(FunctionFriezeGroupHopBlendClampZ,1,1,false,FnStructure)

  virtual const XYZ evaluate(const XYZ& p) const
    {
      return FriezegroupBlend(arg(0),p,Hop(),HopBlend(),ClampZ(param(0)));
    }
  
FUNCTION_END(FunctionFriezeGroupHopBlendClampZ)

//------------------------------------------------------------------------------------------

FUNCTION_BEGIN(FunctionFriezeGroupHopBlendWarpFreeZ,3,2,false,FnStructure)

  virtual const XYZ evaluate(const XYZ& p) const
    {
      return FriezegroupBlendWarp(arg(0),p,Hop(),HopBlend(),HopInvariant(arg(1),XYZ(param(0),param(1),param(2))),FreeZ());
    }
  
FUNCTION_END(FunctionFriezeGroupHopBlendWarpFreeZ)

//------------------------------------------------------------------------------------------

FUNCTION_BEGIN(FunctionFriezeGroupHopBlendWarpClampZ,4,2,false,FnStructure)

  virtual const XYZ evaluate(const XYZ& p) const
    {
      return FriezegroupBlendWarp(arg(0),p,Hop(),HopBlend(),HopInvariant(arg(1),XYZ(param(0),param(1),param(2))),ClampZ(param(3)));
    }
  
FUNCTION_END(FunctionFriezeGroupHopBlendWarpClampZ)

//------------------------------------------------------------------------------------------

#endif
