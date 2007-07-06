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

#ifndef _functions_friezegroup_step_h_
#define _functions_friezegroup_step_h_

//! Step (Conway p1a1): glide reflection only.
/*! Sawtooth x, out of step by half range across y-axis.
\verbatim
    o     o
  ---   ---
     ---   ---
       o     o
\endverbatim
*/
inline const XY friezegroup_step(const XY& p)
{
  return XY
    (
     (p.y()>0.0 ? modulusf(p.x(),1.0) : modulusf(p.x()+0.5,1.0)),
     fabs(p.y())
     );
}

//------------------------------------------------------------------------------------------

FUNCTION_BEGIN(FunctionFriezeGroupStepFreeZ,0,0,false,FnStructure)

  virtual const XYZ evaluate(const XYZ& p) const
    {
      return XYZ(friezegroup_step(p.xy()),p.z());
    }
  
FUNCTION_END(FunctionFriezeGroupStepFreeZ)

//------------------------------------------------------------------------------------------

FUNCTION_BEGIN(FunctionFriezeGroupStepClampZ,1,0,false,FnStructure)

  virtual const XYZ evaluate(const XYZ& p) const
    {
      return XYZ(friezegroup_step(p.xy()),param(0));
    }
  
FUNCTION_END(FunctionFriezeGroupStepClampZ)

//------------------------------------------------------------------------------------------

#endif
