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
  \brief Interface for class MutationParameters.
*/

#ifndef _mutation_parameters_h_
#define _mutation_parameters_h_

#include "useful.h"

#include "random.h"

//! Class encapsulating mutation parameters.
/*! For example, magnitude of variations, probability of leaves being dropped.
  Also provides a random number generator.
 */
class MutationParameters
{
  //! A random number generator.
  /*! Declared mutable so we can pass const MutationParameters& around and still do useful work with it.
   */
  mutable Random01 _r01;

  //! Disables generation of (computationally intensive) iterative nodes.
  bool _allow_iterative_nodes;

  //! Disables generation of Mandelbrot & Julia set node types.
  bool _allow_fractal_nodes;

  //! Specifies the magnitude of random changes to MutatableImageNodeConstant.
  float _magnitude;

  //! Specifies the probability of a child being dropped and replaced with a new random stub.
  float _probability_glitch;

  //! Specifies the probability of all child nodes being reordered.
  float _probability_shuffle;

  //! Specifies the probability of a random stub being inserted before a child.
  float _probability_insert;

  //! Specifies the proportion of Constant nodes vs Position type nodes.
  float _proportion_constant;

  //! Specifies the probability of a using a MutatableImageNodePositionTransformed instead of MutatableImageNodePosition
  float _identity_supression;

  //! The maximum number of iterations an iterative function node can have initially.
  uint _max_initial_iterations;

  //! The probability of the number of iterations changing by plus or minus 1.
  float _probability_iterations_change_step;
  
  //! The probability of the number of iterations changing by times or divide 2.
  float _probability_iterations_change_jump;
  
 public:
  //! Trivial constructor.
  MutationParameters(uint seed);

  //! Trivial destructor.
  /*! virtual becuase Q_OBJECT/slot mechanism involves virtual functions
   */
  virtual ~MutationParameters();

  //! Reset to initial values.
  void reset();

  //! Returns a reference to the random number generator.
  /*! Need this for e.g RandomXYZInSphere constructor.
   */
  Random01& rng01() const
    {
      return _r01;
    }

  //! Return a number in the range [0,1)
  const float r01() const
    {
      return _r01();
    }


  //! Accessor.
  const bool allow_iterative_nodes() const
    {
      return _allow_iterative_nodes;
    }
  //! Accessor.
  void allow_iterative_nodes(bool v) 
    {
      _allow_iterative_nodes=v;
    }

  //! Accessor.
  const bool allow_fractal_nodes() const
    {
      return _allow_fractal_nodes;
    }
  //! Accessor.
  void allow_fractal_nodes(bool v) 
    {
      _allow_fractal_nodes=v;
    }


  //! Accessor.
  const float magnitude() const
    {
      return _magnitude;
    }
  //! Accessor.
  void magnitude(float v) 
    {
      _magnitude=v;
    }

  //! Accessor.
  const float probability_glitch() const
    {
      return _probability_glitch;
    }
  //! Accessor.
  void probability_glitch(float v)
    {
      _probability_glitch=v;
    }

  //! Accessor.
  const float probability_shuffle() const
    {
      return _probability_shuffle;
    }
  //! Accessor.
  void probability_shuffle(float v)
    {
      _probability_shuffle=v;
    }

  //! Accessor.
  const float probability_insert() const
    {
      return _probability_insert;
    }
  //! Accessor.
  void probability_insert(float v)
    {
      _probability_insert=v;
    }

  //! Accessor.
  const float proportion_constant() const
    {
      return _proportion_constant;
    }
  //! Accessor.
  void proportion_constant(float v)
    {
      _proportion_constant=v;
    }

  //! Accessor.
  const float identity_supression() const
    {
      return _identity_supression;
    }
  //! Accessor.
  void identity_supression(float v)
    {
      _identity_supression=v;
    }

  //! Accessor.
  const uint max_initial_iterations() const
    {
      return _max_initial_iterations;
    }
  //! Accessor.
  void max_initial_iterations(uint v)
    {
      _max_initial_iterations=v;
    }

  //! Accessor.
  const float probability_iterations_change_step() const
    {
      return _probability_iterations_change_step;
    }
  //! Accessor.
  void probability_iterations_change_step(float v)
    {
      _probability_iterations_change_step=v;
    }

  //! Accessor.
  const float probability_iterations_change_jump() const
    {
      return _probability_iterations_change_jump;
    }
  //! Accessor.
  void probability_iterations_change_jump(float v)
    {
      _probability_iterations_change_jump=v;
    }
};

#endif
