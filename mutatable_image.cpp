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
  \brief Implementation of class MutatableImageNode and derived classes.
*/

#include <algorithm>

#include "mutatable_image.h"

const std::vector<MutatableImageNode*> MutatableImageNode::cloneargs() const
{
  std::vector<MutatableImageNode*> ret;
  for (std::vector<MutatableImageNode*>::const_iterator it=args().begin();it!=args().end();it++)
    {
      ret.push_back((*it)->deepclone());
    }
  return ret;
}

/*! This returns a random bit of image tree.
  It needs to be capable of generating any sort of node we have.
  \warning Too much probability of highly branching nodes could result in infinite sized stubs.
  \todo Compute (statistically) the expected number of nodes in a stub.
 */
MutatableImageNode* MutatableImageNode::stub(Random01& r01)
{
  const float r=r01();

  //! \todo: Should be a mutation parameter which is ratio between Position nodes and warped position nodes.

  if (r<0.4)
    return new MutatableImageNodeConstant(RandomXYZInSphere(r01,1.0));
  else if (r<0.8)
    return new MutatableImageNodePosition();
  else if (r<0.81) 
    return new MutatableImageNodeSin(stubvector(r01,1));
  else if (r<0.82) 
    return new MutatableImageNodeCos(stubvector(r01,1));
  else if (r<0.83) 
    return new MutatableImageNodeGrad(stubvector(r01,1));
  else if (r<0.84) 
    return new MutatableImageNodeConcatenatePair(stubvector(r01,2));
  else if (r<0.85) 
    return new MutatableImageNodeAdd(stubvector(r01,2));
  else if (r<0.86) 
    return new MutatableImageNodeMultiply(stubvector(r01,2));
  else if (r<0.87) 
    return new MutatableImageNodeCross(stubvector(r01,2));
  else if (r<0.88) 
    return new MutatableImageNodeMax(stubvector(r01,2));
  else if (r<0.89) 
    return new MutatableImageNodeMin(stubvector(r01,2));
  else if (r<0.90) 
    return new MutatableImageNodeMod(stubvector(r01,2));
  else if (r<0.91) 
    return new MutatableImageNodeConcatenateTriple(stubvector(r01,3));
  else if (r<0.92) 
    return new MutatableImageNodeReflect(stubvector(r01,3));
  else if (r<0.93) 
    return new MutatableImageNodeChoose(stubvector(r01,4));
  else if (r<0.965)
    return new MutatableImageNodePostTransform(stubvector(r01,5));
  else 
    return new MutatableImageNodePreTransform(stubvector(r01,5));
}

/*! This returns a vector of random bits of stub, used for initialiing nodes with children. 
 */
const std::vector<MutatableImageNode*> MutatableImageNode::stubvector(Random01& r01,uint n)
{
  std::vector<MutatableImageNode*> ret;
  for (uint i=0;i<n;i++)
    ret.push_back(stub(r01));
  return ret;
}


MutatableImageNode::MutatableImageNode()
{}

MutatableImageNode::MutatableImageNode(const std::vector<MutatableImageNode*>& a)
:_args(a)
{}

/*! Deletes all arguments.  No one else should be referencing except the root node of an image.
 */
MutatableImageNode::~MutatableImageNode()
{
  for (std::vector<MutatableImageNode*>::iterator it=args().begin();it!=args().end();it++)
    delete (*it);
}

/*! There are 2 kinds of mutation: structural mutations (messing with the tree) are all performed here, 
  and random adjustments to constants (which are handled by MutatableImageNodeConstant::mutate). 
  For structural mutations the obvious things to do are:
  - reordering argsuments
  - dropping argsuments and replacing them with new "stubs".
  - duplicating argsuments
  - substituting nodes with other types (can't do this for ourself very easily, but we can do it for children)
  - inserting new nodes between children and ourself

  And of course all children have to be mutated too.
 */
void MutatableImageNode::mutate(const MutationParameters& parameters)
{
  // First mutate all child nodes.
  for (std::vector<MutatableImageNode*>::iterator it=args().begin();it!=args().end();it++)
    (*it)->mutate(parameters);

  // Think about glitching some nodes.
  for (std::vector<MutatableImageNode*>::iterator it=args().begin();it!=args().end();it++)
    {
      if (parameters.r01()<parameters.probability_glitch())
	{
	  delete (*it);
	  (*it)=stub(parameters.rng01());
	}
    }

  // Think about randomising child order
  if (parameters.r01()<parameters.probability_shuffle())
    {
      // This uses rand() (would rather use our one).
      // This bit of STL seems a bit up in the air (at least in GNU implementation), but it works so who cares.
      std::random_shuffle(args().begin(),args().end());
    }
}

/*******************************************/

const XYZ MutatableImageNodeConstant::evaluate(const XYZ&) const
{
  return _value;
}

MutatableImageNodeConstant::MutatableImageNodeConstant(const XYZ& v)
  :_value(v)
{}

MutatableImageNodeConstant::~MutatableImageNodeConstant()
{}

void MutatableImageNodeConstant::mutate(const MutationParameters& parameters)
{
  _value+=RandomXYZInSphere(parameters.rng01(),parameters.magnitude());
}

MutatableImageNode*const MutatableImageNodeConstant::deepclone() const
{
  return new MutatableImageNodeConstant(_value);
}

/*******************************************/

/*! No work to be done for evaluate because _value is already set.
 */
const XYZ MutatableImageNodePosition::evaluate(const XYZ& p) const
{
  return p;
}

MutatableImageNodePosition::MutatableImageNodePosition()
{}

MutatableImageNodePosition::~MutatableImageNodePosition()
{}

/* Since this node has no leaves and no parameters to mess with there's absolutely nothing to be done.
 */
void MutatableImageNodePosition::mutate(const MutationParameters&)
{}

MutatableImageNode*const MutatableImageNodePosition::deepclone() const
{
  return new MutatableImageNodePosition();
}

/*******************************************/

const XYZ MutatableImageNodeSin::evaluate(const XYZ& p) const
{
  const XYZ v(arg(0)(p));
  return XYZ(sin(v.x),sin(v.y),sin(v.z));
}

MutatableImageNodeSin::MutatableImageNodeSin(const std::vector<MutatableImageNode*>& a)
  :MutatableImageNode(a)
{
  assert(args().size()==1);
}

MutatableImageNodeSin::~MutatableImageNodeSin()
{}

MutatableImageNode*const MutatableImageNodeSin::deepclone() const
{
  return new MutatableImageNodeSin(cloneargs());
}

/*******************************************/

const XYZ MutatableImageNodeCos::evaluate(const XYZ& p) const
{
  const XYZ v(arg(0)(p));
  return XYZ(cos(v.x),cos(v.y),cos(v.z));
}

MutatableImageNodeCos::MutatableImageNodeCos(const std::vector<MutatableImageNode*>& a)
  :MutatableImageNode(a)
{
  assert(args().size()==1);
}

MutatableImageNodeCos::~MutatableImageNodeCos()
{}

MutatableImageNode*const MutatableImageNodeCos::deepclone() const
{
  return new MutatableImageNodeCos(cloneargs());
}

/*******************************************/

const XYZ MutatableImageNodeGrad::evaluate(const XYZ& p) const
{
  const float epsilon=1e-6;
  const XYZ vepsilon(epsilon,epsilon,epsilon);

  const XYZ v0(arg(0)(p-vepsilon));
  const XYZ v1(arg(0)(p+vepsilon));
  return (v1-v0)/(2.0*epsilon);
}

MutatableImageNodeGrad::MutatableImageNodeGrad(const std::vector<MutatableImageNode*>& a)
  :MutatableImageNode(a)
{
  assert(args().size()==1);
}

MutatableImageNodeGrad::~MutatableImageNodeGrad()
{}

MutatableImageNode*const MutatableImageNodeGrad::deepclone() const
{
  return new MutatableImageNodeGrad(cloneargs());
}

/*******************************************/

const XYZ MutatableImageNodeConcatenatePair::evaluate(const XYZ& p) const
{
  return arg(1)(arg(0)(p));
}

MutatableImageNodeConcatenatePair::MutatableImageNodeConcatenatePair(const std::vector<MutatableImageNode*>& a)
  :MutatableImageNode(a)
{
  assert(args().size()==2);
}

MutatableImageNodeConcatenatePair::~MutatableImageNodeConcatenatePair()
{}

MutatableImageNode*const MutatableImageNodeConcatenatePair::deepclone() const
{
  return new MutatableImageNodeConcatenatePair(cloneargs());
}

/*******************************************/

const XYZ MutatableImageNodeAdd::evaluate(const XYZ& p) const
{
  return arg(0)(p)+arg(1)(p);
}

MutatableImageNodeAdd::MutatableImageNodeAdd(const std::vector<MutatableImageNode*>& a)
  :MutatableImageNode(a)
{
  assert(args().size()==2);
}

MutatableImageNodeAdd::~MutatableImageNodeAdd()
{}

MutatableImageNode*const MutatableImageNodeAdd::deepclone() const
{
  return new MutatableImageNodeAdd(cloneargs());
}

/*******************************************/

const XYZ MutatableImageNodeMultiply::evaluate(const XYZ& p) const
{
  return arg(0)(p)*arg(1)(p);
}

MutatableImageNodeMultiply::MutatableImageNodeMultiply(const std::vector<MutatableImageNode*>& a)
  :MutatableImageNode(a)
{
  assert(args().size()==2);
}

MutatableImageNodeMultiply::~MutatableImageNodeMultiply()
{}

MutatableImageNode*const MutatableImageNodeMultiply::deepclone() const
{
  return new MutatableImageNodeMultiply(cloneargs());
}

/*******************************************/

const XYZ MutatableImageNodeCross::evaluate(const XYZ& p) const
{
  return arg(0)(p)*arg(1)(p);
}

MutatableImageNodeCross::MutatableImageNodeCross(const std::vector<MutatableImageNode*>& a)
  :MutatableImageNode(a)
{
  assert(args().size()==2);
}

MutatableImageNodeCross::~MutatableImageNodeCross()
{}

MutatableImageNode*const MutatableImageNodeCross::deepclone() const
{
  return new MutatableImageNodeCross(cloneargs());
}

/*******************************************/

const XYZ MutatableImageNodeMax::evaluate(const XYZ& p) const
{
  const XYZ v0(arg(0)(p));
  const XYZ v1(arg(1)(p));
  return XYZ(maximum(v0.x,v1.x),maximum(v0.y,v1.y),maximum(v0.z,v1.z));
}

MutatableImageNodeMax::MutatableImageNodeMax(const std::vector<MutatableImageNode*>& a)
  :MutatableImageNode(a)
{
  assert(args().size()==2);
}

MutatableImageNodeMax::~MutatableImageNodeMax()
{}

MutatableImageNode*const MutatableImageNodeMax::deepclone() const
{
  return new MutatableImageNodeMax(cloneargs());
}

/*******************************************/

const XYZ MutatableImageNodeMin::evaluate(const XYZ& p) const
{
  const XYZ v0(arg(0)(p));
  const XYZ v1(arg(1)(p));
  return XYZ(minimum(v0.x,v1.x),minimum(v0.y,v1.y),minimum(v0.z,v1.z));
}

MutatableImageNodeMin::MutatableImageNodeMin(const std::vector<MutatableImageNode*>& a)
  :MutatableImageNode(a)
{
  assert(args().size()==2);
}

MutatableImageNodeMin::~MutatableImageNodeMin()
{}

MutatableImageNode*const MutatableImageNodeMin::deepclone() const
{
  return new MutatableImageNodeMin(cloneargs());
}

/*******************************************/

const XYZ MutatableImageNodeMod::evaluate(const XYZ& p) const
{
  const XYZ a(arg(0)(p));
  const XYZ b(arg(1)(p));
  
  return XYZ(fmod(a.x,b.x),fmod(a.y,b.y),fmod(a.z,b.z));
}

MutatableImageNodeMod::MutatableImageNodeMod(const std::vector<MutatableImageNode*>& a)
  :MutatableImageNode(a)
{
  assert(args().size()==2);
}

MutatableImageNodeMod::~MutatableImageNodeMod()
{}

MutatableImageNode*const MutatableImageNodeMod::deepclone() const
{
  return new MutatableImageNodeMod(cloneargs());
}

/*******************************************/

const XYZ MutatableImageNodeConcatenateTriple::evaluate(const XYZ& p) const
{
  return arg(2)(arg(1)(arg(0)(p)));
}

MutatableImageNodeConcatenateTriple::MutatableImageNodeConcatenateTriple(const std::vector<MutatableImageNode*>& a)
  :MutatableImageNode(a)
{
  assert(args().size()==3);
}

MutatableImageNodeConcatenateTriple::~MutatableImageNodeConcatenateTriple()
{}

MutatableImageNode*const MutatableImageNodeConcatenateTriple::deepclone() const
{
  return new MutatableImageNodeConcatenateTriple(cloneargs());
}

/*******************************************/

const XYZ MutatableImageNodeReflect::evaluate(const XYZ& p) const
{
  const XYZ pt_in_plane(arg(0)(p));
  const XYZ normal(arg(1)(p).normalised());
  
  XYZ pos(arg(2)(p));

  const float distance_from_plane=(pos-pt_in_plane)%normal;
  
  // If pos is on the wrong side of the plane, reflect it over
  if (distance_from_plane<0.0)
    {
      pos-=(2.0*distance_from_plane)*normal;
    }

  return pos;
}

MutatableImageNodeReflect::MutatableImageNodeReflect(const std::vector<MutatableImageNode*>& a)
  :MutatableImageNode(a)
{
  assert(args().size()==3);
}

MutatableImageNodeReflect::~MutatableImageNodeReflect()
{}

MutatableImageNode*const MutatableImageNodeReflect::deepclone() const
{
  return new MutatableImageNodeReflect(cloneargs());
}

/*******************************************/

const XYZ MutatableImageNodeChoose::evaluate(const XYZ& p) const
{
  const float m0=(arg(0)(p)).magnitude2();
  const float m1=(arg(1)(p)).magnitude2();

  if (m0<m1)
    return arg(2)(p);
  else
    return arg(3)(p);
}

MutatableImageNodeChoose::MutatableImageNodeChoose(const std::vector<MutatableImageNode*>& a)
  :MutatableImageNode(a)
{
  assert(args().size()==4);
}

MutatableImageNodeChoose::~MutatableImageNodeChoose()
{}

MutatableImageNode*const MutatableImageNodeChoose::deepclone() const
{
  return new MutatableImageNodeChoose(cloneargs());
}

/*******************************************/

/*! Transforms the co-ordinate BEFORE passing it through the function argument.
 */
const XYZ MutatableImageNodePreTransform::evaluate(const XYZ& p) const
{
  const XYZ offset (arg(0)(p));
  const XYZ basis_x(arg(1)(p));
  const XYZ basis_y(arg(2)(p));
  const XYZ basis_z(arg(3)(p));

  const XYZ pt(offset+basis_x*p.x+basis_y*p.y+basis_z*p.z);

  return arg(4)(pt);
}

MutatableImageNodePreTransform::MutatableImageNodePreTransform(const std::vector<MutatableImageNode*>& a)
  :MutatableImageNode(a)
{
  assert (args().size()==5);
}

MutatableImageNodePreTransform::~MutatableImageNodePreTransform()
{}

MutatableImageNode*const MutatableImageNodePreTransform::deepclone() const
{
  return new MutatableImageNodePreTransform(cloneargs());
}

/*******************************************/

/*! Transforms the co-ordinate AFTER passing it through the function argument.
 */
const XYZ MutatableImageNodePostTransform::evaluate(const XYZ& p) const
{
  const XYZ offset (arg(0)(p));
  const XYZ basis_x(arg(1)(p));
  const XYZ basis_y(arg(2)(p));
  const XYZ basis_z(arg(3)(p));

  const XYZ pt(arg(4)(p));

  return offset+basis_x*pt.x+basis_y*pt.y+basis_z*pt.z;
}

MutatableImageNodePostTransform::MutatableImageNodePostTransform(const std::vector<MutatableImageNode*>& a)
  :MutatableImageNode(a)
{
  assert (args().size()==5);
}

MutatableImageNodePostTransform::~MutatableImageNodePostTransform()
{}

MutatableImageNode*const MutatableImageNodePostTransform::deepclone() const
{
  return new MutatableImageNodePostTransform(cloneargs());
}

