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
  \brief Interfaces and implementation for FunctionComposeTriple
*/

#ifndef _function_compose_pair_h_
#define _function_compose_pair_h_

#include "common.h"

#include "function_boilerplate.h"

FUNCTION_BEGIN(FunctionComposePair,0,2,false,0)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      return arg(1)(arg(0)(p));
    }

  //! Is constant if any (rather than default "all") function is constant.
  /*! One of the few cases it's worth overriding this method
   */
  virtual bool is_constant() const
    {
      return (arg(0).is_constant() || arg(1).is_constant());
    }

FUNCTION_END(FunctionComposePair)

#endif
