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
  \brief Interfaces for class FunctionPostTransform 
  This class would normally live in functions.h (and is included and registered there), 
  but is split out so it can be efficiently used by MutatableImageDisplay and EvolvotronMain.
  NB There is no class heirarchy here as all virtualisation and boilerplate services are supplied when the functions are plugged into the FunctionNode template.
*/

#ifndef _function_post_transform_h_
#define _function_post_transform_h_

#include "xyz.h"
#include "function_node.h"
#include "function_boilerplate.h"

//! Function class returning leaf node evaluated at given position; result is then transfomed by a 12-component linear transform.
FUNCTION_BEGIN(FunctionPostTransform,12,1,false)

  //! Return the evaluation of arg(0) at the transformed position argument.
  virtual const XYZ evaluate(const XYZ& p) const
  {
    const Transform transform(params());
    return transform.transformed(arg(0)(p));
  }

  //! Has the same const-ness as arg(0)
  virtual const bool is_constant() const
    {
      return arg(0).is_constant();
    }

  //! Override in specific class
  virtual const FunctionPostTransform*const is_a_FunctionPostTransform() const
    {
      return this;
    }

  //! Override in specific class
  virtual FunctionPostTransform*const is_a_FunctionPostTransform()
    {
      return this;
    }

 // Don't use FUNCTION_END because we don't want to REGISTER here 
};

#endif
