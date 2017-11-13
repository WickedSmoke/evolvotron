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
  \brief Implementation of class MutatableImageComputerTask
*/



#include "mutatable_image_computer_task.h"

MutatableImageComputerTask::MutatableImageComputerTask
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
 )
  :_aborted(false)
  ,_display(disp)
  ,_image_function(fn)
  ,_priority(pri)
  ,_fragment_origin(fo)
  ,_fragment_size(fs)
  ,_whole_image_size(wis)
  ,_frames(f)
  ,_level(lev)
  ,_fragment(frag)
  ,_number_of_fragments(nfrag)
  ,_jittered_samples(j)
  ,_multisample_grid(ms)
  ,_current_pixel(0)
  ,_current_col(0)
  ,_current_row(0)
  ,_current_frame(0)
  ,_completed(false)
  ,_serial(n)
{
  /*
  std::cerr 
    << "[" 
    << _number_of_fragments 
    << ":" 
    << _fragment_size.width() << "x" << _fragment_size.height() 
    << " in " 
    << _whole_image_size.width() << "x" << _whole_image_size.height()
    << "]";
  */
  assert(_image_function->ok());
  assert(_fragment<_number_of_fragments);
  assert(_number_of_fragments>1 || _whole_image_size==_fragment_size);
  assert(1<=_multisample_grid);
}

void MutatableImageComputerTask::allocate_images() const
{
  for (uint f=0;f<frames();f++)
    {
      _images.push_back(QImage(fragment_size(),QImage::Format_RGB32));
    }
}

MutatableImageComputerTask::~MutatableImageComputerTask()
{
  assert(_image_function->ok());
}

void MutatableImageComputerTask::pixel_advance()
{
  _current_pixel++;
  _current_col++;
  if (_current_col==fragment_size().width())
    {
      _current_col=0;
      _current_row++;
      if (_current_row==fragment_size().height())
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
