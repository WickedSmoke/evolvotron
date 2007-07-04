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

//! Sidle (Conway pm11):  vertical reflection
/*! Bounce x backwards and forwards.
\verbatim
  o| |o o| |o
   | |   | |
   | |   | |
\endverbatim
*/
inline const XY friezegroup_sidle(const XY& p)
{
  return XY
    (
     trianglef(p.x(),1.0),
     p.y()
     );
}

//------------------------------------------------------------------------------------------

FUNCTION_BEGIN(FunctionFriezeGroupSidleFreeZ,0,0,false,FnStructure)
  // Don't think this form can be warped without breaking symmetry
     
  virtual const XYZ evaluate(const XYZ& p) const
    {  
      return XYZ(friezegroup_sidle(p.xy()),p.z());
    }
  
FUNCTION_END(FunctionFriezeGroupSidleFreeZ)

//------------------------------------------------------------------------------------------

FUNCTION_BEGIN(FunctionFriezeGroupSidleClampZ,1,0,false,FnStructure)
  // Don't think this form can be warped without breaking symmetry
     
  virtual const XYZ evaluate(const XYZ& p) const
    {  
      return XYZ(friezegroup_sidle(p.xy()),param(0));
    }
  
FUNCTION_END(FunctionFriezeGroupSidleClampZ)

//------------------------------------------------------------------------------------------

#endif
