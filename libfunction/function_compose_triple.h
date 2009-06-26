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
  \brief Interfaces and implementation for FunctionComposeTriple
*/

#ifndef _function_compose_triple_h_
#define _function_compose_triple_h_

FUNCTION_BEGIN(FunctionComposeTriple,0,3,false,0)
  
  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      return arg(2)(arg(1)(arg(0)(p)));
    }

  //! Is constant if any (rather than default "all") function is constant.
  /*! One of the few cases it's worth overriding this method
   */
  virtual bool is_constant() const
    {
      return (arg(0).is_constant() || arg(1).is_constant() || arg(2).is_constant());
    }

FUNCTION_END(FunctionComposeTriple)

#endif
