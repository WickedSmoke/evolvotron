// Source file for evolvotron
// Copyright (C) 2002,2003 Tim Day
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

#include <qdialog.h>
#include <qvbox.h>
#include <qgrid.h>
#include <qlabel.h>
#include <qspinbox.h>
#include <qslider.h>
#include <qpushbutton.h>
#include <qcheckbox.h>
#include <qmainwindow.h>
#include <qstatusbar.h>

#include "useful.h"
#include "mutation_parameters.h"

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
  /*! NB it's fairly important no-one else accesses this except through methods of this class, else GUI compoinents will get out of sync
   */
  MutationParameters _mutation_parameters;

  //! Vertical layout.
  QVBox* _vbox;

  //! Grid for buttons;
  QGrid* _grid_buttons;

  //! Grid for parameter control spinners
  QGrid* _grid_parameters;

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
  QSpinBox* _spinbox_glitch;
  QSpinBox* _spinbox_shuffle;
  QSpinBox* _spinbox_insert;
  //@}

  //! Holder for slider decoration
  //@{
  QHBox* _hbox_proportion_constant;
  QHBox* _hbox_identity_supression;
  //@}

  //! Slider for controlling indentity_supression
  //@{
  QSlider* _slider_proportion_constant;
  QSlider* _slider_identity_supression;
  //@}

  //@{
  //! Checkbox to supress certain function node types
  QCheckBox* _checkbox_iterative;
  QCheckBox* _checkbox_fractal;
  //@}

  //! Button to close dialog.
  QPushButton* _ok;

  //! Need to pass resizes on to vbox or things just get chopped.
  virtual void resizeEvent(QResizeEvent*);

  //! Notification to refresh display fields from updated mutation parameters.
  void parameters_changed();

  //! Reload spinboxes from _mutation_parameters.
  void setup_from_mutation_parameters();

  //! Non-const accessor NB protected to limit to internal usage.
  MutationParameters& mutation_parameters()
    {
      return _mutation_parameters;
    }

 public:
  //! Constructor.
  DialogMutationParameters(QMainWindow* parent);

  //! Destructor.
  virtual ~DialogMutationParameters()
    {}

  //! Accessor.  NB No-one else must modify parameters or spinboxes will get out of sync.
  const MutationParameters& mutation_parameters() const
    {
      return _mutation_parameters;
    }

 public slots:

  //@{
  //! Signalled by button (possibly status bar button).
  void reset();
  void heat();
  void cool();
  void irradiate();
  void shield();
  //@}

  //@{
  //! Signalled by spinbox.
  void changed_magnitude(int v);
  void changed_glitch(int v);
  void changed_shuffle(int v);
  void changed_insert(int v);
  //@}
  
  //@{
  //! Signalled by slider.
  void changed_proportion_constant(int v);
  void changed_identity_supression(int v);
  //@}

  //@{
  //! Signalled by checkbox.
  void changed_iterative(int v);
  void changed_fractal(int v);
  //@}

};

#endif