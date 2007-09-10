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
  Jump(int domain=0)
    :_domain(domain)
  {}
  const XY operator()(const XY& p) const
    {
      return XY
	(
	 static_cast<real>(_domain)+modulusf(p.x(),1.0),
	 fabs(p.y())
	 );
    }
  private:
  const int _domain;
};

struct JumpInvariant
{
  JumpInvariant(const Function& f)
    :_f(f)
  {}
  const XY operator()(const XY& p) const
  {
    // An x-only distortion dependent only on fabs(y) and repeating in x.
    // y distortion tricky because could cross y=0 line
    XY d(_f(XYZ(trianglef(p.x()-0.5,0.5),fabs(p.y()),0.0)).x(),0.0);
    return d;
  }
private:
  const Function& _f;
};

//! Constructs two points and a blending weight which will behave sensibly for Jump
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

#endif
