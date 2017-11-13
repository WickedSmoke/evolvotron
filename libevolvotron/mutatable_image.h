/**************************************************************************/
/*  Copyright 2012 Tim Day                                                */
/*                                                                        */
/*  This file is part of Evolvotron                                       */
/*                                                                        */
/*  Evolvotron is free software: you can redistribute it and/or modify    */
/*  it under the terms of the GNU General Public License as published by  */
/*  the Free Software Foundation, either version 3 of the License, or     */
/*  (at your option) any later version.                                   */
/*                                                                        */
/*  Evolvotron is distributed in the hope that it will be useful,         */
/*  but WITHOUT ANY WARRANTY; without even the implied warranty of        */
/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         */
/*  GNU General Public License for more details.                          */
/*                                                                        */
/*  You should have received a copy of the GNU General Public License     */
/*  along with Evolvotron.  If not, see <http://www.gnu.org/licenses/>.   */
/**************************************************************************/

/*! \file 
  \brief Interfaces for class MutatableImage.
*/

#ifndef _mutatable_image_h_
#define _mutatable_image_h_

#include "common.h"

#include "xyz.h"

class FunctionNull;
class FunctionRegistry;
class FunctionTop;
class MutationParameters;

//! Class to hold the base FunctionNode of an image.
/*! Once it owns a root FunctionNode* the whole structure should be fixed (mutate isn't available, only mutated).
  \todo Do reference counting on this object ?  Maybe not: have to worry about stateful nodes,
  \todo Generally tighten up const-ness of interfaces.
 */
class MutatableImage
{
 protected:

  //! The top level FunctionNode of the image.
  /*! This is partly here because FunctionNode::mutate can't change the type of
    the node it is invoked on (only child nodes can be zapped), partly so we
    can keep colour and space transforms under control.
   */
  std::unique_ptr<FunctionTop> _top;

  //! Whether to sweep z sinusoidally (vs linearly)
  bool _sinusoidal_z;

  //! Whether xyz should be interpreted as long/lat/radius
  bool _spheremap;

  //! Whether this image is locked \todo Should be a property of display, not image.
  bool _locked;

  //! Serial number for identity tracking (used by display to discover whether a recompute is needed)
  unsigned long long _serial;

  //! Object count to generate serial numbers
  static unsigned long long _count;

 public:
  
  //! Take ownership of the image tree with the specified root node.
  MutatableImage(std::unique_ptr<FunctionTop>&,bool sinz,bool sm,bool lock);
  
  //! Create a new random image tree.
  MutatableImage(const MutationParameters& parameters,bool exciting,bool sinz,bool sm);
  
  //! Destructor.  NB Deletes owned image function tree.
  virtual ~MutatableImage();

  //! Returns the sampling co-ordinate given a (sub)pixel position in the given frame of an animation.
  /*! This depends on things like sinusoidal_z and spheremap
   */
  const XYZ sampling_coordinate(real x,real y,uint z,uint sx,uint sy,uint sz) const;

  //! Accessor.
  const FunctionTop& top() const;

  //! Accessor.
  bool sinusoidal_z() const
    {
      return _sinusoidal_z;
    }

  //! Accessor.
  bool spheremap() const
    {
      return _spheremap;
    }

  //! Accessor.
  bool locked() const
    {
      return _locked;
    }

  //! Accessor.
  void locked(bool l)
    {
      _locked=l;
    }

  //! Accessor.
  unsigned long long serial() const
    {
      return _serial;
    }

  //! Clone this image.  The cloned image will not have locked state.
  boost::shared_ptr<const MutatableImage> deepclone() const;

  //! Clone this image, setting locked state to that specified.
  boost::shared_ptr<const MutatableImage> deepclone(bool lock) const;

  //! Return a mutated version of this image
  boost::shared_ptr<const MutatableImage> mutated(const MutationParameters& p) const;

  //! Return a simplified version of this image
  boost::shared_ptr<const MutatableImage> simplified() const;

  //! Return the a 0-255-scaled RGB value at the specified location.
  const XYZ get_rgb(const XYZ& p) const;

  //! Return the a 0-255-scaled RGB value at the specified pixel of an image/animation taking jitter (if random number generator provided) and multisampling into account
  const XYZ get_rgb(uint x,uint y,uint f,uint width,uint height,uint frames,Random01* r01,uint multisample) const;

  //! Return whether image value is independent of position.
  bool is_constant() const;

  //! Save the function-tree to the stream
  std::ostream& save_function(std::ostream& out) const;

  //! Obtain some statistics about the image function
  void get_stats(uint& total_nodes,uint& total_parameters,uint& depth,uint& width,real& proportion_constant) const;

  //! Check the function tree is ok.
  bool ok() const;

  //! Read a new function tree from the given stream.
  static boost::shared_ptr<const MutatableImage> load_function(const FunctionRegistry& function_registry,std::istream& in,std::string& report);
};

#endif
