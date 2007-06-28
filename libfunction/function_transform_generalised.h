// Source file for evolvotron
// Copyright (C) 2005 Tim Day
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
  \brief Interfaces and implementation for FunctionTransformGeneralised
*/

#ifndef _function_transform_generalised_h_
#define _function_transform_generalised_h_

#include "function_boilerplate.h"
#include "transform.h"

FUNCTION_BEGIN(FunctionTransformGeneralised,0,4,false,0)

  //! Return the transformed position argument.
  virtual const XYZ evaluate(const XYZ& p) const
  {
    const Transform transform(arg(0)(p),arg(1)(p),arg(2)(p),arg(3)(p));
    return transform.transformed(p);
  }

FUNCTION_END(FunctionTransformGeneralised)

#endif

