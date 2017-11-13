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
  \brief Interface for class MutatableImageDisplayBig
*/

#ifndef _mutatable_image_display_big_h_
#define _mutatable_image_display_big_h_

#include "common.h"

class EvolvotronMain;

//! Intended to be used as a top-level widget holding a single MutatableImageDisplay
/*! We just used to use a display or scroll view itself as a top-level widget,
  but need this to get some specific keyboard effects.
  \todo class name is a bit misleading.  This is really just a slightly modified top-level holder.
 */
class MutatableImageDisplayBig : public QWidget
{
  Q_OBJECT

 protected:
  //! Pointer back to the application object to access fullscreen state
  EvolvotronMain* _main;

 public:
  //! Constructor.
  MutatableImageDisplayBig(EvolvotronMain* mn);

  //! Destructor.
  virtual ~MutatableImageDisplayBig();

  //! Accessor.
  EvolvotronMain* main() const
    {
      assert(_main!=0);
      return _main;
    }

 protected:
  //! Handle key-presses
  void keyPressEvent(QKeyEvent* e);
};

#endif
