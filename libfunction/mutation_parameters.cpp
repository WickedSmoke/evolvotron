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
  \brief Implementation of class MutationParameters.
*/



#include "mutation_parameters.h"

#include "function_registration.h"
#include "function_registry.h"
#include "function_constant.h"
#include "function_identity.h"
#include "function_transform.h"

MutationParameters::MutationParameters(uint seed,bool ac,bool debug_mode)
  :_function_registry(new FunctionRegistry())
   ,_r01(seed)
   ,_r_negexp(seed,1.0)
   ,_autocool_reset_state(ac)
   ,_debug_mode(debug_mode)
{
  reset();
}

MutationParameters::~MutationParameters()
{}

void MutationParameters::reset()
{
  _autocool_enable=_autocool_reset_state;
  _autocool_halflife=20;
  _autocool_generations=0;

  _base_magnitude_parameter_variation=0.25;

  _base_probability_parameter_reset=0.05;
  _base_probability_glitch=0.05;
  _base_probability_shuffle=0.05;
  _base_probability_insert=0.05;
  _base_probability_substitute=0.05;

  _proportion_basic=0.6;

  _proportion_constant=0.5;
  _identity_supression=1.0;

  //! \todo Could do with _max_initial_iterations being higher (64?) for fractal type things but it slows things down too much.
  _max_initial_iterations=16;
  _base_probability_iterations_change_step=0.25;
  _base_probability_iterations_change_jump=0.02;

  _function_weighting.clear();
  for (
       FunctionRegistry::Registrations::const_iterator it=_function_registry->registrations().begin();
       it!=_function_registry->registrations().end();
       it++
       )
    {
      if (_debug_mode)
	{
	  const FunctionRegistration*const fn=
#if BOOST_VERSION >= 103400
	    it->second;
#else
	    &*it;
#endif
	  real initial_weight=(fn->name()=="FunctionNoiseOneChannel" ? 1.0 : 1.0/1024.0);
	  _function_weighting.insert(std::make_pair(fn,initial_weight));
	}
      else
	{
	  real initial_weight=1.0;
	  const FunctionRegistration*const fn=
#if BOOST_VERSION >= 103400
	    it->second;
#else
	    &*it;
#endif
	  if (fn->classification() & FnIterative) initial_weight=1.0/1024.0;  // Ouch iterative functions are expensive
	  if (fn->classification() & FnFractal) initial_weight=1.0/1024.0;  // Yuk fractals are ugly
	  _function_weighting.insert(std::make_pair(fn,initial_weight));
	}
    }

  recalculate_function_stuff();

  report_change();
}

real MutationParameters::decay_factor() const
{
  assert(_autocool_halflife!=0);
  return (_autocool_enable ? pow(0.5,_autocool_generations/static_cast<double>(_autocool_halflife)) : 1.0);
}

void MutationParameters::general_cool(real f)
{
  _base_magnitude_parameter_variation*=f;

  _base_probability_parameter_reset*=f;
  _base_probability_glitch*=f;
  _base_probability_shuffle*=f;
  _base_probability_insert*=f;
  _base_probability_substitute*=f;

  _base_probability_iterations_change_step*=f;
  _base_probability_iterations_change_jump*=f;

  report_change();
}

/*! This returns a random bit of image tree.
  It needs to be capable of generating any sort of node we have.
  \warning Too much probability of highly branching nodes could result in infinite sized stubs.
  \todo Compute (statistically) the expected number of nodes in a stub.
 */
std::unique_ptr<FunctionNode> MutationParameters::random_function_stub(bool exciting) const
{
  // Base mutations are Constant or Identity types.  
  // (Identity can be Identity or PositionTransformed, proportions depending on identity_supression parameter)
  const real base=proportion_basic();

  const real r=(exciting ? base+(1.0-base)*r01() : r01());

  if (r<(1.0-proportion_constant())*identity_supression()*base)
    {
      return FunctionTransform::stubnew(*this,false);
    }
  else if (r<(1.0-proportion_constant())*base)
    {
      return FunctionIdentity::stubnew(*this,false);
    }
  else if (r<base)
    {
      return FunctionConstant::stubnew(*this,false);
    }
  else 
    {
      return random_function();
    }
}

std::unique_ptr<FunctionNode> MutationParameters::random_function() const
{
  const FunctionRegistration* fn_reg=random_weighted_function_registration();
  return (*(fn_reg->stubnew_fn()))(*this,false);    
}

const FunctionRegistration* MutationParameters::random_weighted_function_registration() const
{  
  const real r=r01();
  
  const std::map<real,const FunctionRegistration*>::const_iterator it=_function_pick.lower_bound(r);

  // Just in case last key isn't quite 1.0
  if (it!=_function_pick.end())
    {
      return (*it).second;
    }
  else
    {
      return (*(_function_pick.rbegin())).second;
    }
}

real MutationParameters::random_function_branching_ratio() const
{
  real weighted_args=0.0;

  for (
       std::map<const FunctionRegistration*,real>::const_iterator it=_function_weighting.begin();
       it!=_function_weighting.end();
       it++
       )
    {
      weighted_args+=(*it).second*(*it).first->args();
    }
  return weighted_args/_function_weighting_total;
}

void MutationParameters::change_function_weighting(const FunctionRegistration* fn,real w)
{
  _function_weighting[fn]=w;
  recalculate_function_stuff();
  report_change();
}

void MutationParameters::randomize_function_weightings_for_classifications(uint classification_mask)
{
  for (
       std::map<const FunctionRegistration*,real>::iterator it=_function_weighting.begin();
       it!=_function_weighting.end();
       it++
       )
    {
      if (classification_mask==0 || classification_mask==static_cast<uint>(-1) || ((*it).first->classification() & classification_mask))
	{
	  const int i=static_cast<int>(floor(11.0*r01()));
	  (*it).second=pow(2,-i);
	}
    }

  recalculate_function_stuff();

  report_change();
}

real MutationParameters::get_weighting(const FunctionRegistration* fn)
{
  std::map<const FunctionRegistration*,real>::const_iterator it=_function_weighting.find(fn);
  assert(it!=_function_weighting.end());
  return (*it).second;
}

void MutationParameters::recalculate_function_stuff()
{
  _function_weighting_total=0.0;
  for (
       std::map<const FunctionRegistration*,real>::const_iterator it=_function_weighting.begin();
       it!=_function_weighting.end();
       it++
       )
    _function_weighting_total+=(*it).second;

  real normalised=0.0;
  _function_pick.clear();
  for (
       std::map<const FunctionRegistration*,real>::const_iterator it=_function_weighting.begin();
       it!=_function_weighting.end();
       it++
       )
    {
      normalised+=(*it).second/_function_weighting_total;
      _function_pick.insert(std::make_pair(normalised,(*it).first));
    }
}

void MutationParameters::report_change()
{}

