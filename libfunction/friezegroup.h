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
  \brief Interfaces and implementation for friezegroup related code.
*/

#ifndef _friezegroup_h_
#define _friezegroup_h_

//! Functor implementing a pass-through Z coordinate policy
struct FreeZ
{
  const float operator()(float z) const
  {
    return z;
  }
};

//! Functor implementing a clamping Z coordinate policy
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

//! Function evaluation via symmetry.
template <class SYMMETRY,class ZPOLICY> 
  inline const XYZ Friezegroup
    (
     const Function& f,const XYZ& p,const SYMMETRY& sym,const ZPOLICY& zpol
     )
{
  return f(XYZ(sym(p.xy()),zpol(p.z())));
}

//! Function evaluation via warped symmetry.
template<class SYMMETRY,class WARP,class ZPOLICY> 
  inline const XYZ FriezegroupWarp
    (
     const Function& f,const XYZ& p,const SYMMETRY& sym,const WARP& warp,const ZPOLICY& zpol
     )
{
  const XY d(warp(p.xy()));
  const XY pd(p.xy()+d);
  const XY delta(sym(pd)-pd);
  return f(XYZ(p.xy()+delta,zpol(p.z())));
}

//! Function evaluation via blended symmetry.
template<class SYMMETRY,class BLEND,class ZPOLICY> 
  inline const XYZ FriezegroupBlend
    (
     const Function& f,const XYZ& p,const SYMMETRY& sym,const BLEND& blend,const ZPOLICY& zpol
     )
{
  const boost::tuple<float,XY,XY> b(blend(p.xy()));
  return
          b.get<0>() *f(XYZ(sym(b.get<1>()),zpol(p.z())))
    +(1.0-b.get<0>())*f(XYZ(sym(b.get<2>()),zpol(p.z())));
}

//! Function evaluation via blended warped symmetry.
template<class SYMMETRY,class BLEND,class WARP,class ZPOLICY> 
  inline const XYZ FriezegroupBlendWarp
    (
     const Function& f,const XYZ& p,const SYMMETRY& sym,const BLEND& blend,const WARP& warp,const ZPOLICY& zpol
     )
{
  const XY d(warp(p.xy()));
  const XY pd(p.xy()+d);
  const XY delta(sym(pd)-pd);

  return FriezegroupBlend(f,p+XYZ(delta,0.0),sym,blend,zpol);
}

#endif
