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

#include <qtooltip.h>
#include "dialog_mutation_parameters.h"

/*! About dialog displays author info, web addresses and license info.
 */
DialogMutationParameters::DialogMutationParameters(QMainWindow* parent,MutationParameters* mp)
  :QDialog(parent)
  ,_scale(1000000)
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
  QToolTip::add(_spinbox_magnitude,"Maximum size of constant perturbations");
  
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

  _checkbox_iterative=new QCheckBox("Allow iterative nodes",_vbox);
  _checkbox_fractal  =new QCheckBox("Allow fractal nodes",_vbox);

  QToolTip::add(_checkbox_iterative,"Allow computationally intensive iterative node types (required for Mandelbrot/Julia nodes).");
  QToolTip::add(_checkbox_fractal  ,"Allow Mandelbrot and Julia set nodes (also requires iterative node types).");

  setup_from_mutation_parameters();

  // Do this AFTER setup

  connect(_spinbox_magnitude,SIGNAL(valueChanged(int)),this,SLOT(changed_magnitude(int)));
  connect(_spinbox_glitch,SIGNAL(valueChanged(int)),this,SLOT(changed_glitch(int)));
  connect(_spinbox_shuffle,SIGNAL(valueChanged(int)),this,SLOT(changed_shuffle(int)));
  connect(_spinbox_insert,SIGNAL(valueChanged(int)),this,SLOT(changed_insert(int)));
  connect(_spinbox_substitute,SIGNAL(valueChanged(int)),this,SLOT(changed_substitute(int)));
 
  connect(_checkbox_iterative,SIGNAL(stateChanged(int)),this,SLOT(changed_iterative(int)));
  connect(_checkbox_fractal  ,SIGNAL(stateChanged(int)),this,SLOT(changed_fractal(int)));

  _vbox->setStretchFactor(_grid_parameters,1);

  _ok=new QPushButton("OK",_vbox);

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
  _checkbox_iterative->setChecked(_mutation_parameters->allow_iterative_nodes());
  _checkbox_fractal  ->setChecked(_mutation_parameters->allow_fractal_nodes());

  _spinbox_magnitude ->setValue(static_cast<int>(0.5+_scale*_mutation_parameters->magnitude()));
  _spinbox_glitch    ->setValue(static_cast<int>(0.5+_scale*_mutation_parameters->probability_glitch()));
  _spinbox_shuffle   ->setValue(static_cast<int>(0.5+_scale*_mutation_parameters->probability_shuffle()));
  _spinbox_insert    ->setValue(static_cast<int>(0.5+_scale*_mutation_parameters->probability_insert()));
  _spinbox_substitute->setValue(static_cast<int>(0.5+_scale*_mutation_parameters->probability_substitute()));

  parameters_changed();
}

void DialogMutationParameters::parameters_changed()
{
  QString message
    =QString("%1/%2/%3/%4/%5")
    .arg(_spinbox_magnitude->value())
    .arg(_spinbox_glitch->value())
    .arg(_spinbox_shuffle->value())
    .arg(_spinbox_insert->value())
    .arg(_spinbox_substitute->value())
    ;

  _parent->statusBar()->message(message,2000);
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
  _spinbox_glitch->stepUp();
  _spinbox_shuffle->stepUp();
  _spinbox_insert->stepUp();
  _spinbox_substitute->stepUp();
}

void DialogMutationParameters::shield()
{
  _spinbox_glitch->stepDown();
  _spinbox_shuffle->stepDown();
  _spinbox_insert->stepDown();
  _spinbox_substitute->stepDown();
}

void DialogMutationParameters::changed_magnitude(int v)
{
  _mutation_parameters->magnitude(v/static_cast<float>(_scale));
  parameters_changed();
}

void DialogMutationParameters::changed_glitch(int v)
{
  _mutation_parameters->probability_glitch(v/static_cast<float>(_scale));
  parameters_changed();
}

void DialogMutationParameters::changed_shuffle(int v)
{
  _mutation_parameters->probability_shuffle(v/static_cast<float>(_scale));
  parameters_changed();
}

void DialogMutationParameters::changed_insert(int v)
{
  _mutation_parameters->probability_insert(v/static_cast<float>(_scale));
  parameters_changed();
}

void DialogMutationParameters::changed_substitute(int v)
{
  _mutation_parameters->probability_substitute(v/static_cast<float>(_scale));
  parameters_changed();
}

/*! Disabling iterative nodes also supresses fractal nodes.
 */
void DialogMutationParameters::changed_iterative(int v)
{
  _mutation_parameters->allow_iterative_nodes(v==2);

  if (v!=2)
    {
      _mutation_parameters->allow_fractal_nodes(false);
      _checkbox_fractal->setChecked(false);
    }

  parameters_changed();
}

/*! Enabling fractal nodes also enables iterative nodes.
 */
void DialogMutationParameters::changed_fractal(int v)
{
  _mutation_parameters->allow_fractal_nodes(v==2);

  if (v==2)
    {
      _mutation_parameters->allow_iterative_nodes(true);
      _checkbox_iterative->setChecked(true);
    }
  parameters_changed();
}

