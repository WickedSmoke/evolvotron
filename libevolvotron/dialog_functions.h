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
  \brief Interface for class DialogMutationParameters.
*/

#ifndef _dialog_functions_h_
#define _dialog_functions_h_

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
#include <qscrollview.h>

#include "useful.h"
#include "mutation_parameters.h"

//! Provides a dialog for controlling which functions are available.
class DialogFunctions : public QDialog
{
 private:
  Q_OBJECT

 protected:
  //! Owner of dialog (probably EvolvotronMain).
  QMainWindow*const _parent;

  //! Instance of MutationParameters under dialog control.
  /*! \warning Careful of modifying things which might make DialogMutationParameters get out of sync
   */
  MutationParameters*const _mutation_parameters;

  //! Vertical layout.
  QVBox* _vbox;

  //! Notification of undiluted branching ratio.
  QLabel* _branching_ratio;

  //! Required branching ratio after dilution
  QSlider* _slider_target_branching_ratio;

  //! Proportion of diluting nodes which are pure constants
  QSlider* _slider_proportion_constant;

  //! Proportion of non-constant nodes which are 12-parameter transforms
  QSlider* _slider_identity_supression;

  //! Scrolling area for per-function controls
  QScrollView* _scrollview;

  //! Vbox for layout within scroll area
  QVBox* _scrollview_vbox;

  //! Button to close dialog.
  QPushButton* _ok;

  //! Need to pass resizes on to vbox or things just get chopped.
  virtual void resizeEvent(QResizeEvent*);

 public:
  //! Constructor.
  DialogFunctions(QMainWindow* parent,MutationParameters* mp);

  //! Destructor.
  virtual ~DialogFunctions();

  //! Reload from _mutation_parameters
  void setup_from_mutation_parameters();

 protected slots:
   
   //@{
   //! Signalled by slider.
   void changed_target_branching_ratio(int v);
   void changed_proportion_constant(int v);
   void changed_identity_supression(int v);
   //@}
};

#endif
