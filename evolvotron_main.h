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
#include <qmainwindow.h>
#include <qgrid.h>
#include <qtimer.h>
#include <qpushbutton.h>
#include <qlabel.h>

#include "useful.h"

#include "mutatable_image.h"
#include "mutatable_image_display.h"
#include "mutatable_image_computer_farm.h"
#include "mutation_parameters.h"

#include "dialog_about.h"
#include "dialog_mutation_parameters.h"

//! Top level GUI component for evolvotron application
class EvolvotronMain : public QMainWindow
{
 private:
  Q_OBJECT
    
 protected:

  //! Somewhere to report what's going on
  QStatusBar* _statusbar;

  //! Label for displaying number of tasks running (more permanent than StatusBar's message method).
  QLabel* _statusbar_tasks_label;

  //! Number of tasks statusbar is reporting as active
  uint _statusbar_tasks;

  //! The "About" dialog widget.
  DialogAbout* _dialog_about;

  //! The dialog for adjusting MutationParameters.
  DialogMutationParameters* _dialog_mutation_parameters;

  //! The menubar.
  QMenuBar* _menubar;

  //! The file menu.
  QPopupMenu* _popupmenu_file;

  //! The mutate menu.
  QPopupMenu* _popupmenu_mutate;

  //! The help menu.
  QPopupMenu* _popupmenu_help;

  //@{
  //! Button for quick adjustment of MutationParameters
  QPushButton* _button_cool;
  QPushButton* _button_heat;
  QPushButton* _button_shield;
  QPushButton* _button_irradiate;
  //@}

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

  //! Keeps track of which displays are still resizing
  std::set<const MutatableImageDisplay*> _resizing;

 public:
  //! Constructor.
  EvolvotronMain(QWidget* parent,const QSize& grid_size,uint n_threads);

  //! Accessor.
  std::vector<MutatableImageDisplay*>& display()
    {
      return _display;
    }

  //! Accessor.
  const MutationParameters& mutation_parameters() const
    {
      // Have to have this cheesy cast to pick up the non-protected version.
      return ((const DialogMutationParameters*)_dialog_mutation_parameters)->mutation_parameters();
    }

  //! Accessor.
  MutatableImageComputerFarm*const farm()
    {
      return _farm;
    }

  //! Mutates the image held by the given display to all the other displays owned.
  void spawn(MutatableImageDisplay* spawning_display);

  //! Similar to spawn except just changes the colouration of the image.
  void spawn_recoloured(MutatableImageDisplay* spawning_display);

  //! Similar to spawn except just changes the input co-ordinates to the image.
  void spawn_warped(MutatableImageDisplay* spawning_display);

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

