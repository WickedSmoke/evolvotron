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
  \brief Implementation of class MutatableImageDisplayBig.
*/



#include "mutatable_image_display_big.h"

#include "evolvotron_main.h"

MutatableImageDisplayBig::MutatableImageDisplayBig(EvolvotronMain* mn)
  :QWidget(mn,Qt::Window)  // We're a window, but with a parent
  ,_main(mn)
{
  setAttribute(Qt::WA_DeleteOnClose,true);

  setWindowTitle("Evolvotron");
  setMinimumSize(256,256);
  
  //setSizeGripEnabled(true); // Only an option if we give the window a statusbar, or make it a dialog
}

/*! Don't think destructor needs to do anything to _display... Qt takes care of it
 */
MutatableImageDisplayBig::~MutatableImageDisplayBig()
{
  std::clog << "An enlargement was deleted\n";
}

/*! There's not much point in dropping back to normal mode (from fullscreen) if the main
  app is fullscreen because we'll just be hidden, so close instead under such circumstances.
  (However, on ctrl-f we put both into normal mode).
 */
void MutatableImageDisplayBig::keyPressEvent(QKeyEvent* e)
{
  if (e->key()==Qt::Key_Escape)
    {
      if (main()->isFullScreen())
	close();
      else
	showNormal();  
    }
  else if (e->key()==Qt::Key_F && !e->modifiers())
    {
      if (isFullScreen())
	{
	  if (main()->isFullScreen())
	    main()->toggle_fullscreen();  // Need to use this to maintain menu checkmarks
	  showNormal();
	}
      else 
	showFullScreen();
    }
  else
    {
      // Perhaps it's for someone else
      e->ignore();
    }
}
