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

REGISTER(FunctionTransformQuadratic);

const XYZ FunctionTransformQuadratic::evaluate(const FunctionNode& our,const XYZ& p)
{
  const XYZ translate(our.param( 0),our.param( 1),our.param( 2));
  const XYZ basis_x  (our.param( 3),our.param( 4),our.param( 5));
  const XYZ basis_y  (our.param( 6),our.param( 7),our.param( 8));
  const XYZ basis_z  (our.param( 9),our.param(10),our.param(11));
  const XYZ basis_xy (our.param(12),our.param(13),our.param(14));
  const XYZ basis_xz (our.param(15),our.param(16),our.param(17));
  const XYZ basis_yz (our.param(18),our.param(19),our.param(20));
  const XYZ basis_xx (our.param(21),our.param(22),our.param(23));
  const XYZ basis_yy (our.param(24),our.param(25),our.param(26));
  const XYZ basis_zz (our.param(27),our.param(28),our.param(29));

  return 
    translate
    +basis_x*p.x()+basis_y*p.y()+basis_z*p.z()
    +basis_xy*(p.x()*p.y())+basis_xz*(p.x()*p.z())+basis_yz*(p.y()*p.z())
    +basis_xx*(p.x()*p.x())+basis_yy*(p.y()*p.y())+basis_zz*(p.z()*p.z());
}

const bool FunctionTransformQuadratic::is_constant(const FunctionNode&)
{
  return false;
}

//------------------------------------------------------------------------------------------
