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
  \brief Implementation of class MutationParameters.
*/

#include "mutation_parameters.h"
#include "function_registration.h"
#include "function_registry.h"
#include "function_boilerplate.h"
#include "function_core.h"

MutationParameters::MutationParameters(uint seed,QObject* parent)
  :QObject(parent)
  ,_r01(seed)
{
  reset();
}

MutationParameters::~MutationParameters()
{}

void MutationParameters::reset()
{
  _magnitude=0.5;
  
  _probability_glitch=0.02;
  _probability_shuffle=0.02;
  _probability_insert=0.02;
  _probability_substitute=0.02;

  _proportion_basic=0.7;

  _proportion_constant=0.5;
  _identity_supression=1.0;

  //! \todo Could do with _max_initial_iterations being higher (64?) but it slows things down.
  _max_initial_iterations=16;
  _probability_iterations_change_step=0.25;
  _probability_iterations_change_jump=0.02;

  for (
       std::vector<const FunctionRegistration*>::const_iterator it=FunctionRegistry::get()->registrations().begin();
       it!=FunctionRegistry::get()->registrations().end();
       it++
       )
    _function_weighting.insert(std::make_pair(*it,1.0f));

  recalculate_function_stuff();

  emit changed();
}

void MutationParameters::general_cool(float f)
{
  _magnitude*=f;

  _probability_glitch*=f;
  _probability_shuffle*=f;
  _probability_insert*=f;
  _probability_substitute*=f;

  _probability_iterations_change_step*=f;
  _probability_iterations_change_jump*=f;

  emit changed();
}

/*! This returns a random bit of image tree.
  It needs to be capable of generating any sort of node we have.
  \warning Too much probability of highly branching nodes could result in infinite sized stubs.
  \todo Compute (statistically) the expected number of nodes in a stub.
 */
FunctionNode*const MutationParameters::random_function_stub(bool exciting) const
{
  // Base mutations are Constant or Identity types.  
  // (Identity can be Identity or PositionTransformed, proportions depending on identity_supression parameter)
  const float base=proportion_basic();

  const float r=(exciting ? base+(1.0f-base)*r01() : r01());

  if (r<(1.0f-proportion_constant())*identity_supression()*base)
    {
      return FunctionTransform::stubnew(*this,false);
    }
  else if (r<(1.0f-proportion_constant())*base)
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

FunctionNode*const MutationParameters::random_function() const
{
  const FunctionRegistration*const fn_reg=random_weighted_function_registration();
  return (*(fn_reg->stubnew_fn()))(*this,false);    
}

const FunctionRegistration*const MutationParameters::random_weighted_function_registration() const
{  
  const float r=r01();
  
  const std::map<float,const FunctionRegistration*>::const_iterator it=_function_pick.lower_bound(r);

  // Just in case last key isn't quite 1.0f
  if (it!=_function_pick.end())
    {
      return (*it).second;
    }
  else
    {
      return (*(_function_pick.rbegin())).second;
    }
}

const float MutationParameters::random_function_branching_ratio() const
{
  float weighted_args=0.0f;

  for (
       std::map<const FunctionRegistration*,float>::const_iterator it=_function_weighting.begin();
       it!=_function_weighting.end();
       it++
       )
    {
      weighted_args+=(*it).second*(*it).first->args();
    }
  return weighted_args/_function_weighting_total;
}

void MutationParameters::change_function_weighting(const FunctionRegistration* fn,float w)
{
  _function_weighting[fn]=w;
  recalculate_function_stuff();
  emit changed();
}

const float MutationParameters::get_weighting(const FunctionRegistration* fn)
{
  std::map<const FunctionRegistration*,float>::const_iterator it=_function_weighting.find(fn);
  assert(fn!=_function_weighting.end());
  return (*it).second;
}

void MutationParameters::recalculate_function_stuff()
{
  _function_weighting_total=0.0f;
  for (
       std::map<const FunctionRegistration*,float>::const_iterator it=_function_weighting.begin();
       it!=_function_weighting.end();
       it++
       )
    _function_weighting_total+=(*it).second;

  float normalised=0.0f;
  _function_pick.clear();
  for (
       std::map<const FunctionRegistration*,float>::const_iterator it=_function_weighting.begin();
       it!=_function_weighting.end();
       it++
       )
    {
      normalised+=(*it).second/_function_weighting_total;
      _function_pick.insert(std::make_pair(normalised,(*it).first));
      std::clog << "[" << normalised << ":" << (*it).first->name() << "]";
    }
}
