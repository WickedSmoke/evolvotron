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

//! Hop (Conway p111): no reflections or rotation.
/* Just have to cycle x range.   
\verbatim
    o    o    o
  ---  ---  --- 
\endverbatim
*/
inline const XY friezegroup_hop(const XY& p)
{
  return XY
    (
     modulusf(p.x(),1.0),
     p.y()
     );
}

//! Something which can be added to friezegroup_hop without breaking symmetry.
/*
  A function f(p) has hop symmetry if f(xmod1,y)=f(x,y) for all x,y.
  This is true for friezegroup_hop.

  [What do we mean by added ?
  If f has hop symmetry then f(g(p)) and g(f(p)) for any g do too.
  f+g has hop symmetry if f and g have hop symmetry]

  Let g(p)=-d(p)+f(p+d(p)) where f has hop symmetry.
  If d(p) depends purely on y then g(p) has hop symmetry because then
  g(xmod1,y)=-d(y)+f(xmod1+dx(y),y+dy(y))
  g(x,y)    =-d(y)+f(x    +dx(y),y+dy(y))
  ...hmmm, not convincing...
 */
inline const XY friezegroup_hop_invariant(const FunctionNode& f,const XYZ& p,const XYZ& k)
{
  return f(p.y()*k).xy();
}

//------------------------------------------------------------------------------------------

FUNCTION_BEGIN(FunctionFriezeGroupHopFreeZ,0,0,false,FnStructure)

  virtual const XYZ evaluate(const XYZ& p) const
    {
      return XYZ(friezegroup_hop(p.xy()),p.z());
    }
  
FUNCTION_END(FunctionFriezeGroupHopFreeZ)

//------------------------------------------------------------------------------------------

FUNCTION_BEGIN(FunctionFriezeGroupHopClampZ,1,0,false,FnStructure)

  virtual const XYZ evaluate(const XYZ& p) const
    {
      return XYZ(friezegroup_hop(p.xy()),param(0));
    }
  
FUNCTION_END(FunctionFriezeGroupHopClampZ)

//------------------------------------------------------------------------------------------

FUNCTION_BEGIN(FunctionFriezeGroupHopWarpFreeZ,3,1,false,FnStructure)

  virtual const XYZ evaluate(const XYZ& p) const
    {
      const XY d(friezegroup_hop_invariant(arg(0),p,XYZ(param(0),param(1),param(2))));
      return XYZ(-d+friezegroup_hop(p.xy()+d),p.z());
    }
  
FUNCTION_END(FunctionFriezeGroupHopWarpFreeZ)

//------------------------------------------------------------------------------------------

FUNCTION_BEGIN(FunctionFriezeGroupHopWarpClampZ,4,1,false,FnStructure)

  virtual const XYZ evaluate(const XYZ& p) const
    {
      const XY d(friezegroup_hop_invariant(arg(0),p,XYZ(param(0),param(1),param(2))));
      return XYZ(-d+friezegroup_hop(p.xy()+d),param(3));
    }
  
FUNCTION_END(FunctionFriezeGroupHopWarpClampZ)

//------------------------------------------------------------------------------------------

#endif
