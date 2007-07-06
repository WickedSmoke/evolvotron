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

struct FreeZ
{
  const float operator()(float z) const
  {
    return z;
  }
};

struct ClampZ
{
  ClampZ(float z)
    :_z(z)
  {}
  const float operator()(float) const
  {
    return _z;
  }

  private:
  const float _z;
};

//! Something which can be added to friezegroup_hop without breaking symmetry.
/*
  A function f(p) has hop symmetry if f(xmod1,y)=f(x,y) for all x,y.
  This is true for friezegroup_hop.

  [What do we mean by added ?
  If f has hop symmetry then f(g(p)) and g(f(p)) for any g do too.
  f+g has hop symmetry if f and g have hop symmetry]

  Let g(p)=-d(p)+f(p+d(p)) where f has hop symmetry.
  If d(p) depends purely on y then g(p) has hop symmetry because then
  g(xmod1,y)=-d(y)+f(xmod1+dx(y),y+dy(y))
  g(x,y)    =-d(y)+f(x    +dx(y),y+dy(y))
  ...hmmm, not convincing...
 */
struct HopInvariant
{
  HopInvariant(const FunctionNode& f,const XYZ& k)
    :_f(f)
    ,_k(k)
  {}
  const XY operator()(const XY& p) const
  {
    return _f(_k*p.y()).xy();
  }
private:
  const FunctionNode& _f;
  const XYZ& _k;
};

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

template <class SYMMETRY,class ZPOLICY> const XYZ Friezegroup(const FunctionNode& f,const XYZ& p,const SYMMETRY& sym,const ZPOLICY& zpol)
{
  return f(XYZ(sym(p.xy()),zpol(p.z())));
}

template<class SYMMETRY,class WARP,class ZPOLICY> const XYZ FriezegroupWarp(const FunctionNode& f,const XYZ& p,const SYMMETRY& sym,const WARP& warp,const ZPOLICY& zpol)
{
  const XY d(warp(p.xy()));
  return f(XYZ(d+sym(p.xy()-d),zpol(p.z())));
}

template<class SYMMETRY,class BLEND,class ZPOLICY> const XYZ FriezegroupBlend(const FunctionNode& f,const XYZ& p,const SYMMETRY& sym,const BLEND& blend,const ZPOLICY& zpol)
{
  const boost::tuple<float,XY,XY> b(blend(p.xy()));
  return
    b.get<0>()*f(XYZ(sym(b.get<1>()),zpol(p.z())))
    +(1.0-b.get<0>())*f(XYZ(sym(b.get<2>()),zpol(p.z())));
}

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

#endif
