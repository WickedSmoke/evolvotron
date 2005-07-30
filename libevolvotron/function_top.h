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
  \brief Interfaces for class FunctionTop
  This class would normally live in functions.h (and is included and registered there), 
  but is split out so it can be efficiently used by MutatableImageDisplay and EvolvotronMain.
  NB There is no class heirarchy here as all virtualisation and boilerplate services are supplied when the functions are plugged into the FunctionNode template.

*/

#ifndef _function_top_h_
#define _function_top_h_

#include "xyz.h"
#include "transform.h"
#include "function_node.h"
#include "function_boilerplate.h"

//! Function intended primarily to be the top level function node.
/*! First 12 parameters are a space transform, second 12 paramters are a colour space transform.
*/
FUNCTION_BEGIN(FunctionTop,24,1,false,0)

  static FunctionTop*const create(const MutationParameters& mutation_parameters,FunctionNode* root)
  {
    std::vector<FunctionNode*> a;
    a.push_back(root);
    return new FunctionTop(stubparams(mutation_parameters,24),a,0);
  }

  virtual const XYZ evaluate(const XYZ& p) const
  {
    const Transform space_transform(params(),0);
    const XYZ sp(space_transform.transformed(p)); 
    const XYZ v(arg(0)(sp));
    const XYZ tv(tanh(0.25*v.x()),tanh(0.25*v.y()),tanh(0.25*v.z()));
    // ...each component of tv is in [-1,1] so the transform parameters define a rhomboid in colour space.
    const Transform colour_transform(params(),12);
    return colour_transform.transformed(tv);
  }

  virtual FunctionTop*const is_a_FunctionTop()
  {
      return this;
  }

  virtual const FunctionTop*const is_a_FunctionTop() const
  {
      return this;
  }

  // Override so transform and colours don't keep changing
  void mutate(const MutationParameters& parameters,bool mutate_own_parameters=true)
  {
    FunctionNode::mutate(parameters,false);

    if (parameters.r01()<parameters.probability_parameter_reset())
      {
	reset_pretransform_parameters(parameters);
      }
    else
      {
	if (parameters.r01()<0.5) mutate_pretransform_parameters(parameters);
      }

    if (parameters.r01()<parameters.probability_parameter_reset())
      {
	reset_posttransform_parameters(parameters);
      }
    else
      {
	if (parameters.r01()<0.5) mutate_posttransform_parameters(parameters);
      }
  }

  virtual void concatenate_pretransform_on_right(const Transform& transform)
  {
    Transform current_transform(params(),0);
    current_transform.concatenate_on_right(transform);
    for (uint i=0;i<12;i++)
      params()[i]=current_transform.get_columns()[i];
  }

  virtual void mutate_pretransform_parameters(const MutationParameters& parameters)
  {
    for (uint i=0;i<11;i++)
      params()[i]+=parameters.magnitude_parameter_variation()*(parameters.r01()<0.5 ? -parameters.rnegexp() : parameters.rnegexp());
  }

  virtual void reset_pretransform_parameters(const MutationParameters& parameters)
  {
    const std::vector<real> p(stubparams(parameters,12));
    for (uint i=0;i<11;i++)
      params()[i]=p[i];
  }

  virtual void mutate_posttransform_parameters(const MutationParameters& parameters)
  {
    for (uint i=12;i<23;i++)
      params()[i]+=parameters.magnitude_parameter_variation()*(parameters.r01()<0.5 ? -parameters.rnegexp() : parameters.rnegexp());
  }

  virtual void reset_posttransform_parameters(const MutationParameters& parameters)
  {
    const std::vector<real> p(stubparams(parameters,12));
    for (uint i=0;i<11;i++)
      params()[12+i]=p[i];
  }

FUNCTION_END(FunctionTop)

#endif
