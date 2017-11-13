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
  \brief Implementation for class XYZ.
*/

#include "xyz.h"

#include "random.h"
#include "xy.h"

/*! Outputs whitespace-separated co-ordinates.
 */
std::ostream& XYZ::write(std::ostream& out) const
{
  return out << x() << " " << y() << " " << z();
}

RandomXYZInUnitCube::RandomXYZInUnitCube(Random01& rng)
:XYZ()
{
  x(rng());
  y(rng());
  z(rng());
}

RandomXYZInBox::RandomXYZInBox(Random01& rng,const XYZ& bounds)
:XYZ()
{
  x(-bounds.x()+2.0*bounds.x()*rng());
  y(-bounds.y()+2.0*bounds.y()*rng());
  z(-bounds.z()+2.0*bounds.z()*rng());
}

RandomXYZInSphere::RandomXYZInSphere(Random01& rng,real radius)
:XYZ(0.0,0.0,0.0)
{
  if (radius!=0.0)
    {
      do
	{
	  x(2.0*rng()-1.0);
	  y(2.0*rng()-1.0);
	  z(2.0*rng()-1.0);
	}
      while (magnitude2()>1.0);
      (*this)*=radius;
    }
}

RandomXYZSphereNormal::RandomXYZSphereNormal(Random01& rng)
:XYZ(0.0,0.0,0.0)
{
  real m2;
  do
    {
      assign(RandomXYZInSphere(rng,1.0));
      m2=magnitude2();
    }
  while (m2==0.0);
  
  (*this)/=sqrt(m2);
}

/*! Must handle case of individual axes being zero.
 */
RandomXYZInEllipsoid::RandomXYZInEllipsoid(Random01& rng,const XYZ& axes)
:XYZ()
{
  do
    {
      assign(RandomXYZInBox(rng,axes));
    }
  while (
	  (axes.x()==0.0 ? 0.0 : sqr(x()/axes.x()))
	 +(axes.y()==0.0 ? 0.0 : sqr(y()/axes.y()))
	 +(axes.z()==0.0 ? 0.0 : sqr(z()/axes.z()))
	 >1.0
	 );
}

RandomXYZInXYDisc::RandomXYZInXYDisc(Random01& rng,real radius)
:XYZ(0.0,0.0,0.0)
{
  if (radius!=0.0)
    {
      do
	{
	  x(2.0*rng()-1.0);
	  y(2.0*rng()-1.0);
	}
      while (magnitude2()>1.0);
      (*this)*=radius;
    }
}








