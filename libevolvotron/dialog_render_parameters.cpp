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
  \brief Implementation of class DialogRenderParameters.
*/



#include "dialog_render_parameters.h"

DialogRenderParameters::DialogRenderParameters(QMainWindow* parent,RenderParameters* rp)
  :QDialog(parent)
  ,_render_parameters(rp)
{
  setWindowTitle("Render Parameters");
  setSizeGripEnabled(true);

  setLayout(new QVBoxLayout);
  
  layout()->addWidget(_checkbox_jittered_samples=new QCheckBox("Jittered samples"));
  _checkbox_jittered_samples->setToolTip("Jitter moves sampling positions randomly within a pixel.  This helps to break up aliasing and moire patterns.");

  _buttonvbox=new QGroupBox("Oversampling (antialiasing)");
  _buttonvbox->setLayout(new QVBoxLayout);
  layout()->addWidget(_buttonvbox);
  
  QRadioButton* button[4];
  _buttonvbox->layout()->addWidget(button[0]=new QRadioButton("1x1"));
  _buttonvbox->layout()->addWidget(button[1]=new QRadioButton("2x2"));
  _buttonvbox->layout()->addWidget(button[2]=new QRadioButton("3x3"));
  _buttonvbox->layout()->addWidget(button[3]=new QRadioButton("4x4"));

  button[0]->setToolTip("No oversampling");
  button[1]->setToolTip("Enables a final antialiased rendering with 4 samples per pixel");
  button[2]->setToolTip("Enables a final antialiased rendering with 9 samples per pixel");
  button[3]->setToolTip("Enables a final antialiased rendering with 16 samples per pixel");

  _buttongroup=new QButtonGroup(_buttonvbox);
  _buttongroup->addButton(button[0],1);
  _buttongroup->addButton(button[1],2);
  _buttongroup->addButton(button[2],3);
  _buttongroup->addButton(button[3],4);

  setup_from_render_parameters();

  connect(_checkbox_jittered_samples,SIGNAL(stateChanged(int)),this,SLOT(changed_jittered_samples(int)));
  connect(_buttongroup,SIGNAL(buttonClicked(int)),this,SLOT(changed_oversampling(int)));
 
  _ok=new QPushButton("OK");
  _ok->setDefault(true);
  layout()->addWidget(_ok);

  connect(
	  _ok,SIGNAL(clicked()),
	  this,SLOT(hide())
	  );

  connect(
	  _render_parameters,SIGNAL(changed()),
	  this,SLOT(render_parameters_changed())
	  );
}

DialogRenderParameters::~DialogRenderParameters()
{}

void DialogRenderParameters::setup_from_render_parameters()
{
  _checkbox_jittered_samples->setChecked(_render_parameters->jittered_samples());

  QAbstractButton*const which_button=_buttongroup->button(_render_parameters->multisample_grid());
  if (which_button)
    {
      which_button->click();
    }
}

void DialogRenderParameters::changed_jittered_samples(int buttonstate)
{
  if (buttonstate==Qt::Checked) _render_parameters->jittered_samples(true);
  else if (buttonstate==Qt::Unchecked) _render_parameters->jittered_samples(false);
}

void DialogRenderParameters::changed_oversampling(int id)
{
  assert(1<=id && id<=4);
  _render_parameters->multisample_grid(id);
}

void DialogRenderParameters::render_parameters_changed()
{
  setup_from_render_parameters();
}
