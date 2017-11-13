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



#include "dialog_about.h"

#include "license.h"

DialogAbout::DialogAbout(QWidget* parent,int n_threads,bool separate_farm_for_enlargements)
  :QDialog(parent)
{
  assert(parent!=0);

  setWindowTitle("About evolvotron");
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
    << "Evolvotron "
    << stringify(EVOLVOTRON_BUILD)
    << "\n\n"
    << "Using "
    << (separate_farm_for_enlargements ? "2 pools" : "1 pool")
    << " of "
    << n_threads
    << " compute thread"
    << (n_threads>1 ? "s" : "")
    << "\n\n"
    << "Author: timday@timday.com\n\n"
    << "Home page: http://evolvotron.sourceforge.net\n"
    << "Project page: http://sourceforge.net/projects/evolvotron\n";

  QLabel*const label=new QLabel(about_string.str().c_str());
  tab_info->layout()->addWidget(label);
  label->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter|label->alignment());
  
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
