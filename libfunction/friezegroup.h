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
  \brief Interfaces and implementation for friezegroup related code.
*/

#ifndef _friezegroup_h_
#define _friezegroup_h_

#include "common.h"

#include "function_node.h"

//! Functor implementing a pass-through Z coordinate policy
struct FreeZ
{
  float operator()(float z) const
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
  float operator()(float) const
  {
    return _z;
  }

  private:
  const float _z;
};

//! Function evaluation via symmetry.
template <class SYMMETRY,class ZPOLICY> 
  inline const XYZ FriezegroupEvaluate
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

template<class BLEND,class ZPOLICY> 
  inline const XYZ FriezegroupBlend
    (
     const Function& f,const XYZ& p,const BLEND& blend,const ZPOLICY& zpol
     )
{
  return FriezegroupBlend(f,f,p,blend,zpol);
}

//! Generate domain shift for when cutting.
/*! This function actually far too specific to Hop and Jump, so move into their Cut functions as done for SpinhopCut
 */
/*
template<class CUT,class ZPOLICY>
  inline const int FriezegroupCut
    (
     const Function& f,const XYZ& p,const CUT& cut,const ZPOLICY& zpol
     )
{
  const XY pc(cut(p.xy()));
  const real k=tanh(f(XYZ(pc,zpol(p.z()))).sum_of_components());
  const real t=pc.x()/(0.5*cut.width());   // -1 to +1 over domain used for cut function (should be in -width/2 to +width/2)
  if (pc.x()<0.0 && k<t) return -1;
  else if (pc.x()>=0.0 && k>t) return 1;
  else return 0;
}
*/

//------------------------------------------------------------------------------------------

struct Friezegroup
{
  Friezegroup(real width)
    :_width(width)
  {
    assert(_width>0.0f);
  }

  real width() const
  {
    return _width;
  }
  private:
  const real _width;
};

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
struct Hop : public Friezegroup
{
  Hop(real width,int domain=0)
    :Friezegroup(width)
    ,_domain(domain)
  {}

  const XY operator()(const XY& p) const
  {
    return XY
      (
       (_domain-0.5)*width()+modulusf(p.x()-0.5*width(),width()),
       p.y()
       );
  }
  private:
  const int _domain;
};

//! Constructs two points and a blending weight which will behave sensibly for Hop.
/*! The additional point is half a domain width away.
  The blend weight is the weight of the primary point, should be maximum
  in the centre of the domain (0) and zero at the edges +/-0.5*width
 */
struct HopBlend : public Friezegroup // subclassing doesn't make much sense really
{
  HopBlend(real width)
    :Friezegroup(width)
  {}
  const boost::tuple<real,XY,XY> operator()(const XY& p) const
  {
    const Hop hop(width());
    return boost::tuple<real,XY,XY>
      (
       (2.0/width())*trianglef(p.x()-0.5*width(),0.5*width()),  // 0 at -width/2 and +width/2, 1 at 0
       hop(p),
       hop(p-XY(0.5*width(),0.0))
       );
  }
};

//! Generates points suitable for evaluating cutting function
/*
struct HopCut : public Friezegroup
{
  HopCut(real width)
    :Friezegroup(width)
  {}
  const XY operator()(const XY& p) const
  {
    return Hop(width())(p+XY(0.5*width(),0.0));
  }
};
*/

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

/*
struct JumpCut : public HopCut
{
  JumpCut(real width)
    :HopCut(width)
  {}
  const XY operator()(const XY& p) const
  {
    return HopCut::operator()(XY(p.x(),fabs(p.y())));
  }
};
*/

//------------------------------------------------------------------------------------------

//! Sidle (Conway pm11):  vertical reflection
/*! Bounce x backwards and forwards.
  Can't be blended or cut because there are only reflection lines.
  (Well we could blend it by increasing the domain width and blending between overlapping domains, but the reflection lines aren't really worth going to any trouble to hide.
\verbatim
  o| |o o| |o
   | |   | |
   | |   | |
\endverbatim
*/
struct Sidle : public Friezegroup
{
  Sidle(real width)
    :Friezegroup(width)
  {}
  const XY operator()(const XY& p) const
  {
    return XY
      (
       trianglef(p.x()+0.5*width(),width())-0.5*width(),       // So -width/2 maps to -width/2
       p.y()
       );
  }
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

//! Spinhop (Conway p112): Half turn rotation only.
/*! Sawtooth x increasing or decreasing depending on which side, with y inverting.
Don't think this can be blended because it would change symmetry.
\verbatim
    o     o
  ---   ---
     ---   ---
     o     o
\endverbatim
*/
struct Spinhop : public Friezegroup
{
  Spinhop(real width,int domain=0)
    :Friezegroup(width)
    ,_domain(domain)
  {}
  const XY operator()(const XY& p) const
  {

    bool flipped=(modulusf(p.x()+0.5*width(),2.0*width())>width());
    if (_domain!=0) flipped=!flipped;
    
    real x=modulusf(p.x()+0.5*width(),width())-0.5*width()+_domain*width();
    real y=p.y();
    if (flipped)
      {
	x=-x;
	y=-y;
      }
    
    return XY(x,y);
  }
  private:
  const int _domain;
};

//! Blended Spinhop
/*! Can build this from blending two rotated hop sequences.
  Make the domains wider.
\verbatim
     o      o      o      o
 -----  -----  -----  -----        v blend these two together
    -----  -----  -----  -----     ^ using triangular alpha fn
    o      o      o      o  
\endverbatim
 */
struct SpinhopBlend : public Friezegroup // subclassing doesn't make much sense really
{
  SpinhopBlend(real width)
    :Friezegroup(width)
  {}
  const boost::tuple<real,XY,XY> operator()(const XY& p) const
  {
    const Hop hop(2*width());
    return boost::tuple<real,XY,XY>
      (
       trianglef(p.x()-width(),width())/width(),  // zero at x=-width and +width, 1 at x=0
       hop(p),
       hop(XY(width()-p.x(),-p.y()))
       );
  }
};

//! Only suitable cut I can see looks like Sidle with rotate across y=0
/*! \todo There's something different could be done here.  The cutting is too constrained (needs pictures :^)
  \todo Some "softening" of the cut around y=0 would be good to stop odd looking hard lines.
 */
/*
template <class ZPOLICY> struct SpinhopCut : public Friezegroup
{
  SpinhopCut(real width)
    :Friezegroup(width)
  {}
  const int operator()(const Function& f,const XYZ& p,const ZPOLICY& zpol) const
  {
    const XY pm(p.x()-0.5*width(),fabs(p.y()));    // Shift out of alignment with spinhop being cut, and add reflection about y=0
    const XY r(Sidle(width())(pm));                // in combo with sidle, gets us something suitable for cutting without breaking spinhop
    const XY pc(p.y()<0.0 ? XY(-r.x(),r.y()) : r); // if we also flip it below y=0
    const real k=tanh(f(XYZ(pc,zpol(p.z()))).sum_of_components());

    const real t=(modulusf(pm.x()-0.5*width(),width())-0.5*width())/(0.5*width());  // Scans -1 to 1 across each (shifted, cutting) domain
    int d=0;
    if (t<0.0 && k<t) d=-1;
    else if (t>=0.0 && k>t) d=1;
    return d;
  }
};
*/

//------------------------------------------------------------------------------------------

#endif
