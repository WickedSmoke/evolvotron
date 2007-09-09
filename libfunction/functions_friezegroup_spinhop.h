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
/*! Sawtooth x increasing or decreasing depending on which side, with y inverting
\verbatim
    o     o
  ---   ---
     ---   ---
     o     o
\endverbatim
*/
struct Spinhop
{
  const XY operator()(const XY& p) const
  {
    if (modulusf(p.x(),2.0)<1.0)
      {
	return XY(    modulusf(p.x(),1.0), p.y());
      }
    else
      {
	return XY(1.0-modulusf(p.x(),1.0),-p.y());
      }
  }
};

struct SpinhopInvariant   // HERE
{
  SpinhopInvariant(const Function& f)
    :_f(f)
  {}
  const XY operator()(const XY& p) const
  {
    
    const real y(modulusf(p.x()-0.5,2.0)<1.0 ? p.y() : -p.y());
    const real dx(_f(XYZ(trianglef(p.x()-0.5,0.5),y,0.0)).x());
    return XY(dx,0.0);
  }
private:
  const Function& _f;
};

//! Not sure this makes sense... trying to fade changes symmetry. 
struct SpinhopBlend;

//------------------------------------------------------------------------------------------

FUNCTION_BEGIN(FunctionFriezeGroupSpinhopFreeZ,0,1,false,FnStructure)

  virtual const XYZ evaluate(const XYZ& p) const
    {
      return Friezegroup(arg(0),p,Spinhop(),FreeZ());
    }

FUNCTION_END(FunctionFriezeGroupSpinhopFreeZ)

//------------------------------------------------------------------------------------------

FUNCTION_BEGIN(FunctionFriezeGroupSpinhopClampZ,1,1,false,FnStructure)

  virtual const XYZ evaluate(const XYZ& p) const
    {
      return Friezegroup(arg(0),p,Spinhop(),ClampZ(param(0)));
    }

FUNCTION_END(FunctionFriezeGroupSpinhopClampZ)

//------------------------------------------------------------------------------------------

FUNCTION_BEGIN(FunctionFriezeGroupSpinhopWarpFreeZ,0,2,false,FnStructure)

  virtual const XYZ evaluate(const XYZ& p) const
    {
      return FriezegroupWarp(arg(0),p,Spinhop(),SpinhopInvariant(arg(1)),FreeZ());
    }
  
FUNCTION_END(FunctionFriezeGroupSpinhopWarpFreeZ)

//------------------------------------------------------------------------------------------

FUNCTION_BEGIN(FunctionFriezeGroupSpinhopWarpClampZ,1,2,false,FnStructure)

  virtual const XYZ evaluate(const XYZ& p) const
    {
      return FriezegroupWarp(arg(0),p,Spinhop(),SpinhopInvariant(arg(1)),ClampZ(param(0)));
    }
  
FUNCTION_END(FunctionFriezeGroupSpinhopWarpClampZ)


//------------------------------------------------------------------------------------------

#endif
