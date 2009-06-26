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
  \brief Interface for class DialogRenderParameters.
*/

#ifndef _dialog_render_parameters_h_
#define _dialog_render_parameters_h_

#include "render_parameters.h"

//! Provides an dialog box for controlling RenderParameters.
class DialogRenderParameters : public QDialog
{
 private:
  Q_OBJECT

 protected:

  //! Instance of MutationParameters under dialog control.
  /*! NB it's fairly important no-one modifies this except through methods of this class
    (or another class responsible for another part), else GUI components will get out of sync
   */
  RenderParameters*const _render_parameters;

  //! Enables jittered samples.
  QCheckBox* _checkbox_jittered_samples;

  //! Chooses between multisampling levels.
  QWidget* _buttonvbox;

  //! Chooses between multisampling levels.
  QButtonGroup* _buttongroup;

  //! Button to close dialog.
  QPushButton* _ok;

  //! Reload from _render_parameters.
  void setup_from_render_parameters();

 public:
  //! Constructor.
  DialogRenderParameters(QMainWindow* parent,RenderParameters* rp);

  //! Destructor.
  ~DialogRenderParameters();

 public slots:

  //! Signalled by checkbox.
  void changed_jittered_samples(int buttonstate);

  //! Signalled by radio buttons.
  void changed_oversampling(int id);

  //! Signalled by mutation parameters
  void render_parameters_changed();
};

#endif
