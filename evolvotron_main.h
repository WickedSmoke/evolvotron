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
  \brief Interface for class EvolvotronMain.
*/

#ifndef _evolvotron_main_h_
#define _evolvotron_main_h_

extern "C"
{
#include <time.h>
}

#include <vector>
#include <set>

#include <qmenubar.h>
#include <qstatusbar.h>
#include <qvbox.h>
#include <qgrid.h>
#include <qtimer.h>

#include "useful.h"
#include "dialog_about.h"

#include "mutatable_image.h"
#include "mutatable_image_display.h"
#include "mutatable_image_computer_farm.h"
#include "mutation_parameters.h"

//! Top level GUI component for evolvotron application
class EvolvotronMain : public QVBox
{
 private:
  Q_OBJECT
    
 protected:

  //! The dialog widget.
  DialogAbout* _dialog_about;

  //! The menubar.
  QMenuBar* _menubar;

  //! The file menu.
  QPopupMenu* _popupmenu_file;

  //! The help menu.
  QPopupMenu* _popupmenu_help;

  //! Somewhere to report what's going on
  QStatusBar* _statusbar;

  //! Number of tasks statusbar is reporting as active
  uint _statusbar_tasks;

  //! Grid for image display areas
  QGrid* _grid;

  //! Timer to drive tick() slot
  QTimer* _timer;

  //! Farm of compute threads.
  MutatableImageComputerFarm* _farm;

  //! All the displays in the grid.
  std::vector<MutatableImageDisplay*> _display;

  //! Keeps track of which displays are still available for display (they might have been destroyed while an image was computing).
  std::set<MutatableImageDisplay*> _known_displays;

  //! Parameters for mutating images.
  MutationParameters _mutation_parameters;

  //! Keeps track of which displays are still resizing
  std::set<const MutatableImageDisplay*> _resizing;

  //! Accessor.
  const MutationParameters& mutation_parameters() const
    {
      return _mutation_parameters;
    }

 public:
  //! Constructor.
  EvolvotronMain(QWidget* parent,const QSize& grid_size,uint n_threads);

  //! Accessor.
  std::vector<MutatableImageDisplay*>& display()
    {
      return _display;
    }

  //! Accessor.
  MutatableImageComputerFarm*const farm()
    {
      return _farm;
    }

  //! Mutates the image held by the given display to all the other displays owned.
  void spawn(MutatableImageDisplay* spawning_display);

  //! Called from display constructor to indicate the display is available for the disposal of its completed tasks.
  void hello(MutatableImageDisplay*);

  //! Called from display destructor to indicate the display is no longer available for the disposal of its completed tasks.
  void goodbye(MutatableImageDisplay*);

 protected slots:
   //! Signalled by timer.
  void tick();

 public slots:
   //! Signalled by menu item.
  void reset();
};

#endif

