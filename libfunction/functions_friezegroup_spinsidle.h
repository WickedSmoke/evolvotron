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

#ifndef _functions_friezegroup_spinsidle_h_
#define _functions_friezegroup_spinsidle_h_

//! Spinsidle (Conway pma2): Glide reflection with half rotation.
/*! Oscillate x, reflect y in alternate x cycles.
\verbatim
  |oo|    |oo|
  |  |    |  |
  |  |    |  |
      |  |
      |  |
      |oo|
\endverbatim
*/
inline const XY friezegroup_spinsidle(const XY& p)
{
  return XY
    (
     trianglef(p.x(),0.5),
     modulusf(p.x()+0.5,2.0)<1.0 ? p.y() : -p.y()
     );
}

//------------------------------------------------------------------------------------------

FUNCTION_BEGIN(FunctionFriezeGroupSpinsidleFreeZ,0,0,false,FnStructure)

  virtual const XYZ evaluate(const XYZ& p) const
    {
      return XYZ(friezegroup_spinsidle(p.xy()),p.z());
    }
  
FUNCTION_END(FunctionFriezeGroupSpinsidleFreeZ)

//------------------------------------------------------------------------------------------

FUNCTION_BEGIN(FunctionFriezeGroupSpinsidleClampZ,1,0,false,FnStructure)

  virtual const XYZ evaluate(const XYZ& p) const
    {
      return XYZ(friezegroup_spinsidle(p.xy()),param(0));
    }
  
FUNCTION_END(FunctionFriezeGroupSpinsidleClampZ)

//------------------------------------------------------------------------------------------

#endif