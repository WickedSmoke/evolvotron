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
  \brief Implementation of class VBoxScrollView.
*/

#include <iostream>

#include "vbox_scrollview.h"

VBoxScrollView::VBoxScrollView(QWidget* parent)
  :QScrollView(parent)
{
  setHScrollBarMode(QScrollView::AlwaysOff);
  enableClipper(true);
  
  QSizePolicy spx(QSizePolicy::Expanding,QSizePolicy::Preferred);
  setSizePolicy(spx);
  
  _vbox=new QVBox(viewport());
  _vbox->setSizePolicy(spx);
  
  addChild(_vbox);
}

/*! The contained VBox is resized so it's width tracks the visible width of the scrollview.
 */
void VBoxScrollView::resizeEvent(QResizeEvent* e)
{
  Superclass::resizeEvent(e);
  
  std::clog 
    << "VBoxScrollView::ResizeEvent : with size " 
    << size().width() << "x" << size().height() 
    << " and contents size "
    << contentsWidth() << "x" << contentsHeight()
    << "\n";
    
  _vbox->resize(visibleWidth(),_vbox->size().height());
}

