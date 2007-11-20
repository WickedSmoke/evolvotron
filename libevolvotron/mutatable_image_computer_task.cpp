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
  \brief Implementation of class MutatableImageComputerTask
*/

#include "libevolvotron_precompiled.h"

#include "mutatable_image_computer_task.h"

MutatableImageComputerTask::MutatableImageComputerTask
(
 MutatableImageDisplay*const disp,
 const boost::shared_ptr<const MutatableImage>& img,
 const QSize& o,
 const QSize& s,
 uint f,
 uint lev,
 uint frag,
 uint nfrag,
 bool j,
 uint ms,
 unsigned long long int n
 )
  :
#ifndef NDEBUG
  InstanceCounted(typeid(this).name(),false),
#endif
  _aborted(false)
  ,_display(disp)
  ,_image(img)
  ,_origin(o)
  ,_size(s)
  ,_frames(f)
  ,_level(lev)
  ,_fragment(frag)
  ,_number_of_fragments(nfrag)
  ,_jittered_samples(j)
  ,_multisample_level(ms)
  ,_current_pixel(0)
  ,_current_col(0)
  ,_current_row(0)
  ,_current_frame(0)
  ,_image_data(new uint[s.width()*s.height()*f])
  ,_completed(false)
  ,_serial(n)
{
  assert(_image->ok());
  assert(_fragment<_number_of_fragments);
}

MutatableImageComputerTask::~MutatableImageComputerTask()
{
  assert(_image->ok());
}

void MutatableImageComputerTask::pixel_advance()
{
  _current_pixel++;
  _current_col++;
  if (_current_col==size().width())
    {
      _current_col=0;
      _current_row++;
      if (_current_row==size().height())
	{
	  _current_row=0;
	  _current_frame++;
	  if (_current_frame==frames())
	    {
	      _completed=true;
	    }
	}
    }
}
