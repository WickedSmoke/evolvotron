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

//! Function evaluation with blending.
/*! NB Is symmetry unaware; blend must have already reduced points to base domain.
 */
template<class BLEND,class ZPOLICY> 
  inline const XYZ FriezegroupBlend
    (
     const Function& f0,const Function& f1,const XYZ& p,const BLEND& blend,const ZPOLICY& zpol
     )
{
  const boost::tuple<real,XY,XY> b(blend(p.xy()));
  return
          b.get<0>() *f0(XYZ(b.get<1>(),zpol(p.z())))
    +(1.0-b.get<0>())*f1(XYZ(b.get<2>(),zpol(p.z())));
}

template<class SYMMETRY,class ZPOLICY>
  inline const int FriezegroupCut
    (
     const Function& f,const XYZ& p,const XY& d,const SYMMETRY& sym,const ZPOLICY& zpol
     )
{
  const XY pc(sym(p.xy()+d));              // Might need to be more complicated than +d in future.
  const real k=tanh(f(XYZ(pc,zpol(p.z()))).sum_of_components()); 
  const real t=pc.x()/(0.5*sym.width());   // -1 to +1 over shifted domain used for cut function

  if (pc.x()<0.0 && k<t) return -1;
  else if (pc.x()>=0.0 && k>t) return 1;
  else return 0;
}

//------------------------------------------------------------------------------------------

//! Hop (Conway p111): no reflections or rotation.
/*! Just have to cycle x range.   
\verbatim
    o    o    o
  ---  ---  --- 
\endverbatim
Domain is over (-width/2,width/2), centred on zero.
Default domain we use -0.5 to +0.5 to see symmetry at default sort of zoom.
*/
struct Hop
{
  Hop(real width,int domain=0)
    :_width(width)
    ,_domain(domain)
  {
    assert(width>0.0f);
  }

  const real width() const {return _width;}

  const XY operator()(const XY& p) const
  {
    return XY
      (
       (_domain-0.5)*_width+modulusf(p.x()-0.5*_width,_width),
       p.y()
       );
  }
  private:
  const real _width;
  const int _domain;
};

//! Constructs two points and a blending weight which will behave sensibly for Hop.
/*! The additional point is half a domain width away.
  The blend weight is the weight of the primary point, should be maximum
  in the centre of the domain (0) and zero at the edges +/-0.5*width
 */
struct HopBlend
{
  HopBlend(real width)
    :_width(width)
  {
    assert(width>0.0f);
  }
       
  const boost::tuple<real,XY,XY> operator()(const XY& p) const
  {
    const Hop hop(_width);
    return boost::tuple<real,XY,XY>
      (
       (2.0/_width)*trianglef(p.x()-0.5*_width,0.5*_width),  // 0 at -width/2 and +width/2, 1 at 0
       hop(p),
       hop(p-XY(0.5*_width,0.0))
       );
  }

  private:
  const real _width;
};

//------------------------------------------------------------------------------------------

//! Jump (Conway p1m1): horizontal reflection only
/*! Just cycle x range and reflect in y.
Is simply Hop with a relection about y=0
\verbatim
     o    o    o
   ---  ---  ---
   ---  ---  ---
     o    o    o
\endverbatim
*/
struct Jump : public Hop
{
  Jump(real width,int domain=0)
    :Hop(width,domain)
  {}

  const XY operator()(const XY& p) const
    {
      return Hop::operator()(XY(p.x(),fabs(p.y())));
    }
};

//! Constructs two points and a blending weight which will behave sensibly for Jump
struct JumpBlend : public HopBlend
{
  JumpBlend(real width)
    :HopBlend(width)
  {}
  const boost::tuple<float,XY,XY> operator()(const XY& p) const
  {
    return HopBlend::operator()(XY(p.x(),fabs(p.y())));
  }
};

//------------------------------------------------------------------------------------------

//! Sidle (Conway pm11):  vertical reflection
/*! Bounce x backwards and forwards.
  Can't be blended or cut because there are only reflection lines.
\verbatim
  o| |o o| |o
   | |   | |
   | |   | |
\endverbatim
*/
struct Sidle
{
  Sidle(real width)
    :_width(width)
  {}
  const XY operator()(const XY& p) const
  {
    return XY
      (
       trianglef(p.x()-0.5*_width,_width),
       p.y()
       );
  }
  private:
  const real _width;
};

//------------------------------------------------------------------------------------------

//! Spinjump (Conway pmm2): vertical & horizontal reflection and half-rotation.
/*! Oscillate x and reflect y.  
Is just sidle with a reflection about y=0.
  Can't be blended or cut because there are only reflection lines.
\verbatim
    o o     o o
  --- --- --- ---
  --- --- --- ---
    o o     o o
\endverbatim
*/
struct Spinjump : public Sidle
{
  Spinjump(real width)
    :Sidle(width)
  {}
  const XY operator()(const XY& p) const
  {
    return Sidle::operator()(XY(p.x(),fabs(p.y())));
  }
};

//------------------------------------------------------------------------------------------

#endif
