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
  \brief Implementation of class MutatableImageDisplayBig.
*/

#include "mutatable_image_display_big.h"
#include "evolvotron_main.h"

/*! The constructor is passed:
    - the owning widget (expected to be null but it might get used somewhere else one day)
    - the EvolvotronMain providing spawn and farm services,
    - the MutatableImageDisplay to be held
 */
MutatableImageDisplayBig::MutatableImageDisplayBig(QWidget* parent,EvolvotronMain* mn)
  :QWidget(parent,0,Qt::WDestructiveClose)
   ,_main(mn)
   ,_held(0)
{}

/*! Don't think destructor needs to do anything to _display... Qt takes care of it
 */
MutatableImageDisplayBig::~MutatableImageDisplayBig()
{}

void MutatableImageDisplayBig::resizeEvent(QResizeEvent*)
{
  _held->resize(size());
}
