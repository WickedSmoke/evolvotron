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
