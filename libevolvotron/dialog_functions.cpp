// Source file for evolvotron
// Copyright (C) 2002,2003,2004 Tim Day
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
  \brief Implementation of class DialogFunctions.
*/

#include <iostream>

#include <qtooltip.h>
#include <qgroupbox.h>
#include "dialog_functions.h"

/*! About dialog displays author info, web addresses and license info.
 */
DialogFunctions::DialogFunctions(QMainWindow* parent,MutationParameters* mp)
  :QDialog(parent)
  ,_parent(parent)
   ,_mutation_parameters(mp)
{
  assert(_parent!=0);

  setCaption("Functions");

  _vbox=new QVBox(this);

  _branching_ratio=new QLabel(_vbox);

  QGroupBox* c=new QGroupBox(3,Qt::Horizontal,"Diluted branching ratio",_vbox);
  new QLabel("0.1",c);
  _target_branching_ratio_slider=new QSlider(10,90,1,50,Qt::Horizontal,c);
  QToolTip::add(_target_branching_ratio_slider,"The branching ratio is diluted to <1.0 to prevent new function-trees being infinitely large.");
  new QLabel("0.9",c);

  setup_from_mutation_parameters();

  _ok=new QPushButton("OK",_vbox);

  connect(
	  _target_branching_ratio_slider,SIGNAL(valueChanged(int)),
	  this,SLOT(changed_target_branching_ratio(int))
	  );

  connect(
          _ok,SIGNAL(clicked()),
          this,SLOT(hide())
          );
}

DialogFunctions::~DialogFunctions()
{}

void DialogFunctions::resizeEvent(QResizeEvent*)
{
  _vbox->resize(size());
}

void DialogFunctions::setup_from_mutation_parameters()
{
  const float b=_mutation_parameters->random_function_branching_ratio();
  const float s=_mutation_parameters->proportion_basic();
  const float br=(1.0f-s)*b;

  std::stringstream msg;
  msg 
    << "Undiluted random function branching ratio: " << b << "\n"
    << "Dilution proportion: " << s;
  _branching_ratio->setText(msg.str().c_str());

  _target_branching_ratio_slider->setValue(static_cast<int>(100.0f*br+0.5f));
}


void DialogFunctions::changed_target_branching_ratio(int v)
{
  const float target_branching_ratio=v/100.0f;
  
  // Want to solve tgt=proportion_basic*0.0+(1.0-proportion_basic)*random_function_branching_ratio
  const float proportion_basic=1.0f-target_branching_ratio/_mutation_parameters->random_function_branching_ratio();

  std::clog 
    << "Basic-node dilution proportion set to " 
    << proportion_basic 
    << " giving a branching ratio of "
    << (1.0-proportion_basic)*_mutation_parameters->random_function_branching_ratio()
    << "\n";

  _mutation_parameters->proportion_basic(proportion_basic);

  setup_from_mutation_parameters();
}
