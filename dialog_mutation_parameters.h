// Source file for evolvotron
// Copyright (C) 2002 Tim Day
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
#include <qpushbutton.h>

#include "useful.h"
#include "mutation_parameters.h"


//! Provides an "About" dialog box.
class DialogMutationParameters : public QDialog
{
 private:
  Q_OBJECT

 protected:
  MutationParameters*const _mutation_parameters;
  
 public:
  //! Constructor.
  DialogMutationParameters(QWidget* parent,MutationParameters* p);

  //! Destructor.
  virtual ~DialogMutationParameters()
    {}

  //! Vertical layout.
  QVBox* _vbox;

  //! Grid for controls.
  QGrid* _grid;

  //! Button to close dialog.
  QPushButton* _ok;

  //! Need to pass resizes on to vbox or things just get chopped.
  virtual void resizeEvent(QResizeEvent*);

  //! Notification to refresh display fields from updated mutation parameters.
  void parameters_updated();
};

#endif
