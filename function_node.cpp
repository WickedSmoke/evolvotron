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
  \brief Implementation of class FunctionNode and derived classes.
*/

#include <algorithm>
#include <iostream>

#include "mutatable_image.h"
#include "matrix.h"
#include "function.h"

const std::vector<FunctionNode*> FunctionNode::cloneargs() const
{
  std::vector<FunctionNode*> ret;
  for (std::vector<FunctionNode*>::const_iterator it=args().begin();it!=args().end();it++)
    {
      ret.push_back((*it)->deepclone());
    }
  return ret;
}

const std::vector<float> FunctionNode::cloneparams() const
{
  return params();
}

/*! This returns a random bit of image tree.
  It needs to be capable of generating any sort of node we have.
  \warning Too much probability of highly branching nodes could result in infinite sized stubs.
  \todo Compute (statistically) the expected number of nodes in a stub.
  \todo Don't think FunctionNodeUsing<FunctionPreTransform> appears here ?
 */
FunctionNode* FunctionNode::stub(const MutationParameters& parameters,bool exciting)
{
  // Base mutations are Constant or Identity types.  
  // (Identity can be Identity or PositionTransformed, proportions depending on identity_supression parameter)
  const float base=0.7;

  uint steps=34;

  if (!parameters.allow_fractal_nodes())
    {
      steps=minimum(steps,30u);
    }

  if (!parameters.allow_iterative_nodes())
    {
      steps=minimum(steps,28u);
    }

  const float step=(1.0-base)/steps;

  const float r=(exciting ? base+(1.0f-base)*parameters.r01() : parameters.r01());

  if (r<(1.0f-parameters.proportion_constant())*parameters.identity_supression()*base)
    {
      if (parameters.r01()<0.5f)
	return new FunctionNodeUsing<FunctionTransform>(stubparams(parameters,12),stubargs(parameters,0));
      else
	return new FunctionNodeUsing<FunctionTransformQuadratic>(stubparams(parameters,30),stubargs(parameters,0));	
    }
  else if (r<(1.0f-parameters.proportion_constant())*base)
    return new FunctionNodeUsing<FunctionIdentity>(stubparams(parameters,0),stubargs(parameters,0));
  else if (r<base)
    return new FunctionNodeUsing<FunctionConstant>(stubparams(parameters,3),stubargs(parameters,0));
  else if (r<base+1*step)
    return new FunctionNodeUsing<FunctionCartesianToSpherical>(stubparams(parameters,0),stubargs(parameters,0));
  else if (r<base+2*step) 
    return new FunctionNodeUsing<FunctionSphericalToCartesian>(stubparams(parameters,0),stubargs(parameters,0));
  else if (r<base+3*step) 
    return new FunctionNodeUsing<FunctionEvaluateInSpherical>(stubparams(parameters,0),stubargs(parameters,1));
  else if (r<base+4*step) 
    return new FunctionNodeRotate(stubparams(parameters,0),stubargs(parameters,1));
  else if (r<base+5*step) 
    return new FunctionNodeSin(stubparams(parameters,0),stubargs(parameters,1));
  else if (r<base+6*step) 
    return new FunctionNodeCos(stubparams(parameters,0),stubargs(parameters,1));
  else if (r<base+7*step) 
    return new FunctionNodeSpiralLinear(stubparams(parameters,0),stubargs(parameters,1));
  else if (r<base+8*step) 
    return new FunctionNodeSpiralLogarithmic(stubparams(parameters,0),stubargs(parameters,1));
  else if (r<base+9*step) 
    return new FunctionNodeGrad(stubparams(parameters,0),stubargs(parameters,1));
  else if (r<base+10*step) 
    return new FunctionNodeConcatenatePair(stubparams(parameters,0),stubargs(parameters,2));
  else if (r<base+11*step) 
    return new FunctionNodeAdd(stubparams(parameters,0),stubargs(parameters,2));
  else if (r<base+12*step) 
    return new FunctionNodeMultiply(stubparams(parameters,0),stubargs(parameters,2));
  else if (r<base+13*step) 
    return new FunctionNodeDivide(stubparams(parameters,0),stubargs(parameters,2));
  else if (r<base+14*step) 
    return new FunctionNodeCross(stubparams(parameters,0),stubargs(parameters,2));
  else if (r<base+15*step) 
    return new FunctionNodeGeometricInversion(stubparams(parameters,0),stubargs(parameters,2));
  else if (r<base+16*step) 
    return new FunctionNodeMax(stubparams(parameters,0),stubargs(parameters,2));
  else if (r<base+17*step) 
    return new FunctionNodeMin(stubparams(parameters,0),stubargs(parameters,2));
  else if (r<base+18*step) 
    return new FunctionNodeMod(stubparams(parameters,0),stubargs(parameters,2));
  else if (r<base+19*step) 
    return new FunctionNodeConcatenateTriple(stubparams(parameters,0),stubargs(parameters,3));
  else if (r<base+20*step) 
    return new FunctionNodeReflect(stubparams(parameters,0),stubargs(parameters,3));
  else if (r<base+21*step) 
    return new FunctionNodeMagnitudes(stubparams(parameters,0),stubargs(parameters,3));
  else if (r<base+22*step) 
    return new FunctionNodeChooseSphere(stubparams(parameters,0),stubargs(parameters,4));
  else if (r<base+23*step) 
    return new FunctionNodeChooseRect(stubparams(parameters,0),stubargs(parameters,4));
  else if (r<base+24*step)
    return new FunctionNodeUsing<FunctionTransformGeneralised>(stubparams(parameters,0),stubargs(parameters,4));
  else if (r<base+25*step)
    return new FunctionNodeUsing<FunctionPreTransform>(stubparams(parameters,12),stubargs(parameters,1));
  else if (r<base+26*step)
    return new FunctionNodeUsing<FunctionPreTransformGeneralised>(stubparams(parameters,0),stubargs(parameters,5));
  else if (r<base+27*step)
    return new FunctionNodeUsing<FunctionPostTransform>(stubparams(parameters,12),stubargs(parameters,1));
  else if (r<base+28*step)
    return new FunctionNodeUsing<FunctionPostTransformGeneralised>(stubparams(parameters,0),stubargs(parameters,5));
  else if (r<base+29*step)
    return new FunctionNodeIterativeMap(stubparams(parameters,0),stubargs(parameters,1),1+static_cast<uint>(parameters.r01()*parameters.max_initial_iterations()));
  else if (r<base+30*step)
    return new FunctionNodeIterativeMapAccumulator(stubparams(parameters,0),stubargs(parameters,2),1+static_cast<uint>(parameters.r01()*parameters.max_initial_iterations()));
  else if (r<base+31*step)
    return new FunctionNodeIterativeMandelbrotChoose(stubparams(parameters,0),stubargs(parameters,2),1+static_cast<uint>(parameters.r01()*parameters.max_initial_iterations()));
  else if (r<base+32*step)
    return new FunctionNodeIterativeMandelbrotContour(stubparams(parameters,0),stubargs(parameters,0),1+static_cast<uint>(parameters.r01()*parameters.max_initial_iterations()));
  else if (r<base+33*step)
    return new FunctionNodeIterativeJuliaChoose(stubparams(parameters,0),stubargs(parameters,3),1+static_cast<uint>(parameters.r01()*parameters.max_initial_iterations()));
  else //if (r<base+34*step)
    return new FunctionNodeIterativeJuliaContour(stubparams(parameters,0),stubargs(parameters,1),1+static_cast<uint>(parameters.r01()*parameters.max_initial_iterations()));
}

/*! This returns a vector of random bits of stub, used for initialiing nodes with children. 
 */
const std::vector<FunctionNode*> FunctionNode::stubargs(const MutationParameters& parameters,uint n)
{
  std::vector<FunctionNode*> ret;
  for (uint i=0;i<n;i++)
    ret.push_back(stub(parameters));
  return ret;
}

const std::vector<float> FunctionNode::stubparams(const MutationParameters& parameters,uint n)
{
  std::vector<float> ret;
  for (uint i=0;i<n;i++)
    ret.push_back(-1.0f+2.0f*parameters.r01());
  return ret;
}

FunctionNode::FunctionNode(const std::vector<float>& p,const std::vector<FunctionNode*>& a)
  :_args(a)
  ,_params(p)
{
  assert(ok());
}

/*! Deletes all arguments.  No one else should be referencing nodes except the root node of an image.
 */
FunctionNode::~FunctionNode()
{
  assert(ok());
  for (std::vector<FunctionNode*>::iterator it=args().begin();it!=args().end();it++)
    delete (*it);
}

/*! There are 2 kinds of mutation:
  - random adjustments to constants 
  - structural mutations (messing with the function tree)
  For structural mutations the obvious things to do are:
  - reordering argsuments
  - dropping argsuments and replacing them with new "stubs".
  - duplicating arguments
  - substituting nodes with other types (can't do this for ourself very easily, but we can do it for children)
  - inserting new nodes between children and ourself

  And of course all children have to be mutated too.
 */
void FunctionNode::mutate(const MutationParameters& parameters)
{
  // First mutate all child nodes.
  for (std::vector<FunctionNode*>::iterator it=args().begin();it!=args().end();it++)
    (*it)->mutate(parameters);
  
  // Perturb any parameters we have
  for (std::vector<float>::iterator it=params().begin();it!=params().end();it++)
    (*it)+=parameters.magnitude()*(-1.0f+2.0f*parameters.r01());
  
  // Then go to work on the argument structure...
  
  // Think about glitching some nodes.
  for (std::vector<FunctionNode*>::iterator it=args().begin();it!=args().end();it++)
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
  for (std::vector<FunctionNode*>::iterator it=args().begin();it!=args().end();it++)
    {
      if (parameters.r01()<parameters.probability_insert())
	{
	  std::vector<float> p;
	  std::vector<FunctionNode*> a;

	  a.push_back((*it));
	  a.push_back(stub(parameters));

	  (*it)=new FunctionNodeConcatenatePair(p,a);
	}
    }
}

const FunctionNodeUsing<FunctionPreTransform>*const FunctionNode::is_a_FunctionNodeUsingFunctionPreTransform() const
{
  return 0;
}

FunctionNodeUsing<FunctionPreTransform>*const FunctionNode::is_a_FunctionNodeUsingFunctionPreTransform()
{
  return 0;
}

const bool FunctionNode::ok() const
{
  // Args vector should never contain a null or a non-ok argument
  for (std::vector<FunctionNode*>::const_iterator it=args().begin();it!=args().end();it++)
    {
      if ((*it)==0 || !(*it)->ok())
	return false;
    }
  return true;
}

static Registry registry;

/*******************************************/

const XYZ FunctionNodeRotate::evaluate(const XYZ& p) const
{
  const XYZ a(arg(0)(p)*M_PI);
  
  Matrix33RotateX rx(a.x());
  Matrix33RotateY ry(a.y());
  Matrix33RotateZ rz(a.z());

  return XYZ((rx*ry*rz)*p);
}

const bool FunctionNodeRotate::is_constant() const
{
  return arg(0).is_constant();
}

FunctionNodeRotate::FunctionNodeRotate(const std::vector<float>& p,const std::vector<FunctionNode*>& a)
  :FunctionNode(p,a)
{
  assert(params().size()==0);
  assert(args().size()==1);
}

FunctionNodeRotate::~FunctionNodeRotate()
{}

FunctionNode*const FunctionNodeRotate::deepclone() const
{
  return new FunctionNodeRotate(cloneparams(),cloneargs());
}


/*******************************************/

const XYZ FunctionNodeSin::evaluate(const XYZ& p) const
{
  const XYZ v(arg(0)(p));
  return XYZ(sin(v.x()),sin(v.y()),sin(v.z()));
}

const bool FunctionNodeSin::is_constant() const
{
  return arg(0).is_constant();
}

FunctionNodeSin::FunctionNodeSin(const std::vector<float>& p,const std::vector<FunctionNode*>& a)
  :FunctionNode(p,a)
{
  assert(params().size()==0);
  assert(args().size()==1);
}

FunctionNodeSin::~FunctionNodeSin()
{}

FunctionNode*const FunctionNodeSin::deepclone() const
{
  return new FunctionNodeSin(cloneparams(),cloneargs());
}

/*******************************************/

const XYZ FunctionNodeCos::evaluate(const XYZ& p) const
{
  const XYZ v(arg(0)(p));
  return XYZ(cos(v.x()),cos(v.y()),cos(v.z()));
}

const bool FunctionNodeCos::is_constant() const
{
  return arg(0).is_constant();
}

FunctionNodeCos::FunctionNodeCos(const std::vector<float>& p,const std::vector<FunctionNode*>& a)
  :FunctionNode(p,a)
{
  assert(params().size()==0);
  assert(args().size()==1);
}

FunctionNodeCos::~FunctionNodeCos()
{}

FunctionNode*const FunctionNodeCos::deepclone() const
{
  return new FunctionNodeCos(cloneparams(),cloneargs());
}

/*******************************************/

const XYZ FunctionNodeSpiralLinear::evaluate(const XYZ& p) const
{
  const float r=p.magnitude();
  float theta=atan2(p.y(),p.x());
  if (theta<0.0f) theta+=2.0f*M_PI;
  const float winding=floor(r-theta/(2.0*M_PI));

  const float x=2.0f*winding+theta/M_PI;
  const float y=2.0f*r-x;

  return arg(0)(XYZ(x,y,p.z()));
}

const bool FunctionNodeSpiralLinear::is_constant() const
{
  return (arg(0).is_constant());
}

FunctionNodeSpiralLinear::FunctionNodeSpiralLinear(const std::vector<float>& p,const std::vector<FunctionNode*>& a)
  :FunctionNode(p,a)
{
  assert(params().size()==0);
  assert(args().size()==1);
}

FunctionNodeSpiralLinear::~FunctionNodeSpiralLinear()
{}

FunctionNode*const FunctionNodeSpiralLinear::deepclone() const
{
  return new FunctionNodeSpiralLinear(cloneparams(),cloneargs());
}

/*******************************************/

const XYZ FunctionNodeSpiralLogarithmic::evaluate(const XYZ& p) const
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

const bool FunctionNodeSpiralLogarithmic::is_constant() const
{
  return (arg(0).is_constant());
}

FunctionNodeSpiralLogarithmic::FunctionNodeSpiralLogarithmic(const std::vector<float>& p,const std::vector<FunctionNode*>& a)
  :FunctionNode(p,a)
{
  assert(params().size()==0);
  assert(args().size()==1);
}

FunctionNodeSpiralLogarithmic::~FunctionNodeSpiralLogarithmic()
{}

FunctionNode*const FunctionNodeSpiralLogarithmic::deepclone() const
{
  return new FunctionNodeSpiralLogarithmic(cloneparams(),cloneargs());
}

/*******************************************/

const XYZ FunctionNodeGrad::evaluate(const XYZ& p) const
{
  const float epsilon=1e-6;
  const XYZ vepsilon(epsilon,epsilon,epsilon);

  const XYZ v0(arg(0)(p-vepsilon));
  const XYZ v1(arg(0)(p+vepsilon));
  return (v1-v0)/(2.0*epsilon);
}

const bool FunctionNodeGrad::is_constant() const
{
  return arg(0).is_constant();
}

FunctionNodeGrad::FunctionNodeGrad(const std::vector<float>& p,const std::vector<FunctionNode*>& a)
  :FunctionNode(p,a)
{
  assert(params().size()==0);
  assert(args().size()==1);
}

FunctionNodeGrad::~FunctionNodeGrad()
{}

FunctionNode*const FunctionNodeGrad::deepclone() const
{
  return new FunctionNodeGrad(cloneparams(),cloneargs());
}

/*******************************************/

const XYZ FunctionNodeConcatenatePair::evaluate(const XYZ& p) const
{
  return arg(1)(arg(0)(p));
}

const bool FunctionNodeConcatenatePair::is_constant() const
{
  return (arg(0).is_constant() || arg(1).is_constant());
}

FunctionNodeConcatenatePair::FunctionNodeConcatenatePair(const std::vector<float>& p,const std::vector<FunctionNode*>& a)
  :FunctionNode(p,a)
{
  assert(params().size()==0);
  assert(args().size()==2);
}

FunctionNodeConcatenatePair::~FunctionNodeConcatenatePair()
{}

FunctionNode*const FunctionNodeConcatenatePair::deepclone() const
{
  return new FunctionNodeConcatenatePair(cloneparams(),cloneargs());
}

/*******************************************/

const XYZ FunctionNodeAdd::evaluate(const XYZ& p) const
{
  return arg(0)(p)+arg(1)(p);
}

const bool FunctionNodeAdd::is_constant() const
{
  return (arg(0).is_constant() && arg(1).is_constant());
}

FunctionNodeAdd::FunctionNodeAdd(const std::vector<float>& p,const std::vector<FunctionNode*>& a)
  :FunctionNode(p,a)
{
  assert(params().size()==0);
  assert(args().size()==2);
}

FunctionNodeAdd::~FunctionNodeAdd()
{}

FunctionNode*const FunctionNodeAdd::deepclone() const
{
  return new FunctionNodeAdd(cloneparams(),cloneargs());
}

/*******************************************/

const XYZ FunctionNodeMultiply::evaluate(const XYZ& p) const
{
  return arg(0)(p)*arg(1)(p);
}

const bool FunctionNodeMultiply::is_constant() const
{
  return (arg(0).is_constant() && arg(1).is_constant());
}

FunctionNodeMultiply::FunctionNodeMultiply(const std::vector<float>& p,const std::vector<FunctionNode*>& a)
  :FunctionNode(p,a)
{
  assert(params().size()==0);
  assert(args().size()==2);
}

FunctionNodeMultiply::~FunctionNodeMultiply()
{}

FunctionNode*const FunctionNodeMultiply::deepclone() const
{
  return new FunctionNodeMultiply(cloneparams(),cloneargs());
}

/*******************************************/

const XYZ FunctionNodeDivide::evaluate(const XYZ& p) const
{
  const XYZ v0(arg(0)(p));
  const XYZ v1(arg(1)(p));

  return XYZ(
	     (v1.x()==0.0 ? 0.0 : v0.x()/v1.x()),
	     (v1.y()==0.0 ? 0.0 : v0.y()/v1.y()),
	     (v1.z()==0.0 ? 0.0 : v0.z()/v1.z())
	     );
}

const bool FunctionNodeDivide::is_constant() const
{
  return (arg(0).is_constant() && arg(1).is_constant());
}

FunctionNodeDivide::FunctionNodeDivide(const std::vector<float>& p,const std::vector<FunctionNode*>& a)
  :FunctionNode(p,a)
{
  assert(params().size()==0);
  assert(args().size()==2);
}

FunctionNodeDivide::~FunctionNodeDivide()
{}

FunctionNode*const FunctionNodeDivide::deepclone() const
{
  return new FunctionNodeDivide(cloneparams(),cloneargs());
}

/*******************************************/

const XYZ FunctionNodeCross::evaluate(const XYZ& p) const
{
  return arg(0)(p)*arg(1)(p);
}

const bool FunctionNodeCross::is_constant() const
{
  return (arg(0).is_constant() && arg(1).is_constant());
}

FunctionNodeCross::FunctionNodeCross(const std::vector<float>& p,const std::vector<FunctionNode*>& a)
  :FunctionNode(p,a)
{
  assert(params().size()==0);
  assert(args().size()==2);
}

FunctionNodeCross::~FunctionNodeCross()
{}

FunctionNode*const FunctionNodeCross::deepclone() const
{
  return new FunctionNodeCross(cloneparams(),cloneargs());
}

/*******************************************/

const XYZ FunctionNodeGeometricInversion::evaluate(const XYZ& p) const
{
  const XYZ origin(arg(0)(p));
  const float radius2(arg(1)(p).magnitude2());

  const XYZ pr(p-origin);
  const float prm(pr.magnitude());
  const float prmi(1.0f/prm);
  const XYZ prn(pr*prmi);

  return origin+prn*radius2*prmi;
}

const bool FunctionNodeGeometricInversion::is_constant() const
{
  return false;
}

FunctionNodeGeometricInversion::FunctionNodeGeometricInversion(const std::vector<float>& p,const std::vector<FunctionNode*>& a)
  :FunctionNode(p,a)
{
  assert(params().size()==0);
  assert(args().size()==2);
}

FunctionNodeGeometricInversion::~FunctionNodeGeometricInversion()
{}

FunctionNode*const FunctionNodeGeometricInversion::deepclone() const
{
  return new FunctionNodeGeometricInversion(cloneparams(),cloneargs());
}

/*******************************************/

const XYZ FunctionNodeMax::evaluate(const XYZ& p) const
{
  const XYZ v0(arg(0)(p));
  const XYZ v1(arg(1)(p));
  return XYZ(maximum(v0.x(),v1.x()),maximum(v0.y(),v1.y()),maximum(v0.z(),v1.z()));
}

const bool FunctionNodeMax::is_constant() const
{
  return (arg(0).is_constant() && arg(1).is_constant());
}

FunctionNodeMax::FunctionNodeMax(const std::vector<float>& p,const std::vector<FunctionNode*>& a)
  :FunctionNode(p,a)
{
  assert(params().size()==0);
  assert(args().size()==2);
}

FunctionNodeMax::~FunctionNodeMax()
{}

FunctionNode*const FunctionNodeMax::deepclone() const
{
  return new FunctionNodeMax(cloneparams(),cloneargs());
}

/*******************************************/

const XYZ FunctionNodeMin::evaluate(const XYZ& p) const
{
  const XYZ v0(arg(0)(p));
  const XYZ v1(arg(1)(p));
  return XYZ(minimum(v0.x(),v1.x()),minimum(v0.y(),v1.y()),minimum(v0.z(),v1.z()));
}

const bool FunctionNodeMin::is_constant() const
{
  return (arg(0).is_constant() && arg(1).is_constant());
}

FunctionNodeMin::FunctionNodeMin(const std::vector<float>& p,const std::vector<FunctionNode*>& a)
  :FunctionNode(p,a)
{
  assert(params().size()==0);
  assert(args().size()==2);
}

FunctionNodeMin::~FunctionNodeMin()
{}

FunctionNode*const FunctionNodeMin::deepclone() const
{
  return new FunctionNodeMin(cloneparams(),cloneargs());
}

/*******************************************/

const XYZ FunctionNodeMod::evaluate(const XYZ& p) const
{
  const XYZ a(arg(0)(p));
  const XYZ b(arg(1)(p));
  
  return XYZ(fmod(a.x(),b.x()),fmod(a.y(),b.y()),fmod(a.z(),b.z()));
}

const bool FunctionNodeMod::is_constant() const
{
  return (arg(0).is_constant() && arg(1).is_constant());
}

FunctionNodeMod::FunctionNodeMod(const std::vector<float>& p,const std::vector<FunctionNode*>& a)
  :FunctionNode(p,a)
{
  assert(params().size()==0);
  assert(args().size()==2);
}

FunctionNodeMod::~FunctionNodeMod()
{}

FunctionNode*const FunctionNodeMod::deepclone() const
{
  return new FunctionNodeMod(cloneparams(),cloneargs());
}

/*******************************************/

const XYZ FunctionNodeConcatenateTriple::evaluate(const XYZ& p) const
{
  return arg(2)(arg(1)(arg(0)(p)));
}

const bool FunctionNodeConcatenateTriple::is_constant() const
{
  return (arg(0).is_constant() || arg(1).is_constant() || arg(2).is_constant());
}

FunctionNodeConcatenateTriple::FunctionNodeConcatenateTriple(const std::vector<float>& p,const std::vector<FunctionNode*>& a)
  :FunctionNode(p,a)
{
  assert(params().size()==0);
  assert(args().size()==3);
}

FunctionNodeConcatenateTriple::~FunctionNodeConcatenateTriple()
{}

FunctionNode*const FunctionNodeConcatenateTriple::deepclone() const
{
  return new FunctionNodeConcatenateTriple(cloneparams(),cloneargs());
}

/*******************************************/

const XYZ FunctionNodeReflect::evaluate(const XYZ& p) const
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

const bool FunctionNodeReflect::is_constant() const
{
  return arg(2).is_constant();
}

FunctionNodeReflect::FunctionNodeReflect(const std::vector<float>& p,const std::vector<FunctionNode*>& a)
  :FunctionNode(p,a)
{
  assert(params().size()==0);
  assert(args().size()==3);
}

FunctionNodeReflect::~FunctionNodeReflect()
{}

FunctionNode*const FunctionNodeReflect::deepclone() const
{
  return new FunctionNodeReflect(cloneparams(),cloneargs());
}

/*******************************************/

const XYZ FunctionNodeMagnitudes::evaluate(const XYZ& p) const
{
  return XYZ(arg(0)(p).magnitude(),arg(1)(p).magnitude(),arg(2)(p).magnitude());
}

const bool FunctionNodeMagnitudes::is_constant() const
{
  return (arg(0).is_constant() && arg(1).is_constant() && arg(2).is_constant());
}

FunctionNodeMagnitudes::FunctionNodeMagnitudes(const std::vector<float>& p,const std::vector<FunctionNode*>& a)
  :FunctionNode(p,a)
{
  assert(params().size()==0);
  assert(args().size()==3);
}

FunctionNodeMagnitudes::~FunctionNodeMagnitudes()
{}

FunctionNode*const FunctionNodeMagnitudes::deepclone() const
{
  return new FunctionNodeMagnitudes(cloneparams(),cloneargs());
}

/*******************************************/

const XYZ FunctionNodeChooseSphere::evaluate(const XYZ& p) const
{
  if ((arg(0)(p)).magnitude2()<(arg(1)(p)).magnitude2())
    return arg(2)(p);
  else
    return arg(3)(p);
}

const bool FunctionNodeChooseSphere::is_constant() const
{
  return (arg(0).is_constant() && arg(1).is_constant() && arg(2).is_constant() && arg(3).is_constant());
}

FunctionNodeChooseSphere::FunctionNodeChooseSphere(const std::vector<float>& p,const std::vector<FunctionNode*>& a)
  :FunctionNode(p,a)
{
  assert(params().size()==0);
  assert(args().size()==4);
}

FunctionNodeChooseSphere::~FunctionNodeChooseSphere()
{}

FunctionNode*const FunctionNodeChooseSphere::deepclone() const
{
  return new FunctionNodeChooseSphere(cloneparams(),cloneargs());
}

/*******************************************/

const XYZ FunctionNodeChooseRect::evaluate(const XYZ& p) const
{
  const XYZ p0(arg(0)(p));
  const XYZ p1(arg(1)(p));

  if (p1.origin_centred_rect_contains(p0))
    return arg(2)(p);
  else
    return arg(3)(p);
}

const bool FunctionNodeChooseRect::is_constant() const
{
  return (arg(0).is_constant() && arg(1).is_constant() && arg(2).is_constant() && arg(3).is_constant());
}

FunctionNodeChooseRect::FunctionNodeChooseRect(const std::vector<float>& p,const std::vector<FunctionNode*>& a)
  :FunctionNode(p,a)
{
  assert(params().size()==0);
  assert(args().size()==4);
}

FunctionNodeChooseRect::~FunctionNodeChooseRect()
{}

FunctionNode*const FunctionNodeChooseRect::deepclone() const
{
  return new FunctionNodeChooseRect(cloneparams(),cloneargs());
}

/*******************************************/

FunctionNodeIterative::FunctionNodeIterative(const std::vector<float>& p,const std::vector<FunctionNode*>& a,uint i)
  :FunctionNode(p,a)
   ,_iterations(i)
{}

FunctionNodeIterative::~FunctionNodeIterative()
{}

void FunctionNodeIterative::mutate(const MutationParameters& parameters)
{
  // Take care of mutating any branches using base class code.
  FunctionNode::mutate(parameters);

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

const XYZ FunctionNodeIterativeMap::evaluate(const XYZ& p) const
{
  XYZ ret(p);

  for (uint i=0;i<_iterations;i++)
    {
      ret=arg(0)(p);
    }

  return ret;
}

const bool FunctionNodeIterativeMap::is_constant() const
{
  // Assumes _iterations>0
  return arg(0).is_constant();
}

FunctionNodeIterativeMap::FunctionNodeIterativeMap(const std::vector<float>& p,const std::vector<FunctionNode*>& a,uint i)
  :FunctionNodeIterative(p,a,i)
{
  assert(params().size()==0);
  assert(args().size()==1);
}

FunctionNodeIterativeMap::~FunctionNodeIterativeMap()
{}

FunctionNode*const FunctionNodeIterativeMap::deepclone() const
{
  return new FunctionNodeIterativeMap(cloneparams(),cloneargs(),_iterations);
}

/*******************************************/

const XYZ FunctionNodeIterativeMapAccumulator::evaluate(const XYZ& p) const
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

const bool FunctionNodeIterativeMapAccumulator::is_constant() const
{
  // If arg0 is constant then we get the same value everywhere.  
  // If arg1 is constant we still perform a lookup of arg0 at 2 different places.
  return (arg(0).is_constant());
}

FunctionNodeIterativeMapAccumulator::FunctionNodeIterativeMapAccumulator(const std::vector<float>& p,const std::vector<FunctionNode*>& a,uint i)
  :FunctionNodeIterative(p,a,i)
{
  assert(params().size()==0);
  assert(args().size()==2);
}

FunctionNodeIterativeMapAccumulator::~FunctionNodeIterativeMapAccumulator()
{}

FunctionNode*const FunctionNodeIterativeMapAccumulator::deepclone() const
{
  return new FunctionNodeIterativeMapAccumulator(cloneparams(),cloneargs(),_iterations);
}

/*******************************************/

/*! Returns i in 0 to _iterations inclusive.  i==_iterations implies "in" set.
 */
const uint FunctionNodeIterativeZSquaredPlusC::iterate(const XYZ& z0,const XYZ& c) const
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
  
FunctionNodeIterativeZSquaredPlusC::FunctionNodeIterativeZSquaredPlusC(const std::vector<float>& p,const std::vector<FunctionNode*>& a,uint i)
  :FunctionNodeIterative(p,a,i)
{}

FunctionNodeIterativeZSquaredPlusC::~FunctionNodeIterativeZSquaredPlusC()
{}


/*******************************************/

const XYZ FunctionNodeIterativeMandelbrotChoose::evaluate(const XYZ& p) const
{
  return (iterate(XYZ(0.0f,0.0f,0.0f),p)==_iterations ? arg(0)(p) : arg(1)(p));
}

const bool FunctionNodeIterativeMandelbrotChoose::is_constant() const
{
  return false;
}

FunctionNodeIterativeMandelbrotChoose::FunctionNodeIterativeMandelbrotChoose(const std::vector<float>& p,const std::vector<FunctionNode*>& a,uint i)
  :FunctionNodeIterativeZSquaredPlusC(p,a,i)
{
  assert(params().size()==0);
  assert (args().size()==2);
}

FunctionNodeIterativeMandelbrotChoose::~FunctionNodeIterativeMandelbrotChoose()
{}

FunctionNode*const FunctionNodeIterativeMandelbrotChoose::deepclone() const
{
  return new FunctionNodeIterativeMandelbrotChoose(cloneparams(),cloneargs(),_iterations);
}

/*******************************************/

const XYZ FunctionNodeIterativeMandelbrotContour::evaluate(const XYZ& p) const
{
  const uint i=iterate(XYZ(0.0f,0.0f,0.0f),p);
  return (i==_iterations ? XYZ::fill(-1.0f) : XYZ::fill(static_cast<float>(i)/_iterations));
}

const bool FunctionNodeIterativeMandelbrotContour::is_constant() const
{
  return false;
}

FunctionNodeIterativeMandelbrotContour::FunctionNodeIterativeMandelbrotContour(const std::vector<float>& p,const std::vector<FunctionNode*>& a,uint i)
  :FunctionNodeIterativeZSquaredPlusC(p,a,i)
{
  assert(params().size()==0);
  assert (args().size()==2);
}

FunctionNodeIterativeMandelbrotContour::~FunctionNodeIterativeMandelbrotContour()
{}

FunctionNode*const FunctionNodeIterativeMandelbrotContour::deepclone() const
{
  return new FunctionNodeIterativeMandelbrotContour(cloneparams(),cloneargs(),_iterations);
}

/*******************************************/

const XYZ FunctionNodeIterativeJuliaChoose::evaluate(const XYZ& p) const
{
  return (iterate(p,arg(0)(p))==_iterations ? arg(1)(p) : arg(2)(p));
}

const bool FunctionNodeIterativeJuliaChoose::is_constant() const
{
  return false;
}

FunctionNodeIterativeJuliaChoose::FunctionNodeIterativeJuliaChoose(const std::vector<float>& p,const std::vector<FunctionNode*>& a,uint i)
  :FunctionNodeIterativeZSquaredPlusC(p,a,i)
{
  assert(params().size()==0);
  assert(args().size()==3);
}

FunctionNodeIterativeJuliaChoose::~FunctionNodeIterativeJuliaChoose()
{}

FunctionNode*const FunctionNodeIterativeJuliaChoose::deepclone() const
{
  return new FunctionNodeIterativeJuliaChoose(cloneparams(),cloneargs(),_iterations);
}

/*******************************************/

const XYZ FunctionNodeIterativeJuliaContour::evaluate(const XYZ& p) const
{
  const uint i=iterate(p,arg(0)(p));
  return (i==_iterations ? XYZ::fill(-1.0f) : XYZ::fill(static_cast<float>(i)/_iterations));
}

const bool FunctionNodeIterativeJuliaContour::is_constant() const
{
  return false;
}

FunctionNodeIterativeJuliaContour::FunctionNodeIterativeJuliaContour(const std::vector<float>& p,const std::vector<FunctionNode*>& a,uint i)
  :FunctionNodeIterativeZSquaredPlusC(p,a,i)
{
  assert(params().size()==0);
  assert (args().size()==1);
}

FunctionNodeIterativeJuliaContour::~FunctionNodeIterativeJuliaContour()
{}

FunctionNode*const FunctionNodeIterativeJuliaContour::deepclone() const
{
  return new FunctionNodeIterativeJuliaContour(cloneparams(),cloneargs(),_iterations);
}

