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
  \brief Interface for class MutationParameters.
*/

#ifndef _mutation_parameters_h_
#define _mutation_parameters_h_

#include <map>

#include <qobject.h>

#include "useful.h"
#include "random.h"

class FunctionRegistration;
class FunctionNode;

//! Class encapsulating mutation parameters.
/*! For example, magnitude of variations, probability of leaves being dropped.
  Also provides a random number generator.
  Is a QObject so we can use signals to notify clients of state changes.
 */
class MutationParameters : public QObject
{
 private:
  Q_OBJECT

 protected:
  //! A random number generator.
  /*! Declared mutable so we can pass const MutationParameters& around and still do useful work with it.
   */
  mutable Random01 _r01;

  //! Specifies the magnitude of random changes to FunctionNodeConstant.
  float _magnitude;

  //! Specifies the probability of a child being dropped and replaced with a new random stub.
  float _probability_glitch;

  //! Specifies the probability of all child nodes being reordered.
  float _probability_shuffle;

  //! Specifies the probability of a random stub being inserted before a child.
  float _probability_insert;

  //! Specifies the probability of a node being replaced with an alternate type.
  float _probability_substitute;

  //! Specifies the proportion of basic node types.
  float _proportion_basic;

  //! Specifies the proportion of Constant nodes vs Position type nodes.
  float _proportion_constant;

  //! Specifies the probability of a using a FunctionNodePositionTransformed instead of FunctionNodePosition
  float _identity_supression;

  //! The maximum number of iterations an iterative function node can have initially.
  uint _max_initial_iterations;

  //! The probability of the number of iterations changing by plus or minus 1.
  float _probability_iterations_change_step;
  
  //! The probability of the number of iterations changing by times or divide 2.
  float _probability_iterations_change_jump;

  //! Individual weighting modifiers for each function type
  /*! Will only be applied to random functions we're asked for.
    The bulk of nodes are created by FunctionNode and are boring to keep the branching ratio down.
    \todo Implement a branching ratio query method.
   */
  std::map<const FunctionRegistration*,float> _function_weighting;

  //! Total of function weights, for normalisation.
  float _function_weighting_total;

  //! Map from [0,1] to a function registration, taking weights into account.
  std::map<float,const FunctionRegistration*> _function_pick;

  void recalculate_function_stuff();

 public:
  //! Trivial constructor.
  MutationParameters(uint seed,QObject* parent);

  //! Trivial destructor.
  /*! virtual becuase Q_OBJECT/slot mechanism involves virtual functions
   */
  virtual ~MutationParameters();

  //! Reset to initial values.
  void reset();

  //! Multiply most parameters by the given factor
  void general_cool(float f);

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
      emit changed();
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
      emit changed();
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
      emit changed();
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
      emit changed();
    }

  //! Accessor.
  const float probability_substitute() const
    {
      return _probability_substitute;
    }
  //! Accessor.
  void probability_substitute(float v)
    {
      _probability_substitute=v;
      emit changed();
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
      emit changed();
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
      emit changed();
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
      emit changed();
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
      emit changed();
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
      emit changed();
    }

  //! Accessor.
  const float proportion_basic() const
    {
      return _proportion_basic;
    }
  //! Accessor.
  void proportion_basic(float p)
    {
      _proportion_basic=p;
      emit changed();
    }

  //! Calculate branching ratio for above calls
  /* Call user should be checking this and diluting with boring nodes to keep it under control
   */
  const float random_function_branching_ratio() const;

  //! This returns a new random bit of tree.
  /*! Setting the "exciting" flag avoids the most basic node types, but only at the top level of the stub tree.
   */
  FunctionNode*const MutationParameters::random_function_stub(bool exciting) const;
    
  void change_function_weighting(const FunctionRegistration* fn,float w);

  const float get_weighting(const FunctionRegistration* fn);

 protected:

  //! Return a random function appropriately biased by current settings
  FunctionNode*const MutationParameters::random_function() const;

  //! Return a random function registration, appropriately biased by current settings
  const FunctionRegistration*const random_weighted_function_registration() const;

 public:
 signals:
  void changed();
};

#endif
