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

// Actually we probably only need a few specific functions used outside mutatable_image.cpp: FunctionPreTransform mainly
#include "function.h"

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
  static FunctionNode* stub(const MutationParameters& parameters,bool exciting=false);

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
  std::string _name;
  
  Registration(const std::string& n)
    :_name(n)
    {
      std::cerr << "Register: " << _name << "\n";
    }
};

class Registry
{
 public:
  std::map<std::string,const Registration*> _registry;

  Registry()
    {}

  void add(const Registration* reg)
    {
      _registry[reg->_name]=reg;
    }

  static const Registration* add(const std::string& name,const Registration* reg)
  {
    std::cerr << "Registry: " << name << "\n";
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

//! You'd expect this to live in the .cpp, but instantiation should only be triggered once by REGISTER macros in function.cpp.
/*! There is the possibility of associating a name with the association using typeid(F).name()
  but it's not very useful as it has name mangling stuff attached.
 */
template <typename F> Registration FunctionNodeUsing<F>::registration(typeid(F).name());


//----------------------------------------------------------------------------------------

//! This node evaluates a child function in spherical co-ordinates
class FunctionNodeSphericalize : public FunctionNode
{
 private:

 protected:
  //! Implements this node's function.
  virtual const XYZ evaluate(const XYZ&) const;
  
 public:
  //! Query whether node value is independent of position argument.
  virtual const bool is_constant() const;

  //! Constructor.
  FunctionNodeSphericalize(const std::vector<float>& p,const std::vector<FunctionNode*>& a);

  //! Destructor.
  virtual ~FunctionNodeSphericalize();

  //! Return a clone.
  virtual FunctionNode*const deepclone() const;
};

//! This node applies rotation matrices to it's argument
class FunctionNodeRotate : public FunctionNode
{
 private:

 protected:
  //! Implements this node's function.
  virtual const XYZ evaluate(const XYZ&) const;
  
 public:
  //! Query whether node value is independent of position argument.
  virtual const bool is_constant() const;

  //! Constructor.
  FunctionNodeRotate(const std::vector<float>& p,const std::vector<FunctionNode*>& a);

  //! Destructor.
  virtual ~FunctionNodeRotate();

  //! Return a clone.
  virtual FunctionNode*const deepclone() const;
};

//! This node implements sin
class FunctionNodeSin : public FunctionNode
{
 private:

 protected:
  //! Implements this node's function.
  virtual const XYZ evaluate(const XYZ&) const;
  
 public:
  //! Query whether node value is independent of position argument.
  virtual const bool is_constant() const;

  //! Constructor.
  FunctionNodeSin(const std::vector<float>& p,const std::vector<FunctionNode*>& a);

  //! Destructor.
  virtual ~FunctionNodeSin();

  //! Return a clone.
  virtual FunctionNode*const deepclone() const;
};

//! This node implements cos
class FunctionNodeCos : public FunctionNode
{
 private:

 protected:
  //! Implements this node's function.
  virtual const XYZ evaluate(const XYZ&) const;
  
 public:
  //! Query whether node value is independent of position argument.
  virtual const bool is_constant() const;

  //! Constructor.
  FunctionNodeCos(const std::vector<float>& p,const std::vector<FunctionNode*>& a);

  //! Destructor.
  virtual ~FunctionNodeCos();

  //! Return a clone.
  virtual FunctionNode*const deepclone() const;
};

//! This node implements a linear spiral
class FunctionNodeSpiralLinear : public FunctionNode
{
 private:

 protected:
  //! Implements this node's function.
  virtual const XYZ evaluate(const XYZ&) const;
  
 public:
  //! Query whether node value is independent of position argument.
  virtual const bool is_constant() const;

  //! Constructor.
  FunctionNodeSpiralLinear(const std::vector<float>& p,const std::vector<FunctionNode*>& a);

  //! Destructor.
  virtual ~FunctionNodeSpiralLinear();

  //! Return a clone.
  virtual FunctionNode*const deepclone() const;
};

//! This node implements a logarithmic spiral
class FunctionNodeSpiralLogarithmic : public FunctionNode
{
 private:

 protected:
  //! Implements this node's function.
  virtual const XYZ evaluate(const XYZ&) const;
  
 public:
  //! Query whether node value is independent of position argument.
  virtual const bool is_constant() const;

  //! Constructor.
  FunctionNodeSpiralLogarithmic(const std::vector<float>& p,const std::vector<FunctionNode*>& a);

  //! Destructor.
  virtual ~FunctionNodeSpiralLogarithmic();

  //! Return a clone.
  virtual FunctionNode*const deepclone() const;
};

//! This node implements grad
class FunctionNodeGrad : public FunctionNode
{
 private:

 protected:
  //! Implements this node's function.
  virtual const XYZ evaluate(const XYZ&) const;
  
  //! Query whether node value is independent of position argument.
  virtual const bool is_constant() const;

 public:
  //! Constructor.
  FunctionNodeGrad(const std::vector<float>& p,const std::vector<FunctionNode*>& a);

  //! Destructor.
  virtual ~FunctionNodeGrad();

  //! Return a clone.
  virtual FunctionNode*const deepclone() const;
};

//! This node implements functional concatenation
class FunctionNodeConcatenatePair : public FunctionNode
{
 private:

 protected:
  //! Implements this node's function.
  virtual const XYZ evaluate(const XYZ&) const;
  
 public:
  //! Query whether node value is independent of position argument.
  virtual const bool is_constant() const;

  //! Constructor.
  FunctionNodeConcatenatePair(const std::vector<float>& p,const std::vector<FunctionNode*>& a);

  //! Destructor.
  virtual ~FunctionNodeConcatenatePair();

  //! Return a clone.
  virtual FunctionNode*const deepclone() const;
};

//! This node implements addition
class FunctionNodeAdd : public FunctionNode
{
 private:

 protected:
  //! Implements this node's function.
  virtual const XYZ evaluate(const XYZ&) const;
  
 public:
  //! Query whether node value is independent of position argument.
  virtual const bool is_constant() const;

  //! Constructor.
  FunctionNodeAdd(const std::vector<float>& p,const std::vector<FunctionNode*>& a);

  //! Destructor.
  virtual ~FunctionNodeAdd();

  //! Return a clone.
  virtual FunctionNode*const deepclone() const;
};

//! This node implements componentwise multiply
class FunctionNodeMultiply : public FunctionNode
{
 private:

 protected:
  //! Implements this node's function.
  virtual const XYZ evaluate(const XYZ&) const;
  
 public:
  //! Query whether node value is independent of position argument.
  virtual const bool is_constant() const;

  //! Constructor.
  FunctionNodeMultiply(const std::vector<float>& p,const std::vector<FunctionNode*>& a);

  //! Destructor.
  virtual ~FunctionNodeMultiply();

  //! Return a clone.
  virtual FunctionNode*const deepclone() const;
};

//! This node implements componentwise divide
class FunctionNodeDivide : public FunctionNode
{
 private:

 protected:
  //! Implements this node's function.
  virtual const XYZ evaluate(const XYZ&) const;
  
 public:
  //! Query whether node value is independent of position argument.
  virtual const bool is_constant() const;

  //! Constructor.
  FunctionNodeDivide(const std::vector<float>& p,const std::vector<FunctionNode*>& a);

  //! Destructor.
  virtual ~FunctionNodeDivide();

  //! Return a clone.
  virtual FunctionNode*const deepclone() const;
};

//! This node implements cross-product
class FunctionNodeCross : public FunctionNode
{
 private:

 protected:
  //! Implements this node's function.
  virtual const XYZ evaluate(const XYZ&) const;
  
 public:
  //! Query whether node value is independent of position argument.
  virtual const bool is_constant() const;

  //! Constructor.
  FunctionNodeCross(const std::vector<float>& p,const std::vector<FunctionNode*>& a);

  //! Destructor.
  virtual ~FunctionNodeCross();

  //! Return a clone.
  virtual FunctionNode*const deepclone() const;
};


//! This node implements geometric inversion of the a point in a sphere with an origin and radius given by child functions.
class FunctionNodeGeometricInversion : public FunctionNode
{
 private:

 protected:
  //! Implements this node's function.
  virtual const XYZ evaluate(const XYZ&) const;
  
 public:
  //! Query whether node value is independent of position argument.
  virtual const bool is_constant() const;

  //! Constructor.
  FunctionNodeGeometricInversion(const std::vector<float>& p,const std::vector<FunctionNode*>& a);

  //! Destructor.
  virtual ~FunctionNodeGeometricInversion();

  //! Return a clone.
  virtual FunctionNode*const deepclone() const;
};

//! This node implements maximum
class FunctionNodeMax : public FunctionNode
{
 private:

 protected:
  //! Implements this node's function.
  virtual const XYZ evaluate(const XYZ&) const;
  
 public:
  //! Query whether node value is independent of position argument.
  virtual const bool is_constant() const;

  //! Constructor.
  FunctionNodeMax(const std::vector<float>& p,const std::vector<FunctionNode*>& a);

  //! Destructor.
  virtual ~FunctionNodeMax();

  //! Return a clone.
  virtual FunctionNode*const deepclone() const;
};

//! This node implements minimum
class FunctionNodeMin : public FunctionNode
{
 private:

 protected:
  //! Implements this node's function.
  virtual const XYZ evaluate(const XYZ&) const;
  
 public:
  //! Query whether node value is independent of position argument.
  virtual const bool is_constant() const;

  //! Constructor.
  FunctionNodeMin(const std::vector<float>& p,const std::vector<FunctionNode*>& a);

  //! Destructor.
  virtual ~FunctionNodeMin();

  //! Return a clone.
  virtual FunctionNode*const deepclone() const;
};

//! This node implements modulus
class FunctionNodeMod : public FunctionNode
{
 private:

 protected:
  //! Implements this node's function.
  virtual const XYZ evaluate(const XYZ&) const;
  
 public:
  //! Query whether node value is independent of position argument.
  virtual const bool is_constant() const;

  //! Constructor.
  FunctionNodeMod(const std::vector<float>& p,const std::vector<FunctionNode*>& a);

  //! Destructor.
  virtual ~FunctionNodeMod();

  //! Return a clone.
  virtual FunctionNode*const deepclone() const;
};


//! This node implements functional concatenation
class FunctionNodeConcatenateTriple : public FunctionNode
{
 private:

 protected:
  //! Implements this node's function.
  virtual const XYZ evaluate(const XYZ&) const;
  
 public:
  //! Query whether node value is independent of position argument.
  virtual const bool is_constant() const;

  //! Constructor.
  FunctionNodeConcatenateTriple(const std::vector<float>& p,const std::vector<FunctionNode*>& a);

  //! Destructor.
  virtual ~FunctionNodeConcatenateTriple();

  //! Return a clone.
  virtual FunctionNode*const deepclone() const;
};

//! This node implements reflection about a plane
class FunctionNodeReflect : public FunctionNode
{
 private:

 protected:
  virtual const XYZ evaluate(const XYZ&) const;
  
 public:
  //! Query whether node value is independent of position argument.
  virtual const bool is_constant() const;

  //! Constructor.
  FunctionNodeReflect(const std::vector<float>& p,const std::vector<FunctionNode*>& a);

  //! Destructor.
  virtual ~FunctionNodeReflect();

  //! Return a clone.
  virtual FunctionNode*const deepclone() const;
};

//! This node implements magnitudes of 3 arguments 
class FunctionNodeMagnitudes : public FunctionNode
{
 private:

 protected:
  virtual const XYZ evaluate(const XYZ&) const;
  
 public:
  //! Query whether node value is independent of position argument.
  virtual const bool is_constant() const;

  //! Constructor.
  FunctionNodeMagnitudes(const std::vector<float>& p,const std::vector<FunctionNode*>& a);

  //! Destructor.
  virtual ~FunctionNodeMagnitudes();

  //! Return a clone.
  virtual FunctionNode*const deepclone() const;
};

//! This node implements selection between 2 functions based on the relative magnitudes of 2 other functions
class FunctionNodeChooseSphere : public FunctionNode
{
 private:

 protected:
  //! Implements this node's function.
  virtual const XYZ evaluate(const XYZ&) const;
  
 public:
  //! Query whether node value is independent of position argument.
  virtual const bool is_constant() const;

  //! Constructor.
  FunctionNodeChooseSphere(const std::vector<float>& p,const std::vector<FunctionNode*>& a);

  //! Destructor.
  virtual ~FunctionNodeChooseSphere();

  //! Return a clone.
  virtual FunctionNode*const deepclone() const;
};

//! This node implements selection between 2 functions based on whether a rectangle contains a point
class FunctionNodeChooseRect : public FunctionNode
{
 private:

 protected:
  //! Implements this node's function.
  virtual const XYZ evaluate(const XYZ&) const;
  
 public:
  //! Query whether node value is independent of position argument.
  virtual const bool is_constant() const;

  //! Constructor.
  FunctionNodeChooseRect(const std::vector<float>& p,const std::vector<FunctionNode*>& a);

  //! Destructor.
  virtual ~FunctionNodeChooseRect();

  //! Return a clone.
  virtual FunctionNode*const deepclone() const;
};


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
