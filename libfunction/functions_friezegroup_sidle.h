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

#ifndef _functions_friezegroup_sidle_h_
#define _functions_friezegroup_sidle_h_

#include "friezegroup.h"

//! Sidle (Conway pm11):  vertical reflection
/*! Bounce x backwards and forwards.
\verbatim
  o| |o o| |o
   | |   | |
   | |   | |
\endverbatim
*/
struct Sidle
{
  const XY operator()(const XY& p) const
  {
    return XY
      (
       trianglef(p.x(),1.0),
       p.y()
       );
  }
};

//! Can't warp Sidle because the reflection lines lock the pattern down.
//struct SidleInvariant;

//! Can't blend Sidle because there's nothing to hide.
//struct SidleBlend;

//------------------------------------------------------------------------------------------

FUNCTION_BEGIN(FunctionFriezeGroupSidleFreeZ,0,1,false,FnStructure)
     
  virtual const XYZ evaluate(const XYZ& p) const
    {  
      return Friezegroup(arg(0),p,Sidle(),FreeZ());
    }
  
FUNCTION_END(FunctionFriezeGroupSidleFreeZ)

//------------------------------------------------------------------------------------------

FUNCTION_BEGIN(FunctionFriezeGroupSidleClampZ,1,1,false,FnStructure)
  // Don't think this form can be warped without breaking symmetry
     
  virtual const XYZ evaluate(const XYZ& p) const
    {  
      return Friezegroup(arg(0),p,Sidle(),ClampZ(param(0)));
    }
  
FUNCTION_END(FunctionFriezeGroupSidleClampZ)

//------------------------------------------------------------------------------------------

#endif
