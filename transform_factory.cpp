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
  \brief Implementation for class TransformFactory.
*/

#include "transform_factory.h"

const Transform TransformFactoryRandomWarpXY::operator()(Random01& r01) const
{
    // Gives a scale between 0.5 and 2, average 1.
  const float r=pow(2.0f,2.0f*r01()-1.0f);

  // Random rotation  
  const float a=(2.0f*M_PI)*r01();

  const XYZ basis_x( r*cos(a), r*sin(a),0.0f);
  const XYZ basis_y(-r*sin(a), r*cos(a),0.0f);
  const XYZ basis_z(0.0f,0.0f,1.0f);

  // Random translation
  const float tx=2.0f*r01()-1.0f;
  const float ty=2.0f*r01()-1.0f;
  const XYZ translate(tx,ty,0.0f);

  return Transform(translate,basis_x,basis_y,basis_z);
}

const Transform TransformFactoryRandomScaleXY::operator()(Random01& rng) const
{
  const XYZ translate(0.0f,0.0f,0.0f);

  const float p=rng();
  const float s=pow(2.0f,_lopow2+p*(_hipow2-_lopow2));
  const XYZ basis_x(   s,0.0f,0.0f);
  const XYZ basis_y(0.0f,   s,0.0f);
  const XYZ basis_z(0.0f,0.0f,1.0f);

  return Transform(translate,basis_x,basis_y,basis_z);
}

const Transform TransformFactoryRandomRotateZ::operator()(Random01& r01) const
{
  // Random rotation  
  const float a=(2.0f*M_PI)*r01();
  const float ca=cos(a);
  const float sa=sin(a);

  const XYZ basis_x(  ca,  sa,0.0f);
  const XYZ basis_y( -sa,  ca,0.0f);
  const XYZ basis_z(0.0f,0.0f,1.0f);
  const XYZ translate(0.0f,0.0f,0.0f);

  return Transform(translate,basis_x,basis_y,basis_z);
}

const Transform TransformFactoryRandomTranslateXYZ::operator()(Random01& r01) const
{
  const XYZ translate(_origin+RandomXYZInBox(r01,_range));
  const XYZ basis_x(1.0f,0.0f,0.0f);
  const XYZ basis_y(0.0f,1.0f,0.0f);
  const XYZ basis_z(1.0f,0.0f,1.0f);

  return Transform(translate,basis_x,basis_y,basis_z);
}
