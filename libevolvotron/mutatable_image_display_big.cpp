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

/*! There's not much point in dropping back to normal mode (from fullscreen) if the main
  app is fullscreen because we'll just be hidden, so close instead under such circumstances.
  (However, on ctrl-f we put both into normal mode).
 */
void MutatableImageDisplayBig::keyPressEvent(QKeyEvent* e)
{
#ifdef FULLSCREEN
  if (e->key()==Qt::Key_Escape)
    {
      if (main()->isFullScreen())
	close();
      else
	showNormal();  
    }
  else if (e->key()==Qt::Key_F && !(e->state()^Qt::ControlButton))
    {
      if (isFullScreen())
	{
	  if (main()->isFullScreen())
	    main()->showNormal();
	  showNormal();
	}
      else 
	showFullScreen();
    }
  else
#endif
    {
      // Perhaps it's for someone else
      e->ignore();
    }
}

void MutatableImageDisplayBig::resizeEvent(QResizeEvent*)
{
  _held->resize(size());
}
