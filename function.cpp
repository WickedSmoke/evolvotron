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
  \brief Implementation of Function classes.
*/

#include "function.h"
#include "mutatable_image.h"
#include "matrix.h"
#include "transform.h"

//------------------------------------------------------------------------------------------

const XYZ FunctionConstant::evaluate(const MutatableImageNode& our,const XYZ&)
{
  return XYZ(our.param(0),our.param(1),our.param(2));
}

const bool FunctionConstant::is_constant(const MutatableImageNode&)
{
  return true;
}

//------------------------------------------------------------------------------------------

const XYZ FunctionIdentity::evaluate(const MutatableImageNode&,const XYZ& p)
{
  return p;
}

const bool FunctionIdentity::is_constant(const MutatableImageNode&)
{
  return false;
}

//------------------------------------------------------------------------------------------

const XYZ FunctionTransform::evaluate(const MutatableImageNode& our,const XYZ& p)
{
  const Transform transform(our.params());
  return transform.transformed(p);
}

const bool FunctionTransform::is_constant(const MutatableImageNode&)
{
  return false;
}

//------------------------------------------------------------------------------------------

const XYZ FunctionPreTransform::evaluate(const MutatableImageNode& our,const XYZ& p)
{
  const Transform transform(our.params());
  return our.arg(0)(transform.transformed(p));
}

const bool FunctionPreTransform::is_constant(const MutatableImageNode& our)
{
  return our.arg(0).is_constant();
}

//------------------------------------------------------------------------------------------
