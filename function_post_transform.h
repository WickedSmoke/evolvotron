// Source file for evolvotron
// Copyright (C) 2002 Tim Day
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
  This class would normally live in function.h (and is included and registered there), 
  but is split out so it can be efficiently used by MutatableImageDisplay and EvolvotronMain.
  NB There is no class heirarchy here as all virtualisation and boilerplate services are supplied when the functions are plugged into the FunctionNode template.
*/

#ifndef _function_post_transform_h_
#define _function_post_transform_h_

#include "xyz.h"
#include "function.h"
#include "function_node.h"

//! Function class returning leaf node evaluated at given position; result is then transfomed by a 12-component linear transform.
class FunctionPostTransform : public Function
{
 public:

  //! 12 parameters
  static const uint parameters()
    {
      return 12;
    }

  //! Single leaf arguments
  static const uint arguments()
    {
      return 1;
    }

  //! Return the evaluation of arg(0) at the transformed position argument.
  static const XYZ evaluate(const FunctionNode& our,const XYZ& p)
  {
    const Transform transform(our.params());
    return transform.transformed(our.arg(0)(p));
  }

  //! Has the same const-ness as arg(0)
  static const bool is_constant(const FunctionNode& our)
    {
      return our.arg(0).is_constant();
    }

};

#endif
