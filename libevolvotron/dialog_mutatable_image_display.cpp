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
  \brief Implementation of class DialogMutatableImageDisplay.
*/

#include "libevolvotron_precompiled.h"

#include "dialog_mutatable_image_display.h"

DialogMutatableImageDisplay::DialogMutatableImageDisplay(QWidget* parent)
  :QDialog(parent)
{
  setWindowTitle("Image Properties");
  setSizeGripEnabled(true);

  setLayout(new QVBoxLayout);

  _tabs=new QTabWidget;
  layout()->addWidget(_tabs);

  _label_info=new QLabel(QString(""));
  _tabs->addTab(_label_info,"Summary");

  _textedit_xml=new QTextEdit;
  _textedit_xml->setReadOnly(true);
  _tabs->addTab(_textedit_xml,"Detail");

  _ok=new QPushButton("OK");
  _ok->setDefault(true);
  layout()->addWidget(_ok);

  connect(
	  _ok,SIGNAL(clicked()),
	  this,SLOT(hide())
	  );
}

DialogMutatableImageDisplay::~DialogMutatableImageDisplay()
{}

void DialogMutatableImageDisplay::set_content(const std::string& m,const std::string& x)
{
  _label_info->setText(QString(m.c_str()));
  _label_info->adjustSize();

  _textedit_xml->setPlainText(x.c_str());

  adjustSize();
  updateGeometry();
}
