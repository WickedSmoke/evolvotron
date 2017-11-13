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
  \brief Interface to Noise class.
*/

#ifndef _noise_h_
#define _noise_h_

#include "common.h"

#include "xyz.h"

//! Perlin noise generator.
class Noise
{
public:
  //! Constructor.
  Noise(uint seed);

  //! Return noise value at a point.
  real operator()(const XYZ& p) const;
  
protected:
  //! Number of table entries.
  enum {N=256};
  
  int _p[N+N+2];
  XYZ _g[N+N+2];
  
  //void setup(const XYZ&,int,int&,int&,real&,real&);
};

#endif

