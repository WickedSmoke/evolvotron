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
  \brief Interface for class DialogAbout.
*/

#ifndef _dialog_about_h_
#define _dialog_about_h_

#include "common.h"

//! Provides an "About" dialog box.
/*! About dialog displays author info, web addresses and license info.
 */
class DialogAbout : public QDialog
{
 private:
  Q_OBJECT

 public:

  //! Constructor.
  DialogAbout(QWidget* parent,int n_threads,bool separate_farm_for_enlargements);

  //! Destructor.
  ~DialogAbout();
};

#endif
