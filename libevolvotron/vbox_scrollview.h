// Source file for evolvotron
// Copyright (C) 2002,2003,2004 Tim Day
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
  \brief Interface for class VBoxScrollView.
*/

#ifndef _vbox_scrollview_h_
#define _vbox_scrollview_h_

#include <qscrollview.h>
#include <qobjectlist.h>

//! Scrollview containing a vbox, with the vbox width tracking the visible width of the scrolled area (height behaves as normal).
class VBoxScrollView : public QScrollView
{
  typedef QScrollView Superclass;
 public:

  //! Constructor.
  VBoxScrollView(QWidget* parent);

  //! Handle resizes.
  virtual void resizeEvent(QResizeEvent* e);

  //! Returns the VBox.  Widgets which want to be managed by this component should use this as their parent.
  QWidget*const contentParent()
    {
      return _vbox;
    }
 protected:
  //! Vbox for layout within scroll area
  QVBox* _vbox;
};

#endif
