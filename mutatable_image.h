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
  /*! Set at image construction time and can't be changed.
   */
  FunctionNode*const _root;
 public:
  
  //! Take ownership of the image tree with the specified root node.
  MutatableImage(FunctionNode* r)
    :_root(r)
    {
      assert(_root!=0);
    }

  //! Create a new random image tree.
  MutatableImage(const MutationParameters& parameters,bool exciting=false)
    :_root(FunctionNode::stub(parameters,exciting))
    {
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

  //! Clone this image.
  MutatableImage*const deepclone() const
    {
      return new MutatableImage(_root->deepclone()); 
    }

  //! Mutate this image
  void mutate(const MutationParameters& p)
    {
      root()->mutate(p);
    }

  //! Evaluate the image at specified coordinate.
  const XYZ operator()(const XYZ& p) const
    {
      assert(root()!=0);
      return (*root())(p);
    }

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
