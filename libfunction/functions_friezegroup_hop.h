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
*/

#ifndef _functions_friezegroup_hop_h_
#define _functions_friezegroup_hop_h_

/*
  Hop (Conway p111): no reflections or rotation
  Just have to cycle x range.
  
    o    o    o
  ---  ---  ---
 
*/

//------------------------------------------------------------------------------------------

FUNCTION_BEGIN(FunctionFriezeGroupHop,1,1,false,FnStructure)

  virtual const XYZ evaluate(const XYZ& p) const
    {
      const real x=modulusf(p.x(),1.0);
      const real y=p.y();
      return arg(0)(XYZ(x,y,param(0)));
    }
  
FUNCTION_END(FunctionFriezeGroupHop)

//------------------------------------------------------------------------------------------

FUNCTION_BEGIN(FunctionFriezeGroupHopBlend,2,1,false,FnStructure)

  virtual const XYZ evaluate(const XYZ& p) const
    {
      const real dx=0.5*tanh(fabs(param(1)));  // dx in 0-0.5
      const real x1=modulusf(p.x(),1.0);
      const real x0=x1+1.0;
      const real x2=x1-1.0;

      const real y=p.y();
      const real z=param(0);

      real w0,w1,w2;
      if (x1<dx) 
	{
	  w0=0.5-0.5*(x1/dx);
	  w2=0.0;
	  w1=1.0-w0;
	}
      else if (x1>1.0-dx)
	{
	  w2=0.5*(x1-(1.0-dx))/dx;
	  w0=0.0;
	  w1=1.0-w2;
	}
      else
	{
	  w0=0.0;
	  w1=1.0;
	  w2=0.0;
	}

      return
	 arg(0)(w0,XYZ(x0,y,z))
	+arg(0)(w1,XYZ(x1,y,z))
	+arg(0)(w2,XYZ(x2,y,z));
    }
  
FUNCTION_END(FunctionFriezeGroupHopBlend)

//------------------------------------------------------------------------------------------

FUNCTION_BEGIN(FunctionFriezeGroupHopWarped,4,2,false,FnStructure)
  
  virtual const XYZ evaluate(const XYZ& p) const
    {
      const real dx=XYZ(param(0),param(1),param(2))%arg(1)(XYZ(0.0,p.y(),0.0));
      const real x=modulusf(p.x()-dx,1.0)+dx;
      const real y=p.y();
      return arg(0)(XYZ(x,y,param(3)));
    }

  virtual const bool is_constant() const
    {
      return arg(0).is_constant();  // Warp function makes no difference if primary argument is constant
    }
  
FUNCTION_END(FunctionFriezeGroupHopWarped)

//------------------------------------------------------------------------------------------

FUNCTION_BEGIN(FunctionFriezeGroupHopFreeZ,0,1,false,FnStructure)

  virtual const XYZ evaluate(const XYZ& p) const
    {
      const real x=modulusf(p.x(),1.0);
      const real y=p.y();
      return arg(0)(XYZ(x,y,p.z()));
    }
  
FUNCTION_END(FunctionFriezeGroupHopFreeZ)

//------------------------------------------------------------------------------------------

FUNCTION_BEGIN(FunctionFriezeGroupHopBlendFreeZ,1,1,false,FnStructure)

  virtual const XYZ evaluate(const XYZ& p) const
    {
      const real dx=0.5*tanh(fabs(param(0)));  // dx in 0-0.5
      const real x1=modulusf(p.x(),1.0);
      const real x0=x1+1.0;
      const real x2=x1-1.0;

      const real y=p.y();
      const real z=p.z();

      real w0,w1,w2;
      if (x1<dx) 
	{
	  w0=0.5-0.5*(x1/dx);
	  w2=0.0;
	  w1=1.0-w0;
	}
      else if (x1>1.0-dx)
	{
	  w2=0.5*(x1-(1.0-dx))/dx;
	  w0=0.0;
	  w1=1.0-w2;
	}
      else
	{
	  w0=0.0;
	  w1=1.0;
	  w2=0.0;
	}

      return
	 arg(0)(w0,XYZ(x0,y,z))
	+arg(0)(w1,XYZ(x1,y,z))
	+arg(0)(w2,XYZ(x2,y,z));
    }
  
FUNCTION_END(FunctionFriezeGroupHopBlendFreeZ)

//------------------------------------------------------------------------------------------

FUNCTION_BEGIN(FunctionFriezeGroupHopWarpedFreeZ,3,2,false,FnStructure)
  
  virtual const XYZ evaluate(const XYZ& p) const
    {
      const real dx=XYZ(param(0),param(1),param(2))%arg(1)(XYZ(0.0,p.y(),0.0));
      const real x=modulusf(p.x()-dx,1.0)+dx;
      const real y=p.y();
      return arg(0)(XYZ(x,y,p.z()));
    }

  virtual const bool is_constant() const
    {
      return arg(0).is_constant();  // Warp function makes no difference if primary argument is constant
    }
  
FUNCTION_END(FunctionFriezeGroupHopWarpedFreeZ)

#endif
