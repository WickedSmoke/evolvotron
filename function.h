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
  \brief Interfaces for Function classes.
  NB There is no class heirarchy here as all virtualisation and boilerplate services are supplied when the functions are plugged into the FunctionNode template.
*/

#ifndef _function_h_
#define _function_h_

#include "xyz.h"

class FunctionNode;

//------------------------------------------------------------------------------------------

//! Function class representing a constant value.
class FunctionConstant
{
 public:
  
  // 3 parameters: one for each component of XYZ
  static const uint parameters()
    {
      return 3;
    }

  // No leaf arguments
  static const uint arguments()
    {
      return 0;
    }

  //! Returns the constant value/
  static const XYZ evaluate(const FunctionNode& our,const XYZ& p);

  //! Returns true, obviously.
  static const bool is_constant(const FunctionNode& our);
};

//------------------------------------------------------------------------------------------

//! Function class simply returning the position argument.
class FunctionIdentity
{
 public:

  //! No parameters
  static const uint parameters()
    {
      return 0;
    }

  //! No leaf arguments
  static const uint arguments()
    {
      return 0;
    }

  //! Simply return the position argument.
  static const XYZ evaluate(const FunctionNode& our,const XYZ& p);

  //! Is definitely not constant.
  static const bool is_constant(const FunctionNode& our);
};

//------------------------------------------------------------------------------------------

//! Function class returning position transfomed by a 12-component linear transform.
class FunctionTransform
{
 public:

  //! 12 parameters
  static const uint parameters()
    {
      return 12;
    }

  //! No leaf arguments
  static const uint arguments()
    {
      return 0;
    }

  //! Return the transformed position argument.
  static const XYZ evaluate(const FunctionNode& our,const XYZ& p);

  //! Is definitely not constant.
  static const bool is_constant(const FunctionNode& our);
};

//! Function class returning leaf node evaluated at position transfomed by a 12-component linear transform.
class FunctionPreTransform
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
  static const XYZ evaluate(const FunctionNode& our,const XYZ& p);

  //! Has the same const-ness as arg(0)
  static const bool is_constant(const FunctionNode& our);
};


#endif
