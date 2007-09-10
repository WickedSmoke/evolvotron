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

FUNCTION_BEGIN(FunctionFriezeGroupHopFreeZ,0,1,false,FnStructure)

  virtual const XYZ evaluate(const XYZ& p) const
    {
      return Friezegroup(arg(0),p,Hop(1.0),FreeZ());
    }
  
FUNCTION_END(FunctionFriezeGroupHopFreeZ)

//------------------------------------------------------------------------------------------

FUNCTION_BEGIN(FunctionFriezeGroupHopClampZ,1,1,false,FnStructure)

  virtual const XYZ evaluate(const XYZ& p) const
    {
      return Friezegroup(arg(0),p,Hop(1.0),ClampZ(param(0)));
    }
  
FUNCTION_END(FunctionFriezeGroupHopClampZ)

//------------------------------------------------------------------------------------------

FUNCTION_BEGIN(FunctionFriezeGroupHopBlendClampZ,1,2,false,FnStructure)

  virtual const XYZ evaluate(const XYZ& p) const
    {
      return FriezegroupBlend(arg(0),arg(1),p,HopBlend(1.0),ClampZ(param(0)));
    }
  
FUNCTION_END(FunctionFriezeGroupHopBlendClampZ)

//------------------------------------------------------------------------------------------

FUNCTION_BEGIN(FunctionFriezeGroupHopBlendFreeZ,0,2,false,FnStructure)

  virtual const XYZ evaluate(const XYZ& p) const
    {
      return FriezegroupBlend(arg(0),arg(1),p,HopBlend(1.0),FreeZ());
    }
  
FUNCTION_END(FunctionFriezeGroupHopBlendFreeZ)

//------------------------------------------------------------------------------------------

FUNCTION_BEGIN(FunctionFriezeGroupHopCutClampZ,2,2,false,FnStructure)

  virtual const XYZ evaluate(const XYZ& p) const
    {
      const int d=FriezegroupCut(arg(1),p,XY(0.5,0.0),Hop(1.0),ClampZ(param(1)));
      return Friezegroup(arg(0),p,Hop(1.0,d),ClampZ(param(0)));
    }
  
FUNCTION_END(FunctionFriezeGroupHopCutClampZ)

//------------------------------------------------------------------------------------------

FUNCTION_BEGIN(FunctionFriezeGroupHopCutFreeZ,0,2,false,FnStructure)

  virtual const XYZ evaluate(const XYZ& p) const
    {
      const int d=FriezegroupCut(arg(1),p,XY(0.5,0.0),Hop(1.0),FreeZ());
      return Friezegroup(arg(0),p,Hop(1.0,d),FreeZ());
    }
  
FUNCTION_END(FunctionFriezeGroupHopCutFreeZ)

//------------------------------------------------------------------------------------------

#endif
