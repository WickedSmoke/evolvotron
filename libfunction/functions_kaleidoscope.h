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

#ifndef _functions_kaleidoscope_h_
#define _functions_kaleidoscope_h_

#include "common.h" 

#include "function_boilerplate.h"

//------------------------------------------------------------------------------------------

//! Implements reflection of sampling point about multiple planes
FUNCTION_BEGIN(FunctionKaleidoscope,1,1,false,FnStructure)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      const uint n=2+static_cast<uint>(floor(8.0*fabs(param(0))));

      const real a=atan2(p.x(),p.y());
      const real r=sqrt(p.x()*p.x()+p.y()*p.y());
      
      const real sa=trianglef(a,M_PI/n);

      const XYZ s(r*sin(sa),r*cos(sa),p.z());
      return arg(0)(s);
    }
  
FUNCTION_END(FunctionKaleidoscope)

//------------------------------------------------------------------------------------------

//! Like FunctionKaleidoscope but Z drives rotation of underlying function
/*! Good for animation
 */
FUNCTION_BEGIN(FunctionKaleidoscopeZRotate,2,1,false,FnStructure)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      const uint n=2+static_cast<uint>(floor(8.0*fabs(param(0))));

      const real a=atan2(p.x(),p.y());
      const real r=sqrt(p.x()*p.x()+p.y()*p.y());
      
      const real sa=trianglef(a,M_PI/n)+param(1)*p.z();

      const XYZ s(r*sin(sa),r*cos(sa),0.0);
      return arg(0)(s);
    }
  
FUNCTION_END(FunctionKaleidoscopeZRotate)

//------------------------------------------------------------------------------------------

//! Like FunctionKaleidoscope with a twist
FUNCTION_BEGIN(FunctionKaleidoscopeTwist,2,1,false,FnStructure)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      const uint n=2+static_cast<uint>(floor(8.0*fabs(param(0))));

      const real a=atan2(p.x(),p.y());
      const real r=sqrt(p.x()*p.x()+p.y()*p.y());
      
      const real sa=trianglef(a-r*param(1),M_PI/n);

      const XYZ s(r*sin(sa),r*cos(sa),p.z());
      return arg(0)(s);
    }
  
FUNCTION_END(FunctionKaleidoscopeTwist)

//------------------------------------------------------------------------------------------

//! Implements reflection of sampling point about multiple planes
FUNCTION_BEGIN(FunctionWindmill,1,1,false,FnStructure)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      const uint n=1+static_cast<uint>(floor(8.0*fabs(param(0))));

      const real a=atan2(p.x(),p.y());
      const real r=sqrt(p.x()*p.x()+p.y()*p.y());
      
      const real sa=modulusf(a,M_PI/n);

      const XYZ s(r*sin(sa),r*cos(sa),p.z());
      return arg(0)(s);
    }
  
FUNCTION_END(FunctionWindmill)

//------------------------------------------------------------------------------------------

//! Like FunctionWindmill but Z drives rotation of underlying function
/*! Good for animation
 */
FUNCTION_BEGIN(FunctionWindmillZRotate,2,1,false,FnStructure)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      const uint n=1+static_cast<uint>(floor(8.0*fabs(param(0))));

      const real a=atan2(p.x(),p.y());
      const real r=sqrt(p.x()*p.x()+p.y()*p.y());
      
      const real sa=modulusf(a,M_PI/n)+param(1)*p.z();

      const XYZ s(r*sin(sa),r*cos(sa),0.0);
      return arg(0)(s);
    }
  
FUNCTION_END(FunctionWindmillZRotate)

//------------------------------------------------------------------------------------------

//! Like FunctionWindmill with twist
FUNCTION_BEGIN(FunctionWindmillTwist,2,1,false,FnStructure)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      const uint n=1+static_cast<uint>(floor(8.0*fabs(param(0))));

      const real a=atan2(p.x(),p.y());
      const real r=sqrt(p.x()*p.x()+p.y()*p.y());
      
      const real sa=modulusf(a-r*param(1),M_PI/n);

      const XYZ s(r*sin(sa),r*cos(sa),p.z());
      return arg(0)(s);
    }
  
FUNCTION_END(FunctionWindmillTwist)

//------------------------------------------------------------------------------------------

#endif
