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

#include "mutation_parameters.h"

//! Abstract base class for all kinds of mutatable image node.
class MutatableImageNode
{
 private:
  
  //! The arguments (ie child nodes) of this node.
  std::vector<MutatableImageNode*> _args;
  
 protected:

  //! Accessor.
  std::vector<MutatableImageNode*>& args()
    {
      return _args;
    }

  //! Accessor.
  const std::vector<MutatableImageNode*>& args() const
    {
      return _args;
    }

  //! Convenience accessor. 
  MutatableImageNode& arg(uint n) const
    {
      assert(n<args().size());
      return *(args()[n]);
    }

  //! This returns a deep-cloned copy of the node's children.
  const std::vector<MutatableImageNode*> cloneargs() const;

  //! This what distinguishes different types of node.
  virtual const XYZ evaluate(const XYZ& p) const
    =0;

 public:

  //! This returns a new random bit of tree.
  static MutatableImageNode* stub(Random01& r01);

  //! This returns a vector of new random bits of tree.
  static const std::vector<MutatableImageNode*> stubvector(Random01& r01,uint n);

  //! Constructor.
  MutatableImageNode();

  //! Constructor given an array of args.
  MutatableImageNode(const std::vector<MutatableImageNode*>& a);

  //! Destructor.
  virtual ~MutatableImageNode();

  //! Scramble this node and its leaves up a bit.
  virtual void mutate(const MutationParameters&);
  
  //! Return an clone of this image node and all its children.
  virtual MutatableImageNode*const deepclone() const
    =0;
  
  //! Convenience wrapper for evaluate (actually, evaluate is private so can't be called externally anyway)
  const XYZ operator()(const XYZ& p) const
    {
      return evaluate(p);
    }
};

//! This node just returns a constant value.
class MutatableImageNodeConstant : public MutatableImageNode
{
 private:

 protected:
  //! Stores the constant value.
  XYZ _value;

  //! Returns the value of the constant.
  virtual const XYZ evaluate(const XYZ&) const;
  
 public:
  //! Constructor.
  MutatableImageNodeConstant(const XYZ& v);

  //! Destructor.
  virtual ~MutatableImageNodeConstant();

  //! Return a mutated version of this node.
  virtual void mutate(const MutationParameters&);

  //! Returns a clone.
  virtual MutatableImageNode*const deepclone() const;
};

//! This node just returns the position it's passed.
class MutatableImageNodePosition : public MutatableImageNode
{
 private:

 protected:
  //! Implements this node's function.
  virtual const XYZ evaluate(const XYZ&) const;
  
 public:
  //! Constructor.
  MutatableImageNodePosition();

  //! Destructor.
  virtual ~MutatableImageNodePosition();

  //! Change it a bit (except there's nothing to change in this case).
  virtual void mutate(const MutationParameters&);

  //! Returns a clone.
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
  //! Constructor.
  MutatableImageNodeSin(const std::vector<MutatableImageNode*>& a);

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
  //! Constructor.
  MutatableImageNodeCos(const std::vector<MutatableImageNode*>& a);

  //! Destructor.
  virtual ~MutatableImageNodeCos();

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
  
 public:
  //! Constructor.
  MutatableImageNodeGrad(const std::vector<MutatableImageNode*>& a);

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
  //! Constructor.
  MutatableImageNodeConcatenatePair(const std::vector<MutatableImageNode*>& a);

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
  //! Constructor.
  MutatableImageNodeAdd(const std::vector<MutatableImageNode*>& a);

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
  //! Constructor.
  MutatableImageNodeMultiply(const std::vector<MutatableImageNode*>& a);

  //! Destructor.
  virtual ~MutatableImageNodeMultiply();

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
  //! Constructor.
  MutatableImageNodeCross(const std::vector<MutatableImageNode*>& a);

  //! Destructor.
  virtual ~MutatableImageNodeCross();

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
  //! Constructor.
  MutatableImageNodeMod(const std::vector<MutatableImageNode*>& a);

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
  //! Constructor.
  MutatableImageNodeConcatenateTriple(const std::vector<MutatableImageNode*>& a);

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
  //! Constructor.
  MutatableImageNodeReflect(const std::vector<MutatableImageNode*>& a);

  //! Destructor.
  virtual ~MutatableImageNodeReflect();

  //! Return a clone.
  virtual MutatableImageNode*const deepclone() const;
};

//! This node implements selection between 2 functions based on the relative magnitudes of 2 other functions
class MutatableImageNodeChoose : public MutatableImageNode
{
 private:

 protected:
  //! Implements this node's function.
  virtual const XYZ evaluate(const XYZ&) const;
  
 public:
  //! Constructor.
  MutatableImageNodeChoose(const std::vector<MutatableImageNode*>& a);

  //! Destructor.
  virtual ~MutatableImageNodeChoose();

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
  //! Constructor.
  MutatableImageNodePreTransform(const std::vector<MutatableImageNode*>& a);

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
  //! Constructor.
  MutatableImageNodePostTransform(const std::vector<MutatableImageNode*>& a);

  //! Destructor.
  virtual ~MutatableImageNodePostTransform();

  //! Return a clone.
  virtual MutatableImageNode*const deepclone() const;
};

#endif
