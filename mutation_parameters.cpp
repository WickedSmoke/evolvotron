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
  \brief Implementation of class MutationParameters.
*/

#include "mutation_parameters.h"

MutationParameters::MutationParameters(uint seed)
  :_r01(seed)
{
  reset();
}

MutationParameters::~MutationParameters()
{}

void MutationParameters::reset()
{
  // Leave these off by default because they're slow
  _allow_iterative_nodes=false;

  // Leave these off by default because they're ugly
  _allow_fractal_nodes=false;

  _magnitude=0.5;
  
  _probability_glitch=0.02;
  _probability_shuffle=0.02;
  _probability_insert=0.02;

  _proportion_constant=0.5;
  _identity_supression=1.0;

  //! \todo Could do with _max_initial_iterations being higher (64?) but it slows things down.
  _max_initial_iterations=16;
  _probability_iterations_change_step=0.25;
  _probability_iterations_change_jump=0.02;
}
