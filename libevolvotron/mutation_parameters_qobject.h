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
  \brief Interface for class MutationParametersQObject.
*/

#ifndef _mutation_parameters_qobject_h_
#define _mutation_parameters_qobject_h_

#include "common.h"

#include "mutation_parameters.h"

//! class extending MutationParameters to emit changed signal when appropriate.
class MutationParametersQObject : public QObject, public MutationParameters
{
  Q_OBJECT;
 public:
  MutationParametersQObject(uint seed,bool autocool,bool debug_mode,QObject* parent);
  ~MutationParametersQObject();

signals:
  void changed();

 protected:
  void report_change();
};


#endif
