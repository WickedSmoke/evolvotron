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
  \brief Base stuff for function classes.  Specific implementations in functions.h.
*/

#ifndef _function_h_
#define _function_h_

#include "function_node.h"
#include "function_node_using.h"
#include "function_registry.h"

#include "xyz.h"
#include "mutatable_image.h"
#include "matrix.h"
#include "transform.h"


//! Base function class.
/*! Doesn't do much because binding into FunctionNode via template instantiation doesn't need virtual
  methods.  Used to roll out common methods to large numbers of derived functions.
 */
class Function
{
 public:
  static const bool iterative()
    {
      return false;
    }
};

//! Base function class for iterative functions.
class FunctionIterative : public Function
{
 public:
  static const bool iterative()
    {
      return true;
    }
};

#endif
