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
  \brief Interface for class MutatableImageComputerTask.
*/

#ifndef _mutatable_image_computer_task_h_
#define _mutatable_image_computer_task_h_

#include <vector>

#include <qsize.h>

#include "useful.h"
#include "mutatable_image.h"
#include "mutatable_image_display.h"

//! Class encapsulating all the parameters of, and output from, a single image generation run.
class MutatableImageComputerTask
{
 protected:

  //! Flag indicating (to compute thread) that this task should be aborted.  Also indicates to MutatableImageDisplay that it's a dud and shouldn't be displayed..
  /*! \todo Think more about whether this needs to be mutex protected for sanity.
   */
  bool _aborted;

  //! The display originating the task, and to which the output will be returned.
  MutatableImageDisplay*const _display;

  //! The root node of the image tree to be generated.
  /*! Currently each task has it's own deepcloned copy of the image node tree.
    This is complete overkill as the image node tree has no state
    (although previously it did while I was attempting to optimise away constant branches)
    so some sort of reference counting scheme should be used.
    \todo Reference counting scheme to avoid excessive deepcloning of images.
   */
  const MutatableImage*const _image;

  //! The size of the image to be generated.
  const QSize _size;

  //! The resolution level of this image (0=1-for-1 pixels, 1=half res etc)
  /*! This is needed because multiple compute threads could return the completed tasks out of order
    (Unlikely given the huge difference in the amount of compute between levels, but possible).
   */
  const uint _level;

  //! Count of pixels computed, so tasks can be restarted after defer.
  uint _pixel;

  //! The image data generated.
  /*! It might have been nice to use a QImage, but every access would probably have to be qApp mutex protected.
    (Mustn't use Qt library except from main application thread).
   */
  std::vector<uint> _image_data;

 public:
  //! Constructor.
  MutatableImageComputerTask(MutatableImageDisplay*const disp,const MutatableImage* img,const QSize& s,uint lev);
  
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
  const MutatableImage*const image() const
    {
      return _image;
    }

  //! Accessor.
  const QSize& size() const
    {
      return _size;
    }

  //! Accessor.
  const uint level() const
    {
      return _level;
    }

  //! Compute task priority.  Smallest images go first.
  const uint priority() const
    {
      return size().width()*size().height();
    }

  //! Accessor.
  const std::vector<uint>& image_data() const
    {
      return _image_data;
    }

  //! Accessor.
  std::vector<uint>& image_data()
    {
      return _image_data;
    }

  //! Accessor.
  const uint pixel() const
    {
      return _pixel;
    }

  //! Increment pixel count.
  const void pixel_advance()
    {
      _pixel++;
    }
};

#endif
