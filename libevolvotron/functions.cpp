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
  \brief Implementation of specific Function classes.
  Except there's nothing here because it's all in the header.
*/
 
// This was supposed to localize class registration in this file, but doesn't.  See REGISTER definition.
#define DO_REGISTRATION

#include "useful.h"
#include "function_boilerplate.h"
#include "function_registry.h"             
#include "functions.h"

Noise FunctionNoiseOneChannel::_noise(100);

Noise FunctionMultiscaleNoiseOneChannel::_noise(101);

Noise FunctionNoiseThreeChannel::_noise0(200);
Noise FunctionNoiseThreeChannel::_noise1(300);
Noise FunctionNoiseThreeChannel::_noise2(400);

Noise FunctionMultiscaleNoiseThreeChannel::_noise0(201);
Noise FunctionMultiscaleNoiseThreeChannel::_noise1(202);
Noise FunctionMultiscaleNoiseThreeChannel::_noise2(203);

