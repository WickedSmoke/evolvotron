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
  \brief Interfaces for class MutatableImage.
*/

#ifndef _mutatable_image_h_
#define _mutatable_image_h_

#include <iosfwd>

#include "function_node.h"

//! Class to hold the base FunctionNode of an image.
/*! \todo Do reference counting on this object.
 */
class MutatableImage
{
 protected:

  //! The top level FunctionNode of the image.
  FunctionNode* _root;

  //! Whether to sweep z sinusoidally (vs linearly)
  bool _sinusoidal_z;

 public:
  
  //! Take ownership of the image tree with the specified root node.
  MutatableImage(FunctionNode* r,bool sinz)
    :_root(r)
    ,_sinusoidal_z(sinz)
    {
      assert(_root!=0);
    }

  //! Create a new random image tree.
  MutatableImage(const MutationParameters& parameters,bool exciting=false)
    :_root(FunctionNode::stub(parameters,exciting))
    ,_sinusoidal_z(true)
    {
      //! \todo _sinusoidal_z should be obtained from AnimationParameters when it exists
      assert(_root!=0);
    }

  //! Destructor.  NB Deletes owned image function tree.
  ~MutatableImage()
    {
      delete _root;
    }

  //! Accessor.
  const FunctionNode*const root() const
    {
      return _root;
    }

  //! Accessor.
  FunctionNode*const root()
    {
      return _root;
    }

  //! Accessor.
  const bool sinusoidal_z() const
    {
      return _sinusoidal_z;
    }

  //! Clone this image.
  MutatableImage*const deepclone() const
    {
      return new MutatableImage(_root->deepclone(),_sinusoidal_z); 
    }

  //! Mutate this image
  void mutate(const MutationParameters& p);

  //! Evaluate the image at specified coordinate.
  const XYZ operator()(const XYZ& p) const
    {
      assert(root()!=0);
      return (*root())(p);
    }

  void get_rgb(const XYZ& p,uint c[3]) const;

  //! Return whether image value is independent of position.
  const bool is_constant() const
    {
      return root()->is_constant();
    }

  //! Save the function-tree to the stream
  std::ostream& save_function(std::ostream& out) const;

  //! Read a new function tree from the given stream.
  static MutatableImage*const load_function(std::istream& in,std::string& report);

  //! Check the function tree is ok.
  const bool ok() const
    {
      return root()->ok();
    }  
};

#endif
