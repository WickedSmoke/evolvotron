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
  \brief Interface for class DialogHelp.
*/

#ifndef _dialog_help_h_
#define _dialog_help_h_

#include "common.h"

//! Provides a dialog box with some user documentation.
/*! More of a quick reference guide than a manual.
 */
class DialogHelp : public QDialog
{
 private:
  Q_OBJECT

 public:

  //! Constructor.
  DialogHelp(QWidget* parent,bool full);

  //! Destructor.
  ~DialogHelp();
};

#endif
