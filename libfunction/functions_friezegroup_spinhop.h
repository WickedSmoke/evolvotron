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

#ifndef _functions_friezegroup_spinhop_h_
#define _functions_friezegroup_spinhop_h_

#include "friezegroup.h"

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
struct Spinhop
{
  Spinhop(real width)
    :_width(width)
  {}
  const XY operator()(const XY& p) const
  {
    const real x=modulusf(p.x()+0.5*_width,2.0*_width)-0.5*_width;
    if (x<0.5*_width)
      {
	return XY(x,p.y());
      }
    else
      {
	return XY(_width-x,-p.y());
      }
  }
  private:
  const real _width;
};

//------------------------------------------------------------------------------------------

FUNCTION_BEGIN(FunctionFriezeGroupSpinhopFreeZ,0,1,false,FnStructure)

  virtual const XYZ evaluate(const XYZ& p) const
    {
      return Friezegroup(arg(0),p,Spinhop(1.0),FreeZ());
    }

FUNCTION_END(FunctionFriezeGroupSpinhopFreeZ)

//------------------------------------------------------------------------------------------

FUNCTION_BEGIN(FunctionFriezeGroupSpinhopClampZ,1,1,false,FnStructure)

  virtual const XYZ evaluate(const XYZ& p) const
    {
      return Friezegroup(arg(0),p,Spinhop(1.0),ClampZ(param(0)));
    }

FUNCTION_END(FunctionFriezeGroupSpinhopClampZ)

//------------------------------------------------------------------------------------------

#endif
