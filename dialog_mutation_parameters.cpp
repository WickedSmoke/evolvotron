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
DialogMutationParameters::DialogMutationParameters(QMainWindow* parent)
  :QDialog(parent)
  ,_scale(1000000)
  ,_parent(parent)
  ,_mutation_parameters(time(0))
{
  assert(_parent!=0);

  setCaption("Mutation Parameters");

  _vbox=new QVBox(this);

  _grid_buttons=new QGrid(5,Qt::Horizontal,_vbox);

  _button_reset=new QPushButton("&Reset",_grid_buttons);
  _button_cool=new QPushButton("&Cool",_grid_buttons);
  _button_heat=new QPushButton("&Heat",_grid_buttons);
  _button_shield=new QPushButton("&Shield",_grid_buttons);
  _button_irradiate=new QPushButton("&Irradiate",_grid_buttons);

  connect(_button_reset,    SIGNAL(clicked()),this,SLOT(reset()));
  connect(_button_cool,     SIGNAL(clicked()),this,SLOT(cool()));
  connect(_button_heat,     SIGNAL(clicked()),this,SLOT(heat()));
  connect(_button_shield,   SIGNAL(clicked()),this,SLOT(shield()));
  connect(_button_irradiate,SIGNAL(clicked()),this,SLOT(irradiate()));

  _grid_parameters=new QGrid(2,Qt::Horizontal,_vbox);

  new QLabel("Perturbation magnitude:",_grid_parameters);
  _spinbox_magnitude=new QSpinBox(0,_scale,maximum(1,_scale/100),_grid_parameters);
  _spinbox_magnitude->setSuffix(QString("/%1").arg(_scale));
  
  new QLabel("p(Glitch)",_grid_parameters);
  _spinbox_glitch=new QSpinBox(0,_scale,maximum(1,_scale/1000),_grid_parameters);
  _spinbox_glitch->setSuffix(QString("/%1").arg(_scale));

  new QLabel("p(Shuffle)",_grid_parameters);
  _spinbox_shuffle=new QSpinBox(0,_scale,maximum(1,_scale/1000),_grid_parameters);
  _spinbox_shuffle->setSuffix(QString("/%1").arg(_scale));

  new QLabel("p(Insert)",_grid_parameters);
  _spinbox_insert=new QSpinBox(0,_scale,maximum(1,_scale/1000),_grid_parameters);
  _spinbox_insert->setSuffix(QString("/%1").arg(_scale));

  setup_from_mutation_parameters();

  // Do this AFTER setup
  connect(_spinbox_magnitude,SIGNAL(valueChanged(int)),this,SLOT(changed_magnitude(int)));
  connect(_spinbox_glitch,SIGNAL(valueChanged(int)),this,SLOT(changed_glitch(int)));
  connect(_spinbox_shuffle,SIGNAL(valueChanged(int)),this,SLOT(changed_shuffle(int)));
  connect(_spinbox_insert,SIGNAL(valueChanged(int)),this,SLOT(changed_insert(int)));

  _vbox->setStretchFactor(_grid_parameters,1);

  _ok=new QPushButton("Hide",_vbox);

  connect(
	  _ok,SIGNAL(clicked()),
	  this,SLOT(hide())
	  );
}

void DialogMutationParameters::resizeEvent(QResizeEvent*)
{
  _vbox->resize(size());
}

void DialogMutationParameters::setup_from_mutation_parameters()
{
  _spinbox_magnitude->setValue(static_cast<int>(0.5+_scale*mutation_parameters().magnitude()));
  _spinbox_glitch   ->setValue(static_cast<int>(0.5+_scale*mutation_parameters().probability_glitch()));
  _spinbox_shuffle  ->setValue(static_cast<int>(0.5+_scale*mutation_parameters().probability_shuffle()));
  _spinbox_insert   ->setValue(static_cast<int>(0.5+_scale*mutation_parameters().probability_insert()));

  parameters_changed();
}

void DialogMutationParameters::parameters_changed()
{
  QString message
    =QString("%1/%2/%3/%4")
    .arg(_spinbox_magnitude->value())
    .arg(_spinbox_glitch->value())
    .arg(_spinbox_shuffle->value())
    .arg(_spinbox_insert->value());

  _parent->statusBar()->message(message,2000);
}

void DialogMutationParameters::reset()
{
  mutation_parameters().reset();
  setup_from_mutation_parameters();
}

void DialogMutationParameters::heat()
{
  _spinbox_magnitude->stepUp();
}

void DialogMutationParameters::cool()
{
  _spinbox_magnitude->stepDown();
}

void DialogMutationParameters::irradiate()
{
  _spinbox_glitch->stepUp();
  _spinbox_shuffle->stepUp();
  _spinbox_insert->stepUp();
}

void DialogMutationParameters::shield()
{
  _spinbox_glitch->stepDown();
  _spinbox_shuffle->stepDown();
  _spinbox_insert->stepDown();
}

void DialogMutationParameters::changed_magnitude(int v)
{
  mutation_parameters().magnitude(v/static_cast<float>(_scale));
  parameters_changed();
}

void DialogMutationParameters::changed_glitch(int v)
{
  mutation_parameters().probability_glitch(v/static_cast<float>(_scale));
  parameters_changed();
}

void DialogMutationParameters::changed_shuffle(int v)
{
  mutation_parameters().probability_shuffle(v/static_cast<float>(_scale));
  parameters_changed();
}

void DialogMutationParameters::changed_insert(int v)
{
  mutation_parameters().probability_insert(v/static_cast<float>(_scale));
  parameters_changed();
}
