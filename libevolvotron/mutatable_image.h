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
#include "function_null.h"

//! Class to hold the base FunctionNode of an image.
/*! Once it owns a root FunctionNode* the whole structure should be fixed (mutate isn't available, only mutated).
  \todo Do reference counting on this object ?  Maybe not: have to worry about stateful nodes,
  \todo Generally tighten up const-ness of interfaces.
 */
class MutatableImage
{
 protected:

  //! Holds the top level FunctionNode of the image.
  /*! This is only used because FunctionNode::mutate can't change the type of the node it is invoked on,
    but child nodes can be zapped.
   */
  FunctionNull* _root_holder;

  //! Whether to sweep z sinusoidally (vs linearly)
  bool _sinusoidal_z;

 public:
  
  //! Take ownership of the image tree with the specified root node.
  MutatableImage(FunctionNode* r,bool sinz)
    :_root_holder(0)
    ,_sinusoidal_z(sinz)
    {
      assert(r!=0);
      std::vector<float> pv;
      std::vector<FunctionNode*> av;
      av.push_back(r);
      _root_holder=new FunctionNull(pv,av,0);
    }

  //! Create a new random image tree.
  MutatableImage(const MutationParameters& parameters,bool exciting=false)
    :_root_holder(0)
    ,_sinusoidal_z(true)
    {      
      std::vector<float> pv;
      std::vector<FunctionNode*> av;
      av.push_back(FunctionNode::stub(parameters,exciting));
      _root_holder=new FunctionNull(pv,av,0);
      //! \todo _sinusoidal_z should be obtained from AnimationParameters when it exists
    }

  //! Destructor.  NB Deletes owned image function tree.
  ~MutatableImage()
    {
      delete _root_holder;
    }

  //! Accessor.
  const FunctionNode*const root() const
    {
      return _root_holder->argptr(0);
    }

  //! Accessor.
  /*
  FunctionNode*const root()
    {
      return &(_root_holder->arg(0));
    }
  */

  //! Accessor.
  const bool sinusoidal_z() const
    {
      return _sinusoidal_z;
    }

  //! Clone this image.
  MutatableImage*const deepclone() const
    {
      return new MutatableImage(root()->deepclone(),sinusoidal_z()); 
    }

  //! Return a mutated version of this image
  MutatableImage*const mutated(const MutationParameters& p) const;

  //! Return a simplified version of this image
  MutatableImage*const simplified() const;

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

  //! Obtain some statistics about the image function
  void get_stats(uint& total_nodes,uint& total_parameters,uint& depth,uint& width,float& proportion_constant) const;

  //! Check the function tree is ok.
  const bool ok() const
    {
      return root()->ok();
    }  
};

#endif
