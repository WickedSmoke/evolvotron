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
#include <iostream>

#include "mutatable_image.h"
#include "matrix.h"

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
MutatableImageNode* MutatableImageNode::stub(const MutationParameters& parameters,bool exciting)
{
  // Base mutations are Constant or Position types.  
  // (Position can be Position or PositionTransformed, proportions depending on identity_supression parameter)
  const float base=0.7;

  uint steps=31;

  if (!parameters.allow_fractal_nodes())
    {
      steps=minimum(steps,27u);
    }

  if (!parameters.allow_iterative_nodes())
    {
      steps=minimum(steps,25u);
    }

  const float step=(1.0-base)/steps;

  const float r=(exciting ? base+(1.0f-base)*parameters.r01() : parameters.r01());

  if (r<(1.0f-parameters.proportion_constant())*parameters.identity_supression()*base)
    {
      if (parameters.r01()<0.5f)
	return new MutatableImageNodePositionTransformed
	  (
	   RandomXYZInSphere(parameters.rng01(),1.0),
	   RandomXYZInSphere(parameters.rng01(),1.0),
	   RandomXYZInSphere(parameters.rng01(),1.0),
	   RandomXYZInSphere(parameters.rng01(),1.0)
	   );
      else
	return new MutatableImageNodePositionTransformedQuadratic
	  (
	   RandomXYZInSphere(parameters.rng01(),1.0),
	   RandomXYZInSphere(parameters.rng01(),1.0),
	   RandomXYZInSphere(parameters.rng01(),1.0),
	   RandomXYZInSphere(parameters.rng01(),1.0),
	   RandomXYZInSphere(parameters.rng01(),1.0),
	   RandomXYZInSphere(parameters.rng01(),1.0),
	   RandomXYZInSphere(parameters.rng01(),1.0),
	   RandomXYZInSphere(parameters.rng01(),1.0),
	   RandomXYZInSphere(parameters.rng01(),1.0),
	   RandomXYZInSphere(parameters.rng01(),1.0)
	   );	
    }
  else if (r<(1.0f-parameters.proportion_constant())*base)
    return new MutatableImageNodePosition();
  else if (r<base)
    return new MutatableImageNodeConstant
      (
       RandomXYZInSphere(parameters.rng01(),1.0)
       );
  else if (r<base+1*step)
    return new MutatableImageNodeXYZToSpherical();
  else if (r<base+2*step) 
    return new MutatableImageNodeSphericalToXYZ();
  else if (r<base+3*step) 
    return new MutatableImageNodeSphericalize(stubvector(parameters,1));
  else if (r<base+4*step) 
    return new MutatableImageNodeRotate(stubvector(parameters,1));
  else if (r<base+5*step) 
    return new MutatableImageNodeSin(stubvector(parameters,1));
  else if (r<base+6*step) 
    return new MutatableImageNodeCos(stubvector(parameters,1));
  else if (r<base+7*step) 
    return new MutatableImageNodeSpiralLinear(stubvector(parameters,1));
  else if (r<base+8*step) 
    return new MutatableImageNodeSpiralLogarithmic(stubvector(parameters,1));
  else if (r<base+9*step) 
    return new MutatableImageNodeGrad(stubvector(parameters,1));
  else if (r<base+10*step) 
    return new MutatableImageNodeConcatenatePair(stubvector(parameters,2));
  else if (r<base+11*step) 
    return new MutatableImageNodeAdd(stubvector(parameters,2));
  else if (r<base+12*step) 
    return new MutatableImageNodeMultiply(stubvector(parameters,2));
  else if (r<base+13*step) 
    return new MutatableImageNodeDivide(stubvector(parameters,2));
  else if (r<base+14*step) 
    return new MutatableImageNodeCross(stubvector(parameters,2));
  else if (r<base+15*step) 
    return new MutatableImageNodeGeometricInversion(stubvector(parameters,2));
  else if (r<base+16*step) 
    return new MutatableImageNodeMax(stubvector(parameters,2));
  else if (r<base+17*step) 
    return new MutatableImageNodeMin(stubvector(parameters,2));
  else if (r<base+18*step) 
    return new MutatableImageNodeMod(stubvector(parameters,2));
  else if (r<base+19*step) 
    return new MutatableImageNodeConcatenateTriple(stubvector(parameters,3));
  else if (r<base+20*step) 
    return new MutatableImageNodeReflect(stubvector(parameters,3));
  else if (r<base+21*step) 
    return new MutatableImageNodeMagnitudes(stubvector(parameters,3));
  else if (r<base+22*step) 
    return new MutatableImageNodeChooseSphere(stubvector(parameters,4));
  else if (r<base+23*step) 
    return new MutatableImageNodeChooseRect(stubvector(parameters,4));
  else if (r<base+24*step)
    return new MutatableImageNodePostTransform(stubvector(parameters,5));
  else if (r<base+25*step)
    return new MutatableImageNodePreTransform(stubvector(parameters,5));

  else if (r<base+26*step)
    return new MutatableImageNodeIterativeMap(stubvector(parameters,1),1+static_cast<uint>(parameters.r01()*parameters.max_initial_iterations()));
  else if (r<base+27*step)
    return new MutatableImageNodeIterativeMapAccumulator(stubvector(parameters,2),1+static_cast<uint>(parameters.r01()*parameters.max_initial_iterations()));

  else if (r<base+28*step)
    return new MutatableImageNodeIterativeMandelbrotChoose(stubvector(parameters,2),1+static_cast<uint>(parameters.r01()*parameters.max_initial_iterations()));
  else if (r<base+29*step)
    return new MutatableImageNodeIterativeMandelbrotContour(stubvector(parameters,0),1+static_cast<uint>(parameters.r01()*parameters.max_initial_iterations()));
  else if (r<base+30*step)
    return new MutatableImageNodeIterativeJuliaChoose(stubvector(parameters,3),1+static_cast<uint>(parameters.r01()*parameters.max_initial_iterations()));
  else //if (r<base+31*step)
    return new MutatableImageNodeIterativeJuliaContour(stubvector(parameters,1),1+static_cast<uint>(parameters.r01()*parameters.max_initial_iterations()));
}


/*! This returns a vector of random bits of stub, used for initialiing nodes with children. 
 */
const std::vector<MutatableImageNode*> MutatableImageNode::stubvector(const MutationParameters& parameters,uint n)
{
  std::vector<MutatableImageNode*> ret;
  for (uint i=0;i<n;i++)
    ret.push_back(stub(parameters));
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
	  (*it)=stub(parameters);
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
	  a.push_back(stub(parameters));

	  (*it)=new MutatableImageNodeConcatenatePair(a);
	}
    }
}

const MutatableImageNodeTransformWrapper*const MutatableImageNode::is_a_MutatableImageNodeTransformWrapper() const
{
  return 0;
}

MutatableImageNodeTransformWrapper*const MutatableImageNode::is_a_MutatableImageNodeTransformWrapper()
{
  return 0;
}

/*******************************************/

/*! No work to be done for evaluate because _value is already set.
 */
const XYZ MutatableImageNodeConstant::evaluate(const XYZ&) const
{
  return _value;
}

const bool MutatableImageNodeConstant::is_constant() const
{
  return true;
}

MutatableImageNodeConstant::MutatableImageNodeConstant(const XYZ& v)
  :_value(v)
{}

MutatableImageNodeConstant::~MutatableImageNodeConstant()
{}

void MutatableImageNodeConstant::mutate(const MutationParameters& parameters)
{
  // No need to call MutatableImageNode::mutate(parameters) since this is a leaf.
  _value+=RandomXYZInSphere(parameters.rng01(),parameters.magnitude());
}

MutatableImageNode*const MutatableImageNodeConstant::deepclone() const
{
  return new MutatableImageNodeConstant(_value);
}

/*******************************************/

const XYZ MutatableImageNodePosition::evaluate(const XYZ& p) const
{
  return p;
}

const bool MutatableImageNodePosition::is_constant() const
{
  return false;
}

MutatableImageNodePosition::MutatableImageNodePosition()
{}

MutatableImageNodePosition::~MutatableImageNodePosition()
{}

/* Since this node has no branches and no parameters to mess with there's absolutely nothing to be done.
 */
void MutatableImageNodePosition::mutate(const MutationParameters&)
{
  // No need to call MutatableImageNode::mutate(parameters) because this is a leaf node.
}

MutatableImageNode*const MutatableImageNodePosition::deepclone() const
{
  return new MutatableImageNodePosition();
}

/*******************************************/

const XYZ MutatableImageNodePositionTransformed::evaluate(const XYZ& p) const
{
  return _translate+_basis_x*p.x()+_basis_y*p.y()+_basis_z*p.z();
}

const bool MutatableImageNodePositionTransformed::is_constant() const
{
  return false;
}

MutatableImageNodePositionTransformed::MutatableImageNodePositionTransformed(const XYZ& t,const XYZ& bx,const XYZ& by,const XYZ& bz)
:_translate(t)
,_basis_x(bx)
,_basis_y(by)
,_basis_z(bz)
{}

MutatableImageNodePositionTransformed::~MutatableImageNodePositionTransformed()
{}

void MutatableImageNodePositionTransformed::mutate(const MutationParameters& parameters)
{
  // No need to call MutatableImageNode::mutate(parameters) since this is a leaf.

  _translate+=RandomXYZInSphere(parameters.rng01(),parameters.magnitude());
  _basis_x  +=RandomXYZInSphere(parameters.rng01(),parameters.magnitude());
  _basis_y  +=RandomXYZInSphere(parameters.rng01(),parameters.magnitude());
  _basis_z  +=RandomXYZInSphere(parameters.rng01(),parameters.magnitude());
}

MutatableImageNode*const MutatableImageNodePositionTransformed::deepclone() const
{
  return new MutatableImageNodePositionTransformed(_translate,_basis_x,_basis_y,_basis_z);
}

/*******************************************/

const XYZ MutatableImageNodeTransformWrapper::evaluate(const XYZ& p) const
{
  return arg(0)(_transform.transformed(p));
}

const bool MutatableImageNodeTransformWrapper::is_constant() const
{
  return arg(0).is_constant();
}

const MutatableImageNodeTransformWrapper*const MutatableImageNodeTransformWrapper::is_a_MutatableImageNodeTransformWrapper() const
{
  return this;
}

MutatableImageNodeTransformWrapper*const MutatableImageNodeTransformWrapper::is_a_MutatableImageNodeTransformWrapper()
{
  return this;
}

MutatableImageNodeTransformWrapper::MutatableImageNodeTransformWrapper(const std::vector<MutatableImageNode*>& a,const Transform& transform)
:MutatableImageNode(a)
,_transform(transform)
{
  assert(args().size()==1);
}

MutatableImageNodeTransformWrapper::~MutatableImageNodeTransformWrapper()
{}

void MutatableImageNodeTransformWrapper::mutate(const MutationParameters& parameters)
{
  MutatableImageNode::mutate(parameters);

  _transform.translate(_transform.translate()+RandomXYZInSphere(parameters.rng01(),parameters.magnitude()));
  _transform.basis_x(_transform.basis_x()+RandomXYZInSphere(parameters.rng01(),parameters.magnitude()));
  _transform.basis_y(_transform.basis_y()+RandomXYZInSphere(parameters.rng01(),parameters.magnitude()));
  _transform.basis_z(_transform.basis_z()+RandomXYZInSphere(parameters.rng01(),parameters.magnitude()));
}

MutatableImageNode*const MutatableImageNodeTransformWrapper::deepclone() const
{
  return new MutatableImageNodeTransformWrapper(cloneargs(),_transform);
}

/*******************************************/

const XYZ MutatableImageNodePositionTransformedQuadratic::evaluate(const XYZ& p) const
{
  return 
    _translate
    +_basis_x*p.x()+_basis_y*p.y()+_basis_z*p.z()
    +_basis_xy*(p.x()*p.y())+_basis_xz*(p.x()*p.z())+_basis_yz*(p.y()*p.z())
    +_basis_xx*(p.x()*p.x())+_basis_yy*(p.y()*p.y())+_basis_zz*(p.z()*p.z());
}

const bool MutatableImageNodePositionTransformedQuadratic::is_constant() const
{
  return false;
}

MutatableImageNodePositionTransformedQuadratic::MutatableImageNodePositionTransformedQuadratic
(
 const XYZ& t,const XYZ& bx,const XYZ& by,const XYZ& bz,
 const XYZ& bxy,const XYZ& bxz,const XYZ& byz,
 const XYZ& bxx,const XYZ& byy,const XYZ& bzz
 )
  :_translate(t)
   ,_basis_x(bx)
   ,_basis_y(by)
   ,_basis_z(bz)
   ,_basis_xy(bxy)
   ,_basis_xz(bxz)
   ,_basis_yz(byz)
   ,_basis_xx(bxx)
   ,_basis_yy(byy)
   ,_basis_zz(bzz)
{}

MutatableImageNodePositionTransformedQuadratic::~MutatableImageNodePositionTransformedQuadratic()
{}

void MutatableImageNodePositionTransformedQuadratic::mutate(const MutationParameters& parameters)
{
  // No need to call MutatableImageNode::mutate(parameters) since this is a leaf.

  _translate+=RandomXYZInSphere(parameters.rng01(),parameters.magnitude());
  _basis_x  +=RandomXYZInSphere(parameters.rng01(),parameters.magnitude());
  _basis_y  +=RandomXYZInSphere(parameters.rng01(),parameters.magnitude());
  _basis_z  +=RandomXYZInSphere(parameters.rng01(),parameters.magnitude());
  _basis_xy +=RandomXYZInSphere(parameters.rng01(),parameters.magnitude());
  _basis_xz +=RandomXYZInSphere(parameters.rng01(),parameters.magnitude());
  _basis_yz +=RandomXYZInSphere(parameters.rng01(),parameters.magnitude());
  _basis_xx +=RandomXYZInSphere(parameters.rng01(),parameters.magnitude());
  _basis_yy +=RandomXYZInSphere(parameters.rng01(),parameters.magnitude());
  _basis_zz +=RandomXYZInSphere(parameters.rng01(),parameters.magnitude());
}

MutatableImageNode*const MutatableImageNodePositionTransformedQuadratic::deepclone() const
{
  return new MutatableImageNodePositionTransformedQuadratic
    (
     _translate,
     _basis_x,_basis_y,_basis_z,
     _basis_xy,_basis_xz,_basis_yz,
     _basis_xx,_basis_yy,_basis_zz
     );
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

const bool MutatableImageNodeXYZToSpherical::is_constant() const
{
  return false;
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

const bool MutatableImageNodeSphericalToXYZ::is_constant() const
{
  return false;
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

const bool MutatableImageNodeSphericalize::is_constant() const
{
  return arg(0).is_constant();
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
  return new MutatableImageNodeSphericalize(cloneargs());
}

/*******************************************/

const XYZ MutatableImageNodeRotate::evaluate(const XYZ& p) const
{
  const XYZ a(arg(0)(p)*M_PI);
  
  Matrix33RotateX rx(a.x());
  Matrix33RotateY ry(a.y());
  Matrix33RotateZ rz(a.z());

  return XYZ((rx*ry*rz)*p);
}

const bool MutatableImageNodeRotate::is_constant() const
{
  return arg(0).is_constant();
}

MutatableImageNodeRotate::MutatableImageNodeRotate(const std::vector<MutatableImageNode*>& a)
  :MutatableImageNode(a)
{
  assert(args().size()==1);
}

MutatableImageNodeRotate::~MutatableImageNodeRotate()
{}

MutatableImageNode*const MutatableImageNodeRotate::deepclone() const
{
  return new MutatableImageNodeRotate(cloneargs());
}


/*******************************************/

const XYZ MutatableImageNodeSin::evaluate(const XYZ& p) const
{
  const XYZ v(arg(0)(p));
  return XYZ(sin(v.x()),sin(v.y()),sin(v.z()));
}

const bool MutatableImageNodeSin::is_constant() const
{
  return arg(0).is_constant();
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

const bool MutatableImageNodeCos::is_constant() const
{
  return arg(0).is_constant();
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

const XYZ MutatableImageNodeSpiralLinear::evaluate(const XYZ& p) const
{
  const float r=p.magnitude();
  float theta=atan2(p.y(),p.x());
  if (theta<0.0f) theta+=2.0f*M_PI;
  const float winding=floor(r-theta/(2.0*M_PI));

  const float x=2.0f*winding+theta/M_PI;
  const float y=2.0f*r-x;

  return arg(0)(XYZ(x,y,p.z()));
}

const bool MutatableImageNodeSpiralLinear::is_constant() const
{
  return (arg(0).is_constant());
}

MutatableImageNodeSpiralLinear::MutatableImageNodeSpiralLinear(const std::vector<MutatableImageNode*>& a)
  :MutatableImageNode(a)
{
  assert(args().size()==1);
}

MutatableImageNodeSpiralLinear::~MutatableImageNodeSpiralLinear()
{}

MutatableImageNode*const MutatableImageNodeSpiralLinear::deepclone() const
{
  return new MutatableImageNodeSpiralLinear(cloneargs());
}

/*******************************************/

const XYZ MutatableImageNodeSpiralLogarithmic::evaluate(const XYZ& p) const
{
  const float r=p.magnitude();
  float theta=atan2(p.y(),p.x());
  if (theta<0.0f) theta+=2.0f*M_PI;
  const float lnr=log(r);
  const float winding=floor(lnr-theta/(2.0*M_PI));

  const float x=2.0f*winding+theta/M_PI;
  const float y=2.0f*lnr-x;

  return arg(0)(XYZ(x,y,p.z()));
}

const bool MutatableImageNodeSpiralLogarithmic::is_constant() const
{
  return (arg(0).is_constant());
}

MutatableImageNodeSpiralLogarithmic::MutatableImageNodeSpiralLogarithmic(const std::vector<MutatableImageNode*>& a)
  :MutatableImageNode(a)
{
  assert(args().size()==1);
}

MutatableImageNodeSpiralLogarithmic::~MutatableImageNodeSpiralLogarithmic()
{}

MutatableImageNode*const MutatableImageNodeSpiralLogarithmic::deepclone() const
{
  return new MutatableImageNodeSpiralLogarithmic(cloneargs());
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

const bool MutatableImageNodeGrad::is_constant() const
{
  return arg(0).is_constant();
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

const bool MutatableImageNodeConcatenatePair::is_constant() const
{
  return (arg(0).is_constant() || arg(1).is_constant());
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

const bool MutatableImageNodeAdd::is_constant() const
{
  return (arg(0).is_constant() && arg(1).is_constant());
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

const bool MutatableImageNodeMultiply::is_constant() const
{
  return (arg(0).is_constant() && arg(1).is_constant());
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

const bool MutatableImageNodeDivide::is_constant() const
{
  return (arg(0).is_constant() && arg(1).is_constant());
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

const bool MutatableImageNodeCross::is_constant() const
{
  return (arg(0).is_constant() && arg(1).is_constant());
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

const XYZ MutatableImageNodeGeometricInversion::evaluate(const XYZ& p) const
{
  const XYZ origin(arg(0)(p));
  const float radius2(arg(1)(p).magnitude2());

  const XYZ pr(p-origin);
  const float prm(pr.magnitude());
  const float prmi(1.0f/prm);
  const XYZ prn(pr*prmi);

  return origin+prn*radius2*prmi;
}

const bool MutatableImageNodeGeometricInversion::is_constant() const
{
  return false;
}

MutatableImageNodeGeometricInversion::MutatableImageNodeGeometricInversion(const std::vector<MutatableImageNode*>& a)
  :MutatableImageNode(a)
{
  assert(args().size()==2);
}

MutatableImageNodeGeometricInversion::~MutatableImageNodeGeometricInversion()
{}

MutatableImageNode*const MutatableImageNodeGeometricInversion::deepclone() const
{
  return new MutatableImageNodeGeometricInversion(cloneargs());
}

/*******************************************/

const XYZ MutatableImageNodeMax::evaluate(const XYZ& p) const
{
  const XYZ v0(arg(0)(p));
  const XYZ v1(arg(1)(p));
  return XYZ(maximum(v0.x(),v1.x()),maximum(v0.y(),v1.y()),maximum(v0.z(),v1.z()));
}

const bool MutatableImageNodeMax::is_constant() const
{
  return (arg(0).is_constant() && arg(1).is_constant());
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

const bool MutatableImageNodeMin::is_constant() const
{
  return (arg(0).is_constant() && arg(1).is_constant());
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

const bool MutatableImageNodeMod::is_constant() const
{
  return (arg(0).is_constant() && arg(1).is_constant());
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

const bool MutatableImageNodeConcatenateTriple::is_constant() const
{
  return (arg(0).is_constant() || arg(1).is_constant() || arg(2).is_constant());
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

const bool MutatableImageNodeReflect::is_constant() const
{
  return arg(2).is_constant();
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

const bool MutatableImageNodeMagnitudes::is_constant() const
{
  return (arg(0).is_constant() && arg(1).is_constant() && arg(2).is_constant());
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
  if ((arg(0)(p)).magnitude2()<(arg(1)(p)).magnitude2())
    return arg(2)(p);
  else
    return arg(3)(p);
}

const bool MutatableImageNodeChooseSphere::is_constant() const
{
  return (arg(0).is_constant() && arg(1).is_constant() && arg(2).is_constant() && arg(3).is_constant());
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

const XYZ MutatableImageNodeChooseRect::evaluate(const XYZ& p) const
{
  const XYZ p0(arg(0)(p));
  const XYZ p1(arg(1)(p));

  if (p1.origin_centred_rect_contains(p0))
    return arg(2)(p);
  else
    return arg(3)(p);
}

const bool MutatableImageNodeChooseRect::is_constant() const
{
  return (arg(0).is_constant() && arg(1).is_constant() && arg(2).is_constant() && arg(3).is_constant());
}

MutatableImageNodeChooseRect::MutatableImageNodeChooseRect(const std::vector<MutatableImageNode*>& a)
  :MutatableImageNode(a)
{
  assert(args().size()==4);
}

MutatableImageNodeChooseRect::~MutatableImageNodeChooseRect()
{}

MutatableImageNode*const MutatableImageNodeChooseRect::deepclone() const
{
  return new MutatableImageNodeChooseRect(cloneargs());
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

  const XYZ pt(
	       offset
	       +basis_x*p.x()
	       +basis_y*p.y()
	       +basis_z*p.z()
	       );

  return arg(4)(pt);
}

const bool MutatableImageNodePreTransform::is_constant() const
{
  return arg(4).is_constant();
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

const bool MutatableImageNodePostTransform::is_constant() const
{
  return (arg(0).is_constant() && arg(1).is_constant() && arg(2).is_constant() && arg(3).is_constant() && arg(4).is_constant());
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

/*******************************************/

MutatableImageNodeIterative::MutatableImageNodeIterative(const std::vector<MutatableImageNode*>& a,uint i)
  :MutatableImageNode(a)
   ,_iterations(i)
{}

MutatableImageNodeIterative::~MutatableImageNodeIterative()
{}

void MutatableImageNodeIterative::mutate(const MutationParameters& parameters)
{
  // Take care of mutating any branches using base code.
  MutatableImageNode::mutate(parameters);

  if (parameters.r01()<parameters.probability_iterations_change_step())
    {
      if (parameters.r01()<0.5)
	{
	  if (_iterations>1) _iterations--;
	}
      else
	{
	  _iterations++;
	}
    }

  if (parameters.r01()<parameters.probability_iterations_change_jump())
    {
      if (parameters.r01()<0.5)
	{
	  if (_iterations>1) _iterations/=2;
	}
      else
	{
	  _iterations*=2;
	}
    }
}

/*******************************************/

const XYZ MutatableImageNodeIterativeMap::evaluate(const XYZ& p) const
{
  XYZ ret(p);

  for (uint i=0;i<_iterations;i++)
    {
      ret=arg(0)(p);
    }

  return ret;
}

const bool MutatableImageNodeIterativeMap::is_constant() const
{
  // Assumes _iterations>0
  return arg(0).is_constant();
}

MutatableImageNodeIterativeMap::MutatableImageNodeIterativeMap(const std::vector<MutatableImageNode*>& a,uint i)
  :MutatableImageNodeIterative(a,i)
{
  assert (args().size()==1);
}

MutatableImageNodeIterativeMap::~MutatableImageNodeIterativeMap()
{}

MutatableImageNode*const MutatableImageNodeIterativeMap::deepclone() const
{
  return new MutatableImageNodeIterativeMap(cloneargs(),_iterations);
}

/*******************************************/

const XYZ MutatableImageNodeIterativeMapAccumulator::evaluate(const XYZ& p) const
{
  XYZ q(p);
  XYZ ret(0.0f,0.0f,0.0f);

  for (uint i=0;i<_iterations;i++)
    {
      ret+=arg(0)(q);
      q=arg(1)(q);
    }

  return ret/_iterations;
}

const bool MutatableImageNodeIterativeMapAccumulator::is_constant() const
{
  // If arg0 is constant then we get the same value everywhere.  
  // If arg1 is constant we still perform a lookup of arg0 at 2 different places.
  return (arg(0).is_constant());
}

MutatableImageNodeIterativeMapAccumulator::MutatableImageNodeIterativeMapAccumulator(const std::vector<MutatableImageNode*>& a,uint i)
  :MutatableImageNodeIterative(a,i)
{
  assert (args().size()==2);
}

MutatableImageNodeIterativeMapAccumulator::~MutatableImageNodeIterativeMapAccumulator()
{}

MutatableImageNode*const MutatableImageNodeIterativeMapAccumulator::deepclone() const
{
  return new MutatableImageNodeIterativeMapAccumulator(cloneargs(),_iterations);
}

/*******************************************/

/*! Returns i in 0 to _iterations inclusive.  i==_iterations implies "in" set.
 */
const uint MutatableImageNodeIterativeZSquaredPlusC::iterate(const XYZ& z0,const XYZ& c) const
{
  XYZ z(z0);
  uint i;
  for (i=0;i<_iterations;i++)
    {
      const float x2=z.x()*z.x();
      const float y2=z.y()*z.y();
      const float z2=z.z()*z.z();

      if (x2+y2+z2>4.0f)
	break;

      XYZ nz;
      nz.x(x2-y2-z2+c.x());
      nz.y(2.0f*z.x()*z.y()+c.y());
      nz.z(2.0f*z.x()*z.z()+c.z());

      z=nz;
    }

  return i;
}
  
MutatableImageNodeIterativeZSquaredPlusC::MutatableImageNodeIterativeZSquaredPlusC(const std::vector<MutatableImageNode*>& a,uint i)
  :MutatableImageNodeIterative(a,i)
{}

MutatableImageNodeIterativeZSquaredPlusC::~MutatableImageNodeIterativeZSquaredPlusC()
{}


/*******************************************/

const XYZ MutatableImageNodeIterativeMandelbrotChoose::evaluate(const XYZ& p) const
{
  return (iterate(XYZ(0.0f,0.0f,0.0f),p)==_iterations ? arg(0)(p) : arg(1)(p));
}

const bool MutatableImageNodeIterativeMandelbrotChoose::is_constant() const
{
  return false;
}

MutatableImageNodeIterativeMandelbrotChoose::MutatableImageNodeIterativeMandelbrotChoose(const std::vector<MutatableImageNode*>& a,uint i)
  :MutatableImageNodeIterativeZSquaredPlusC(a,i)
{
  assert (args().size()==0);
}

MutatableImageNodeIterativeMandelbrotChoose::~MutatableImageNodeIterativeMandelbrotChoose()
{}

MutatableImageNode*const MutatableImageNodeIterativeMandelbrotChoose::deepclone() const
{
  return new MutatableImageNodeIterativeMandelbrotChoose(cloneargs(),_iterations);
}

/*******************************************/

const XYZ MutatableImageNodeIterativeMandelbrotContour::evaluate(const XYZ& p) const
{
  const uint i=iterate(XYZ(0.0f,0.0f,0.0f),p);
  return (i==_iterations ? XYZ::fill(-1.0f) : XYZ::fill(static_cast<float>(i)/_iterations));
}

const bool MutatableImageNodeIterativeMandelbrotContour::is_constant() const
{
  return false;
}

MutatableImageNodeIterativeMandelbrotContour::MutatableImageNodeIterativeMandelbrotContour(const std::vector<MutatableImageNode*>& a,uint i)
  :MutatableImageNodeIterativeZSquaredPlusC(a,i)
{
  assert (args().size()==2);
}

MutatableImageNodeIterativeMandelbrotContour::~MutatableImageNodeIterativeMandelbrotContour()
{}

MutatableImageNode*const MutatableImageNodeIterativeMandelbrotContour::deepclone() const
{
  return new MutatableImageNodeIterativeMandelbrotContour(cloneargs(),_iterations);
}

/*******************************************/

const XYZ MutatableImageNodeIterativeJuliaChoose::evaluate(const XYZ& p) const
{
  return (iterate(p,arg(0)(p))==_iterations ? arg(1)(p) : arg(2)(p));
}

const bool MutatableImageNodeIterativeJuliaChoose::is_constant() const
{
  return false;
}

MutatableImageNodeIterativeJuliaChoose::MutatableImageNodeIterativeJuliaChoose(const std::vector<MutatableImageNode*>& a,uint i)
  :MutatableImageNodeIterativeZSquaredPlusC(a,i)
{
  assert (args().size()==3);
}

MutatableImageNodeIterativeJuliaChoose::~MutatableImageNodeIterativeJuliaChoose()
{}

MutatableImageNode*const MutatableImageNodeIterativeJuliaChoose::deepclone() const
{
  return new MutatableImageNodeIterativeJuliaChoose(cloneargs(),_iterations);
}

/*******************************************/

const XYZ MutatableImageNodeIterativeJuliaContour::evaluate(const XYZ& p) const
{
  const uint i=iterate(p,arg(0)(p));
  return (i==_iterations ? XYZ::fill(-1.0f) : XYZ::fill(static_cast<float>(i)/_iterations));
}

const bool MutatableImageNodeIterativeJuliaContour::is_constant() const
{
  return false;
}

MutatableImageNodeIterativeJuliaContour::MutatableImageNodeIterativeJuliaContour(const std::vector<MutatableImageNode*>& a,uint i)
  :MutatableImageNodeIterativeZSquaredPlusC(a,i)
{
  assert (args().size()==1);
}

MutatableImageNodeIterativeJuliaContour::~MutatableImageNodeIterativeJuliaContour()
{}

MutatableImageNode*const MutatableImageNodeIterativeJuliaContour::deepclone() const
{
  return new MutatableImageNodeIterativeJuliaContour(cloneargs(),_iterations);
}

