// Source file for evolvotron
// Copyright (C) 2007 Tim Day
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
  \brief Implementation of class DialogRenderParameters.
*/

#include "libevolvotron_precompiled.h"

#include "dialog_render_parameters.h"

DialogRenderParameters::DialogRenderParameters(QMainWindow* parent,RenderParameters* rp)
  :QDialog(parent)
  ,_render_parameters(rp)
{
  setCaption("Render Parameters");

  _dialog_content=new QVBox(this);
  
  _grid=new QGrid(2,Qt::Horizontal,_dialog_content);

  new QLabel("Jittered samples",_grid);
  _checkbox_jittered_samples=new QCheckBox(_grid);
  QToolTip::add(_checkbox_jittered_samples,"Jitter moves sampling positions randomly within a pixel.  This helps to break up aliasing and moire patterns.");

  _buttongroup=new QVButtonGroup("Oversampling (antialiasing)",_dialog_content);
  _buttongroup->insert(new QRadioButton("1x1",_buttongroup),1);
  _buttongroup->insert(new QRadioButton("2x2",_buttongroup),2);
  _buttongroup->insert(new QRadioButton("3x3",_buttongroup),3);
  _buttongroup->insert(new QRadioButton("4x4",_buttongroup),4);
  _buttongroup->setRadioButtonExclusive(true);
  QToolTip::add(_buttongroup->find(1),"No oversampling");
  QToolTip::add(_buttongroup->find(2),"Enables a final antialiased rendering with 4 samples per pixel");
  QToolTip::add(_buttongroup->find(3),"Enables a final antialiased rendering with 3 samples per pixel");
  QToolTip::add(_buttongroup->find(4),"Enables a final antialiased rendering with 16 samples per pixel");

  setup_from_render_parameters();

  connect(_checkbox_jittered_samples,SIGNAL(stateChanged(int)),this,SLOT(changed_jittered_samples(int)));
  connect(_buttongroup,SIGNAL(clicked(int)),this,SLOT(changed_oversampling(int)));
 
  _ok=new QPushButton("OK",_dialog_content);
  _ok->setDefault(true);

  connect(
	  _ok,SIGNAL(clicked()),
	  this,SLOT(hide())
	  );

  connect(
	  _render_parameters,SIGNAL(changed()),
	  this,SLOT(render_parameters_changed())
	  );
}

void DialogRenderParameters::resizeEvent(QResizeEvent* e)
{
  Superclass::resizeEvent(e);
  _dialog_content->resize(size());
}

void DialogRenderParameters::setup_from_render_parameters()
{
  _checkbox_jittered_samples->setChecked(_render_parameters->jittered_samples());

  QButton*const which_button=_buttongroup->find(_render_parameters->multisample_level());
  if (which_button)
    {
      _buttongroup->setButton(_buttongroup->id(which_button));
    }
}

void DialogRenderParameters::changed_jittered_samples(int buttonstate)
{
  if (buttonstate==QButton::On) _render_parameters->jittered_samples(true);
  else if (buttonstate==QButton::Off) _render_parameters->jittered_samples(false);
}

void DialogRenderParameters::changed_oversampling(int id)
{
  _render_parameters->multisample_level(id);
}

void DialogRenderParameters::render_parameters_changed()
{
  setup_from_render_parameters();
}
