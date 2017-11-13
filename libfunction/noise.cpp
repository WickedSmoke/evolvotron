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
  \brief Implementation of Noise class
*/



#include "noise.h"
#include "random.h"

Noise::Noise(uint seed)
{
  // We use our own random number generator so saved pictures will be the same when reloaded!
  // (Besides, one isn't actually conveniently available)
  Random01 r_01(seed);

  // Create an array of random gradient vectors uniformly on the unit sphere

  int i;
  
  for (i=0;i<N;i++)
    _g[i]=RandomXYZSphereNormal(r_01);
  
  // Create a pseudorandom permutation of [1..B] 
  
  for (i=0;i<=N;i++)
    _p[i]=i;
  
  for (i=N;i>0;i-=2) 
    {
      int j=((int)(r_01()*N));
      int k=_p[i];
      _p[i]=_p[j];
      _p[j]=k;
    }
  
  // Extend g and p arrays to allow for faster indexing
  
  for (i=0;i<N+2;i++)
    {
      _p[N+i]=_p[i];
      _g[N+i]=_g[i];
    }
}

inline real value(const XYZ& q,real rx,real ry,real rz)
{
  return rx*q.x()+ry*q.y()+rz*q.z();
}

inline real surve(real t)
{
  return t*t*(3.0-2.0*t);
}

inline real lerp(real t,real a,real b)
{
  return a+t*(b-a);
}

real Noise::operator()(const XYZ& p) const
{
  // Crank up the frequency a bit otherwise don't see much variation in base case
  const real tx=2.0*p.x()+10000.0;
  const real ty=2.0*p.y()+10000.0;
  const real tz=2.0*p.z()+10000.0;
  
  const int itx=(int)tx;
  const int ity=(int)ty;
  const int itz=(int)tz;

  const real rx0=tx-itx;
  const real ry0=ty-ity;
  const real rz0=tz-itz;

  const real rx1=rx0-1.0;
  const real ry1=ry0-1.0;
  const real rz1=rz0-1.0;

  const int bx0=(itx&(N-1));
  const int bx1=((bx0+1)&(N-1));
  const int by0=(ity&(N-1));
  const int by1=((by0+1)&(N-1));
  const int bz0=(itz&(N-1));
  const int bz1=((bz0+1)&(N-1));
      
  const int i=_p[bx0];
  const int b00=_p[i+by0];
  const int b01=_p[i+by1];
      
  const int j=_p[bx1];
  const int b10=_p[j+by0];
  const int b11=_p[j+by1];
  
  const real sx=surve(rx0);

  const real a0=lerp(sx,value(_g[b00+bz0],rx0,ry0,rz0),value(_g[b10+bz0],rx1,ry0,rz0));
  const real b0=lerp(sx,value(_g[b01+bz0],rx0,ry1,rz0),value(_g[b11+bz0],rx1,ry1,rz0));
  const real a1=lerp(sx,value(_g[b00+bz1],rx0,ry0,rz1),value(_g[b10+bz1],rx1,ry0,rz1));
  const real b1=lerp(sx,value(_g[b01+bz1],rx0,ry1,rz1),value(_g[b11+bz1],rx1,ry1,rz1));

  const real sy=surve(ry0);
      
  const real c=lerp(sy,a0,b0);  
  const real d=lerp(sy,a1,b1);

  const real sz=surve(rz0);
  
  return 1.5*lerp(sz,c,d);
}
