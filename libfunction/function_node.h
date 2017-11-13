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
  \brief Interfaces for class FunctionNode and derived classes.
*/

#ifndef _function_node_h_
#define _function_node_h_

#include "common.h"

#include "xy.h"
#include "xyz.h"

class FunctionNodeInfo;
class FunctionTop;
class FunctionPreTransform;
class FunctionPostTransform;
class FunctionRegistry;
class MutatableImage;
class MutationParameters;

class Function : boost::noncopyable
{
 public:

  virtual ~Function()
    {}

  //! Convenience wrapper for evaluate (actually, evaluate is protected so can't be called externally anyway)
  const XYZ operator()(const XYZ& p) const
    {
      return evaluate(p);
    }

  //! Weighted evaluate; fastpath for zero weight.
  const XYZ operator()(const real weight,const XYZ& p) const
    {
      return (weight==0.0 ? XYZ(0.0,0.0,0.0) : weight*evaluate(p));
    }

  //! This what distinguishes different types of function.
  virtual const XYZ evaluate(const XYZ&) const
    =0;
};

//! Abstract base class for all kinds of mutatable image node.
/*! MutatableImage declared a friend to help constification of the public accessors.
 */
class FunctionNode : public Function
{
 public:
  friend class MutatableImage;

 private:
  //! The arguments (ie child nodes) for this node.
  boost::ptr_vector<FunctionNode> _args;

  //! The parameters (ie constant values) for this node.
  std::vector<real> _params;
    
  //! Number of iterations for iterative function types.  If zero, indicates non-iterative function.
  /*! \todo Perhaps someday push this out into a derived class.
   */
  uint _iterations;

 protected:

  //! This returns a deep-cloned copy of the node's children.
  std::unique_ptr<boost::ptr_vector<FunctionNode> > cloneargs() const;

  //! This returns a copy of the node's parameters
  const std::vector<real> cloneparams() const;

  //! Obtain some statistics about the image function
  void get_stats(uint& total_nodes,uint& total_parameters,uint& depth,uint& width,real& proportion_constant) const;
  
  //! Check function info against given number of parameters/arguments/iterative-flag.
  /*! Return true on success, false on fail with reasons in report string.
    Mainly for use by derived FunctionBoilerplate template to avoid duplicate code proliferation.
   */
  static bool verify_info(const FunctionNodeInfo& info,unsigned int np,unsigned int na,bool it,std::string& report);

  //! Build argument list.
  /*! Return true on success, false on fail with reasons in report string.
    Mainly for use by derived FunctionBoilerplate template to avoid duplicate code proliferation.
   */
  static bool create_args(const FunctionRegistry&,const FunctionNodeInfo& info,boost::ptr_vector<FunctionNode>& args,std::string& report);

 public:

  //! Returns true if the function is independent of it's position argument.
  /*! This isn't used for optimisation (which would require FunctionNode to have computation-specific state,
      which would wreck plans for reference counted deepclone()), 
      but to cull boring constant images on creation.
      Default implementation (and probably the only sensible one)
      is constant if all args are constant; no args returns false.
   */
  virtual bool is_constant() const;

  //! Internal self consistency check.
  virtual bool ok() const;

  //! Bits give some classification of the function type
  virtual uint self_classification() const
    =0;

  //@{
  //! Query the node as to whether it is a FunctionTop (return null if not).
  virtual const FunctionTop* is_a_FunctionTop() const;
  virtual FunctionTop* is_a_FunctionTop();
  //@}

  //! This returns a new random bit of tree.  Setting the "exciting" flag avoids basic node types, but only at the top level of the stub tree.
  static std::unique_ptr<FunctionNode> stub(const MutationParameters& parameters,bool exciting);

  //! This returns a vector of random parameter values.
  static void stubparams(std::vector<real>&,const MutationParameters& parameters,uint n);

  //! This returns a vector of new random bits of tree.
  static void stubargs(boost::ptr_vector<FunctionNode>&,const MutationParameters& parameters,uint n,bool exciting=false);

  //! Return a suitable starting value for a node's iteration count (assuming it's iterative).
  static uint stubiterations(const MutationParameters& parameters);

  //! Constructor given an array of params and args and an iteration count.
  /*! These MUST be provided; there are no alterative constructors.
   */
  FunctionNode(const std::vector<real>& p,boost::ptr_vector<FunctionNode>& a,uint iter);
  
  //! Build a FunctionNode given a description
  static std::unique_ptr<FunctionNode> create(const FunctionRegistry& function_registry,const FunctionNodeInfo& info,std::string& report);
  
  //! Destructor.
  virtual ~FunctionNode();

  //! Accessor
  void params(const std::vector<real>& p)
    {
      _params=p;
    }

  //! Accessor.
  const std::vector<real>& params() const
    {
      return _params;
    }

  //! Accessor. 
  real param(uint n) const
    {
      assert(n<params().size());
      return params()[n];
    }

  //! Accessor.
  uint iterations() const
    {
      return _iterations;
    }

  //! Accessor.
  const boost::ptr_vector<FunctionNode>& args() const
    {
      return _args;
    }
  
  //! Accessor.
  void args(boost::ptr_vector<FunctionNode>& a)
    {
      _args=a.release();
    }

  //! Accessor. 
  const FunctionNode& arg(uint n) const
    {
      assert(n<args().size());
      return args()[n];
    }

  //! Scramble this node and its leaves up a bit.
  virtual void mutate(const MutationParameters&,bool mutate_own_parameters=true);
  
  //! Return an clone of this image node and all its children.
  virtual std::unique_ptr<FunctionNode> deepclone() const
    =0;

  //! Prune any is_constant() nodes and replace them with an actual constant node
  virtual void simplify_constants();

  //! Return a deepcloned copy of the node's arguments
  virtual std::unique_ptr<boost::ptr_vector<FunctionNode> > deepclone_args() const;
  
  //! Save the function tree.
  virtual std::ostream& save_function(std::ostream& out,uint indent) const
    =0;

 protected:

  //! Save the function tree.  Common code needing a function name.
  std::ostream& save_function(std::ostream& out,uint indent,const std::string& function_name) const;

  //! Accessor (non-const).
  boost::ptr_vector<FunctionNode>& args()
    {
      return _args;
    }

  //! Accessor (non-const).
  std::vector<real>& params()
    {
      return _params;
    }

  //! Accessor. 
  FunctionNode& arg(uint n)
    {
      assert(n<args().size());
      return args()[n];
    }
 protected:
  //@{
  //! Useful constants used when some small sampling step is required (e.g gradient operators).
  static real epsilon() {return 1e-6;}
  static real epsilon2() {return 2.0*epsilon();}
  static real inv_epsilon() {return 1.0/epsilon();}
  static real inv_epsilon2() {return 1.0/epsilon2();}
  static real big_epsilon() {return sqrt(epsilon());}
  //@}
};

#endif
