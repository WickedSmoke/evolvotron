// Source file for evolvotron
// Copyright (C) 2002,2003 Tim Day
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
  \brief Interface for class FunctionNull 
  This class would normally live in functions.h (and is included and registered there), 
  but is split out so it can be efficiently used by MutatableImage.
*/

#ifndef _function_null_h_
#define _function_null_h_

#include "xyz.h"
#include "function_node.h"
#include "function_boilerplate.h"

//! Function class simply forwarding to leaf node
FUNCTION_BEGIN(FunctionNull,0,1,false,0)

  //! Simply return the position argument.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      return arg(0)(p);
    }

  //! Is constant if leaf is.
  virtual const bool is_constant() const
    {
      return arg(0).is_constant();
    }

// Don't use FUNCTION_END because we don't want to REGISTER here.
};

#endif
