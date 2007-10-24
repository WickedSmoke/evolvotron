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
  \brief Interface for class DialogMutationParameters.
*/

#ifndef _dialog_mutation_parameters_h_
#define _dialog_mutation_parameters_h_

#include <qgrid.h>
#include <qspinbox.h>
#include <qslider.h>
#include <qmainwindow.h>
#include <qstatusbar.h>

#include "mutation_parameters_qobject.h"

//! Provides an "About" dialog box, manages an instance of MutationParameters.
class DialogMutationParameters : public QDialog
{
 private:
  Q_OBJECT

 protected:
  //! Scale to spinbox's integer values.
  const int _scale;

  //! Owner of dialog (probably EvolvotronMain), used to access a statusbar.
  QMainWindow*const _parent;

  //! Instance of MutationParameters under dialog control.
  /*! NB it's fairly important no-one modifies this except through methods of this class
    (or another class responsible for another part), else GUI components will get out of sync
   */
  MutationParametersQObject*const _mutation_parameters;

  //! Vertical layout.
  QVBox* _vbox;

  //! Grid for buttons;
  QGrid* _grid_buttons;

  //! Group for base mutation parameter controls
  QGroupBox* _group_base_mutation;

  //! Grid for base parameter control spinners
  QGrid* _grid_base_mutation;

  //! Group for autocool parameters
  QGroupBox* _group_autocool;

  //! Grid for autocool parameters
  QGrid* _grid_autocool;

  //@{
  //! Button for quick adjustment of MutationParameters
  QPushButton* _button_reset;
  QPushButton* _button_cool;
  QPushButton* _button_heat;
  QPushButton* _button_shield;
  QPushButton* _button_irradiate;
  //@}

  //@{
  //! Spinners for detailed control of specific parameters
  QSpinBox* _spinbox_magnitude;
  QSpinBox* _spinbox_parameter_reset;
  QSpinBox* _spinbox_glitch;
  QSpinBox* _spinbox_shuffle;
  QSpinBox* _spinbox_insert;
  QSpinBox* _spinbox_substitute;
  QSpinBox* _spinbox_autocool_halflife;
  //@}

  //! Control autocooling
  QCheckBox* _checkbox_autocool_enable;

  //! Button to close dialog.
  QPushButton* _ok;

  //! Need to pass resizes on to vbox or things just get chopped.
  virtual void resizeEvent(QResizeEvent*);

  //! Reload spinboxes from _mutation_parameters.
  void setup_from_mutation_parameters();

 public:
  //! Constructor.
  DialogMutationParameters(QMainWindow* parent,MutationParametersQObject* mp);

  //! Destructor.
  virtual ~DialogMutationParameters()
    {}

 public slots:

  //@{
  //! Signalled by button.
  void reset();
  void heat();
  void cool();
  void irradiate();
  void shield();
  //@}

  //! Signalled by checkbox.
  void changed_autocool_enable(int buttonstate);

  //@{
  //! Signalled by spinbox.
  void changed_magnitude(int v);
  void changed_parameter_reset(int v);
  void changed_glitch(int v);
  void changed_shuffle(int v);
  void changed_insert(int v);
  void changed_substitute(int v);
  void changed_autocool_halflife(int v);
  //@}

  //! Signalled by mutation parameters
  void mutation_parameters_changed();
};

#endif
