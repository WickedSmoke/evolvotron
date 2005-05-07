// Source file for evolvotron
// Copyright (C) 2005 Tim Day
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
  \brief Interface for class DialogFavourite.
*/

#ifndef _dialog_favourite_h_
#define _dialog_favourite_h_

#include <qdialog.h>
#include <qvbox.h>

class EvolvotronMain;

//! Provides a dialog for controlling which functions are available.
class DialogFavourite : public QDialog
{
 private:
  Q_OBJECT

  typedef QDialog Superclass;

 protected:
  //! Owner.  Also holder of favourite state.
  EvolvotronMain* _parent;

  //! Top level holder of all the dialog content.
  QVBox* _dialog_content;

 public:
  //! Constructor.
  DialogFavourite(EvolvotronMain* parent);

  //! Destructor.
  virtual ~DialogFavourite();

  //! Handle resizes.
  virtual void resizeEvent(QResizeEvent* e);

};

#endif
