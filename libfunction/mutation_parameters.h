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
  \brief Interface for class MutationParameters.
*/

#ifndef _mutation_parameters_h_
#define _mutation_parameters_h_

#include "common.h"
#include "random.h"

class FunctionNode;
class FunctionRegistration;
class FunctionRegistry;

//! Class encapsulating mutation parameters.
/*! For example, magnitude of variations, probability of leaves being dropped.
  Also provides a random number generator.
 */
class MutationParameters
{
 private:

  const std::unique_ptr<FunctionRegistry> _function_registry;

 protected:

  //! A random number generator.
  /*! Declared mutable so we can pass const MutationParameters& around and still do useful work with it.
   */
  mutable Random01 _r01;

  //! Negative-exponential generator might be useful too.
  mutable RandomNegExp _r_negexp;

  //! Specifies the base magnitude of random changes the function parameters.
  real _base_magnitude_parameter_variation;

  //! Specifies the base probability of a the parameter set being completely reset.
  real _base_probability_parameter_reset;

  //! Specifies the base probability of a child being dropped and replaced with a new random stub.
  real _base_probability_glitch;

  //! Specifies the base probability of all child nodes being reordered.
  real _base_probability_shuffle;

  //! Specifies the base probability of a random stub being inserted before a child.
  real _base_probability_insert;

  //! Specifies the base probability of a node being replaced with an alternate type.
  real _base_probability_substitute;

  //! Specifies the proportion of basic node types.
  real _proportion_basic;

  //! Specifies the proportion of Constant nodes vs Position type nodes.
  real _proportion_constant;

  //! Specifies the probability of a using a FunctionNodePositionTransformed instead of FunctionNodePosition
  real _identity_supression;

  //! The maximum number of iterations an iterative function node can have initially.
  uint _max_initial_iterations;

  //! The base probability of the number of iterations changing by plus or minus 1.
  real _base_probability_iterations_change_step;
  
  //! The base probability of the number of iterations changing by times or divide 2.
  real _base_probability_iterations_change_jump;

  //! Individual weighting modifiers for each function type
  /*! Will only be applied to random functions we're asked for.
    The bulk of nodes are created by FunctionNode and are boring to keep the branching ratio down.
    \todo Implement a branching ratio query method.
   */
  std::map<const FunctionRegistration*,real> _function_weighting;

  //! Total of function weights, for normalisation.
  real _function_weighting_total;

  //! Map from [0,1] to a function registration, taking weights into account.
  std::map<real,const FunctionRegistration*> _function_pick;

  //! What state a reset should return autocool to.
  const bool _autocool_reset_state;

  //! Whether autocooling is being applied.
  bool _autocool_enable;

  //! Number of generations at which parameters will be half cooled.
  uint _autocool_halflife;

  //! Count of number of generations for decay cooling.
  uint _autocool_generations;

  //! Just use SingleChannelNoise for almost all functions (useful for debugging).
  const bool _debug_mode;

  void recalculate_function_stuff();

 public:
  //! Trivial constructor.
  MutationParameters(uint seed,bool ac,bool debug_mode);

  //! Trivial destructor.
  virtual ~MutationParameters();

  //! Accessor.
  const FunctionRegistry& function_registry() const
    {
      return *_function_registry;
    }

  //! Reset to initial values.
  void reset();

  //! Multiply most parameters by the given factor
  void general_cool(real f);

  //! Returns a reference to the random number generator.
  /*! Need this for e.g RandomXYZInSphere constructor.
   */
  Random01& rng01() const
    {
      return _r01;
    }

  //! Return a number in the range [0,1)
  real r01() const
    {
      return _r01();
    }

  real rnegexp() const
    {
      return _r_negexp();
    }

  //! Accessor, with decay.
  real effective_magnitude_parameter_variation() const
    {
      return base_magnitude_parameter_variation()*decay_factor();
    }
  //! Accessor.
  real base_magnitude_parameter_variation() const
    {
      return _base_magnitude_parameter_variation;
    }
  //! Accessor.
  void base_magnitude_parameter_variation(real v) 
    {
      _base_magnitude_parameter_variation=v;
      report_change();
    }

  //! Accessor, with decay.
  real effective_probability_parameter_reset() const
    {
      return base_probability_parameter_reset()*decay_factor();
    }
  //! Accessor.
  real base_probability_parameter_reset() const
    {
      return _base_probability_parameter_reset;
    } 
  //! Accessor.
  void base_probability_parameter_reset(real v) 
    {
      _base_probability_parameter_reset=v;
      report_change();
    }

  //! Accessor, with decay.
  real effective_probability_glitch() const
    {
      return base_probability_glitch()*decay_factor();
    }
  //! Accessor.
  real base_probability_glitch() const
    {
      return _base_probability_glitch;
    }
  //! Accessor.
  void base_probability_glitch(real v)
    {
      _base_probability_glitch=v;
      report_change();
    }

  //! Accessor, with decay.
  real effective_probability_shuffle() const
    {
      return base_probability_shuffle()*decay_factor();
    }
  //! Accessor.
  real base_probability_shuffle() const
    {
      return _base_probability_shuffle;
    }
  //! Accessor.
  void base_probability_shuffle(real v)
    {
      _base_probability_shuffle=v;
      report_change();
    }

  //! Accessor, with decay.
  real effective_probability_insert() const
    {
      return base_probability_insert()*decay_factor();
    }
  //! Accessor.
  real base_probability_insert() const
    {
      return _base_probability_insert;
    }
  //! Accessor.
  void base_probability_insert(real v)
    {
      _base_probability_insert=v;
      report_change();
    }

  //! Accessor.
  real effective_probability_substitute() const
    {
      return base_probability_substitute()*decay_factor();
    }
  //! Accessor.
  real base_probability_substitute() const
    {
      return _base_probability_substitute;
    }
  //! Accessor.
  void base_probability_substitute(real v)
    {
      _base_probability_substitute=v;
      report_change();
    }

  //! Accessor.
  real proportion_constant() const
    {
      return _proportion_constant;
    }
  //! Accessor.
  void proportion_constant(real v)
    {
      _proportion_constant=v;
      report_change();
    }

  //! Accessor.
  real identity_supression() const
    {
      return _identity_supression;
    }
  //! Accessor.
  void identity_supression(real v)
    {
      _identity_supression=v;
      report_change();
    }

  //! Accessor.
  uint max_initial_iterations() const
    {
      return _max_initial_iterations;
    }
  //! Accessor.
  void max_initial_iterations(uint v)
    {
      _max_initial_iterations=v;
      report_change();
    }

  //! Accessor, with decay.
  real effective_probability_iterations_change_step() const
    {
      return base_probability_iterations_change_step()*decay_factor();
    }
  //! Accessor.
  real base_probability_iterations_change_step() const
    {
      return _base_probability_iterations_change_step;
    }
  //! Accessor.
  void base_probability_iterations_change_step(real v)
    {
      _base_probability_iterations_change_step=v;
      report_change();
    }

  //! Accessor, with decay.
  real effective_probability_iterations_change_jump() const
    {
      return base_probability_iterations_change_jump()*decay_factor();
    }
  //! Accessor.
  real base_probability_iterations_change_jump() const
    {
      return _base_probability_iterations_change_jump;
    }
  //! Accessor.
  void base_probability_iterations_change_jump(real v)
    {
      _base_probability_iterations_change_jump=v;
      report_change();
    }

  //! Accessor.
  real proportion_basic() const
    {
      return _proportion_basic;
    }
  //! Accessor.
  void proportion_basic(real p)
    {
      _proportion_basic=p;
      report_change();
    }

  //! Accessor.
  bool autocool_enable() const
    {
      return _autocool_enable;
    }
  //! Accessor.
  void autocool_enable(bool v)
    {
      _autocool_enable=v;
      std::clog << "Autocooling " << (autocool_enable() ? "ON" : "OFF") << "\n";
      report_change();
    }

  //! Accessor.
  int autocool_halflife() const
    {
      return _autocool_halflife;
    }
  //! Accessor.
  void autocool_halflife(int v)
    {
      _autocool_halflife=v;
      report_change();
    }

  //! Accessor
  int autocool_generations() const
    {
      return _autocool_generations;
    }
  //! Accessor.
  void autocool_generations(int v)
    {
      _autocool_generations=v;
      report_change();
    }
  //! Accessor.
  void autocool_generations_increment()
    {
      _autocool_generations++;
      report_change();      
    }

  //! Calculate branching ratio for above calls
  /* Call user should be checking this and diluting with boring nodes to keep it under control
   */
  real random_function_branching_ratio() const;

  //! This returns a new random bit of tree.
  /*! Setting the "exciting" flag avoids the most basic node types, but only at the top level of the stub tree.
   */
  std::unique_ptr<FunctionNode> random_function_stub(bool exciting) const;
    
  void change_function_weighting(const FunctionRegistration* fn,real w);

  void randomize_function_weightings_for_classifications(uint classification_mask);

  real get_weighting(const FunctionRegistration* fn);

 protected:

  //! Compute current decay factor
  real decay_factor() const;

  //! Return a random function appropriately biased by current settings
  std::unique_ptr<FunctionNode> random_function() const;

  //! Return a random function registration, appropriately biased by current settings
  const FunctionRegistration* random_weighted_function_registration() const;

  //! Intended for Qt-world subclass to override to emit signal. 
  virtual void report_change();
};

#endif
