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
  \brief Interfaces for class FunctionTop
  This class would normally live in functions.h (and is included and registered there), 
  but is split out so it can be efficiently used by MutatableImageDisplay and EvolvotronMain.
  NB There is no class heirarchy here as all virtualisation and boilerplate services are supplied when the functions are plugged into the FunctionNode template
*/

#ifndef _function_top_h_
#define _function_top_h_

#include "common.h"

#include "function_boilerplate.h"

class Transform;

//! Function intended primarily to be the top level function node.
/*! First 12 parameters are a space transform, second 12 paramters are a colour space transform.
*/
FUNCTION_BEGIN(FunctionTop,24,1,false,0)

public:
  //! This returns a random tree suitable for use as a starting image.
  static std::unique_ptr<FunctionTop> initial(const MutationParameters& parameters,const FunctionRegistration* specific_fn=0,bool unwrapped=false);

  virtual const XYZ evaluate(const XYZ& p) const;

  virtual FunctionTop* is_a_FunctionTop()
  {
      return this;
  }

  virtual const FunctionTop* is_a_FunctionTop() const
  {
      return this;
  }

  //! Overridden so transform and colours don't keep changing
  virtual void mutate(const MutationParameters& parameters,bool mutate_own_parameters=true);

  virtual void concatenate_pretransform_on_right(const Transform& transform);

  virtual void mutate_pretransform_parameters(const MutationParameters& parameters);
  virtual void reset_pretransform_parameters(const MutationParameters& parameters);
  virtual void mutate_posttransform_parameters(const MutationParameters& parameters);
  virtual void reset_posttransform_parameters(const MutationParameters& parameters);

private:

  const Transform interesting_pretransform(const MutationParameters& parameters,const real k);

FUNCTION_END(FunctionTop)

#endif
