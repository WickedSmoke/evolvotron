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
  \brief Implementation of class DialogMutationParameters.
*/



#include "dialog_mutation_parameters.h"

DialogMutationParameters::DialogMutationParameters(QMainWindow* parent,MutationParametersQObject* mp)
  :QDialog(parent)
  ,_scale(100)
  ,_parent(parent)
  ,_mutation_parameters(mp)
{
  assert(_parent!=0);

  setWindowTitle("Mutation Parameters");
  setSizeGripEnabled(true);

  setLayout(new QVBoxLayout);

  _tabs=new QTabWidget();
  layout()->addWidget(_tabs);

  _vbox_base_mutation=new QWidget;
  _vbox_base_mutation->setLayout(new QVBoxLayout);
  _tabs->addTab(_vbox_base_mutation,"Base");

  _grid_buttons=new QWidget;
  _vbox_base_mutation->layout()->addWidget(_grid_buttons);
  
  QGridLayout*const grid_buttons_layout=new QGridLayout();
  _grid_buttons->setLayout(grid_buttons_layout);

  grid_buttons_layout->addWidget(_button_reset=new QPushButton("&Reset",_grid_buttons),0,0);
  grid_buttons_layout->addWidget(_button_cool=new QPushButton("&Cool",_grid_buttons),0,1);
  grid_buttons_layout->addWidget(_button_shield=new QPushButton("&Shield",_grid_buttons),0,2);
  grid_buttons_layout->addWidget(_button_heat=new QPushButton("&Heat",_grid_buttons),0,3);
  grid_buttons_layout->addWidget(_button_irradiate=new QPushButton("&Irradiate",_grid_buttons),0,4);

  _button_cool->setToolTip("Decrease size of constant perturbations during mutation.");
  _button_heat->setToolTip("Increase size of constant perturbations during mutation.");
  _button_shield->setToolTip("Decrease probability of function tree structural mutations.");
  _button_irradiate->setToolTip("Increase probability of function tree structural mutations.");  

  connect(_button_reset,    SIGNAL(clicked()),this,SLOT(reset()));
  connect(_button_cool,     SIGNAL(clicked()),this,SLOT(cool()));
  connect(_button_heat,     SIGNAL(clicked()),this,SLOT(heat()));
  connect(_button_shield,   SIGNAL(clicked()),this,SLOT(shield()));
  connect(_button_irradiate,SIGNAL(clicked()),this,SLOT(irradiate()));

  _vbox_autocool=new QWidget;
  _vbox_autocool->setLayout(new QVBoxLayout);
  _tabs->addTab(_vbox_autocool,"Autocool");

  _grid_base_mutation=new QWidget;
  _vbox_base_mutation->layout()->addWidget(_grid_base_mutation);

  QGridLayout*const grid_base_mutation_layout=new QGridLayout();
  _grid_base_mutation->setLayout(grid_base_mutation_layout);

  grid_base_mutation_layout->addWidget(new QLabel("Perturbation magnitude:"),0,0);
  grid_base_mutation_layout->addWidget(_spinbox_magnitude=new QSpinBox,0,1);
  _spinbox_magnitude->setRange(0,_scale);
  _spinbox_magnitude->setSingleStep(maximum(1,_scale/1000));
  _spinbox_magnitude->setSuffix(QString("/%1").arg(_scale));
  _spinbox_magnitude->setToolTip("Scale of function parameter perturbations.");
  
  grid_base_mutation_layout->addWidget(new QLabel("p(Parameter reset)"),1,0);
  grid_base_mutation_layout->addWidget(_spinbox_parameter_reset=new QSpinBox,1,1);
  _spinbox_parameter_reset->setRange(0,_scale);
  _spinbox_parameter_reset->setSingleStep(maximum(1,_scale/1000));
  _spinbox_parameter_reset->setSuffix(QString("/%1").arg(_scale));
  _spinbox_parameter_reset->setToolTip("Probability of function parameters being completely reset.");

  grid_base_mutation_layout->addWidget(new QLabel("p(Glitch)"),2,0);
  grid_base_mutation_layout->addWidget(_spinbox_glitch=new QSpinBox,2,1);
  _spinbox_glitch->setRange(0,_scale);
  _spinbox_glitch->setSingleStep(maximum(1,_scale/1000));
  _spinbox_glitch->setSuffix(QString("/%1").arg(_scale));
  _spinbox_glitch->setToolTip("Probability of function branch being replaced by new random stub.");

  grid_base_mutation_layout->addWidget(new QLabel("p(Shuffle)"),3,0);
  grid_base_mutation_layout->addWidget(_spinbox_shuffle=new QSpinBox,3,1);
  _spinbox_shuffle->setRange(0,_scale);
  _spinbox_shuffle->setSingleStep(maximum(1,_scale/1000));
  _spinbox_shuffle->setSuffix(QString("/%1").arg(_scale));
  _spinbox_shuffle->setToolTip("Probability of function branches being reordered.");

  grid_base_mutation_layout->addWidget(new QLabel("p(Insert)"),4,0);
  grid_base_mutation_layout->addWidget(_spinbox_insert=new QSpinBox,4,1);
  _spinbox_insert->setRange(0,_scale);
  _spinbox_insert->setSingleStep(maximum(1,_scale/1000));
  _spinbox_insert->setSuffix(QString("/%1").arg(_scale));
  _spinbox_insert->setToolTip("Probability of function branch having random stub inserted.");

  grid_base_mutation_layout->addWidget(new QLabel("p(Substitute)"),5,0);
  grid_base_mutation_layout->addWidget(_spinbox_substitute=new QSpinBox,5,1);
  _spinbox_substitute->setRange(0,_scale);
  _spinbox_substitute->setSingleStep(maximum(1,_scale/1000));
  _spinbox_substitute->setSuffix(QString("/%1").arg(_scale));
  _spinbox_substitute->setToolTip("Probability of function node's type being changed.");

  _grid_autocool=new QWidget;
  _vbox_autocool->layout()->addWidget(_grid_autocool);
  QGridLayout*const grid_autocool_layout=new QGridLayout();
  _grid_autocool->setLayout(grid_autocool_layout);

  grid_autocool_layout->addWidget(_checkbox_autocool_enable=new QCheckBox("Enable autocool"),0,1);
  _checkbox_autocool_enable->setToolTip("Autocooling reduces the strength and probablility of mutations with increasing numbers of generations.");

  grid_autocool_layout->addWidget(new QLabel("Half-life"),1,0);
  grid_autocool_layout->addWidget(_spinbox_autocool_halflife=new QSpinBox,1,1);
  _spinbox_autocool_halflife->setRange(1,1000);
  _spinbox_autocool_halflife->setSingleStep(1);  
  _spinbox_autocool_halflife->setToolTip("Number of generations needed to halve mutation influence when autocooling.");

  grid_autocool_layout->addWidget(_label_autocool_generations=new QLabel(""),2,0);

  grid_autocool_layout->addWidget(_button_autocool_reheat=new QPushButton("Reheat"),2,1);
  connect(_button_autocool_reheat,SIGNAL(clicked()),this,SLOT(reheat()));

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
 
  _ok=new QPushButton("OK");
  layout()->addWidget(_ok);
  _ok->setDefault(true);

  connect(
	  _ok,SIGNAL(clicked()),
	  this,SLOT(hide())
	  );

  connect(
	  _mutation_parameters,SIGNAL(changed()),
	  this,SLOT(mutation_parameters_changed())
	  );
}

DialogMutationParameters::~DialogMutationParameters()
{}

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
  _label_autocool_generations->setText(QString("Generations: ")+QString::number(_mutation_parameters->autocool_generations()));

  // Grey-out any irrelevant settings
  _spinbox_autocool_halflife->setEnabled(_mutation_parameters->autocool_enable());
  _button_autocool_reheat->setEnabled(_mutation_parameters->autocool_enable() && _mutation_parameters->autocool_generations()>0);
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

void DialogMutationParameters::reheat()
{
  _mutation_parameters->autocool_generations(0);
}

void DialogMutationParameters::changed_autocool_enable(int buttonstate)
{
  if (buttonstate==Qt::Checked) _mutation_parameters->autocool_enable(true);
  else if (buttonstate==Qt::Unchecked) _mutation_parameters->autocool_enable(false);
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
