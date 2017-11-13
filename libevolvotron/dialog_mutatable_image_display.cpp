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
  \brief Implementation of class DialogMutatableImageDisplay.
*/



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
