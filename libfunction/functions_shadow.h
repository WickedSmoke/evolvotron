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

#ifndef _functions_shadow_h_
#define _functions_shadow_h_

 

//------------------------------------------------------------------------------------------

//! Sum of two evaluations of a function, one sampled at a constant offset and weighted.
FUNCTION_BEGIN(FunctionShadow,4,1,false,0)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      return
	arg(0)(p)+param(3)*arg(0)(p+XYZ(param(0),param(1),param(2)));
    }
  
FUNCTION_END(FunctionShadow)

//------------------------------------------------------------------------------------------

//! Like FunctionShadow but the offset is obtained from a function.
FUNCTION_BEGIN(FunctionShadowGeneralised,1,2,false,0)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      return
	arg(0)(p)+param(0)*arg(0)(p+arg(1)(p));
    }
  
FUNCTION_END(FunctionShadowGeneralised)

//------------------------------------------------------------------------------------------

#endif
