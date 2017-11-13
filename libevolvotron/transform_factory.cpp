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
  \brief Implementation for class TransformFactory.
*/



#include "transform_factory.h"

#include "random.h"
#include "transform.h"

const Transform TransformFactoryRandomWarpXY::operator()(Random01& r01) const
{
    // Gives a scale between 0.5 and 2, average 1.
  const real r=pow(2.0,2.0*r01()-1.0);

  // Random rotation  
  const real a=(2.0*M_PI)*r01();

  const XYZ basis_x( r*cos(a), r*sin(a),0.0);
  const XYZ basis_y(-r*sin(a), r*cos(a),0.0);
  const XYZ basis_z(0.0,0.0,1.0);

  // Random translation
  const real tx=2.0*r01()-1.0;
  const real ty=2.0*r01()-1.0;
  const XYZ translate(tx,ty,0.0);

  return Transform(translate,basis_x,basis_y,basis_z);
}

const Transform TransformFactoryRandomScaleXY::operator()(Random01& rng) const
{
  const XYZ translate(0.0,0.0,0.0);

  const real p=rng();
  const real s=pow(2.0,_lopow2+p*(_hipow2-_lopow2));
  const XYZ basis_x(   s,0.0,0.0);
  const XYZ basis_y(0.0,   s,0.0);
  const XYZ basis_z(0.0,0.0,1.0);

  return Transform(translate,basis_x,basis_y,basis_z);
}

const Transform TransformFactoryRandomRotateZ::operator()(Random01& r01) const
{
  // Random rotation  
  const real a=(2.0*M_PI)*r01();
  const real ca=cos(a);
  const real sa=sin(a);

  const XYZ basis_x(  ca,  sa,0.0);
  const XYZ basis_y( -sa,  ca,0.0);
  const XYZ basis_z(0.0,0.0,1.0);
  const XYZ translate(0.0,0.0,0.0);

  return Transform(translate,basis_x,basis_y,basis_z);
}

const Transform TransformFactoryRandomTranslateXYZ::operator()(Random01& r01) const
{
  const XYZ translate(_origin+RandomXYZInBox(r01,_range));
  const XYZ basis_x(1.0,0.0,0.0);
  const XYZ basis_y(0.0,1.0,0.0);
  const XYZ basis_z(1.0,0.0,1.0);

  return Transform(translate,basis_x,basis_y,basis_z);
}
