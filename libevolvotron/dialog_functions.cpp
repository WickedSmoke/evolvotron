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

  QGroupBox* c0=new QGroupBox(3,Qt::Horizontal,"Diluted branching ratio",_vbox);
  new QLabel("0.1",c0);
  _slider_target_branching_ratio=new QSlider(10,90,1,50,Qt::Horizontal,c0);
  QToolTip::add(_slider_target_branching_ratio,"The branching ratio is diluted to <1.0 to prevent new function-trees being infinitely large.");
  new QLabel("0.9",c0);

  QGroupBox* c1=new QGroupBox(3,Qt::Horizontal,"Dilution by constant nodes",_vbox);
  new QLabel("0.0",c1);
  _slider_proportion_constant=new QSlider(0,100,1,50,Qt::Horizontal,c1);
  new QLabel("1.0",c1);
  QToolTip::add(_slider_proportion_constant,"This specifies the proportion of diluting nodes which will be constant.");

  QGroupBox* c2=new QGroupBox(3,Qt::Horizontal,"Dilution by transforming nodes",_vbox);
  new QLabel("0.0",c2);
  _slider_identity_supression=new QSlider(0,100,1,50,Qt::Horizontal,c2);
  QToolTip::add(_slider_identity_supression,"This specifies the proportion of non-constant diluting nodes which will be transforms (c.f identity nodes).");
  new QLabel("1.0",c2);
  
  QLabel* pad=new QLabel(_vbox);
  _vbox->setStretchFactor(pad,1);

  setup_from_mutation_parameters();

  _ok=new QPushButton("OK",_vbox);

  connect(
	  _slider_target_branching_ratio,SIGNAL(valueChanged(int)),
	  this,SLOT(changed_target_branching_ratio(int))
	  );

  connect(
          _ok,SIGNAL(clicked()),
          this,SLOT(hide())
          );

  connect(
	  _slider_proportion_constant,SIGNAL(valueChanged(int)),
	  this,SLOT(changed_proportion_constant(int))
	  );
  connect(
	  _slider_identity_supression,SIGNAL(valueChanged(int)),
	  this,SLOT(changed_identity_supression(int))
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

  _slider_target_branching_ratio->setValue(static_cast<int>(100.0f*br+0.5f));

  _slider_proportion_constant->setValue(static_cast<int>(0.5f+100.0f*_mutation_parameters->proportion_constant()));
  _slider_identity_supression->setValue(static_cast<int>(0.5f+100.0f*_mutation_parameters->identity_supression()));

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

void DialogFunctions::changed_proportion_constant(int v)
{
  _mutation_parameters->proportion_constant(v/100.0f);
}

void DialogFunctions::changed_identity_supression(int v)
{
  _mutation_parameters->identity_supression(v/100.0f);
}

