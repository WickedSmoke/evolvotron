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

  // Base mutations are Constant or Position types
  const float base=0.7;
  const uint steps=19;
  const float step=(1.0-base)/steps;

  if (r<0.5*base)
    return new MutatableImageNodeConstant(RandomXYZInSphere(r01,1.0));
  else if (r<base)
    return new MutatableImageNodePosition();
  else if (r<base+1*step) 
    return new MutatableImageNodeXYZToSpherical();
  else if (r<base+2*step) 
    return new MutatableImageNodeSphericalToXYZ();
  else if (r<base+3*step) 
    return new MutatableImageNodeSphericalize(stubvector(r01,1));
  else if (r<base+4*step) 
    return new MutatableImageNodeSin(stubvector(r01,1));
  else if (r<base+5*step) 
    return new MutatableImageNodeCos(stubvector(r01,1));
  else if (r<base+6*step) 
    return new MutatableImageNodeGrad(stubvector(r01,1));
  else if (r<base+7*step) 
    return new MutatableImageNodeConcatenatePair(stubvector(r01,2));
  else if (r<base+8*step) 
    return new MutatableImageNodeAdd(stubvector(r01,2));
  else if (r<base+9*step) 
    return new MutatableImageNodeMultiply(stubvector(r01,2));
  else if (r<base+10*step) 
    return new MutatableImageNodeDivide(stubvector(r01,2));
  else if (r<base+11*step) 
    return new MutatableImageNodeCross(stubvector(r01,2));
  else if (r<base+12*step) 
    return new MutatableImageNodeMax(stubvector(r01,2));
  else if (r<base+13*step) 
    return new MutatableImageNodeMin(stubvector(r01,2));
  else if (r<base+14*step) 
    return new MutatableImageNodeMod(stubvector(r01,2));
  else if (r<base+15*step) 
    return new MutatableImageNodeConcatenateTriple(stubvector(r01,3));
  else if (r<base+16*step) 
    return new MutatableImageNodeReflect(stubvector(r01,3));
  else if (r<base+17*step) 
    return new MutatableImageNodeMagnitudes(stubvector(r01,3));
  else if (r<base+18*step) 
    return new MutatableImageNodeChooseSphere(stubvector(r01,4));
  else if (r<base+18.5*step)
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
  - duplicating arguments
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

  // Think about inserting a random stub between us and some children
  for (std::vector<MutatableImageNode*>::iterator it=args().begin();it!=args().end();it++)
    {
      if (parameters.r01()<parameters.probability_insert())
	{
	  std::vector<MutatableImageNode*> a;
	  a.push_back((*it));
	  a.push_back(stub(parameters.rng01()));

	  (*it)=new MutatableImageNodeConcatenatePair(a);
	}
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

const XYZ MutatableImageNodeXYZToSpherical::evaluate(const XYZ& p) const
{
  const float r=p.magnitude();

  // Angles are normalised over their possible range.
  const float theta=atan2(p.y(),p.x())*(1.0f/M_PI);
  const float phi=(r== 0.0f ? 0.0f : asin(p.z()/r)*(1.0f/(0.5f*M_PI)));

  return XYZ(r,theta,phi);
}

MutatableImageNodeXYZToSpherical::MutatableImageNodeXYZToSpherical()
{}

MutatableImageNodeXYZToSpherical::~MutatableImageNodeXYZToSpherical()
{}

MutatableImageNode*const MutatableImageNodeXYZToSpherical::deepclone() const
{
  return new MutatableImageNodeXYZToSpherical();
}

/*******************************************/

const XYZ MutatableImageNodeSphericalToXYZ::evaluate(const XYZ& p) const
{
  const float r=p.x();
  const float theta=M_PI*p.y();
  const float phi=0.5*M_PI*p.z();

  const float x=r*cos(theta)*sin(phi);
  const float y=r*sin(theta)*sin(phi);
  const float z=r*cos(phi);

  return XYZ(x,y,z);
}

MutatableImageNodeSphericalToXYZ::MutatableImageNodeSphericalToXYZ()
{}

MutatableImageNodeSphericalToXYZ::~MutatableImageNodeSphericalToXYZ()
{}

MutatableImageNode*const MutatableImageNodeSphericalToXYZ::deepclone() const
{
  return new MutatableImageNodeSphericalToXYZ();
}

/*******************************************/

const XYZ MutatableImageNodeSphericalize::evaluate(const XYZ& p) const
{
  const float in_r=p.magnitude();
  const float in_theta=atan2(p.y(),p.x())*(1.0f/M_PI);
  const float in_phi=(in_r== 0.0f ? 0.0f : asin(p.z()/in_r)*(1.0f/(0.5f*M_PI)));

  const XYZ v(arg(0)(XYZ(in_r,in_theta,in_phi)));

  const float out_r=v.x();
  const float out_theta=M_PI*v.y();
  const float out_phi=0.5*M_PI*v.z();

  const float x=out_r*cos(out_theta)*sin(out_phi);
  const float y=out_r*sin(out_theta)*sin(out_phi);
  const float z=out_r*cos(out_phi);

  return XYZ(x,y,z);
}

MutatableImageNodeSphericalize::MutatableImageNodeSphericalize(const std::vector<MutatableImageNode*>& a)
  :MutatableImageNode(a)
{
  assert(args().size()==1);
}

MutatableImageNodeSphericalize::~MutatableImageNodeSphericalize()
{}

MutatableImageNode*const MutatableImageNodeSphericalize::deepclone() const
{
  return new MutatableImageNodeSin(cloneargs());
}

/*******************************************/

const XYZ MutatableImageNodeSin::evaluate(const XYZ& p) const
{
  const XYZ v(arg(0)(p));
  return XYZ(sin(v.x()),sin(v.y()),sin(v.z()));
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
  return XYZ(cos(v.x()),cos(v.y()),cos(v.z()));
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

const XYZ MutatableImageNodeDivide::evaluate(const XYZ& p) const
{
  const XYZ v0(arg(0)(p));
  const XYZ v1(arg(1)(p));

  return XYZ(
	     (v1.x()==0.0 ? 0.0 : v0.x()/v1.x()),
	     (v1.y()==0.0 ? 0.0 : v0.y()/v1.y()),
	     (v1.z()==0.0 ? 0.0 : v0.z()/v1.z())
	     );
}

MutatableImageNodeDivide::MutatableImageNodeDivide(const std::vector<MutatableImageNode*>& a)
  :MutatableImageNode(a)
{
  assert(args().size()==2);
}

MutatableImageNodeDivide::~MutatableImageNodeDivide()
{}

MutatableImageNode*const MutatableImageNodeDivide::deepclone() const
{
  return new MutatableImageNodeDivide(cloneargs());
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
  return XYZ(maximum(v0.x(),v1.x()),maximum(v0.y(),v1.y()),maximum(v0.z(),v1.z()));
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
  return XYZ(minimum(v0.x(),v1.x()),minimum(v0.y(),v1.y()),minimum(v0.z(),v1.z()));
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
  
  return XYZ(fmod(a.x(),b.x()),fmod(a.y(),b.y()),fmod(a.z(),b.z()));
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

const XYZ MutatableImageNodeMagnitudes::evaluate(const XYZ& p) const
{
  return XYZ(arg(0)(p).magnitude(),arg(1)(p).magnitude(),arg(2)(p).magnitude());
}

MutatableImageNodeMagnitudes::MutatableImageNodeMagnitudes(const std::vector<MutatableImageNode*>& a)
  :MutatableImageNode(a)
{
  assert(args().size()==3);
}

MutatableImageNodeMagnitudes::~MutatableImageNodeMagnitudes()
{}

MutatableImageNode*const MutatableImageNodeMagnitudes::deepclone() const
{
  return new MutatableImageNodeMagnitudes(cloneargs());
}

/*******************************************/

const XYZ MutatableImageNodeChooseSphere::evaluate(const XYZ& p) const
{
  const float m0=(arg(0)(p)).magnitude2();
  const float m1=(arg(1)(p)).magnitude2();

  if (m0<m1)
    return arg(2)(p);
  else
    return arg(3)(p);
}

MutatableImageNodeChooseSphere::MutatableImageNodeChooseSphere(const std::vector<MutatableImageNode*>& a)
  :MutatableImageNode(a)
{
  assert(args().size()==4);
}

MutatableImageNodeChooseSphere::~MutatableImageNodeChooseSphere()
{}

MutatableImageNode*const MutatableImageNodeChooseSphere::deepclone() const
{
  return new MutatableImageNodeChooseSphere(cloneargs());
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

  const XYZ pt(offset+basis_x*p.x()+basis_y*p.y()+basis_z*p.z());

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

  return offset+basis_x*pt.x()+basis_y*pt.y()+basis_z*pt.z();
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

