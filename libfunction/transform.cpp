/**************************************************************************/
/*  Copyright 2012 Tim Day                                                */
/*                                                                        */
/*  This file is part of Evolvotron                                       */
/*                                                                        */
/*  Evolvotron is free software: you can redistribute it and/or modify    */
/*  it under the terms of the GNU General Public License as published by  */
/*  the Free Software Foundation, either version 3 of the License, or     */
/*  (at your option) any later version.                                   */
/*                                                                        */
/*  Evolvotron is distributed in the hope that it will be useful,         */
/*  but WITHOUT ANY WARRANTY; without even the implied warranty of        */
/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         */
/*  GNU General Public License for more details.                          */
/*                                                                        */
/*  You should have received a copy of the GNU General Public License     */
/*  along with Evolvotron.  If not, see <http://www.gnu.org/licenses/>.   */
/**************************************************************************/

/*! \file
  \brief Implementation for class Transform.
*/



#include "transform.h"

Transform::Transform()
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

Transform::Transform(const std::vector<real>& v,uint s)
{
  _translate.x(v[s+ 0]);_translate.y(v[s+ 1]);_translate.z(v[s+ 2]);
  _basis_x.x(  v[s+ 3]);_basis_x.y(  v[s+ 4]);_basis_x.z(  v[s+ 5]);
  _basis_y.x(  v[s+ 6]);_basis_y.y(  v[s+ 7]);_basis_y.z(  v[s+ 8]);
  _basis_z.x(  v[s+ 9]);_basis_z.y(  v[s+10]);_basis_z.z(  v[s+11]);
}

Transform::~Transform()
{}

const std::vector<real> Transform::get_columns() const
{
  std::vector<real> ret(12);

  ret[ 0]=_translate.x();ret[ 1]=_translate.y();ret[ 2]=_translate.z();
  ret[ 3]=_basis_x.x()  ;ret[ 4]=_basis_x.y()  ;ret[ 5]=_basis_x.z();
  ret[ 6]=_basis_y.x()  ;ret[ 7]=_basis_y.y()  ;ret[ 8]=_basis_y.z();
  ret[ 9]=_basis_z.x()  ;ret[10]=_basis_z.y()  ;ret[11]=_basis_z.z();

  return ret;
}

const XYZ Transform::transformed(const XYZ& p) const
{
  return _translate+_basis_x*p.x()+_basis_y*p.y()+_basis_z*p.z();
}

const XYZ Transform::transformed_no_translate(const XYZ& p) const
{
  return _basis_x*p.x()+_basis_y*p.y()+_basis_z*p.z();
}

Transform& Transform::concatenate_on_right(const Transform& t)
{
  const XYZ bx(transformed_no_translate(t.basis_x()));
  const XYZ by(transformed_no_translate(t.basis_y()));
  const XYZ bz(transformed_no_translate(t.basis_z()));
  const XYZ tr(transformed(t.translate()));
  
  translate(tr);
  basis_x(bx);
  basis_y(by);
  basis_z(bz);
  
  return *this;
}

Transform& Transform::concatenate_on_left(const Transform& t)
{
  const XYZ bx(t.transformed_no_translate(basis_x()));
  const XYZ by(t.transformed_no_translate(basis_y()));
  const XYZ bz(t.transformed_no_translate(basis_z()));
  const XYZ tr(t.transformed(translate()));
  
  translate(tr);
  basis_x(bx);
  basis_y(by);
  basis_z(bz);
  
  return *this;
}
