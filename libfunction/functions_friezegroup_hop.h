// Source file for evolvotron
// Copyright (C) 2002,2003,2004,2005 Tim Day
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

/*
  Hop (Conway p111): no reflections or rotation
  Just have to cycle x range.
  
    o    o    o
  ---  ---  ---
 
*/

//------------------------------------------------------------------------------------------

FUNCTION_BEGIN(FunctionFriezeGroupHop,1,1,false,FnStructure)

  virtual const XYZ evaluate(const XYZ& p) const
    {
      const real x=modulusf(p.x(),1.0);
      const real y=p.y();
      const real z=maximum(0.0,param(0))*p.z();
      return arg(0)(XYZ(x,y,z));
    }
  
FUNCTION_END(FunctionFriezeGroupHop)

//------------------------------------------------------------------------------------------

#endif
