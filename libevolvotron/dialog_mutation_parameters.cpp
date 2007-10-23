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
  \brief Implementation of class DialogMutationParameters.
*/

#include "libevolvotron_precompiled.h"

#include "dialog_mutation_parameters.h"

#include <qtooltip.h>

/*! About dialog displays author info, web addresses and license info.
 */
DialogMutationParameters::DialogMutationParameters(QMainWindow* parent,MutationParametersQObject* mp)
  :QDialog(parent)
  ,_scale(100)
  ,_parent(parent)
  ,_mutation_parameters(mp)
{
  assert(_parent!=0);

  setCaption("Mutation Parameters");

  _vbox=new QVBox(this);

  _grid_buttons=new QGrid(5,Qt::Horizontal,_vbox);

  _button_reset=new QPushButton("&Reset",_grid_buttons);
  _button_cool=new QPushButton("&Cool",_grid_buttons);
  _button_shield=new QPushButton("&Shield",_grid_buttons);
  _button_heat=new QPushButton("&Heat",_grid_buttons);
  _button_irradiate=new QPushButton("&Irradiate",_grid_buttons);

  QToolTip::add(_button_cool,"Decrease size of constant perturbations during mutation");
  QToolTip::add(_button_heat,"Increase size of constant perturbations during mutation");
  QToolTip::add(_button_shield,"Decrease probability of function tree structural mutations");
  QToolTip::add(_button_irradiate,"Increase probability of function tree structural mutations");  

  connect(_button_reset,    SIGNAL(clicked()),this,SLOT(reset()));
  connect(_button_cool,     SIGNAL(clicked()),this,SLOT(cool()));
  connect(_button_heat,     SIGNAL(clicked()),this,SLOT(heat()));
  connect(_button_shield,   SIGNAL(clicked()),this,SLOT(shield()));
  connect(_button_irradiate,SIGNAL(clicked()),this,SLOT(irradiate()));

  _grid_parameters=new QGrid(2,Qt::Horizontal,_vbox);

  new QLabel("Perturbation magnitude:",_grid_parameters);
  _spinbox_magnitude=new QSpinBox(0,_scale,maximum(1,_scale/100),_grid_parameters);
  _spinbox_magnitude->setSuffix(QString("/%1").arg(_scale));
  QToolTip::add(_spinbox_magnitude,"Scale of function parameter perturbations");
  
  new QLabel("p(Parameter reset)",_grid_parameters);
  _spinbox_parameter_reset=new QSpinBox(0,_scale,maximum(1,_scale/1000),_grid_parameters);
  _spinbox_parameter_reset->setSuffix(QString("/%1").arg(_scale));
  QToolTip::add(_spinbox_parameter_reset,"Probability of function parameters being completely reset");

  new QLabel("p(Glitch)",_grid_parameters);
  _spinbox_glitch=new QSpinBox(0,_scale,maximum(1,_scale/1000),_grid_parameters);
  _spinbox_glitch->setSuffix(QString("/%1").arg(_scale));
  QToolTip::add(_spinbox_glitch,"Probability of function branch being replaced by new random stub");

  new QLabel("p(Shuffle)",_grid_parameters);
  _spinbox_shuffle=new QSpinBox(0,_scale,maximum(1,_scale/1000),_grid_parameters);
  _spinbox_shuffle->setSuffix(QString("/%1").arg(_scale));
  QToolTip::add(_spinbox_shuffle,"Probability of function branches being reordered");

  new QLabel("p(Insert)",_grid_parameters);
  _spinbox_insert=new QSpinBox(0,_scale,maximum(1,_scale/1000),_grid_parameters);
  _spinbox_insert->setSuffix(QString("/%1").arg(_scale));
  QToolTip::add(_spinbox_insert,"Probability of function branch having random stub inserted");

  new QLabel("p(Substitute)",_grid_parameters);
  _spinbox_substitute=new QSpinBox(0,_scale,maximum(1,_scale/1000),_grid_parameters);
  _spinbox_substitute->setSuffix(QString("/%1").arg(_scale));
  QToolTip::add(_spinbox_substitute,"Probability of function node's type being changed");

  new QLabel("Autocool",_grid_parameters);
  _checkbox_autocool_enable=new QCheckBox(_grid_parameters);
  QToolTip::add(_checkbox_autocool_enable,"Autocooling reduces the strength and probablility of mutations with increasing numbers of generations.\nUnselect and re-select to reset the generation count.");

  new QLabel("Autocool halflife",_grid_parameters);
  _spinbox_autocool_halflife=new QSpinBox(1,1000,1,_grid_parameters);
  QToolTip::add(_spinbox_autocool_halflife,"Number of generations needed to halve mutation influence when autocooling");

  setup_from_mutation_parameters();

  // Do this AFTER setup

  connect(_spinbox_magnitude,SIGNAL(valueChanged(int)),this,SLOT(changed_magnitude(int)));
  connect(_spinbox_parameter_reset,SIGNAL(valueChanged(int)),this,SLOT(changed_parameter_reset(int)));
  connect(_spinbox_glitch,SIGNAL(valueChanged(int)),this,SLOT(changed_glitch(int)));
  connect(_spinbox_shuffle,SIGNAL(valueChanged(int)),this,SLOT(changed_shuffle(int)));
  connect(_spinbox_insert,SIGNAL(valueChanged(int)),this,SLOT(changed_insert(int)));
  connect(_spinbox_substitute,SIGNAL(valueChanged(int)),this,SLOT(changed_substitute(int)));

  connect(_checkbox_autocool_enable,SIGNAL(stateChanged(int)),this,SLOT(changed_autocool_enable(int)));
  connect(_spinbox_autocool_halflife,SIGNAL(valueChanged(int)),this,SLOT(changed_autocool_halflife(int)));
 
  _vbox->setStretchFactor(_grid_parameters,1);

  _ok=new QPushButton("OK",_vbox);

  connect(
	  _ok,SIGNAL(clicked()),
	  this,SLOT(hide())
	  );

  connect(
	  _mutation_parameters,SIGNAL(changed()),
	  this,SLOT(mutation_parameters_changed())
	  );
}

void DialogMutationParameters::resizeEvent(QResizeEvent*)
{
  _vbox->resize(size());
}

void DialogMutationParameters::setup_from_mutation_parameters()
{
  _spinbox_magnitude        ->setValue(static_cast<int>(0.5+_scale*_mutation_parameters->base_magnitude_parameter_variation()));
  _spinbox_parameter_reset  ->setValue(static_cast<int>(0.5+_scale*_mutation_parameters->base_probability_parameter_reset()));
  _spinbox_glitch           ->setValue(static_cast<int>(0.5+_scale*_mutation_parameters->base_probability_glitch()));
  _spinbox_shuffle          ->setValue(static_cast<int>(0.5+_scale*_mutation_parameters->base_probability_shuffle()));
  _spinbox_insert           ->setValue(static_cast<int>(0.5+_scale*_mutation_parameters->base_probability_insert()));
  _spinbox_substitute       ->setValue(static_cast<int>(0.5+_scale*_mutation_parameters->base_probability_substitute()));

  _checkbox_autocool_enable->setChecked(_mutation_parameters->autocool_enable());
  _spinbox_autocool_halflife->setValue(_mutation_parameters->autocool_halflife());
}

void DialogMutationParameters::reset()
{
  _mutation_parameters->reset();
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
  _spinbox_parameter_reset->stepUp();
  _spinbox_glitch->stepUp();
  _spinbox_shuffle->stepUp();
  _spinbox_insert->stepUp();
  _spinbox_substitute->stepUp();
}

void DialogMutationParameters::shield()
{
  _spinbox_parameter_reset->stepDown();
  _spinbox_glitch->stepDown();
  _spinbox_shuffle->stepDown();
  _spinbox_insert->stepDown();
  _spinbox_substitute->stepDown();
}

void DialogMutationParameters::changed_autocool_enable(int buttonstate)
{
  if (buttonstate==QButton::On) _mutation_parameters->autocool_enable(true);
  else if (buttonstate==QButton::Off) _mutation_parameters->autocool_enable(false);
}

void DialogMutationParameters::changed_magnitude(int v)
{
  _mutation_parameters->base_magnitude_parameter_variation(v/static_cast<real>(_scale));
}

void DialogMutationParameters::changed_parameter_reset(int v)
{
  _mutation_parameters->base_probability_parameter_reset(v/static_cast<real>(_scale));
}

void DialogMutationParameters::changed_glitch(int v)
{
  _mutation_parameters->base_probability_glitch(v/static_cast<real>(_scale));
}

void DialogMutationParameters::changed_shuffle(int v)
{
  _mutation_parameters->base_probability_shuffle(v/static_cast<real>(_scale));
}

void DialogMutationParameters::changed_insert(int v)
{
  _mutation_parameters->base_probability_insert(v/static_cast<real>(_scale));
}

void DialogMutationParameters::changed_substitute(int v)
{
  _mutation_parameters->base_probability_substitute(v/static_cast<real>(_scale));
}

void DialogMutationParameters::changed_autocool_halflife(int v)
{
  _mutation_parameters->autocool_halflife(v);
}

void DialogMutationParameters::mutation_parameters_changed()
{
  setup_from_mutation_parameters();
}
