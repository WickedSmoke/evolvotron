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
  \brief Interfaces for class MutatableImageNode and derived classes.
*/

#ifndef _mutatable_image_h_
#define _mutatable_image_h_

#include <vector>

#include "useful.h"
#include "xyz.h"
#include "transform.h"

#include "mutation_parameters.h"

// Actually we probably only need a few specific functions used outside mutatable_image.cpp: FunctionPreTransform mainly
#include "function.h"

class FunctionPreTransform;
template <typename F> class MutatableImageNodeUsing;

//! Abstract base class for all kinds of mutatable image node.
class MutatableImageNode
{
 private:

  //! The arguments (ie child nodes) for this node.
  std::vector<MutatableImageNode*> _args;

  //! The parameters (ie constant values) for this node.
  std::vector<float> _params;
    
 protected:

  //! This returns a deep-cloned copy of the node's children.
  const std::vector<MutatableImageNode*> cloneargs() const;

  //! This returns a copy of the node's parameters
  const std::vector<float> cloneparams() const;

  //! This what distinguishes different types of node.
  virtual const XYZ evaluate(const XYZ& p) const
    =0;

 public:

  //! Returns true if the function is independent of it's position argument.
  /*! This isn't used for optimisation (which would require MutatableImageNode to have computation-specific state,
      which would wreck plans for reference counted deepclone()), 
      but to cull boring constant images on creation.
   */
  virtual const bool is_constant() const
    =0;

  //@{
  //! Query the node as to whether it is a MutatableImageUsing<FunctionPreTransform> (return null if not).
  /*! This is useful for accumulating view transforms at the front end of a function tree without creating a new transform each time.
   */
  virtual const MutatableImageNodeUsing<FunctionPreTransform>*const is_a_MutatableImageNodeUsingFunctionPreTransform() const;
  virtual MutatableImageNodeUsing<FunctionPreTransform>*const is_a_MutatableImageNodeUsingFunctionPreTransform();
  //@}

  //! This returns a new random bit of tree.  Setting the "exciting" flag avoids basic node types, but only at the top level of the stub tree.
  static MutatableImageNode* stub(const MutationParameters& parameters,bool exciting=false);

  //! This returns a vector of random parameter values.
  static const std::vector<float> stubparams(const MutationParameters& parameters,uint n);

  //! This returns a vector of new random bits of tree.
  static const std::vector<MutatableImageNode*> stubargs(const MutationParameters& parameters,uint n);

  //! Constructor given an array of params. and args (these MUST be provided; there are no alterative constructors).
  MutatableImageNode(const std::vector<float>& p,const std::vector<MutatableImageNode*>& a);

  //! Destructor.
  virtual ~MutatableImageNode();

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
  void args(const std::vector<MutatableImageNode*>& a)
    {
      _args=a;
    }

  //! Accessor.
  const std::vector<MutatableImageNode*>& args() const
    {
      return _args;
    }

  //! Accessor. 
  const MutatableImageNode& arg(uint n) const
    {
      assert(n<args().size());
      return *(args()[n]);
    }

  //! Scramble this node and its leaves up a bit.
  virtual void mutate(const MutationParameters&);
  
  //! Return an clone of this image node and all its children.
  virtual MutatableImageNode*const deepclone() const
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
  std::vector<MutatableImageNode*>& args()
    {
      return _args;
    }

};

//! Template class to generate boilerplate for virtual methods
template <typename F> class MutatableImageNodeUsing : public MutatableImageNode
{
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
  MutatableImageNodeUsing(const std::vector<float>& p,const std::vector<MutatableImageNode*>& a)
    :MutatableImageNode(p,a)
    {
      assert(params().size()==F::parameters());
      assert(args().size()==F::arguments());
    }
  
  //! Destructor.
  virtual ~MutatableImageNodeUsing()
    {}
  
  //! Return a deeploned copy.
  virtual MutatableImageNode*const deepclone() const
    {
      return new MutatableImageNodeUsing<F>(cloneparams(),cloneargs());
    }
  
  //! Internal self-consistency check.  We can add some extra checks.
  virtual const bool ok() const
    {
      return 
	(
	 params().size()==F::parameters() &&
	 args().size()==F::arguments() &&
	 MutatableImageNode::ok()
	 );
    }

  //! Implementation depends on template parameter
  virtual const MutatableImageNodeUsing<FunctionPreTransform>*const is_a_MutatableImageNodeUsingFunctionPreTransform() const;

  //! Implementation depends on template parameter
  virtual MutatableImageNodeUsing<FunctionPreTransform>*const is_a_MutatableImageNodeUsingFunctionPreTransform();
};

//! In the general case this still returns 0
template <typename F> inline const MutatableImageNodeUsing<FunctionPreTransform>*const MutatableImageNodeUsing<F>::is_a_MutatableImageNodeUsingFunctionPreTransform() const
{
  return 0;
}

//! In the general case this still returns 0
template <typename F> inline MutatableImageNodeUsing<FunctionPreTransform>*const MutatableImageNodeUsing<F>::is_a_MutatableImageNodeUsingFunctionPreTransform()
{
  return 0;
}

//! Specialisation for FunctionPreTransform
template <> inline const MutatableImageNodeUsing<FunctionPreTransform>*const MutatableImageNodeUsing<FunctionPreTransform>::is_a_MutatableImageNodeUsingFunctionPreTransform() const
{
  return this;
}

//! Specialisation for FunctionPreTransform
template <> inline MutatableImageNodeUsing<FunctionPreTransform>*const MutatableImageNodeUsing<FunctionPreTransform>::is_a_MutatableImageNodeUsingFunctionPreTransform()
{
  return this;
}

//----------------------------------------------------------------------------------------

//! This node type is intended as a substitute for MutatableImageNodePositionTransformed.  
class MutatableImageNodePositionTransformedQuadratic : public MutatableImageNode
{
 protected:
  //! Implements this node's function.
  virtual const XYZ evaluate(const XYZ&) const;
  
 public:
  //! Returns true if the function is independent of it's position argument (obviously generally false in this case).
  virtual const bool is_constant() const;

  //! Constructor.
  MutatableImageNodePositionTransformedQuadratic(const std::vector<float>& p,const std::vector<MutatableImageNode*>& a);

  //! Destructor.
  virtual ~MutatableImageNodePositionTransformedQuadratic();

  //! Returns a clone.
  virtual MutatableImageNode*const deepclone() const;
};

//! This node implements XYZ to spherical co-ords
class MutatableImageNodeXYZToSpherical : public MutatableImageNode
{
 private:

 protected:
  //! Implements this node's function.
  virtual const XYZ evaluate(const XYZ&) const;
  
 public:
  //! Query whether node value is independent of position argument.
  virtual const bool is_constant() const;

  //! Constructor.
  MutatableImageNodeXYZToSpherical(const std::vector<float>& p,const std::vector<MutatableImageNode*>& a);

  //! Destructor.
  virtual ~MutatableImageNodeXYZToSpherical();

  //! Return a clone.
  virtual MutatableImageNode*const deepclone() const;
};

//! This node implements spherical to XYZ co-ords
class MutatableImageNodeSphericalToXYZ : public MutatableImageNode
{
 private:

 protected:
  //! Implements this node's function.
  virtual const XYZ evaluate(const XYZ&) const;
  
 public:
  //! Query whether node value is independent of position argument.
  virtual const bool is_constant() const;

  //! Constructor.
  MutatableImageNodeSphericalToXYZ(const std::vector<float>& p,const std::vector<MutatableImageNode*>& a);

  //! Destructor.
  virtual ~MutatableImageNodeSphericalToXYZ();

  //! Return a clone.
  virtual MutatableImageNode*const deepclone() const;
};

//! This node evaluates a child function in spherical co-ordinates
class MutatableImageNodeSphericalize : public MutatableImageNode
{
 private:

 protected:
  //! Implements this node's function.
  virtual const XYZ evaluate(const XYZ&) const;
  
 public:
  //! Query whether node value is independent of position argument.
  virtual const bool is_constant() const;

  //! Constructor.
  MutatableImageNodeSphericalize(const std::vector<float>& p,const std::vector<MutatableImageNode*>& a);

  //! Destructor.
  virtual ~MutatableImageNodeSphericalize();

  //! Return a clone.
  virtual MutatableImageNode*const deepclone() const;
};

//! This node applies rotation matrices to it's argument
class MutatableImageNodeRotate : public MutatableImageNode
{
 private:

 protected:
  //! Implements this node's function.
  virtual const XYZ evaluate(const XYZ&) const;
  
 public:
  //! Query whether node value is independent of position argument.
  virtual const bool is_constant() const;

  //! Constructor.
  MutatableImageNodeRotate(const std::vector<float>& p,const std::vector<MutatableImageNode*>& a);

  //! Destructor.
  virtual ~MutatableImageNodeRotate();

  //! Return a clone.
  virtual MutatableImageNode*const deepclone() const;
};

//! This node implements sin
class MutatableImageNodeSin : public MutatableImageNode
{
 private:

 protected:
  //! Implements this node's function.
  virtual const XYZ evaluate(const XYZ&) const;
  
 public:
  //! Query whether node value is independent of position argument.
  virtual const bool is_constant() const;

  //! Constructor.
  MutatableImageNodeSin(const std::vector<float>& p,const std::vector<MutatableImageNode*>& a);

  //! Destructor.
  virtual ~MutatableImageNodeSin();

  //! Return a clone.
  virtual MutatableImageNode*const deepclone() const;
};

//! This node implements cos
class MutatableImageNodeCos : public MutatableImageNode
{
 private:

 protected:
  //! Implements this node's function.
  virtual const XYZ evaluate(const XYZ&) const;
  
 public:
  //! Query whether node value is independent of position argument.
  virtual const bool is_constant() const;

  //! Constructor.
  MutatableImageNodeCos(const std::vector<float>& p,const std::vector<MutatableImageNode*>& a);

  //! Destructor.
  virtual ~MutatableImageNodeCos();

  //! Return a clone.
  virtual MutatableImageNode*const deepclone() const;
};

//! This node implements a linear spiral
class MutatableImageNodeSpiralLinear : public MutatableImageNode
{
 private:

 protected:
  //! Implements this node's function.
  virtual const XYZ evaluate(const XYZ&) const;
  
 public:
  //! Query whether node value is independent of position argument.
  virtual const bool is_constant() const;

  //! Constructor.
  MutatableImageNodeSpiralLinear(const std::vector<float>& p,const std::vector<MutatableImageNode*>& a);

  //! Destructor.
  virtual ~MutatableImageNodeSpiralLinear();

  //! Return a clone.
  virtual MutatableImageNode*const deepclone() const;
};

//! This node implements a logarithmic spiral
class MutatableImageNodeSpiralLogarithmic : public MutatableImageNode
{
 private:

 protected:
  //! Implements this node's function.
  virtual const XYZ evaluate(const XYZ&) const;
  
 public:
  //! Query whether node value is independent of position argument.
  virtual const bool is_constant() const;

  //! Constructor.
  MutatableImageNodeSpiralLogarithmic(const std::vector<float>& p,const std::vector<MutatableImageNode*>& a);

  //! Destructor.
  virtual ~MutatableImageNodeSpiralLogarithmic();

  //! Return a clone.
  virtual MutatableImageNode*const deepclone() const;
};

//! This node implements grad
class MutatableImageNodeGrad : public MutatableImageNode
{
 private:

 protected:
  //! Implements this node's function.
  virtual const XYZ evaluate(const XYZ&) const;
  
  //! Query whether node value is independent of position argument.
  virtual const bool is_constant() const;

 public:
  //! Constructor.
  MutatableImageNodeGrad(const std::vector<float>& p,const std::vector<MutatableImageNode*>& a);

  //! Destructor.
  virtual ~MutatableImageNodeGrad();

  //! Return a clone.
  virtual MutatableImageNode*const deepclone() const;
};

//! This node implements functional concatenation
class MutatableImageNodeConcatenatePair : public MutatableImageNode
{
 private:

 protected:
  //! Implements this node's function.
  virtual const XYZ evaluate(const XYZ&) const;
  
 public:
  //! Query whether node value is independent of position argument.
  virtual const bool is_constant() const;

  //! Constructor.
  MutatableImageNodeConcatenatePair(const std::vector<float>& p,const std::vector<MutatableImageNode*>& a);

  //! Destructor.
  virtual ~MutatableImageNodeConcatenatePair();

  //! Return a clone.
  virtual MutatableImageNode*const deepclone() const;
};

//! This node implements addition
class MutatableImageNodeAdd : public MutatableImageNode
{
 private:

 protected:
  //! Implements this node's function.
  virtual const XYZ evaluate(const XYZ&) const;
  
 public:
  //! Query whether node value is independent of position argument.
  virtual const bool is_constant() const;

  //! Constructor.
  MutatableImageNodeAdd(const std::vector<float>& p,const std::vector<MutatableImageNode*>& a);

  //! Destructor.
  virtual ~MutatableImageNodeAdd();

  //! Return a clone.
  virtual MutatableImageNode*const deepclone() const;
};

//! This node implements componentwise multiply
class MutatableImageNodeMultiply : public MutatableImageNode
{
 private:

 protected:
  //! Implements this node's function.
  virtual const XYZ evaluate(const XYZ&) const;
  
 public:
  //! Query whether node value is independent of position argument.
  virtual const bool is_constant() const;

  //! Constructor.
  MutatableImageNodeMultiply(const std::vector<float>& p,const std::vector<MutatableImageNode*>& a);

  //! Destructor.
  virtual ~MutatableImageNodeMultiply();

  //! Return a clone.
  virtual MutatableImageNode*const deepclone() const;
};

//! This node implements componentwise divide
class MutatableImageNodeDivide : public MutatableImageNode
{
 private:

 protected:
  //! Implements this node's function.
  virtual const XYZ evaluate(const XYZ&) const;
  
 public:
  //! Query whether node value is independent of position argument.
  virtual const bool is_constant() const;

  //! Constructor.
  MutatableImageNodeDivide(const std::vector<float>& p,const std::vector<MutatableImageNode*>& a);

  //! Destructor.
  virtual ~MutatableImageNodeDivide();

  //! Return a clone.
  virtual MutatableImageNode*const deepclone() const;
};

//! This node implements cross-product
class MutatableImageNodeCross : public MutatableImageNode
{
 private:

 protected:
  //! Implements this node's function.
  virtual const XYZ evaluate(const XYZ&) const;
  
 public:
  //! Query whether node value is independent of position argument.
  virtual const bool is_constant() const;

  //! Constructor.
  MutatableImageNodeCross(const std::vector<float>& p,const std::vector<MutatableImageNode*>& a);

  //! Destructor.
  virtual ~MutatableImageNodeCross();

  //! Return a clone.
  virtual MutatableImageNode*const deepclone() const;
};


//! This node implements geometric inversion of the a point in a sphere with an origin and radius given by child functions.
class MutatableImageNodeGeometricInversion : public MutatableImageNode
{
 private:

 protected:
  //! Implements this node's function.
  virtual const XYZ evaluate(const XYZ&) const;
  
 public:
  //! Query whether node value is independent of position argument.
  virtual const bool is_constant() const;

  //! Constructor.
  MutatableImageNodeGeometricInversion(const std::vector<float>& p,const std::vector<MutatableImageNode*>& a);

  //! Destructor.
  virtual ~MutatableImageNodeGeometricInversion();

  //! Return a clone.
  virtual MutatableImageNode*const deepclone() const;
};

//! This node implements maximum
class MutatableImageNodeMax : public MutatableImageNode
{
 private:

 protected:
  //! Implements this node's function.
  virtual const XYZ evaluate(const XYZ&) const;
  
 public:
  //! Query whether node value is independent of position argument.
  virtual const bool is_constant() const;

  //! Constructor.
  MutatableImageNodeMax(const std::vector<float>& p,const std::vector<MutatableImageNode*>& a);

  //! Destructor.
  virtual ~MutatableImageNodeMax();

  //! Return a clone.
  virtual MutatableImageNode*const deepclone() const;
};

//! This node implements minimum
class MutatableImageNodeMin : public MutatableImageNode
{
 private:

 protected:
  //! Implements this node's function.
  virtual const XYZ evaluate(const XYZ&) const;
  
 public:
  //! Query whether node value is independent of position argument.
  virtual const bool is_constant() const;

  //! Constructor.
  MutatableImageNodeMin(const std::vector<float>& p,const std::vector<MutatableImageNode*>& a);

  //! Destructor.
  virtual ~MutatableImageNodeMin();

  //! Return a clone.
  virtual MutatableImageNode*const deepclone() const;
};

//! This node implements modulus
class MutatableImageNodeMod : public MutatableImageNode
{
 private:

 protected:
  //! Implements this node's function.
  virtual const XYZ evaluate(const XYZ&) const;
  
 public:
  //! Query whether node value is independent of position argument.
  virtual const bool is_constant() const;

  //! Constructor.
  MutatableImageNodeMod(const std::vector<float>& p,const std::vector<MutatableImageNode*>& a);

  //! Destructor.
  virtual ~MutatableImageNodeMod();

  //! Return a clone.
  virtual MutatableImageNode*const deepclone() const;
};


//! This node implements functional concatenation
class MutatableImageNodeConcatenateTriple : public MutatableImageNode
{
 private:

 protected:
  //! Implements this node's function.
  virtual const XYZ evaluate(const XYZ&) const;
  
 public:
  //! Query whether node value is independent of position argument.
  virtual const bool is_constant() const;

  //! Constructor.
  MutatableImageNodeConcatenateTriple(const std::vector<float>& p,const std::vector<MutatableImageNode*>& a);

  //! Destructor.
  virtual ~MutatableImageNodeConcatenateTriple();

  //! Return a clone.
  virtual MutatableImageNode*const deepclone() const;
};

//! This node implements reflection about a plane
class MutatableImageNodeReflect : public MutatableImageNode
{
 private:

 protected:
  virtual const XYZ evaluate(const XYZ&) const;
  
 public:
  //! Query whether node value is independent of position argument.
  virtual const bool is_constant() const;

  //! Constructor.
  MutatableImageNodeReflect(const std::vector<float>& p,const std::vector<MutatableImageNode*>& a);

  //! Destructor.
  virtual ~MutatableImageNodeReflect();

  //! Return a clone.
  virtual MutatableImageNode*const deepclone() const;
};

//! This node implements magnitudes of 3 arguments 
class MutatableImageNodeMagnitudes : public MutatableImageNode
{
 private:

 protected:
  virtual const XYZ evaluate(const XYZ&) const;
  
 public:
  //! Query whether node value is independent of position argument.
  virtual const bool is_constant() const;

  //! Constructor.
  MutatableImageNodeMagnitudes(const std::vector<float>& p,const std::vector<MutatableImageNode*>& a);

  //! Destructor.
  virtual ~MutatableImageNodeMagnitudes();

  //! Return a clone.
  virtual MutatableImageNode*const deepclone() const;
};

//! This node implements selection between 2 functions based on the relative magnitudes of 2 other functions
class MutatableImageNodeChooseSphere : public MutatableImageNode
{
 private:

 protected:
  //! Implements this node's function.
  virtual const XYZ evaluate(const XYZ&) const;
  
 public:
  //! Query whether node value is independent of position argument.
  virtual const bool is_constant() const;

  //! Constructor.
  MutatableImageNodeChooseSphere(const std::vector<float>& p,const std::vector<MutatableImageNode*>& a);

  //! Destructor.
  virtual ~MutatableImageNodeChooseSphere();

  //! Return a clone.
  virtual MutatableImageNode*const deepclone() const;
};

//! This node implements selection between 2 functions based on whether a rectangle contains a point
class MutatableImageNodeChooseRect : public MutatableImageNode
{
 private:

 protected:
  //! Implements this node's function.
  virtual const XYZ evaluate(const XYZ&) const;
  
 public:
  //! Query whether node value is independent of position argument.
  virtual const bool is_constant() const;

  //! Constructor.
  MutatableImageNodeChooseRect(const std::vector<float>& p,const std::vector<MutatableImageNode*>& a);

  //! Destructor.
  virtual ~MutatableImageNodeChooseRect();

  //! Return a clone.
  virtual MutatableImageNode*const deepclone() const;
};


//! This node implements something like a 3D transform.
class MutatableImageNodePreTransform : public MutatableImageNode
{
 private:

 protected:
  //! Implements this node's function.
  virtual const XYZ evaluate(const XYZ&) const;
  
 public:
  //! Query whether node value is independent of position argument.
  virtual const bool is_constant() const;

  //! Constructor.
  MutatableImageNodePreTransform(const std::vector<float>& p,const std::vector<MutatableImageNode*>& a);

  //! Destructor.
  virtual ~MutatableImageNodePreTransform();

  //! Return a clone.
  virtual MutatableImageNode*const deepclone() const;
};

//! This node implements something like a 3D transform.
class MutatableImageNodePostTransform : public MutatableImageNode
{
 private:

 protected:
  //! Implements this node's function.
  virtual const XYZ evaluate(const XYZ&) const;
  
 public:
  //! Query whether node value is independent of position argument.
  virtual const bool is_constant() const;

  //! Constructor.
  MutatableImageNodePostTransform(const std::vector<float>& p,const std::vector<MutatableImageNode*>& a);

  //! Destructor.
  virtual ~MutatableImageNodePostTransform();

  //! Return a clone.
  virtual MutatableImageNode*const deepclone() const;
};

//! Base class for iterative node types.
/*! evaluate, is_constant and deepclone methods remain virtual.
 */
class MutatableImageNodeIterative : public MutatableImageNode
{
 private:

 protected:
  
  //! Number of iterations performed (or maximum number).
  uint _iterations;

 public:
  //! Constructor.
  MutatableImageNodeIterative(const std::vector<float>& p,const std::vector<MutatableImageNode*>& a,uint i);

  //! Destructor.
  virtual ~MutatableImageNodeIterative();
  
  void mutate(const MutationParameters& parameters);
};

//! Repeatedly apply function to argument.
class MutatableImageNodeIterativeMap : public MutatableImageNodeIterative
{
 private:

 protected:
  //! Implements this node's function.
  virtual const XYZ evaluate(const XYZ&) const;
  
 public:
  //! Query whether node value is independent of position argument.
  virtual const bool is_constant() const;

  //! Constructor.
  MutatableImageNodeIterativeMap(const std::vector<float>& p,const std::vector<MutatableImageNode*>& a,uint i);

  //! Destructor.
  virtual ~MutatableImageNodeIterativeMap();

  //! Return a clone.
  virtual MutatableImageNode*const deepclone() const;
};

//! Accumulate function values as parameter is mapped
class MutatableImageNodeIterativeMapAccumulator : public MutatableImageNodeIterative
{
 private:

 protected:
  //! Implements this node's function.
  virtual const XYZ evaluate(const XYZ&) const;
  
 public:
  //! Query whether node value is independent of position argument.
  virtual const bool is_constant() const;

  //! Constructor.
  MutatableImageNodeIterativeMapAccumulator(const std::vector<float>& p,const std::vector<MutatableImageNode*>& a,uint i);

  //! Destructor.
  virtual ~MutatableImageNodeIterativeMapAccumulator();

  //! Return a clone.
  virtual MutatableImageNode*const deepclone() const;
};

//! Abstract base class for Mandelbrot and Julie sets.
class MutatableImageNodeIterativeZSquaredPlusC : public MutatableImageNodeIterative
{
 protected:
  //! Shared iteration code.
  const uint iterate(const XYZ& z0,const XYZ& c) const;
  
 public:
  //! Constructor.
  MutatableImageNodeIterativeZSquaredPlusC(const std::vector<float>& p,const std::vector<MutatableImageNode*>& a,uint i);

  //! Destructor;
  ~MutatableImageNodeIterativeZSquaredPlusC();
};

//! Do Mandelbrot Set test to choose between two functions.
class MutatableImageNodeIterativeMandelbrotChoose : public MutatableImageNodeIterativeZSquaredPlusC
{
 private:

 protected:
  //! Implements this node's function.
  virtual const XYZ evaluate(const XYZ&) const;
  
 public:
  //! Query whether node value is independent of position argument.
  virtual const bool is_constant() const;

  //! Constructor.
  MutatableImageNodeIterativeMandelbrotChoose(const std::vector<float>& p,const std::vector<MutatableImageNode*>& a,uint i);

  //! Destructor.
  virtual ~MutatableImageNodeIterativeMandelbrotChoose();

  //! Return a clone.
  virtual MutatableImageNode*const deepclone() const;
};

//! Do Mandelbrot Set returning co-ordinate filled with normalised iterations, or -1 in set.
class MutatableImageNodeIterativeMandelbrotContour : public MutatableImageNodeIterativeZSquaredPlusC
{
 private:

 protected:
  //! Implements this node's function.
  virtual const XYZ evaluate(const XYZ&) const;
  
 public:
  //! Query whether node value is independent of position argument.
  virtual const bool is_constant() const;

  //! Constructor.
  MutatableImageNodeIterativeMandelbrotContour(const std::vector<float>& p,const std::vector<MutatableImageNode*>& a,uint i);

  //! Destructor.
  virtual ~MutatableImageNodeIterativeMandelbrotContour();

  //! Return a clone.
  virtual MutatableImageNode*const deepclone() const;
};

//! Do Julia test to choose between two functions.
/*! Julia set parameter is picked up from an extra argument c.f Mandelbrot
 */
class MutatableImageNodeIterativeJuliaChoose : public MutatableImageNodeIterativeZSquaredPlusC
{
 private:

 protected:
  //! Implements this node's function.
  virtual const XYZ evaluate(const XYZ&) const;
  
 public:
  //! Query whether node value is independent of position argument.
  virtual const bool is_constant() const;

  //! Constructor.
  MutatableImageNodeIterativeJuliaChoose(const std::vector<float>& p,const std::vector<MutatableImageNode*>& a,uint i);

  //! Destructor.
  virtual ~MutatableImageNodeIterativeJuliaChoose();

  //! Return a clone.
  virtual MutatableImageNode*const deepclone() const;
};

//! Do Julia set returning co-ordinate filled with normalised iterations, or -1 in set.
/*! Julia set parameter is picked up from an extra argument c.f Mandelbrot
 */
class MutatableImageNodeIterativeJuliaContour : public MutatableImageNodeIterativeZSquaredPlusC
{
 private:

 protected:
  //! Implements this node's function.
  virtual const XYZ evaluate(const XYZ&) const;
  
 public:
  //! Query whether node value is independent of position argument.
  virtual const bool is_constant() const;

  //! Constructor.
  MutatableImageNodeIterativeJuliaContour(const std::vector<float>& p,const std::vector<MutatableImageNode*>& a,uint i);

  //! Destructor.
  virtual ~MutatableImageNodeIterativeJuliaContour();

  //! Return a clone.
  virtual MutatableImageNode*const deepclone() const;
};

#endif
