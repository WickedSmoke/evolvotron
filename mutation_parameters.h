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

  //! Specifies the magnitude of random changes to MutatableImageNodeConstant.
  float _magnitude;

  //! Specifies the probability of a child being dropped and replaced with a new random stub.
  float _probability_glitch;

  //! Specifies the probability of all child nodes being reordered.
  float _probability_shuffle;

  //! Specifies the probability of a random stub being inserted before a child.
  float _probability_insert;
  
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

};

#endif
