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
  \brief Interface for class DialogMutationParameters.
*/

#ifndef _dialog_mutation_parameters_h_
#define _dialog_mutation_parameters_h_

#include "common.h"

#include "mutation_parameters_qobject.h"

//! Provides an dialog box for controlling MutationParameters.
class DialogMutationParameters : public QDialog
{
 private:
  Q_OBJECT

  typedef QDialog Superclass;

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

  //! Tabs for base parameters and autocool
  QTabWidget* _tabs;

  //! Tab for base mutation parameter controls
  QWidget* _vbox_base_mutation;

  //! Grid for buttons;
  QWidget* _grid_buttons;

  //! Grid for base parameter control spinners
  QWidget* _grid_base_mutation;

  //! Group for autocool parameters
  QWidget* _vbox_autocool;

  //! Grid for autocool parameters
  QWidget* _grid_autocool;

  //! Label to show number of generations
  QLabel* _label_autocool_generations;

  //! Button to reheeat autocooling
  QPushButton* _button_autocool_reheat;

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

  //! Reload spinboxes from _mutation_parameters.
  void setup_from_mutation_parameters();

 public:
  //! Constructor.
  DialogMutationParameters(QMainWindow* parent,MutationParametersQObject* mp);

  //! Destructor.
  ~DialogMutationParameters();

 public slots:

  //@{
  //! Signalled by button.
  void reset();
  void heat();
  void cool();
  void irradiate();
  void shield();
  void reheat();
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
