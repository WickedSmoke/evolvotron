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
  \brief Implementation of class DialogMutationParameters.
*/

#include "dialog_mutation_parameters.h"

/*! About dialog displays author info, web addresses and license info.
 */
DialogMutationParameters::DialogMutationParameters(QWidget* parent,MutationParameters* p)
  :QDialog(parent)
  ,_mutation_parameters(p)
{
  setCaption("Mutation Parameters");

  _vbox=new QVBox(this);

  _grid=new QGrid(2,Qt::Horizontal,_vbox);

  _ok=new QPushButton("Hide",_vbox);

  _vbox->setStretchFactor(_grid,1);

  new QLabel("Perturbation magnitude (%):",_grid);
  new QSpinBox(0,200,1,_grid);

  //! \todo Wirte these up
  new QLabel("p(Glitch)/1000000",_grid);
  new QSpinBox(0,1000000,1000,_grid);
  new QLabel("p(Shuffle)/1000000",_grid);
  new QSpinBox(0,1000000,1000,_grid);

  //! \todo: These button settings don't seem to do anything.  Find out what's up.
  _ok->setAutoDefault(true);
  _ok->setDefault(true);

  connect(
	  _ok,SIGNAL(clicked()),
	  this,SLOT(hide())
	  );

  parameters_updated();
}

void DialogMutationParameters::resizeEvent(QResizeEvent*)
{
  _vbox->resize(size());
}

void DialogMutationParameters::parameters_updated()
{
}
