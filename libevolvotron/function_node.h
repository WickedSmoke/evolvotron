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
  \brief Interfaces for class FunctionNode and derived classes.
*/

#ifndef _function_node_h_
#define _function_node_h_

#include <vector>
#include <map>
#include <iostream>
#include <string>

#include "useful.h"
#include "xyz.h"
#include "transform.h"

#include "mutation_parameters.h"

#include "function_registry.h"

class FunctionNodeInfo;

class FunctionPreTransform;
template <typename F> class FunctionNodeUsing;

//! Abstract base class for all kinds of mutatable image node.
class FunctionNode
{
 private:

  //! The arguments (ie child nodes) for this node.
  std::vector<FunctionNode*> _args;

  //! The parameters (ie constant values) for this node.
  std::vector<float> _params;
    
  //! Number of iterations for iterative function types.  If zero, indicates non-iterative function.
  /*! \todo Perhaps someday push this out into a derived class.
   */
  uint _iterations;

 protected:

  //! This returns a deep-cloned copy of the node's children.
  const std::vector<FunctionNode*> cloneargs() const;

  //! This returns a copy of the node's parameters
  const std::vector<float> cloneparams() const;

  //! This what distinguishes different types of node.
  virtual const XYZ evaluate(const XYZ& p) const
    =0;

 public:

  //! Returns true if the function is independent of it's position argument.
  /*! This isn't used for optimisation (which would require FunctionNode to have computation-specific state,
      which would wreck plans for reference counted deepclone()), 
      but to cull boring constant images on creation.
   */
  virtual const bool is_constant() const
    =0;

  //@{
  //! Query the node as to whether it is a MutatableImageUsing<FunctionPreTransform> (return null if not).
  /*! This is useful for accumulating view transforms at the front end of a function tree without creating a new transform each time.
   */
  virtual const FunctionNodeUsing<FunctionPreTransform>*const is_a_FunctionNodeUsingFunctionPreTransform() const;
  virtual FunctionNodeUsing<FunctionPreTransform>*const is_a_FunctionNodeUsingFunctionPreTransform();
  //@}

  //! This returns a new random bit of tree.  Setting the "exciting" flag avoids basic node types, but only at the top level of the stub tree.
  static FunctionNode*const stub(const MutationParameters& parameters,bool exciting);

  //! This returns a random bit of tree suitable for use as a starting image.
  static FunctionNode*const initial(const MutationParameters& parameters);

  //! This returns a vector of random parameter values.
  static const std::vector<float> stubparams(const MutationParameters& parameters,uint n);

  //! This returns a vector of new random bits of tree.
  static const std::vector<FunctionNode*> stubargs(const MutationParameters& parameters,uint n);

  //! Return a suitable starting value for a node's iteration count (assuming it's iterative).
  static const uint FunctionNode::stubiterations(const MutationParameters& parameters);

  //! Constructor given an array of params and args and an iteration count.
  /*! These MUST be provided; there are no alterative constructors.
   */
  FunctionNode(const std::vector<float>& p,const std::vector<FunctionNode*>& a,uint iter);
  
  //! Build a MutatableImage given a description
  static FunctionNode*const create(const FunctionNodeInfo* info,std::string& report);
  
  //! Destructor.
  virtual ~FunctionNode();

  //! Accessor
  void params(const std::vector<float>& p)
    {
      _params=p;
    }

  //! Accessor.
  const std::vector<float>& params() const
    {
      return _params;
    }

  //! Accessor. 
  const float param(uint n) const
    {
      assert(n<params().size());
      return params()[n];
    }

  //! Accessor
  const uint iterations() const
    {
      return _iterations;
    }

  //! Accessor.
  void args(const std::vector<FunctionNode*>& a)
    {
      _args=a;
    }

  //! Accessor.
  const std::vector<FunctionNode*>& args() const
    {
      return _args;
    }

  //! Accessor. 
  const FunctionNode& arg(uint n) const
    {
      assert(n<args().size());
      return *(args()[n]);
    }

  //! Scramble this node and its leaves up a bit.
  virtual void mutate(const MutationParameters&);
  
  //! Return an clone of this image node and all its children.
  virtual FunctionNode*const deepclone() const
    =0;
  
  //! Convenience wrapper for evaluate (actually, evaluate is protected so can't be called externally anyway)
  const XYZ operator()(const XYZ& p) const
    {
      return evaluate(p);
    }

  //! Internal self-consistency check.
  virtual const bool ok() const;

  //! Save the function tree.
  virtual std::ostream& save_function(std::ostream& out,uint indent) const;

 protected:

  //! Accessor (non-const version is protected, required internally to obtain non-const iterators).
  std::vector<float>& params()
    {
      return _params;
    }

  //! Accessor (non-const version is protected, required internally to obtain non-const iterators).
  std::vector<FunctionNode*>& args()
    {
      return _args;
    }
};

#endif
