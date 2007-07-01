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
*/

#ifndef _functions_friezegroup_step_h_
#define _functions_friezegroup_step_h_

/*
  Step (Conway p1a1): glide reflection only.
  Sawtooth x, out of step by half range across y-axis.

    o     o
  ---   ---
     ---   ---
       o     o
*/

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

#endif
