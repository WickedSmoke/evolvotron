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
  \brief Implementation of class DialogAbout.
*/


#include <sstream>
#include "dialog_about.h"

#include "license.h"

DialogAbout::DialogAbout(QWidget* parent,int n_threads,bool separate_farm_for_enlargements)
  :QDialog(parent)
{
  assert(parent!=0);

  setWindowTitle("About Evolvotron");
  setMinimumSize(480,360);
  setSizeGripEnabled(true);

  setLayout(new QVBoxLayout);

  QTabWidget*const tabs=new QTabWidget;
  layout()->addWidget(tabs);

  QWidget*const tab_info=new QWidget;
  tab_info->setLayout(new QVBoxLayout);
  tabs->addTab(tab_info,"Info");

  QWidget*const tab_license=new QWidget;
  tab_license->setLayout(new QVBoxLayout);
  tabs->addTab(tab_license,"License");

  std::ostringstream about_string;
  about_string
    << "<h1>Evolvotron</h1>\n"
    << "<h3>" APP_BUILD "</h3>\n"
    << "<p>Using "
    << (separate_farm_for_enlargements ? "2 pools" : "1 pool")
    << " of "
    << n_threads
    << " compute thread"
    << (n_threads>1 ? "s" : "")
    << "</p>\n"
       "<p>Authors: Tim Day, "
       "<a href=\"mailto:wickedsmoke@users.sf.net\">Karl Robillard</a></p>\n"
       "<p><a href=\"http://www.bottlenose.net/share/evolvotron/\">[Home Page]"
       "</a>&nbsp;&nbsp;"
       "<a href=\"http://sourceforge.net/projects/evolvotron\">[Project Page]"
       "</a></p>\n";

  QLabel*const label=new QLabel(about_string.str().c_str());
  tab_info->layout()->addWidget(label);
  label->setIndent(32);
  label->setAlignment(Qt::AlignTop);
  label->setOpenExternalLinks(true);
  
  QTextEdit*const license=new QTextEdit;
  tab_license->layout()->addWidget(license);
  license->setReadOnly(true);
  license->setPlainText((std::string("License:\n")+std::string(license_string)).c_str());

  QPushButton* ok=new QPushButton("OK");
  layout()->addWidget(ok);
  ok->setDefault(true);

  connect
    (
     ok,SIGNAL(clicked()),
     this,SLOT(hide())
     );
}

DialogAbout::~DialogAbout()
{}
