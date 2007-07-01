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

#ifndef _functions_friezegroup_spinsidle_h_
#define _functions_friezegroup_spinsidle_h_

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

#endif
