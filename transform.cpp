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

