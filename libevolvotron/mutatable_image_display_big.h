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
  \brief Interface for class MutatableImageDisplayBig
*/

#ifndef _mutatable_image_display_big_h_
#define _mutatable_image_display_big_h_

#include <qwidget.h>

#include "useful.h"

class EvolvotronMain;

//! Intended to be used as a top-level widget holding a single MutatableImageDisplay
/*! We just used to use a display or scroll view itself as a top-level widget,
  but need this to get some specific keyboard effects.
  As a MutatableImageDisplayHolder it actually has more in common with EvolvotronMain.
  \todo class name is a bit misleading.  This is really just a slightly modified top-level holder.
 */
class MutatableImageDisplayBig : public QWidget
{
  Q_OBJECT

 protected:
  //! Pointer back to the application object to access services.
  EvolvotronMain* _main;

  //! The widget being held.  Probably be a QScrollView or MutatableImageDisplay.
  QWidget* _held;

 public:
  //! Constructor.
  MutatableImageDisplayBig(QWidget* parent,EvolvotronMain* mn);

  //! Destructor.
  virtual ~MutatableImageDisplayBig();

  //! Accessor.
  EvolvotronMain*const main() const
    {
      assert(_main!=0);
      return _main;
    }

  //! Accessor.
  void hold(QWidget* w)
    {
      _held=w;
    }

 protected:
  virtual void resizeEvent(QResizeEvent*);

};

#endif
