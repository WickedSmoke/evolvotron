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
  \brief Interfaces for class FunctionNode and derived classes.
*/

#ifndef _function_node_h_
#define _function_node_h_

#include <vector>
#include <map>
#include <iostream>

#include "useful.h"
#include "xyz.h"
#include "transform.h"

#include "mutation_parameters.h"

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
  static FunctionNode*const stub(const MutationParameters& parameters,bool exciting=false);

  //! This returns a random bit of tree suitable for use as a starting image.
  static FunctionNode*const initial(const MutationParameters& parameters);

  //! This returns a vector of random parameter values.
  static const std::vector<float> stubparams(const MutationParameters& parameters,uint n);

  //! This returns a vector of new random bits of tree.
  static const std::vector<FunctionNode*> stubargs(const MutationParameters& parameters,uint n);

  //! Constructor given an array of params. and args (these MUST be provided; there are no alterative constructors).
  FunctionNode(const std::vector<float>& p,const std::vector<FunctionNode*>& a);

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

class Registration
{
 public:
  Registration()
    {}
};

class Registry
{
 public:
  std::map<std::string,const Registration*> _registry;

  Registry()
    {}

  /*
    void add(const Registration* reg)
    {
      _registry[reg->_name]=reg;
    }
  */
  static const Registration* add(const std::string& name,const Registration* reg)
  {
    std::clog << "Registry : add " << name << "\n";
    return reg;
  }
};


//! Template class to generate boilerplate for virtual methods.
template <typename F> class FunctionNodeUsing : public FunctionNode
{
 public:

  //! Registration member encapsulates class meta-information needed to 
  static Registration registration;
  
 protected:
  
  //! Evaluation supplied by the wrapped class.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      return F::evaluate(*this,p);
    }
  
 public:
  
  //! Constant-ness supplied by the wrapped class.
  virtual const bool is_constant() const
    {
      return F::is_constant(*this);
    }
  
  //! Constructor
  FunctionNodeUsing(const std::vector<float>& p,const std::vector<FunctionNode*>& a)
    :FunctionNode(p,a)
    {
      assert(params().size()==F::parameters());
      assert(args().size()==F::arguments());
    }
  
  //! Destructor.
  virtual ~FunctionNodeUsing()
    {}
  
  //! Return a deeploned copy.
  virtual FunctionNode*const deepclone() const
    {
      return new FunctionNodeUsing<F>(cloneparams(),cloneargs());
    }
  
  //! Internal self-consistency check.  We can add some extra checks.
  virtual const bool ok() const
    {
      return 
	(
	 params().size()==F::parameters() &&
	 args().size()==F::arguments() &&
	 FunctionNode::ok()
	 );
    }

  //! Implementation depends on template parameter
  virtual const FunctionNodeUsing<FunctionPreTransform>*const is_a_FunctionNodeUsingFunctionPreTransform() const;

  //! Implementation depends on template parameter
  virtual FunctionNodeUsing<FunctionPreTransform>*const is_a_FunctionNodeUsingFunctionPreTransform();
};

//! In the general case this still returns 0
template <typename F> inline const FunctionNodeUsing<FunctionPreTransform>*const FunctionNodeUsing<F>::is_a_FunctionNodeUsingFunctionPreTransform() const
{
  return 0;
}

//! In the general case this still returns 0
template <typename F> inline FunctionNodeUsing<FunctionPreTransform>*const FunctionNodeUsing<F>::is_a_FunctionNodeUsingFunctionPreTransform()
{
  return 0;
}

//! Specialisation for FunctionPreTransform
template <> inline const FunctionNodeUsing<FunctionPreTransform>*const FunctionNodeUsing<FunctionPreTransform>::is_a_FunctionNodeUsingFunctionPreTransform() const
{
  return this;
}

//! Specialisation for FunctionPreTransform
template <> inline FunctionNodeUsing<FunctionPreTransform>*const FunctionNodeUsing<FunctionPreTransform>::is_a_FunctionNodeUsingFunctionPreTransform()
{
  return this;
}

//! You'd expect this to live in the .cpp, but instantiation should only be triggered ONCE by REGISTER macros in function.h which is only included in function_node.cpp.
/*! There is the possibility of associating a name with the association using typeid(F).name()
  but it's not very useful as it has name mangling stuff attached.
 */
template <typename F> Registration FunctionNodeUsing<F>::registration;

//----------------------------------------------------------------------------------------

//! Base class for iterative node types.
/*! evaluate, is_constant and deepclone methods remain virtual.
 */
class FunctionNodeIterative : public FunctionNode
{
 private:

 protected:
  
  //! Number of iterations performed (or maximum number).
  uint _iterations;

 public:
  //! Constructor.
  FunctionNodeIterative(const std::vector<float>& p,const std::vector<FunctionNode*>& a,uint i);

  //! Destructor.
  virtual ~FunctionNodeIterative();
  
  void mutate(const MutationParameters& parameters);
};

//! Repeatedly apply function to argument.
class FunctionNodeIterativeMap : public FunctionNodeIterative
{
 private:

 protected:
  //! Implements this node's function.
  virtual const XYZ evaluate(const XYZ&) const;
  
 public:
  //! Query whether node value is independent of position argument.
  virtual const bool is_constant() const;

  //! Constructor.
  FunctionNodeIterativeMap(const std::vector<float>& p,const std::vector<FunctionNode*>& a,uint i);

  //! Destructor.
  virtual ~FunctionNodeIterativeMap();

  //! Return a clone.
  virtual FunctionNode*const deepclone() const;
};

//! Accumulate function values as parameter is mapped
class FunctionNodeIterativeMapAccumulator : public FunctionNodeIterative
{
 private:

 protected:
  //! Implements this node's function.
  virtual const XYZ evaluate(const XYZ&) const;
  
 public:
  //! Query whether node value is independent of position argument.
  virtual const bool is_constant() const;

  //! Constructor.
  FunctionNodeIterativeMapAccumulator(const std::vector<float>& p,const std::vector<FunctionNode*>& a,uint i);

  //! Destructor.
  virtual ~FunctionNodeIterativeMapAccumulator();

  //! Return a clone.
  virtual FunctionNode*const deepclone() const;
};

//! Abstract base class for Mandelbrot and Julie sets.
class FunctionNodeIterativeZSquaredPlusC : public FunctionNodeIterative
{
 protected:
  //! Shared iteration code.
  const uint iterate(const XYZ& z0,const XYZ& c) const;
  
 public:
  //! Constructor.
  FunctionNodeIterativeZSquaredPlusC(const std::vector<float>& p,const std::vector<FunctionNode*>& a,uint i);

  //! Destructor;
  ~FunctionNodeIterativeZSquaredPlusC();
};

//! Do Mandelbrot Set test to choose between two functions.
class FunctionNodeIterativeMandelbrotChoose : public FunctionNodeIterativeZSquaredPlusC
{
 private:

 protected:
  //! Implements this node's function.
  virtual const XYZ evaluate(const XYZ&) const;
  
 public:
  //! Query whether node value is independent of position argument.
  virtual const bool is_constant() const;

  //! Constructor.
  FunctionNodeIterativeMandelbrotChoose(const std::vector<float>& p,const std::vector<FunctionNode*>& a,uint i);

  //! Destructor.
  virtual ~FunctionNodeIterativeMandelbrotChoose();

  //! Return a clone.
  virtual FunctionNode*const deepclone() const;
};

//! Do Mandelbrot Set returning co-ordinate filled with normalised iterations, or -1 in set.
class FunctionNodeIterativeMandelbrotContour : public FunctionNodeIterativeZSquaredPlusC
{
 private:

 protected:
  //! Implements this node's function.
  virtual const XYZ evaluate(const XYZ&) const;
  
 public:
  //! Query whether node value is independent of position argument.
  virtual const bool is_constant() const;

  //! Constructor.
  FunctionNodeIterativeMandelbrotContour(const std::vector<float>& p,const std::vector<FunctionNode*>& a,uint i);

  //! Destructor.
  virtual ~FunctionNodeIterativeMandelbrotContour();

  //! Return a clone.
  virtual FunctionNode*const deepclone() const;
};

//! Do Julia test to choose between two functions.
/*! Julia set parameter is picked up from an extra argument c.f Mandelbrot
 */
class FunctionNodeIterativeJuliaChoose : public FunctionNodeIterativeZSquaredPlusC
{
 private:

 protected:
  //! Implements this node's function.
  virtual const XYZ evaluate(const XYZ&) const;
  
 public:
  //! Query whether node value is independent of position argument.
  virtual const bool is_constant() const;

  //! Constructor.
  FunctionNodeIterativeJuliaChoose(const std::vector<float>& p,const std::vector<FunctionNode*>& a,uint i);

  //! Destructor.
  virtual ~FunctionNodeIterativeJuliaChoose();

  //! Return a clone.
  virtual FunctionNode*const deepclone() const;
};

//! Do Julia set returning co-ordinate filled with normalised iterations, or -1 in set.
/*! Julia set parameter is picked up from an extra argument c.f Mandelbrot
 */
class FunctionNodeIterativeJuliaContour : public FunctionNodeIterativeZSquaredPlusC
{
 private:

 protected:
  //! Implements this node's function.
  virtual const XYZ evaluate(const XYZ&) const;
  
 public:
  //! Query whether node value is independent of position argument.
  virtual const bool is_constant() const;

  //! Constructor.
  FunctionNodeIterativeJuliaContour(const std::vector<float>& p,const std::vector<FunctionNode*>& a,uint i);

  //! Destructor.
  virtual ~FunctionNodeIterativeJuliaContour();

  //! Return a clone.
  virtual FunctionNode*const deepclone() const;
};

#endif
