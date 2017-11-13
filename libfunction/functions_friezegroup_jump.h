/**************************************************************************/
/*  Copyright 2012 Tim Day                                                */
/*                                                                        */
/*  This file is part of Evolvotron                                       */
/*                                                                        */
/*  Evolvotron is free software: you can redistribute it and/or modify    */
/*  it under the terms of the GNU General Public License as published by  */
/*  the Free Software Foundation, either version 3 of the License, or     */
/*  (at your option) any later version.                                   */
/*                                                                        */
/*  Evolvotron is distributed in the hope that it will be useful,         */
/*  but WITHOUT ANY WARRANTY; without even the implied warranty of        */
/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         */
/*  GNU General Public License for more details.                          */
/*                                                                        */
/*  You should have received a copy of the GNU General Public License     */
/*  along with Evolvotron.  If not, see <http://www.gnu.org/licenses/>.   */
/**************************************************************************/

/*! \file
  \brief Interfaces and implementation for specific Function classes.
*/

#ifndef _functions_friezegroup_jump_h_
#define _functions_friezegroup_jump_h_

#include "common.h"

#include "function_boilerplate.h"

#include "friezegroup.h"

//------------------------------------------------------------------------------------------

FUNCTION_BEGIN(FunctionFriezeGroupJumpFreeZ,0,1,false,FnStructure)

  virtual const XYZ evaluate(const XYZ& p) const
    {
      return FriezegroupEvaluate(arg(0),p,Jump(1.0),FreeZ());
    }
  
FUNCTION_END(FunctionFriezeGroupJumpFreeZ)

//------------------------------------------------------------------------------------------

FUNCTION_BEGIN(FunctionFriezeGroupJumpClampZ,1,1,false,FnStructure)

  virtual const XYZ evaluate(const XYZ& p) const
    {
      return FriezegroupEvaluate(arg(0),p,Jump(1.0),ClampZ(param(0)));
    }
  
FUNCTION_END(FunctionFriezeGroupJumpClampZ)

//------------------------------------------------------------------------------------------

FUNCTION_BEGIN(FunctionFriezeGroupJumpBlendClampZ,1,2,false,FnStructure)

  virtual const XYZ evaluate(const XYZ& p) const
    {
      return FriezegroupBlend(arg(0),arg(1),p,JumpBlend(1.0),ClampZ(param(0)));
    }
  
FUNCTION_END(FunctionFriezeGroupJumpBlendClampZ)

//------------------------------------------------------------------------------------------

FUNCTION_BEGIN(FunctionFriezeGroupJumpBlendFreeZ,0,2,false,FnStructure)

  virtual const XYZ evaluate(const XYZ& p) const
    {
      return FriezegroupBlend(arg(0),arg(1),p,JumpBlend(1.0),FreeZ());
    }
  
FUNCTION_END(FunctionFriezeGroupJumpBlendFreeZ)

//------------------------------------------------------------------------------------------

/*
FUNCTION_BEGIN(FunctionFriezeGroupJumpCutClampZ,2,2,false,FnStructure)

  virtual const XYZ evaluate(const XYZ& p) const
    {
      const int d=FriezegroupCut(arg(1),p,JumpCut(1.0),ClampZ(param(1)));
      return FriezegroupEvaluate(arg(0),p,Jump(1.0,d),ClampZ(param(0)));
    }
  
FUNCTION_END(FunctionFriezeGroupJumpCutClampZ)
*/

//------------------------------------------------------------------------------------------

/*
FUNCTION_BEGIN(FunctionFriezeGroupJumpCutFreeZ,0,2,false,FnStructure)

  virtual const XYZ evaluate(const XYZ& p) const
    {
      const int d=FriezegroupCut(arg(1),p,JumpCut(1.0),FreeZ());
      return FriezegroupEvaluate(arg(0),p,Jump(1.0,d),FreeZ());
    }
  
FUNCTION_END(FunctionFriezeGroupJumpCutFreeZ)
*/

//------------------------------------------------------------------------------------------

#endif
