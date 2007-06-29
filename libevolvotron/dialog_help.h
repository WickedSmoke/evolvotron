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
  \brief Interface for class DialogHelp.
*/

#ifndef _dialog_help_h_
#define _dialog_help_h_

#include <qtextbrowser.h>

//! Provides a dialog box with some user documentation.
/*! More of a quick reference guide than a manual.
 */
class DialogHelp : public QDialog
{
 private:
  Q_OBJECT

 public:
  //! Constructor.
  DialogHelp(QWidget* parent,bool full);

  //! Destructor.
  virtual ~DialogHelp()
    {}

 protected:
  //! Vertical layout.
  QVBox* _vbox;

  //! Label for name/release/author etc
  QTextBrowser* _browser;

  //! Button to close dialog.
  QPushButton* _ok;

  //! Need to pass resizes on to vbox or things just get chopped.
  virtual void resizeEvent(QResizeEvent*);
};



#endif
