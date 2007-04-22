// Source file for evolvotron
// Copyright (C) 2002,2003 Tim Day
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
  \brief Interface for class TransformFactory.
*/

#ifndef _transform_factory_h_
#define _transform_factory_h_

#include "useful.h"
#include "random.h"
#include "transform.h"

//! Abstract base class for classes creating Transforms
class TransformFactory 
{
 public:
  TransformFactory()
    {}
  virtual ~TransformFactory()
    {}

  //! Clone functionality needed to retain typed copies of factories.
  virtual std::auto_ptr<TransformFactory> clone() const
    =0;

  //! Method to build a Transform.
  virtual const Transform operator()(Random01&) const
    =0;
 protected:
};

//! Factory for creating random warps
/*! These are the mixed scale/rotate/translate ones used by the combo warp.
*/
class TransformFactoryRandomWarpXY : public TransformFactory
{
 public:
  //! Constructor accepts power-of-2 specifiers, so -1 & 1 would generate scalings between 0.5 and 2, half zooming in and half zooming out.
  // Constructor.
  /*! \todo Should be parameterised with parameters specified at point of usage in MutatableImageDisplay
   */
  TransformFactoryRandomWarpXY()
    {}

  //! Clone.
  virtual std::auto_ptr<TransformFactory> clone() const
    {
      return std::auto_ptr<TransformFactory>(new TransformFactoryRandomWarpXY());
    }

  //! Return a random transform.
  virtual const Transform operator()(Random01& rng) const;

 protected:
};

//! Factory for creating random scaling transforms
class TransformFactoryRandomScaleXY : public TransformFactory
{
 public:
  //! Constructor accepts power-of-2 specifiers, so -1 & 1 would generate scalings between 0.5 and 2, half zooming in and half zooming out.
  TransformFactoryRandomScaleXY(real lopow2,real hipow2)
    :_lopow2(lopow2),_hipow2(hipow2)
    {}

  //! Clone method.
  virtual std::auto_ptr<TransformFactory> clone() const
    {
      return std::auto_ptr<TransformFactory>(new TransformFactoryRandomScaleXY(_lopow2,_hipow2));
    }

  //! Return a random scaling transform.
  virtual const Transform operator()(Random01& rng) const;
 protected:
  //! The low end of the scaling as a power of 2
  real _lopow2;

  //! The high end of the scaling as a power of 2
  real _hipow2;
};

//! Factory for creating random z-axis rotation transforms
class TransformFactoryRandomRotateZ : public TransformFactory
{
 public:
  //! Constructor
  TransformFactoryRandomRotateZ()
    {}

  //! Clone method.
  virtual std::auto_ptr<TransformFactory> clone() const
    {
      return std::auto_ptr<TransformFactory>(new TransformFactoryRandomRotateZ());
    }

  //! Create a transform.
  virtual const Transform operator()(Random01& rng) const;
 protected:
};

//! Factory for creating random translation transforms
class TransformFactoryRandomTranslateXYZ : public TransformFactory
{
 public:
  //! Constructor accepts XYZ origin (centre) and range (+/-)
  TransformFactoryRandomTranslateXYZ(const XYZ& o,const XYZ& r)
    :_origin(o)
    ,_range(r)
    {}

  //! Clone method.
  virtual std::auto_ptr<TransformFactory> clone() const
    {
      return std::auto_ptr<TransformFactory>(new TransformFactoryRandomTranslateXYZ(_origin,_range));
    }

  //! Return a random Transform
  virtual const Transform operator()(Random01& rng) const;

 protected:
  //! Base value for translations
  XYZ _origin;

  //! Range for translations
  XYZ _range;
};


#endif





