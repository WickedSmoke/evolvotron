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

//! Macro to force instantiation of static registration members.
/*! Not declared static because that generates "unused" message.
 */
#define REGISTER(F) const Registration* force_ ## F = Registry::add(#F,&FunctionNodeUsing<F>::registration)


//------------------------------------------------------------------------------------------

REGISTER(FunctionConstant);

const XYZ FunctionConstant::evaluate(const FunctionNode& our,const XYZ&)
{
  return XYZ(our.param(0),our.param(1),our.param(2));
}

const bool FunctionConstant::is_constant(const FunctionNode&)
{
  return true;
}

//------------------------------------------------------------------------------------------

REGISTER(FunctionIdentity);

const XYZ FunctionIdentity::evaluate(const FunctionNode&,const XYZ& p)
{
  return p;
}

const bool FunctionIdentity::is_constant(const FunctionNode&)
{
  return false;
}

//------------------------------------------------------------------------------------------

REGISTER(FunctionTransform);

const XYZ FunctionTransform::evaluate(const FunctionNode& our,const XYZ& p)
{
  const Transform transform(our.params());
  return transform.transformed(p);
}

const bool FunctionTransform::is_constant(const FunctionNode&)
{
  return false;
}

//------------------------------------------------------------------------------------------

REGISTER(FunctionTransformGeneralised);

const XYZ FunctionTransformGeneralised::evaluate(const FunctionNode& our,const XYZ& p)
{
  const Transform transform(our.arg(0)(p),our.arg(1)(p),our.arg(2)(p),our.arg(3)(p));
  return transform.transformed(p);
}

const bool FunctionTransformGeneralised::is_constant(const FunctionNode&)
{
  return false;
}

//------------------------------------------------------------------------------------------

REGISTER(FunctionPreTransform);

const XYZ FunctionPreTransform::evaluate(const FunctionNode& our,const XYZ& p)
{
  const Transform transform(our.params());
  return our.arg(0)(transform.transformed(p));
}

const bool FunctionPreTransform::is_constant(const FunctionNode& our)
{
  return our.arg(0).is_constant();
}
//------------------------------------------------------------------------------------------

REGISTER(FunctionPreTransformGeneralised);

const XYZ FunctionPreTransformGeneralised::evaluate(const FunctionNode& our,const XYZ& p)
{
  const Transform transform(our.arg(1)(p),our.arg(2)(p),our.arg(3)(p),our.arg(4)(p));
  return our.arg(0)(transform.transformed(p));
}

const bool FunctionPreTransformGeneralised::is_constant(const FunctionNode& our)
{
  return our.arg(0).is_constant();
}

//------------------------------------------------------------------------------------------

REGISTER(FunctionPostTransform);

const XYZ FunctionPostTransform::evaluate(const FunctionNode& our,const XYZ& p)
{
  const Transform transform(our.params());
  return transform.transformed(our.arg(0)(p));
}

const bool FunctionPostTransform::is_constant(const FunctionNode& our)
{
  return our.arg(0).is_constant();
}

//------------------------------------------------------------------------------------------

REGISTER(FunctionPostTransformGeneralised);

const XYZ FunctionPostTransformGeneralised::evaluate(const FunctionNode& our,const XYZ& p)
{
  const Transform transform(our.arg(1)(p),our.arg(2)(p),our.arg(3)(p),our.arg(4)(p));
  return transform.transformed(our.arg(0)(p));
}

const bool FunctionPostTransformGeneralised::is_constant(const FunctionNode& our)
{
  return (
	  our.arg(0).is_constant()
	  && our.arg(1).is_constant()
	  && our.arg(2).is_constant()
	  && our.arg(3).is_constant()
	  && our.arg(4).is_constant()
	  );
}

//------------------------------------------------------------------------------------------
