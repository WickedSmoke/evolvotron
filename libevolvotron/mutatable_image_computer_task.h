// Source file for evolvotron
// Copyright (C) 2002,2003,2007 Tim Day
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
  \brief Interface for class MutatableImageComputerTask.
*/

#ifndef _mutatable_image_computer_task_h_
#define _mutatable_image_computer_task_h_

#include <qsize.h>

#include "mutatable_image.h"
#include "mutatable_image_display.h"

//! Class encapsulating all the parameters of, and output from, a single image generation run.
class MutatableImageComputerTask
#ifndef NDEBUG
: public InstanceCounted
#endif
{
 protected:

  //! Flag indicating (to compute thread) that this task should be aborted.  Also indicates to MutatableImageDisplay that it's a dud and shouldn't be displayed..
  /*! \todo Think more about whether this needs to be mutex protected for sanity.
   */
  bool _aborted;

  //! The display originating the task, and to which the output will be returned.
  MutatableImageDisplay*const _display;

  //! The root node of the image tree to be generated.
  /*! Constness of the MutatableImage referenced is important as the instance is shared between all tasks and the original display.
   */
  const boost::shared_ptr<const MutatableImage> _image;

  //! The size of the image to be generated.
  const QSize _size;

  //! Number of animation frames to be rendered
  const uint _frames;

  //! The resolution level of this image (0=1-for-1 pixels, 1=half res etc)
  /*! This is tracked because multiple compute threads could return the completed tasks out of order
    (Unlikely given the huge difference in the amount of compute between levels, but possible).
   */
  const uint _level;

  //@{
  //! Track pixels computed, so tasks can be restarted after defer.
  uint _current_pixel;
  int _current_col;
  int _current_row;
  uint _current_frame;
  //@}

  //! The image data generated.
  /*! It might have been nice to use a QImage, but every access would probably have to be qApp mutex protected.
    (Mustn't use Qt library except from main application thread).
   */
  boost::shared_array<uint> _image_data;

  //! Set true by pixel_advance when it advances off the last frame.
  bool _completed;

  //! Serial number, to fix some occasional out-of-order display problems
  unsigned long long int _serial;

 public:
  //! Constructor.
  MutatableImageComputerTask(MutatableImageDisplay*const disp,const boost::shared_ptr<const MutatableImage>& img,const QSize& s,uint f,uint lev,unsigned long long int n);
  
  //! Destructor.
  ~MutatableImageComputerTask();

  //! Accessor.
  bool aborted() const
    {
      return _aborted;
    }

  //! Mark task as aborted.
  void abort()
    {
      _aborted=true;
    }

  //! Accessor.
  MutatableImageDisplay*const display() const
    {
      return _display;
    }

  //! Accessor.
  const boost::shared_ptr<const MutatableImage>& image() const
    {
      return _image;
    }

  //! Accessor.
  const QSize& size() const
    {
      return _size;
    }

  //! Accessor.
  const uint frames() const
    {
      return _frames;
    }

  //! Accessor.
  const uint level() const
    {
      return _level;
    }

  //! Serial number
  const unsigned long long int serial() const
    {
      return _serial;
    }

  //! Compute task priority.  Smallest images go first.
  const uint priority() const
    {
      return size().width()*size().height();
    }

  //! Accessor.
  const boost::shared_array<uint>& image_data() const
    {
      return _image_data;
    }

  //! Accessor.
  boost::shared_array<uint>& image_data()
    {
      return _image_data;
    }

  //! Accessor.
  const uint current_col() const
    {
      return _current_col;
    }

  //! Accessor.
  const uint current_row() const
    {
      return _current_row;
    }

  //! Accessor.
  const uint current_frame() const
    {
      return _current_frame;
    }

  //! Accessor.
  const uint current_pixel() const
    {
      return _current_pixel;
    }

  //!Accessor.
  const bool completed() const
    {
      return _completed;
    }

  //! Increment pixel count, set completed flag if advanced off end of last frame.
  void pixel_advance();
};

#endif
