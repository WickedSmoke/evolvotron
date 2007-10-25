// Source file for evolvotron
// Copyright (C) 2002,2003,2007 Tim Day
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
  \brief Interface for class DialogAbout.
*/

#ifndef _dialog_about_h_
#define _dialog_about_h_

#include <qtextedit.h>

//! Provides an "About" dialog box.
/*! About dialog displays author info, web addresses and license info.
 */
class DialogAbout : public QDialog
{
 private:
  Q_OBJECT

  typedef QDialog Superclass;

 public:
  //! Constructor.
  DialogAbout(QWidget* parent,int n_threads);

  //! Destructor.
  virtual ~DialogAbout()
    {}

 protected:
  //! Vertical layout.
  QVBox* _vbox;

  //! Label for name/release/author etc
  QLabel* _label;

  //! Scrolling text area for GPL.
  QTextEdit* _license;

  //! Button to close dialog.
  QPushButton* _ok;

  //! Need to pass resizes on to vbox or things just get chopped.
  virtual void resizeEvent(QResizeEvent*);
};



#endif
