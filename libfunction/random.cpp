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
  \brief Implementation of class Random and derived classes.
*/

#include "random.h"

Random01::Random01(uint seed)
:Random()
{
  for (uint i=0;i<N;i++) 
    {
      mt[i]=(seed&0xffff0000);
      seed=(69069*seed+1);
      mt[i]|=((seed & 0xffff0000)>>16);
      seed=69069*seed+1;
    }
  mti = N;
}

Random01::~Random01()
{}

const double Random01::operator()()
{
  unsigned long y;

  // mag01[x] = x * MATRIX_A  for x=0,1
  static unsigned long mag01[2]={0x0,MATRIX_A};
  
  if (mti >= N) 
    {
      /* generate N words at one time */
      uint kk;
      
      for (kk=0;kk<N-M;kk++) 
	{
	  y = (mt[kk]&UPPER_MASK)|(mt[kk+1]&LOWER_MASK);
	  mt[kk] = mt[kk+M] ^ (y >> 1) ^ mag01[y & 0x1];
	}
      for (;kk<N-1;kk++)
	{
	  y = (mt[kk]&UPPER_MASK)|(mt[kk+1]&LOWER_MASK);
	  mt[kk] = mt[kk+(M-N)] ^ (y >> 1) ^ mag01[y & 0x1];
	}
      y = (mt[N-1]&UPPER_MASK)|(mt[0]&LOWER_MASK);
      mt[N-1] = mt[M-1] ^ (y >> 1) ^ mag01[y & 0x1];
      
      mti = 0;
    }
  
  y=mt[mti++];
  y^=TEMPERING_SHIFT_U(y);
  y^=TEMPERING_SHIFT_S(y)&TEMPERING_MASK_B;
  y^=TEMPERING_SHIFT_T(y)&TEMPERING_MASK_C;
  y^=TEMPERING_SHIFT_L(y);

  // Return [0,1) interval 
  return (static_cast<double>(y)*2.3283064365386963e-10); 
}
