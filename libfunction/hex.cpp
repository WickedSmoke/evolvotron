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
  \brief Implementation of helper functions for hexagons.
*/

#include "hex.h"

//! Co-ordinates of hexagon with given hex-grid coords
const XY hex_coord(int x,int y)
{
  const real k=sqrt(3.0)/2.0;
  return XY(
	    x*k,
	    y+((x&1) ? 0.5 : 0.0)
	    );
}
 
const std::pair<int,int> nearest_hex(real px,real py)
{
  // Initial guess at which hex we're in:
  const real k=sqrt(3.0)/2.0;
  
  const int nx=static_cast<int>(rintf(px/k));
  const int ny=static_cast<int>
    (
     (nx&1) 
     ? 
     rintf(py-0.5) 
     :
     rintf(py)
     );
  
  int hx=nx;
  int hy=ny;
  const XY ph=hex_coord(nx,ny);
  real m2b=(XY(px,py)-ph).magnitude2();
  
  for (int dy=-1;dy<=1;dy++)
    for (int dx=-1;dx<=1;dx++)
      if (!(dy==0 && dx==0))
	{
	  const real m2=(XY(px,py)-hex_coord(nx+dx,ny+dy)).magnitude2();
	  if (m2<m2b)
	    {
	      hx=nx+dx;
	      hy=ny+dy;
	      m2b=m2;
	    }
	}
  return std::make_pair(hx,hy);
}
