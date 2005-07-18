// Source file for evolvotron
// Copyright (C) 2005 Tim Day
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
  \brief Implementation of noise related function classes.
*/
 
#include "functions_noise.h"

Noise FunctionNoiseOneChannel::_noise(100);

Noise FunctionMultiscaleNoiseOneChannel::_noise(101);

Noise FunctionNoiseThreeChannel::_noise0(200);
Noise FunctionNoiseThreeChannel::_noise1(300);
Noise FunctionNoiseThreeChannel::_noise2(400);

Noise FunctionMultiscaleNoiseThreeChannel::_noise0(201);
Noise FunctionMultiscaleNoiseThreeChannel::_noise1(202);
Noise FunctionMultiscaleNoiseThreeChannel::_noise2(203);

