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
  \brief Interfaces and implementation for certain core Function classes.
  Split out from functions.h as included in mutation_parameters.cpp
*/

#ifndef _function_identity_h_
#define _function_identity_h_


//! Function class simply returning the position argument.
FUNCTION_BEGIN(FunctionIdentity,0,0,false,FnCore)

  //! Simply return the position argument.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      return p;
    }

FUNCTION_END(FunctionIdentity)

//------------------------------------------------------------------------------------------

#endif
