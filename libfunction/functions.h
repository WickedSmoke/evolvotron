// Source file for evolvotron
// Copyright (C) 2002,2003,2004,2005,2007 Tim Day
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
  \brief Interfaces and implementation for specific Function classes.
  As much as possible of the implementation should be pushed into the FunctionBoilerplate template.
*/

#ifndef _functions_h_
#define _functions_h_

#include "useful.h"
#include "function_registry.h"
#include "function_boilerplate.h"

//------------------------------------------------------------------------------------------

// Strip of one function across another
FUNCTION_BEGIN(FunctionStrip,3,3,false,FnStructure)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      if (fabs(p.y()) > fabs(arg(2)(p)%XYZ(param(0),param(1),param(2)))) return arg(1)(p);
      else return arg(0)(p);
    }
  
FUNCTION_END(FunctionStrip)

//------------------------------------------------------------------------------------------

// Strip of one function across another
FUNCTION_BEGIN(FunctionStripBlend,6,4,false,FnStructure)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      const real r0=fabs(arg(2)(p)%XYZ(param(0),param(1),param(2)));
      const real r1=fabs(arg(3)(p)%XYZ(param(3),param(4),param(5)));
      const real inner=std::min(r0,r1);
      const real outer=std::max(r0,r1);

      const real ay=fabs(p.y());
      if (ay<=inner) return arg(0)(p);
      if (ay>=outer) return arg(1)(p);
      
      const XYZ v0(arg(0)(p));
      const XYZ v1(arg(1)(p));

      return v0+(v1-v0)*(ay-inner)/(outer-inner);
    }
  
FUNCTION_END(FunctionStripBlend)


//------------------------------------------------------------------------------------------

FUNCTION_BEGIN(FunctionFriezeGroupJump,1,1,false,FnStructure)

  virtual const XYZ evaluate(const XYZ& p) const
    {
      const real x=modulusf(p.x(),1.0);
      const real y=fabs(p.y());
      return arg(0)(XYZ(x,y,param(0)));
    }
  
FUNCTION_END(FunctionFriezeGroupJump)

//------------------------------------------------------------------------------------------

FUNCTION_BEGIN(FunctionFriezeGroupJumpWarped,4,2,false,FnStructure)

  virtual const XYZ evaluate(const XYZ& p) const
    {
      const real dx=XYZ(param(0),param(1),param(2))%arg(1)(XYZ(0.0,fabs(p.y()),0.0));
      const real x=modulusf(p.x()-dx,1.0)+dx;
      const real y=fabs(p.y());
      return arg(0)(XYZ(x,y,param(3)));
    }

  virtual const bool is_constant() const
    {
      return arg(0).is_constant();  // Warp function makes no difference if primary argument is constant
    }
  
FUNCTION_END(FunctionFriezeGroupJumpWarped)

//------------------------------------------------------------------------------------------

FUNCTION_BEGIN(FunctionFriezeGroupJumpFreeZ,0,1,false,FnStructure)

  virtual const XYZ evaluate(const XYZ& p) const
    {
      const real x=modulusf(p.x(),1.0);
      const real y=fabs(p.y());
      return arg(0)(XYZ(x,y,p.z()));
    }
  
FUNCTION_END(FunctionFriezeGroupJumpFreeZ)

//------------------------------------------------------------------------------------------

FUNCTION_BEGIN(FunctionFriezeGroupJumpWarpedFreeZ,3,2,false,FnStructure)

  virtual const XYZ evaluate(const XYZ& p) const
    {
      const real dx=XYZ(param(0),param(1),param(2))%arg(1)(XYZ(0.0,fabs(p.y()),0.0));
      const real x=modulusf(p.x()-dx,1.0)+dx;
      const real y=fabs(p.y());
      return arg(0)(XYZ(x,y,p.z()));
    }
  
  virtual const bool is_constant() const
    {
      return arg(0).is_constant();  // Warp function makes no difference if primary argument is constant
    }

FUNCTION_END(FunctionFriezeGroupJumpWarpedFreeZ)

//------------------------------------------------------------------------------------------

FUNCTION_BEGIN(FunctionFriezeGroupSidle,1,1,false,FnStructure)
  // Don't think this form can be warped without breaking symmetry

  virtual const XYZ evaluate(const XYZ& p) const
    {
      const real x=trianglef(p.x(),1.0);
      const real y=p.y();
      return arg(0)(XYZ(x,y,param(0)));
    }
  
FUNCTION_END(FunctionFriezeGroupSidle)

//------------------------------------------------------------------------------------------

FUNCTION_BEGIN(FunctionFriezeGroupSidleFreeZ,0,1,false,FnStructure)
  // Don't think this form can be warped without breaking symmetry

  virtual const XYZ evaluate(const XYZ& p) const
    {
      const real x=trianglef(p.x(),1.0);
      const real y=p.y();
      return arg(0)(XYZ(x,y,p.z()));
    }
  
FUNCTION_END(FunctionFriezeGroupSidleFreeZ)

//------------------------------------------------------------------------------------------

FUNCTION_BEGIN(FunctionFriezeGroupStep,1,1,false,FnStructure)

  virtual const XYZ evaluate(const XYZ& p) const
    {
      const real x=(p.y()>0.0 ? modulusf(p.x(),1.0) : modulusf(p.x()+0.5,1.0));
      const real y=fabs(p.y());
      return arg(0)(XYZ(x,y,param(0)));
    }
  
FUNCTION_END(FunctionFriezeGroupStep)

//------------------------------------------------------------------------------------------

FUNCTION_BEGIN(FunctionFriezeGroupStepWarped,7,3,false,FnStructure)

  virtual const XYZ evaluate(const XYZ& p) const
    {
      const real dx=fabs(p.y())*(XYZ(param(0),param(1),param(2))%arg(2)(XYZ(0.0,fabs(p.y()),0.0)));
      const real wx1=modulusf(p.x()-dx,1.0);
      const real wx2=modulusf(p.x()-dx,0.5);
      const real dy=(wx1>0.5 ? 1.0 : -1.0)*(XYZ(param(3),param(4),param(5))%arg(1)(XYZ(wx2,0.0,0.0)));
      const bool upper_domain=(p.y()>dy);
      real x;
      real y;
      if (upper_domain)
	{
	  x=wx1+dx;
	  y=p.y();
	}
      else
	{
	  x=modulusf(p.x()+0.5-dx,1.0)+dx;
	  y=-p.y();
	}
      return arg(0)(XYZ(x,y,param(6)));
    }

  virtual const bool is_constant() const
    {
      return arg(0).is_constant();  // Warp function makes no difference if primary argument is constant
    }

FUNCTION_END(FunctionFriezeGroupStepWarped)

//------------------------------------------------------------------------------------------

FUNCTION_BEGIN(FunctionFriezeGroupStepFreeZ,0,1,false,FnStructure)

  virtual const XYZ evaluate(const XYZ& p) const
    {
      const real x=(p.y()>0.0 ? modulusf(p.x(),1.0) : modulusf(p.x()+0.5,1.0));
      const real y=fabs(p.y());
      return arg(0)(XYZ(x,y,p.z()));
    }
  
FUNCTION_END(FunctionFriezeGroupStepFreeZ)

//------------------------------------------------------------------------------------------

FUNCTION_BEGIN(FunctionFriezeGroupStepWarpedFreeZ,6,3,false,FnStructure)

  virtual const XYZ evaluate(const XYZ& p) const
    {
      const real dx=fabs(p.y())*(XYZ(param(0),param(1),param(2))%arg(2)(XYZ(0.0,fabs(p.y()),0.0)));
      const real wx1=modulusf(p.x()-dx,1.0);
      const real wx2=modulusf(p.x()-dx,0.5);
      const real dy=(wx1>0.5 ? 1.0 : -1.0)*(XYZ(param(3),param(4),param(5))%arg(1)(XYZ(wx2,0.0,0.0)));
      const bool upper_domain=(p.y()>dy);
      real x;
      real y;
      if (upper_domain)
	{
	  x=wx1+dx;
	  y=p.y();
	}
      else
	{
	  x=modulusf(p.x()+0.5-dx,1.0)+dx;
	  y=-p.y();
	}
      return arg(0)(XYZ(x,y,p.z()));
    }

  virtual const bool is_constant() const
    {
      return arg(0).is_constant();  // Warp function makes no difference if primary argument is constant
    }

FUNCTION_END(FunctionFriezeGroupStepWarpedFreeZ)

//------------------------------------------------------------------------------------------

FUNCTION_BEGIN(FunctionFriezeGroupSpinjump,1,1,false,FnStructure)
  // Don't think this form can be warped without breaking symmetry

  virtual const XYZ evaluate(const XYZ& p) const
    {
      const real x=trianglef(p.x(),1.0);
      const real y=fabs(p.y());
      return arg(0)(XYZ(x,y,param(0)));
    }
  
FUNCTION_END(FunctionFriezeGroupSpinjump)

//------------------------------------------------------------------------------------------

FUNCTION_BEGIN(FunctionFriezeGroupSpinjumpFreeZ,0,1,false,FnStructure)
  // Don't think this form can be warped without breaking symmetry

  virtual const XYZ evaluate(const XYZ& p) const
    {
      const real x=trianglef(p.x(),1.0);
      const real y=fabs(p.y());
      return arg(0)(XYZ(x,y,p.z()));
    }
  
FUNCTION_END(FunctionFriezeGroupSpinjumpFreeZ)

//------------------------------------------------------------------------------------------

FUNCTION_BEGIN(FunctionFriezeGroupSpinsidle,1,1,false,FnStructure)

  virtual const XYZ evaluate(const XYZ& p) const
    {
      const real x=trianglef(p.x(),0.5);
      const real y=(modulusf(p.x()+0.5,2.0)<1.0 ? p.y() : -p.y());
      return arg(0)(XYZ(x,y,param(0)));
    }
  
FUNCTION_END(FunctionFriezeGroupSpinsidle)

//------------------------------------------------------------------------------------------

FUNCTION_BEGIN(FunctionFriezeGroupSpinsidleWarped,4,2,false,FnStructure)

  virtual const XYZ evaluate(const XYZ& p) const
    {
      const real dx=p.y()*(XYZ(param(0),param(1),param(2))%arg(1)(XYZ(0.0,fabs(p.y()),0.0)));
      const bool flipped_unit=(modulusf(p.x(),2.0)>1.0);
      const bool flipped_element=(modulusf(p.x(),1.0)>0.5+(flipped_unit ? -dx : dx));

      real x;
      real y;
      if (flipped_unit)
	{
	  if (flipped_element) {x=modulusf(1.0-p.x(),1.0);y=p.y();}
	  else {x=modulusf(p.x(),1.0);y=-p.y();}
	}
      else
	{
	  if (flipped_element) {x=modulusf(1.0-p.x(),1.0);y=-p.y();}
	  else {x=modulusf(p.x(),1.0);y=p.y();}
	}

      return arg(0)(XYZ(x,y,param(3)));
    }

  virtual const bool is_constant() const
    {
      return arg(0).is_constant();  // Warp function makes no difference if primary argument is constant
    }
  
FUNCTION_END(FunctionFriezeGroupSpinsidleWarped)

//------------------------------------------------------------------------------------------

FUNCTION_BEGIN(FunctionFriezeGroupSpinsidleFreeZ,0,1,false,FnStructure)

  virtual const XYZ evaluate(const XYZ& p) const
    {
      const real x=trianglef(p.x(),0.5);
      const real y=(modulusf(p.x()+0.5,2.0)<1.0 ? p.y() : -p.y());
      return arg(0)(XYZ(x,y,p.z()));
    }
  
FUNCTION_END(FunctionFriezeGroupSpinsidleFreeZ)

//------------------------------------------------------------------------------------------

FUNCTION_BEGIN(FunctionFriezeGroupSpinsidleWarpedFreeZ,3,2,false,FnStructure)

  virtual const XYZ evaluate(const XYZ& p) const
    {
      const real dx=p.y()*(XYZ(param(0),param(1),param(2))%arg(1)(XYZ(0.0,fabs(p.y()),0.0)));
      const bool flipped_unit=(modulusf(p.x(),2.0)>1.0);
      const bool flipped_element=(modulusf(p.x(),1.0)>0.5+(flipped_unit ? -dx : dx));

      real x;
      real y;
      if (flipped_unit)
	{
	  if (flipped_element) {x=modulusf(1.0-p.x(),1.0);y=p.y();}
	  else {x=modulusf(p.x(),1.0);y=-p.y();}
	}
      else
	{
	  if (flipped_element) {x=modulusf(1.0-p.x(),1.0);y=-p.y();}
	  else {x=modulusf(p.x(),1.0);y=p.y();}
	}

      return arg(0)(XYZ(x,y,p.z()));
    }

  virtual const bool is_constant() const
    {
      return arg(0).is_constant();  // Warp function makes no difference if primary argument is constant
    }
  
FUNCTION_END(FunctionFriezeGroupSpinsidleWarpedFreeZ)

//------------------------------------------------------------------------------------------

//! Function implements selection between 2 functions based on the relative magnitudes of 2 other functions
FUNCTION_BEGIN(FunctionChooseSphere,0,4,false,FnStructure)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      if ((arg(0)(p)).magnitude2()<(arg(1)(p)).magnitude2())
	return arg(2)(p);
      else
	return arg(3)(p);
    }
  
FUNCTION_END(FunctionChooseSphere)

//------------------------------------------------------------------------------------------

//! Function implements selection between 2 functions based on whether a rectangle contains a point
FUNCTION_BEGIN(FunctionChooseRect,0,4,false,FnStructure)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      const XYZ p0(arg(0)(p));
      const XYZ p1(arg(1)(p));
      
      if (p1.origin_centred_rect_contains(p0))
	return arg(2)(p);
      else
	return arg(3)(p);
    }
  
FUNCTION_END(FunctionChooseRect)

//------------------------------------------------------------------------------------------

//! Function implements selection between 2 functions based on position in 3d mesh
FUNCTION_BEGIN(FunctionChooseFrom2InCubeMesh,0,2,false,FnStructure)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      const int x=static_cast<int>(floorf(p.x()));
      const int y=static_cast<int>(floorf(p.y()));
      const int z=static_cast<int>(floorf(p.z()));

      if ((x+y+z)&1)
	return arg(0)(p);
      else
	return arg(1)(p);
    }
  
FUNCTION_END(FunctionChooseFrom2InCubeMesh);

//------------------------------------------------------------------------------------------

//! Function implements selection between 2 functions based on position in 3d mesh
FUNCTION_BEGIN(FunctionChooseFrom3InCubeMesh,0,3,false,FnStructure)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      const int x=static_cast<int>(floorf(p.x()));
      const int y=static_cast<int>(floorf(p.y()));
      const int z=static_cast<int>(floorf(p.z()));

      return arg(modulusi(x+y+z,3))(p);
    }
  
FUNCTION_END(FunctionChooseFrom3InCubeMesh)

//------------------------------------------------------------------------------------------

//! Function implements selection between 2 functions based on position in 2d grid
FUNCTION_BEGIN(FunctionChooseFrom2InSquareGrid,0,2,false,FnStructure)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      const int x=static_cast<int>(floorf(p.x()));
      const int y=static_cast<int>(floorf(p.y()));

      if ((x+y)&1)
	return arg(0)(p);
      else
	return arg(1)(p);
    }
  
FUNCTION_END(FunctionChooseFrom2InSquareGrid)

//------------------------------------------------------------------------------------------

//! Function implements selection between 3 functions based on position in 2d grid
FUNCTION_BEGIN(FunctionChooseFrom3InSquareGrid,0,3,false,FnStructure)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      const int x=static_cast<int>(floorf(p.x()));
      const int y=static_cast<int>(floorf(p.y()));

      return arg(modulusi(x+y,3))(p);
    }
  
FUNCTION_END(FunctionChooseFrom3InSquareGrid)

//------------------------------------------------------------------------------------------

//! Function implements selection between 2 functions based on position in grid of triangles 
FUNCTION_BEGIN(FunctionChooseFrom2InTriangleGrid,0,2,false,FnStructure)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      static const XYZ d0(1.0         ,0.0         ,0.0);
      static const XYZ d1(cos(  M_PI/3),sin(  M_PI/3),0.0);
      static const XYZ d2(cos(2*M_PI/3),sin(2*M_PI/3),0.0);
      
      const int a=static_cast<int>(floorf(p%d0));
      const int b=static_cast<int>(floorf(p%d1));
      const int c=static_cast<int>(floorf(p%d2));

      if ((a+b+c)&1)
	return arg(0)(p);
      else
	return arg(1)(p);
    }
  
FUNCTION_END(FunctionChooseFrom2InTriangleGrid)

//------------------------------------------------------------------------------------------

//! Function implements selection between 2 functions based on position in grid of triangles 
/*! Not entirely sure this one produces a sensible pattern.  Needs explicitly testing.
 */
FUNCTION_BEGIN(FunctionChooseFrom3InTriangleGrid,0,3,false,FnStructure)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      static const XYZ d0(1.0         ,0.0         ,0.0);
      static const XYZ d1(cos(  M_PI/3),sin(  M_PI/3),0.0);
      static const XYZ d2(cos(2*M_PI/3),sin(2*M_PI/3),0.0);
      
      const int a=static_cast<int>(floorf(p%d0));
      const int b=static_cast<int>(floorf(p%d1));
      const int c=static_cast<int>(floorf(p%d2));

      return arg(modulusi(a+b+c,3))(p);
    }
  
FUNCTION_END(FunctionChooseFrom3InTriangleGrid)

//------------------------------------------------------------------------------------------

//! Function implements selection between 3 functions based on position in grid of hexagons
/*! Don't entirely understand how this works, but it looks nice.
 */
FUNCTION_BEGIN(FunctionChooseFrom3InDiamondGrid,0,3,false,FnStructure)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      // Basis vectors for hex grid
      static const XYZ d0(1.0         ,0.0         ,0.0);
      static const XYZ d1(cos(  M_PI/3),sin(  M_PI/3),0.0);
      static const XYZ d2(cos(2*M_PI/3),sin(2*M_PI/3),0.0);
      
      // Dot with basis
      const real p0=p%d0;
      const real p1=p%d1;
      const real p2=p%d2;

      // Find nearest on-grid point
      const int i0=(int)floorf(p0+0.5);
      const int i1=(int)floorf(p1+0.5);
      const int i2=(int)floorf(p2+0.5);

      // Measure distance
      const real m0=fabsf(p0-i0);
      const real m1=fabsf(p1-i1);
      const real m2=fabsf(p2-i2);

      // Closest one decides which function
      if (m0<=m1 && m0<=m2)
	return arg(0)(p);
      else if (m1<=m0 && m1<=m2)
	return arg(1)(p);
      else 
	return arg(2)(p);
    }
  
FUNCTION_END(FunctionChooseFrom3InDiamondGrid)

//------------------------------------------------------------------------------------------

//! Function implements selection between 3 functions based on position in grid of hexagons
FUNCTION_BEGIN(FunctionChooseFrom3InHexagonGrid,0,3,false,FnStructure)

  //! Co-ordinates of hexagon with given hex-grid coords
  static const XYZ hex(int x,int y)
    {
      const real k=sqrt(3.0)/2.0;
      return XYZ(
		 x*k,
		 y+((x&1) ? 0.5 : 0.0),
		 0.0
		 );
    }

  //! Finds hex-grid coordinates of hex containing cartesian px,py
  static void nearest_hex(real px,real py,int &hx,int& hy)
    {
      // Initial guess at which hex we're in:
      const real k=sqrt(3.0)/2.0;

      const int nx=(int)rintf(px/k);
      const int ny=(int)(
			 (nx&1) 
			 ? 
			 rintf(py-0.5) 
			 : 
			 rintf(py)
			 );

      hx=nx;
      hy=ny;
      const XYZ ph=hex(nx,ny);
      real m2b=(XYZ(px,py,0.0)-ph).magnitude2();
      
      for (int dy=-1;dy<=1;dy++)
	for (int dx=-1;dx<=1;dx++)
	  if (!(dy==0 && dx==0))
	    {
	      const real m2=(XYZ(px,py,0.0)-hex(nx+dx,ny+dy)).magnitude2();
	      if (m2<m2b)
		{
		  hx=nx+dx;
		  hy=ny+dy;
		  m2b=m2;
		}
	    }
    } 
  
  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      int hx;
      int hy;
      nearest_hex(p.x(),p.y(),hx,hy);
      const uint which=hy+((hx&1)? 2 : 0);
      return arg(modulusi(which,3))(p);
    }
    
FUNCTION_END(FunctionChooseFrom3InHexagonGrid)

//------------------------------------------------------------------------------------------

//! Function implements selection between 2 functions based on position in grid of hexagons
FUNCTION_BEGIN(FunctionChooseFrom2InBorderedHexagonGrid,1,2,false,FnStructure)

  //! Co-ordinates of hexagon with given hex-grid coords
  static const XYZ hex(int x,int y)
    {
      const real k=sqrt(3.0)/2.0;
      return XYZ(
		 x*k,
		 y+((x&1) ? 0.5 : 0.0),
		 0.0
		 );
    }

  //! Finds hex-grid coordinates of hex containing cartesian px,py
  static void nearest_hex(real px,real py,int &hx,int& hy)
    {
      // Initial guess at which hex we're in:
      const real k=sqrt(3.0)/2.0;

      const int nx=(int)rintf(px/k);
      const int ny=(int)(
			 (nx&1) 
			 ? 
			 rintf(py-0.5) 
			 : 
			 rintf(py)
			 );

      hx=nx;
      hy=ny;
      const XYZ ph=hex(nx,ny);
      real m2b=(XYZ(px,py,0.0)-ph).magnitude2();
      
      for (int dy=-1;dy<=1;dy++)
	for (int dx=-1;dx<=1;dx++)
	  if (!(dy==0 && dx==0))
	    {
	      const real m2=(XYZ(px,py,0.0)-hex(nx+dx,ny+dy)).magnitude2();
	      if (m2<m2b)
		{
		  hx=nx+dx;
		  hy=ny+dy;
		  m2b=m2;
		}
	    }
    } 
  
  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      int hx;
      int hy;
      nearest_hex(p.x(),p.y(),hx,hy);

      bool in_border=false;

      // Hex centres are separated by 1.0 so limit border size
      const real b=modulusf(param(0),0.5);

      // Step along grid co-ordinates in various directions.  If there's a nearer point, we're in the border.
      for (uint a=0;a<6;a++)
	{
	  const real dx=b*sin(a*M_PI/3.0);
	  const real dy=b*cos(a*M_PI/3.0);
	  
	  int ax;
	  int ay;
	  nearest_hex(p.x()+dx,p.y()+dy,ax,ay);

	  if (hx!=ax || hy!=ay)
	    {
	      in_border=true;
	      break;
	    }
	}

      return arg(in_border)(p);
    }
  
FUNCTION_END(FunctionChooseFrom2InBorderedHexagonGrid)

//------------------------------------------------------------------------------------------

//! Rays intersecting a textured unit sphere
/*! arg(0) is background
    arg(1) is 3D texture for sphere
  param(0,1,2) is light source direction
  p.x, p.y is the 2D position of a ray from infinity travelling in direction (0 0 1)
*/
FUNCTION_BEGIN(FunctionOrthoSphereShaded,3,2,false,FnRender)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      const real pr2=p.x()*p.x()+p.y()*p.y();
      if (pr2<1.0)
	{
	  const real z=-sqrt(1.0-pr2);
	  const XYZ n(p.x(),p.y(),z);

	  const XYZ lu(param(0),param(1),param(2));
	  const XYZ l(lu.normalised());

	  const real i=0.5*(1.0+l%n); // In range 0-1
	  return i*arg(1)(n);
	}
      else
	{
	  return arg(0)(p);
	}
    }
  
FUNCTION_END(FunctionOrthoSphereShaded)

//------------------------------------------------------------------------------------------

//! Rays intersecting a textured unit sphere, with bump mapping
/*! arg(0) is background
    arg(1) is 3D texture for sphere
    arg(2) is bump-map for sphere (take magnitude2)
  param(0,1,2) is light source direction
  p.x, p.y is the 2D position of a ray from infinity travelling in direction (0 0 1)
*/
FUNCTION_BEGIN(FunctionOrthoSphereShadedBumpMapped,3,3,false,FnRender)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      const real pr2=p.x()*p.x()+p.y()*p.y();
      if (pr2<1.0)
	{
	  const real z=-sqrt(1.0-pr2);
	  const XYZ n(p.x(),p.y(),z);

	  // Tangent vectors
	  const XYZ east((XYZ(0.0,1.0,0.0)*n).normalised());
	  const XYZ north(n*east);

	  const real e0=(arg(2)(n-epsilon*east)).magnitude2();
	  const real e1=(arg(2)(n+epsilon*east)).magnitude2();
	  const real n0=(arg(2)(n-epsilon*north)).magnitude2();
	  const real n1=(arg(2)(n+epsilon*north)).magnitude2();

	  const real de=(e1-e0)*inv_epsilon2;
	  const real dn=(n1-n0)*inv_epsilon2;

	  const XYZ perturbed_n((n-east*de-north*dn).normalised());

	  const XYZ lu(param(0),param(1),param(2));
	  const XYZ l(lu.normalised());

	  const real i=0.5*(1.0+l%perturbed_n); // In range 0-1
	  return i*arg(1)(n);
	}
      else
	{
	  return arg(0)(p);
	}
    }
  
FUNCTION_END(FunctionOrthoSphereShadedBumpMapped)


//------------------------------------------------------------------------------------------

//! Rays reflecting off a unit sphere
/*! arg(0) is background
    arg(1) sampled using a normalised vector defines an environment for reflected rays
  p.x, p.y is the 2D position of a ray from infinity travelling in direction (0 0 1)
*/
FUNCTION_BEGIN(FunctionOrthoSphereReflect,0,2,false,FnRender)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      const real pr2=p.x()*p.x()+p.y()*p.y();
      if (pr2<1.0)
	{
	  const real z=-sqrt(1.0-pr2);

	  // This is on surface of unit radius sphere - no need to normalise
	  XYZ n(p.x(),p.y(),z);

	  // The ray _towards_ the viewer v is (0 0 -1)
	  const XYZ v(0.0,0.0,-1.0);

	  // The reflected ray is (2n.v)n-v
	  const XYZ reflected((2.0*(n%v))*n-v);

	  return arg(1)(reflected);
	}
      else
	{
	  return arg(0)(p);
	}
    }
  
FUNCTION_END(FunctionOrthoSphereReflect)

//------------------------------------------------------------------------------------------

//! Rays reflecting off a bump mapped unit sphere
/*! arg(0) is background
    arg(1) sampled using a normalised vector defines an environment for reflected rays
    arg(2) is bump map
  p.x, p.y is the 2D position of a ray from infinity travelling in direction (0 0 1)
*/
FUNCTION_BEGIN(FunctionOrthoSphereReflectBumpMapped,0,3,false,FnRender)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      const real pr2=p.x()*p.x()+p.y()*p.y();
      if (pr2<1.0)
	{
	  const real z=-sqrt(1.0-pr2);

	  // This is on surface of unit radius sphere - no need to normalise
	  XYZ n(p.x(),p.y(),z);

	  // Tangent vectors
	  const XYZ east((XYZ(0.0,1.0,0.0)*n).normalised());
	  const XYZ north(n*east);

	  const real e0=(arg(2)(n-epsilon*east)).magnitude2();
	  const real e1=(arg(2)(n+epsilon*east)).magnitude2();
	  const real n0=(arg(2)(n-epsilon*north)).magnitude2();
	  const real n1=(arg(2)(n+epsilon*north)).magnitude2();

	  const real de=(e1-e0)*inv_epsilon2;
	  const real dn=(n1-n0)*inv_epsilon2;

	  const XYZ perturbed_n((n-east*de-north*dn).normalised());

	  // The ray _towards_ the viewer is (0 0 -1)
	  const XYZ v(0.0,0.0,-1.0);

	  // The reflected ray is (2n.v)n-v
	  const XYZ reflected((2.0*(perturbed_n%v))*perturbed_n-v);

	  return arg(1)(reflected);
	}
      else
	{
	  return arg(0)(p);
	}
    }
  
FUNCTION_END(FunctionOrthoSphereReflectBumpMapped)

//------------------------------------------------------------------------------------------

FUNCTION_BEGIN(FunctionFilter2D,2,1,false,0)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      return
	arg(0)(p)
	-(
	  arg(0)(p+XYZ(param(0),0.0,0.0))
	  +arg(0)(p+XYZ(-param(0),0.0,0.0))
	  +arg(0)(p+XYZ(0.0,param(1),0.0))
	  +arg(0)(p+XYZ(0.0,-param(1),0.0))
	  )/4.0;
    }
  
FUNCTION_END(FunctionFilter2D)

//------------------------------------------------------------------------------------------

FUNCTION_BEGIN(FunctionFilter3D,3,1,false,0)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      return
	arg(0)(p)
	-(
	  arg(0)(p+XYZ(param(0),0.0,0.0))
	  +arg(0)(p+XYZ(-param(0),0.0,0.0))
	  +arg(0)(p+XYZ(0.0,param(1),0.0))
	  +arg(0)(p+XYZ(0.0,-param(1),0.0))
	  +arg(0)(p+XYZ(0.0,0.0,param(2)))
	  +arg(0)(p+XYZ(0.0,0.0,-param(2)))
	  )/6.0;
    }
  
FUNCTION_END(FunctionFilter3D)

//------------------------------------------------------------------------------------------

//! Sum of two evaluations of a function, one sampled at an offset and weighted.
FUNCTION_BEGIN(FunctionShadow,4,1,false,0)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      return
	arg(0)(p)+param(3)*arg(0)(p+XYZ(param(0),param(1),param(2)));
    }
  
FUNCTION_END(FunctionShadow)


//------------------------------------------------------------------------------------------

//! Like FunctionShadow but the offset is obtained from a function.
FUNCTION_BEGIN(FunctionShadowGeneralised,1,2,false,0)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      return
	arg(0)(p)+param(0)*arg(0)(p+arg(1)(p));
    }
  
FUNCTION_END(FunctionShadowGeneralised)

//------------------------------------------------------------------------------------------

//! Multiply x and y by z
FUNCTION_BEGIN(FunctionCone,0,0,false,0)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      return XYZ(p.x()*p.z(),p.y()*p.z(),p.z());
    }
  
FUNCTION_END(FunctionCone)

//------------------------------------------------------------------------------------------

//! Multiply x and y by exp(z)
FUNCTION_BEGIN(FunctionExpCone,0,0,false,0)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      const real k=exp(p.z());
      return XYZ(p.x()*k,p.y()*k,p.z());
    }
  
FUNCTION_END(FunctionExpCone)

//------------------------------------------------------------------------------------------

//! Separate influence of z co-ordinate.
/*! Interesting as a top level node for animations as structure will tend to be fixed, with only colour map changing
 */
FUNCTION_BEGIN(FunctionSeparateZ,3,2,false,0)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      const XYZ v=arg(0)(XYZ(p.x(),p.y(),0.0));
      return arg(1)(v+p.z()*XYZ(param(0),param(1),param(2)));
    }
  
FUNCTION_END(FunctionSeparateZ)

//------------------------------------------------------------------------------------------

//! Function repeatedly applying it's leaf function to the argument
FUNCTION_BEGIN(FunctionIterate,0,1,true,FnIterative)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      XYZ ret(p);
      for (uint i=0;i<iterations();i++)
	ret=arg(0)(ret);
      return ret;
    }
  
FUNCTION_END(FunctionIterate)

//------------------------------------------------------------------------------------------

//! Function returning average value of evenly spaced samples between two points
FUNCTION_BEGIN(FunctionAverageSamples,3,1,true,FnIterative)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      const XYZ baseline(param(0),param(1),param(2));
     
      XYZ p0;
      XYZ p1;
      XYZ delta;
      
      if (iterations()==1)
	{
	  p0=p;
	  p1=p;
	  delta=XYZ(0.0,0.0,0.0);
	}
      else
	{
	  // In the case of two iterations the samples will be at p0 and p1
	  p0=p-baseline;
	  p1=p+baseline;
	  delta=(p1-p0)/(iterations()-1);
	}

      XYZ ret(0.0,0.0,0.0);
      XYZ ps=p0;

      for (uint i=0;i<iterations();i++)
	{
	  ret+=arg(0)(ps);
	  ps+=delta;
	}
      ret/=iterations();
      return ret;
    }
  
FUNCTION_END(FunctionAverageSamples)

//------------------------------------------------------------------------------------------

//! Similar to average samples except one end has a higher weighting
FUNCTION_BEGIN(FunctionStreak,3,1,true,FnIterative)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      const XYZ baseline(param(0),param(1),param(2));
     
      XYZ p0;
      XYZ p1;
      XYZ delta;
      
      if (iterations()==1)
	{
	  p0=p;
	  p1=p;
	  delta=XYZ(0.0,0.0,0.0);
	}
      else
	{
	  p0=p;
	  p1=p+baseline;
	  delta=(p1-p0)/(iterations()-1);
	}

      XYZ ret(0.0,0.0,0.0);
      XYZ ps=p0;
      real w=0.0;

      for (uint i=0;i<iterations();i++)
	{
	  const real k=1.0-static_cast<real>(i)/iterations();
	  ret+=k*arg(0)(ps);
	  w+=k;
	  ps+=delta;
	}
      ret/=w;
      return ret;
    }
  
FUNCTION_END(FunctionStreak)

//------------------------------------------------------------------------------------------

//! Average of samples around a ring
FUNCTION_BEGIN(FunctionAverageRing,1,1,true,FnIterative)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      if (iterations()==1) return arg(0)(p);

      const real da=2.0*M_PI/iterations();
      XYZ ret(0.0,0.0,0.0);
      for (uint i=0;i<iterations();i++)
	{
	  const real a=i*da;
	  const XYZ delta(param(0)*cos(a),param(0)*sin(a),0.0);
	  ret+=arg(0)(p+delta);
	}
      return ret/iterations();
    }
  
FUNCTION_END(FunctionAverageRing)

//------------------------------------------------------------------------------------------

//! Like FunctionAverageRing but subtract off the centre value
FUNCTION_BEGIN(FunctionFilterRing,1,1,true,FnIterative)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      if (iterations()==1) return XYZ(0.0,0.0,0.0);

      const real da=2.0*M_PI/iterations();
      XYZ ret(0.0,0.0,0.0);
      for (uint i=0;i<iterations();i++)
	{
	  const real a=i*da;
	  const XYZ delta(param(0)*cos(a),param(0)*sin(a),0.0);
	  ret+=arg(0)(p+delta);
	}
      return ret/iterations()-arg(0)(p);
    }
  
FUNCTION_END(FunctionFilterRing)

//------------------------------------------------------------------------------------------

//! Function similar to FunctionAverageSamples but doing convolution
FUNCTION_BEGIN(FunctionConvolveSamples,3,2,true,FnIterative)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      const XYZ baseline(param(0),param(1),param(2));
     
      XYZ p0;
      XYZ p1;
      XYZ delta;
      
      if (iterations()==1)
	{
	  p0=p;
	  p1=p;
	  delta=XYZ(0.0,0.0,0.0);
	}
      else
	{
	  p0=p-baseline;
	  p1=p+baseline;
	  delta=(p1-p0)/(iterations()-1);
	}

      XYZ ret(0.0,0.0,0.0);
      XYZ pd(0.0,0.0,0.0);

      for (uint i=0;i<iterations();i++)
	{
	  //! \todo Hmmm.. this is cross product, not inner product
	  ret+=(arg(0)(p+pd)*arg(1)(pd));
	  pd+=delta;
	}
      ret/=iterations();
      return ret;
    }
  
FUNCTION_END(FunctionConvolveSamples)

//------------------------------------------------------------------------------------------

//! Function summing decreasing amounts of higher frequency versions of image
FUNCTION_BEGIN(FunctionAccumulateOctaves,0,1,true,FnIterative)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      XYZ ret(0.0,0.0,0.0);
      real k=0.0;
      for (uint i=0;i<iterations();i++)
	{
	  const real scale=(1<<i);
	  const real iscale=1.0/scale;
	  ret+=iscale*(arg(0)(scale*p));
	  k+=iscale;
	}
      ret/=k;
      return ret;
    }
  
FUNCTION_END(FunctionAccumulateOctaves)


//------------------------------------------------------------------------------------------

//! Mandelbrot iterator for fractal functions.
/*! Returns i in 0 to iterations inclusive.  i==iterations implies "in" set.
 */
inline const uint brot(const real z0r,const real z0i,const real cr,const real ci,const uint iterations)
{
  real zr=z0r;
  real zi=z0i;
  uint i;
  for (i=0;i<iterations;i++)
    {
      const real zr2=zr*zr;
      const real zi2=zi*zi;
      
      if (zr2+zi2>4.0)
	break;
      
      const real nzr=zr2-zi2+cr;
      const real nzi=2.0*zr*zi+ci;

      zr=nzr;
      zi=nzi;
    }

  return i;
}

//------------------------------------------------------------------------------------------

//! Function selects arg to evaluate based on test for point in Mandelbrot set.
FUNCTION_BEGIN(FunctionMandelbrotChoose,0,2,true,FnIterative|FnFractal)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      return (brot(0.0,0.0,p.x(),p.y(),iterations())==iterations() ? arg(0)(p) : arg(1)(p));
    }
  
FUNCTION_END(FunctionMandelbrotChoose)

//-----------------------------------------------------------------------------------------

//! Function returns -1 for points in set, 0-1 for escaped points
FUNCTION_BEGIN(FunctionMandelbrotContour,0,0,true,FnIterative|FnFractal)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      const uint i=brot(0.0,0.0,p.x(),p.y(),iterations());
      return (i==iterations() ? XYZ::fill(-1.0) : XYZ::fill(static_cast<real>(i)/iterations()));
    }
  
FUNCTION_END(FunctionMandelbrotContour)

//------------------------------------------------------------------------------------------

//! Function selects arg to evaluate based on test for point in Julia set.
FUNCTION_BEGIN(FunctionJuliaChoose,2,2,true,FnIterative|FnFractal)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      return (brot(p.x(),p.y(),param(0),param(1),iterations())==iterations() ? arg(0)(p) : arg(1)(p));
    }
  
FUNCTION_END(FunctionJuliaChoose)

//------------------------------------------------------------------------------------------

//! Function returns -1 for points in set, 0-1 for escaped points
FUNCTION_BEGIN(FunctionJuliaContour,2,0,true,FnIterative|FnFractal)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      const uint i=brot(p.x(),p.y(),param(0),param(1),iterations());
      return (i==iterations() ? XYZ::fill(-1.0) : XYZ::fill(static_cast<real>(i)/iterations()));
    }
  
FUNCTION_END(FunctionJuliaContour)

//------------------------------------------------------------------------------------------

//! Function selects arg to evaluate based on test for point in Juliabrot set.
/*! Juliabrot is 4 dimensional, but we only have 3 incoming parameters,
  so have 4 4d-basis vector parameters.
 */
FUNCTION_BEGIN(FunctionJuliabrotChoose,16,2,true,FnIterative|FnFractal)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      const real zr=p.x()*param( 0)+p.y()*param( 1)+p.z()*param( 2)+param( 3);
      const real zi=p.x()*param( 4)+p.y()*param( 5)+p.z()*param( 6)+param( 7);
      const real cr=p.x()*param( 8)+p.y()*param( 9)+p.z()*param(10)+param(11);
      const real ci=p.x()*param(12)+p.y()*param(13)+p.z()*param(14)+param(15);
      return (brot(zr,zi,cr,ci,iterations())==iterations() ? arg(0)(p) : arg(1)(p));
    }
  
FUNCTION_END(FunctionJuliabrotChoose)

//------------------------------------------------------------------------------------------

//! Function returns -1 for points in set, 0-1 for escaped points
/*! Juliabrot is 4 dimensional, but we only have 3 incoming parameters,
  so have 4 4d-basis vector parameters.
 */
FUNCTION_BEGIN(FunctionJuliabrotContour,16,0,true,FnIterative|FnFractal)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p) const
    {
      const real zr=p.x()*param( 0)+p.y()*param( 1)+p.z()*param( 2)+param( 3);
      const real zi=p.x()*param( 4)+p.y()*param( 5)+p.z()*param( 6)+param( 7);
      const real cr=p.x()*param( 8)+p.y()*param( 9)+p.z()*param(10)+param(11);
      const real ci=p.x()*param(12)+p.y()*param(13)+p.z()*param(14)+param(15);
      const uint i=brot(zr,zi,cr,ci,iterations());
      return (i==iterations() ? XYZ::fill(-1.0) : XYZ::fill(static_cast<real>(i)/iterations()));
    }
  
FUNCTION_END(FunctionJuliabrotContour)


//------------------------------------------------------------------------------------------

//! \todo Something like Conway's Game of Life or a reaction-diffusion system, seeded from an initial function.
/*! Probably operate on an iterations()xiterations()xiterations() grid.
  Need ability to cache state at a particular time by identity (address-of?) of "our".
  WILL cause slow start-up due to precompute, although compute-on-demand should help a bit.
  Maybe the whole static thing was a mistake.
  Should derive Function classes from FunctionBoilerplate<T> to provide Clone etc.
  NO! Do this properly.  Threads should share a MutatableImage (ref. counted) which can hold this kind of state.
  Locking method to check it is valid before compute starts.  Read during main evaluation should be lock-free.
 */
/*
FUNCTION_BEGIN(FunctionCellular,0,1,true,FnIterative)

  //! Evaluate function.
  virtual const XYZ evaluate(const XYZ& p)
    {
      return XYZ(0.0,0.0,0.0);
    }
  
  //! Not constant.
  virtual const bool is_constant()
    {
      return false;
    }

 protected:

};
*/
// Not done yet.
//REGISTER(FunctionCellular);

//------------------------------------------------------------------------------------------

#endif
