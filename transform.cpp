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
  \brief Implementation for class Transform.
*/

#include "transform.h"
#include <iostream>

Transform::Transform()
  :_translate(0.0f,0.0f,0.0f)
   ,_basis_x(1.0f,0.0f,0.0f)
   ,_basis_y(0.0f,1.0f,0.0f)
   ,_basis_z(0.0f,0.0f,1.0f)
{}

Transform::Transform(const Transform& t)
  :_translate(t.translate())
   ,_basis_x(t.basis_x())
   ,_basis_y(t.basis_y())
   ,_basis_z(t.basis_z())
{}

Transform::Transform(const XYZ& t,const XYZ& x,const XYZ& y,const XYZ& z)
  :_translate(t)
   ,_basis_x(x)
   ,_basis_y(y)
   ,_basis_z(z)
{}

Transform::Transform(const std::vector<float>& v)
{
  assert(v.size()==12);

  _translate.x(v[ 0]);_translate.y(v[ 1]);_translate.z(v[ 2]);
  _basis_x.x(  v[ 3]);_basis_x.y(  v[ 4]);_basis_x.z(  v[ 5]);
  _basis_y.x(  v[ 6]);_basis_y.y(  v[ 7]);_basis_y.z(  v[ 8]);
  _basis_z.x(  v[ 9]);_basis_z.y(  v[10]);_basis_z.z(  v[11]);
}

const std::vector<float> Transform::get_columns() const
{
  std::vector<float> ret(12);

  ret[ 0]=_translate.x();ret[ 1]=_translate.y();ret[ 2]=_translate.z();
  ret[ 3]=_basis_x.x()  ;ret[ 4]=_basis_x.y()  ;ret[ 5]=_basis_x.z();
  ret[ 6]=_basis_y.x()  ;ret[ 7]=_basis_y.y()  ;ret[ 8]=_basis_y.z();
  ret[ 9]=_basis_z.x()  ;ret[10]=_basis_z.y()  ;ret[11]=_basis_z.z();

  return ret;
}
