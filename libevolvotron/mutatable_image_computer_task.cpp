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

#include "mutatable_image_computer_task.h"

MutatableImageComputerTask::MutatableImageComputerTask(MutatableImageDisplay*const disp,const MutatableImage* img,const QSize& s,uint lev)
  :_aborted(false)
   ,_display(disp)
   ,_image(img)
   ,_size(s)
   ,_level(lev)
   ,_pixel(0)
   ,_image_data(s.width()*s.height())
{
  assert(_image->ok());
}

/*! Destructor currently deletes its image because it owns its deepcloned copy.
  However, see todo comments in MutatableImageDisplay::image about reference counting plan for this. 
 */
MutatableImageComputerTask::~MutatableImageComputerTask()
{
  assert(_image->ok());
  delete _image;
}