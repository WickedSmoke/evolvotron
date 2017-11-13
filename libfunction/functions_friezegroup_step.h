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

#ifndef _functions_friezegroup_step_h_
#define _functions_friezegroup_step_h_

#include "common.h"

#include "function_boilerplate.h"

#include "friezegroup.h"

//! Step (Conway p1a1): glide reflection only.
/*! Sawtooth x, out of step by half range across y-axis.
\verbatim
    o     o
  ---   ---
     ---   ---
       o     o
\endverbatim
*/

struct Step
{
  const XY operator()(const XY& p) const
  {
    return XY
      (
       (p.y()>0.0 ? modulusf(p.x(),1.0) : modulusf(p.x()+0.5,1.0)),
       fabs(p.y())
       );
  }
};

struct StepInvariant;

struct StepBlend;

//------------------------------------------------------------------------------------------

FUNCTION_BEGIN(FunctionFriezeGroupStepFreeZ,0,1,false,FnStructure)

  virtual const XYZ evaluate(const XYZ& p) const
    {
      return FriezegroupEvaluate(arg(0),p,Step(),FreeZ());
    }
  
FUNCTION_END(FunctionFriezeGroupStepFreeZ)

//------------------------------------------------------------------------------------------

FUNCTION_BEGIN(FunctionFriezeGroupStepClampZ,1,1,false,FnStructure)

  virtual const XYZ evaluate(const XYZ& p) const
    {
      return FriezegroupEvaluate(arg(0),p,Step(),ClampZ(param(0)));
    }
  
FUNCTION_END(FunctionFriezeGroupStepClampZ)

//------------------------------------------------------------------------------------------

#endif
