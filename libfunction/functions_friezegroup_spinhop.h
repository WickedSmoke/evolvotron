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

#ifndef _functions_friezegroup_spinhop_h_
#define _functions_friezegroup_spinhop_h_

#include "common.h"

#include "function_boilerplate.h"

#include "friezegroup.h"

//------------------------------------------------------------------------------------------

FUNCTION_BEGIN(FunctionFriezeGroupSpinhopFreeZ,0,1,false,FnStructure)

  virtual const XYZ evaluate(const XYZ& p) const
    {
      return FriezegroupEvaluate(arg(0),p,Spinhop(1.0),FreeZ());
    }

FUNCTION_END(FunctionFriezeGroupSpinhopFreeZ)

//------------------------------------------------------------------------------------------

FUNCTION_BEGIN(FunctionFriezeGroupSpinhopClampZ,1,1,false,FnStructure)

  virtual const XYZ evaluate(const XYZ& p) const
    {
      return FriezegroupEvaluate(arg(0),p,Spinhop(1.0),ClampZ(param(0)));
    }

FUNCTION_END(FunctionFriezeGroupSpinhopClampZ)

//------------------------------------------------------------------------------------------

FUNCTION_BEGIN(FunctionFriezeGroupSpinhopBlendClampZ,1,1,false,FnStructure)

  virtual const XYZ evaluate(const XYZ& p) const
    {
      return FriezegroupBlend(arg(0),p,SpinhopBlend(1.0),ClampZ(param(0)));
    }
  
FUNCTION_END(FunctionFriezeGroupSpinhopBlendClampZ)

//------------------------------------------------------------------------------------------

FUNCTION_BEGIN(FunctionFriezeGroupSpinhopBlendFreeZ,0,1,false,FnStructure)

  virtual const XYZ evaluate(const XYZ& p) const
    {
      return FriezegroupBlend(arg(0),p,SpinhopBlend(1.0),FreeZ());
    }
  
FUNCTION_END(FunctionFriezeGroupSpinhopBlendFreeZ)

//------------------------------------------------------------------------------------------

/*
FUNCTION_BEGIN(FunctionFriezeGroupSpinhopCutClampZ,2,2,false,FnStructure)

  virtual const XYZ evaluate(const XYZ& p) const
    {
      const int d=SpinhopCut<ClampZ>(1.0)(arg(1),p,ClampZ(param(1)));
      return FriezegroupEvaluate(arg(0),p,Spinhop(1.0,d),ClampZ(param(0)));
    }
  
FUNCTION_END(FunctionFriezeGroupSpinhopCutClampZ)
*/

//------------------------------------------------------------------------------------------

/*
FUNCTION_BEGIN(FunctionFriezeGroupSpinhopCutFreeZ,0,2,false,FnStructure)

  virtual const XYZ evaluate(const XYZ& p) const
    {
      const int d=SpinhopCut<FreeZ>(1.0)(arg(1),p,FreeZ());
      return FriezegroupEvaluate(arg(0),p,Spinhop(1.0,d),FreeZ());
    }
  
FUNCTION_END(FunctionFriezeGroupSpinhopCutFreeZ)
*/

//------------------------------------------------------------------------------------------

#endif
