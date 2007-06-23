// Source file for evolvotron
// Copyright (C) 2002,2003,2004,2005 Tim Day
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
  \warning This file should ONLY be included in function_node.cpp, which instantiates everything.
  A few functions are allowed to escape "into the wild" (e.g FunctionPreTransform and FunctionPostTransform 
  which have their own header file)
*/

#ifndef _functions_friezegroup_spinhop_h_
#define _functions_friezegroup_spinhop_h_

#include "useful.h"
#include "function_registry.h"
#include "function_boilerplate.h"

//------------------------------------------------------------------------------------------

FUNCTION_BEGIN(FunctionFriezeGroupSpinhop,1,1,false,FnStructure)

  virtual const XYZ evaluate(const XYZ& p) const
    {
      const real x=(p.y()>0.0 ? modulusf(p.x(),1.0) : 1.0-modulusf(p.x(),1.0));
      const real y=fabs(p.y());
      return arg(0)(XYZ(x,y,param(0)));
    }

FUNCTION_END(FunctionFriezeGroupSpinhop)

//------------------------------------------------------------------------------------------

/*
FUNCTION_BEGIN(FunctionFriezeGroupSpinhopBlend,3,1,false,FnStructure)

  virtual const XYZ evaluate(const XYZ& p) const
    {
      const real dx=0.5*tanh(fabs(param(1)));
      const real dy=0.5*fabs(param(2));

      const real x=p.x();
      const real y=p.y();

      const real x1=modulusf(p.x(),1.0);
      const real x0=x1+1.0;
      const real x2=x1-1.0;
      
      const real x4=1.0-modulusf(p.x(),1.0);
      const real x3=x4-1.0;
      const real x5=x4+1.0;

      const real y0=p.y();
      const real y1=-p.y();

      const real wx0=(x<dx ? 0.5+0.5*(x/dx) : 0.0);
      const real wx2=(x>1.0-dx ? 1.0-0.5*(x-(1.0-dx))/dx : 0.0);
      const real wx1=1.0-wx0-wx2;

      const real y0=???

      const real z=param(0);

      return
	 arg(0)(wx0*wy0,XYZ(x0,y0,z))
	+arg(0)(wx1*wy0,XYZ(x1,y0,z))
	+arg(0)(wx2*wy0,XYZ(x2,y0,z))
	+arg(0)(wx3*wy1,XYZ(x3,y1,z))
	+arg(0)(wx4*wy1,XYZ(x4,y1,z))
	+arg(0)(wx5*wy1,XYZ(x5,y1,z));
    }

FUNCTION_END(FunctionFriezeGroupSpinhopBlend)
*/
//------------------------------------------------------------------------------------------

FUNCTION_BEGIN(FunctionFriezeGroupSpinhopWarped,7,3,false,FnStructure)

  virtual const XYZ evaluate(const XYZ& p) const
    {
      const real dx=p.y()*(XYZ(param(0),param(1),param(2))%arg(2)(XYZ(0.0,fabs(p.y()),0.0)));
      const real wx1=modulusf(p.x()-dx,1.0);
      const real dy=(wx1>0.5 ? 1.0 : -1.0)*(XYZ(param(3),param(4),param(5))%arg(1)(XYZ(fabs(wx1-0.5),0.0,0.0)));

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
	  x=1.0-(wx1+dx);
	  y=-p.y();
	}

      return arg(0)(XYZ(x,y,param(6)));
    }

  virtual const bool is_constant() const
    {
      return arg(0).is_constant();  // Warp function makes no difference if primary argument is constant
    }

FUNCTION_END(FunctionFriezeGroupSpinhopWarped)

//------------------------------------------------------------------------------------------

FUNCTION_BEGIN(FunctionFriezeGroupSpinhopFreeZ,0,1,false,FnStructure)

  virtual const XYZ evaluate(const XYZ& p) const
    {
      const real x=(p.y()>0.0 ? modulusf(p.x(),1.0) : 1.0-modulusf(p.x(),1.0));
      const real y=fabs(p.y());
      return arg(0)(XYZ(x,y,p.z()));
    }

FUNCTION_END(FunctionFriezeGroupSpinhopFreeZ)

//------------------------------------------------------------------------------------------

FUNCTION_BEGIN(FunctionFriezeGroupSpinhopWarpedFreeZ,6,3,false,FnStructure)

  virtual const XYZ evaluate(const XYZ& p) const
    {
      const real dx=p.y()*(XYZ(param(0),param(1),param(2))%arg(2)(XYZ(0.0,fabs(p.y()),0.0)));
      const real wx1=modulusf(p.x()-dx,1.0);
      const real dy=(wx1>0.5 ? 1.0 : -1.0)*(XYZ(param(3),param(4),param(5))%arg(1)(XYZ(fabs(wx1-0.5),0.0,0.0)));

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
	  x=1.0-(wx1+dx);
	  y=-p.y();
	}

      return arg(0)(XYZ(x,y,p.z()));
    }

  virtual const bool is_constant() const
    {
      return arg(0).is_constant();  // Warp function makes no difference if primary argument is constant
    }

FUNCTION_END(FunctionFriezeGroupSpinhopWarpedFreeZ)


#endif
