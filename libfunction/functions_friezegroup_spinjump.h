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

#ifndef _functions_friezegroup_spinjump_h_
#define _functions_friezegroup_spinjump_h_

#include "common.h"

#include "function_boilerplate.h"

#include "friezegroup.h"

//------------------------------------------------------------------------------------------

FUNCTION_BEGIN(FunctionFriezeGroupSpinjumpFreeZ,0,1,false,FnStructure)

  virtual const XYZ evaluate(const XYZ& p) const
    {
      return FriezegroupEvaluate(arg(0),p,Spinjump(1.0),FreeZ());
    }
  
FUNCTION_END(FunctionFriezeGroupSpinjumpFreeZ)

//------------------------------------------------------------------------------------------

FUNCTION_BEGIN(FunctionFriezeGroupSpinjumpClampZ,1,1,false,FnStructure)
  // Don't think this form can be warped without breaking symmetry

  virtual const XYZ evaluate(const XYZ& p) const
    {
      return FriezegroupEvaluate(arg(0),p,Spinjump(1.0),ClampZ(param(0)));
    }
  
FUNCTION_END(FunctionFriezeGroupSpinjumpClampZ)

//------------------------------------------------------------------------------------------

#endif
