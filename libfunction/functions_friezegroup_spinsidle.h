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

#ifndef _functions_friezegroup_spinsidle_h_
#define _functions_friezegroup_spinsidle_h_

#include "common.h"

#include "function_boilerplate.h"

#include "friezegroup.h"

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
struct Spinsidle 
{
  const XY operator()(const XY& p) const
  {
    return XY
      (
       trianglef(p.x(),0.5),
       modulusf(p.x()+0.5,2.0)<1.0 ? p.y() : -p.y()
       );
  }
};

struct SpinsidleInvariant;

struct SpinsidleBlend;

//------------------------------------------------------------------------------------------

FUNCTION_BEGIN(FunctionFriezeGroupSpinsidleFreeZ,0,1,false,FnStructure)

  virtual const XYZ evaluate(const XYZ& p) const
    {
      return FriezegroupEvaluate(arg(0),p,Spinsidle(),FreeZ());
    }
  
FUNCTION_END(FunctionFriezeGroupSpinsidleFreeZ)

//------------------------------------------------------------------------------------------

FUNCTION_BEGIN(FunctionFriezeGroupSpinsidleClampZ,1,1,false,FnStructure)

  virtual const XYZ evaluate(const XYZ& p) const
    {
      return FriezegroupEvaluate(arg(0),p,Spinsidle(),ClampZ(param(0)));
    }
  
FUNCTION_END(FunctionFriezeGroupSpinsidleClampZ)

//------------------------------------------------------------------------------------------

#endif
