// Source file for evolvotron
// Copyright (C) 2002 Tim Day
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
  \brief Implementation of class DialogAbout.
*/

#include "dialog_about.h"

/*! About dialog displays author info, web addresses and license info.
 */
DialogAbout::DialogAbout(QWidget* parent)
  :QDialog(parent)
{
  setCaption("About evolvotron");
  setMinimumSize(400,300);

  _vbox=new QVBox(this);

  // This should be the ONLY place a release number appears (other than CHANGES and the tar file name).  
  // I can't be doing with changing release numbers in a zillion places... one ALWAYS gets missed.
  _label=new QLabel("\nEvolvotron 0.0.1\n\nAuthor: timday@timday.com\n\nHome page: http://evolvotron.sourceforge.net\n\nProject page: http://sourceforge.net/projects/evolvotron\n\nLicense:",_vbox);
  _label->setAlignment(Qt::AlignHCenter|_label->alignment());
  
  _license=new QTextEdit(_vbox);
  _license->setReadOnly(true);
  _license->setTextFormat(PlainText);
  _license->setText(license_string);

  _ok=new QPushButton("OK",_vbox);

  //! \todo: These button settings don't seem to do anything.  Find out what's up.
  _ok->setAutoDefault(true);
  _ok->setDefault(true);

  connect(
	  _ok,SIGNAL(clicked()),
	  this,SLOT(hide())
	  );
}

void DialogAbout::resizeEvent(QResizeEvent*)
{
  _vbox->resize(size());
}
