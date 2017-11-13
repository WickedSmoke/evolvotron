/**************************************************************************/
/*  Copyright 2012 Tim Day                                                */
/*                                                                        */
/*  This file is part of Evolvotron                                       */
/*                                                                        */
/*  Evolvotron is free software: you can redistribute it and/or modify    */
/*  it under the terms of the GNU General Public License as published by  */
/*  the Free Software Foundation, either version 3 of the License, or     */
/*  (at your option) any later version.                                   */
/*                                                                        */
/*  Evolvotron is distributed in the hope that it will be useful,         */
/*  but WITHOUT ANY WARRANTY; without even the implied warranty of        */
/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         */
/*  GNU General Public License for more details.                          */
/*                                                                        */
/*  You should have received a copy of the GNU General Public License     */
/*  along with Evolvotron.  If not, see <http://www.gnu.org/licenses/>.   */
/**************************************************************************/

/*! \file 
  \brief Interface for class DialogMutatableImageDisplay.
*/

#ifndef _dialog_mutatable_image_display_h_
#define _dialog_mutatable_image_display_h_

#include "common.h"

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

  //! Tabs for info and xml (summary and detail)
  QTabWidget* _tabs;

  //! Message displaying some info about the image.
  QLabel* _label_info;

  //! Scrolling text area for XML description.
  QTextEdit* _textedit_xml;

  //! Button to close dialog.
  QPushButton* _ok;

 public:
  //! Constructor.
  DialogMutatableImageDisplay(QWidget* parent);

  //! Destructor.
  ~DialogMutatableImageDisplay();

  //! Set content of main text and scrolling area.
  void set_content(const std::string& m,const std::string& x);
};

#endif
