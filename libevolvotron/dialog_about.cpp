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
  \brief Implementation of class DialogAbout.
*/

#include "libevolvotron_precompiled.h"

#include "dialog_about.h"

#include "license.h"

DialogAbout::DialogAbout(QWidget* parent,int n_threads)
  :QDialog(parent)
{
  setCaption("About evolvotron");
  setMinimumSize(400,300);

  _vbox=new QVBox(this);

  std::ostringstream about_string;
  about_string
    << "Evolvotron "
    << EVOLVOTRON_BUILD
    << "\n"
    << "Using "
    << n_threads
    << " compute thread" 
    << (n_threads>1 ? "s" : "") << "\n\n"
    << "Author: timday@timday.com\n"
    << "Home page: http://evolvotron.sourceforge.net\n"
    << "Project page: http://sourceforge.net/projects/evolvotron\n";

  _label=new QLabel(about_string.str().c_str(),_vbox);
  _label->setAlignment(Qt::AlignHCenter|_label->alignment());
  
  _license=new QTextEdit(_vbox);
  _license->setReadOnly(true);
  _license->setTextFormat(PlainText);
  _license->setText((std::string("License:\n")+std::string(license_string)).c_str());

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
