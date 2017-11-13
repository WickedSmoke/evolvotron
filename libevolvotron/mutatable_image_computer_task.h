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
  \brief Interface for class MutatableImageComputerTask.
*/

#ifndef _mutatable_image_computer_task_h_
#define _mutatable_image_computer_task_h_

#include "common.h"

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
  /*! Constness of the MutatableImage referenced is important as the instance is shared between all tasks and the original display.
   */
  const boost::shared_ptr<const MutatableImage> _image_function;

  //! Task priority.
  /*! Low numbers go to the head of the queue.
    The total number of samples in the complete (non-fragmented) image is used,
    so small low resolution images which can be quickly completed are run first.
   */
  const uint _priority;

  //! The origin (on the display) of the image being generated.
  const QSize _fragment_origin;

  //! The size of the image to be generated.
  const QSize _fragment_size;

  //! The full size of the image of which this is a fragment.
  const QSize _whole_image_size;

  //! Number of animation frames to be rendered
  const uint _frames;

  //! The resolution level of this image (0=1-for-1 pixels, 1=half res etc)
  /*! This is tracked because multiple compute threads could return the completed tasks out of order
    (Unlikely given the huge difference in the amount of compute between levels, but possible).
   */
  const uint _level;

  //! The fragment number, used when a rendering job is split into multiple fragments.
  const uint _fragment;

  //! The number of fragments in the rendering job
  const uint _number_of_fragments;

  //! Whether samples should be jittered.
  const bool _jittered_samples;

  //! Multisampling grid resolution e.g 4 implies a 4x4 grid
  const uint _multisample_grid;

  //@{
  //! Track pixels computed, so tasks can be restarted after defer.  Row and column are relative to the fragment origin.
  uint _current_pixel;
  int _current_col;
  int _current_row;
  uint _current_frame;
  //@}

  //! The image data generated for the fragments.
  /*! This is lazily created to avoid multiple high resolution
    images (especially with multisampling) being unnecessarily
    concurrently allocated.
   */
  mutable std::vector<QImage> _images;

  //! Lazy allocator for _images (which is mutable)
  void allocate_images() const;
  
  //! Set true by pixel_advance when it advances off the last frame.
  bool _completed;

  //! Serial number, to fix some occasional out-of-order display problems
  unsigned long long int _serial;

 public:
  //! Constructor.
  MutatableImageComputerTask
    (
     MutatableImageDisplay*const disp,
     const boost::shared_ptr<const MutatableImage>& fn,
     uint pri,
     const QSize& fo,
     const QSize& fs,
     const QSize& wis,
     uint f,
     uint lev,
     uint frag,
     uint nfrag,
     bool j,
     uint ms,
     unsigned long long int n
     );
  
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
  MutatableImageDisplay* display() const
    {
      return _display;
    }

  //! Accessor.
  const boost::shared_ptr<const MutatableImage>& image_function() const
    {
      return _image_function;
    }

  //! Accessor.
  const QSize& fragment_origin() const
    {
      return _fragment_origin;
    }

  //! Accessor.
  const QSize& fragment_size() const
    {
      return _fragment_size;
    }

  //! Accessor.
  const QSize& whole_image_size() const
    {
      return _whole_image_size;
    }

  //! Accessor.
  uint frames() const
    {
      return _frames;
    }

  //! Accessor.
  uint level() const
    {
      return _level;
    }

  //! Accessor.
  uint fragment() const
    {
      return _fragment;
    }

  //! Accessor.
  uint number_of_fragments() const
    {
      return _number_of_fragments;
    }

  //! Accessor.
  bool jittered_samples() const
    {
      return _jittered_samples;
    }

  //! Accessor.
  uint multisample_grid() const
    {
      return _multisample_grid;
    }

  //! Serial number
  unsigned long long int serial() const
    {
      return _serial;
    }

  //! Accessor.
  uint priority() const
    {
      return _priority;
    }

  //! Accessor, with lazy creation.
  std::vector<QImage>& images()
    {
      if (_images.empty()) allocate_images();
      return _images;
    }

  //! Accessor.
  const std::vector<QImage>& images() const
    {
      if (_images.empty()) allocate_images();
      return _images;
    }

  //! Accessor.
  uint current_col() const
    {
      return _current_col;
    }

  //! Accessor.
  uint current_row() const
    {
      return _current_row;
    }

  //! Accessor.
  uint current_frame() const
    {
      return _current_frame;
    }

  //! Accessor.
  uint current_pixel() const
    {
      return _current_pixel;
    }

  //!Accessor.
  bool completed() const
    {
      return _completed;
    }

  //! Increment pixel count, set completed flag if advanced off end of last frame.
  void pixel_advance();
};

#endif
