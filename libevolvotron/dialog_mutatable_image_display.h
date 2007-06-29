// Source file for evolvotron
// Copyright (C) 2007 Tim Day
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
  \brief Interface for class DialogMutatableImageDisplay.
*/

#ifndef _dialog_mutatable_image_display_h_
#define _dialog_mutatable_image_display_h_

//! Provides a "Properties" style dialog box for manipulating 
/*! Make this modal for simplicity: 
  avoids spawned images changing underneath us, 
  and the possibility of opening one for every display.
 */
class DialogMutatableImageDisplay : public QDialog
{
 private:
  Q_OBJECT
    
 protected:

  //! Vertical layout.
  QVBox* _vbox;

  //! Message displaying some info about the image.
  QLabel* _message;

  //! Button to close dialog.
  QPushButton* _ok;

 public:
  //! Constructor.
  DialogMutatableImageDisplay(QWidget* parent);

  //! Destructor.
  virtual ~DialogMutatableImageDisplay()
    {}

  //! Need to pass resizes on to vbox or things just get chopped.
  virtual void resizeEvent(QResizeEvent*);

  //! Set message in text area
  void set_message(const std::string& m);
};

#endif
