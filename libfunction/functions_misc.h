// Source file for evolvotron
// Copyright (C) 2007 Tim Day
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
Use this for functions under development, but they should really be split
out into separate files eventually.
*/

#ifndef _functions_misc_h_
#define _functions_misc_h_

 

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

//! Multiply x and y by z.
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


//! \todo Something like Conway's Game of Life or a reaction-diffusion system, seeded from an initial function.
/*! Probably operate on an iterations()xiterations()xiterations() grid.
  Need ability to cache state at a particular time by identity (address-of?) of "our".
  WILL cause slow start-up due to precompute, although compute-on-demand should help a bit.
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
