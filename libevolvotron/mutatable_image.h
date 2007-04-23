// Source file for evolvotron
// Copyright (C) 2002,2003,2004 Tim Day
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
  \brief Interfaces for class MutatableImage.
*/

#ifndef _mutatable_image_h_
#define _mutatable_image_h_

#include <iosfwd>

#include "function_node.h"

class FunctionNull;
class FunctionTop;

//! Class to hold the base FunctionNode of an image.
/*! Once it owns a root FunctionNode* the whole structure should be fixed (mutate isn't available, only mutated).
  \todo Do reference counting on this object ?  Maybe not: have to worry about stateful nodes,
  \todo Generally tighten up const-ness of interfaces.
 */
class MutatableImage
{
 protected:

  //! The top level FunctionNode of the image.
  /*! This is partly here FunctionNode::mutate can't change the type of
    the node it is invoked on (only child nodes can be zapped), partly so we
    can keep colour and space transforms under control.
   */
  FunctionTop* _top;

  //! Whether to sweep z sinusoidally (vs linearly)
  bool _sinusoidal_z;

  //! Whether xyz should be interpreted as long/lat/radius
  bool _spheremap;

  bool _locked;

 public:
  
  //! Take ownership of the image tree with the specified root node.
  MutatableImage(FunctionTop* r,bool sinz,bool sm);
  
  //! Create a new random image tree.
  MutatableImage(const MutationParameters& parameters,bool exciting,bool sinz,bool sm);
  
  //! Destructor.  NB Deletes owned image function tree.
  virtual ~MutatableImage();

  //! Returns the sampling co-ordinate given a pixel position
  /*! This depends on things like sinusoidal_z and spheremap
   */
  const XYZ sampling_coordinate(uint x,uint y,uint z,uint sx,uint sy,uint sz) const;

  //! Accessor.
  const FunctionTop*const top() const;

  //! Accessor.
  const bool sinusoidal_z() const
    {
      return _sinusoidal_z;
    }

  //! Accessor.
  const bool spheremap() const
    {
      return _spheremap;
    }

  //! Accessor.
  const bool locked() const
    {
      return _locked;
    }

  //! Accessor.
  void locked(bool l)
    {
      _locked=l;
    }

  //! Clone this image.
  std::auto_ptr<MutatableImage> deepclone() const;

  //! Return a mutated version of this image
  std::auto_ptr<MutatableImage> mutated(const MutationParameters& p) const;

  //! Return a simplified version of this image
  std::auto_ptr<MutatableImage> simplified() const;

  //! Evaluate the image at specified coordinate.
  const XYZ operator()(const XYZ& p) const;

  void get_rgb(const XYZ& p,uint c[3]) const;

  //! Return whether image value is independent of position.
  const bool is_constant() const;

  //! Save the function-tree to the stream
  std::ostream& save_function(std::ostream& out) const;

  //! Read a new function tree from the given stream.
  static std::auto_ptr<MutatableImage> load_function(const FunctionRegistry& function_registry,std::istream& in,std::string& report);

  //! Obtain some statistics about the image function
  void get_stats(uint& total_nodes,uint& total_parameters,uint& depth,uint& width,real& proportion_constant) const;

  //! Check the function tree is ok.
  const bool ok() const;
};

#endif
