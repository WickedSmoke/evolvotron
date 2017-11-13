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
  As much as possible of the implementation should be pushed into the FunctionBoilerplate template.
*/

#ifndef _functions_spiral_h_
#define _functions_spiral_h_

#include "common.h" 

#include "function_boilerplate.h"

//------------------------------------------------------

FUNCTION_BEGIN(FunctionSpiralLinear,0,1,false,FnStructure)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      const real r=p.magnitude();
      real theta=atan2(p.y(),p.x());
      if (theta<0.0) theta+=2.0*M_PI;
      const real winding=floorf(r-theta/(2.0*M_PI));
      
      const real x=2.0*winding+theta/M_PI;
      const real y=2.0*r-x;
      
      return arg(0)(XYZ(x,y,p.z()));
    }
  
FUNCTION_END(FunctionSpiralLinear)

//------------------------------------------------------------------------------------------

FUNCTION_BEGIN(FunctionSpiralLogarithmic,0,1,false,FnStructure)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      const real r=p.magnitude();
      real theta=atan2(p.y(),p.x());
      if (theta<0.0) theta+=2.0*M_PI;
      const real lnr=log(r);
      const real winding=floorf(lnr-theta/(2.0*M_PI));
      
      const real x=2.0*winding+theta/M_PI;
      const real y=2.0*lnr-x;
      
      return arg(0)(XYZ(x,y,p.z()));
    }
  
FUNCTION_END(FunctionSpiralLogarithmic)

//------------------------------------------------------------------------------------------

#endif
