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

#include <deque>
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
#include "transform_factory.h"

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

  //! Class encapsulating everything needed for undo functionality.
  class History
    {
    protected:
      //! Pointer to main app.
      EvolvotronMain*const _main;

      //! Each deque slot contains the collection of display-image pairs replaced by an single action.
      /*! We use a stack rather than a deque because we want to clean up the tail end (limited number of Undos).
       */
      std::deque<std::vector<std::pair<MutatableImageDisplay*,MutatableImageNode*> > > _history;

      //! Number of slots retained for history.
      uint max_slots;

      //! Clean up the last slot in the queue.
      void purge();

    public:

      //! Constructor.
      History(EvolvotronMain*);

      //! Destructor.
      ~History();
      
      //! Record that we are overwriting the given display.
      void replacing(MutatableImageDisplay* display);

      //! Starts a new action slot
      void begin_action(); 

      //! Ends an action slot and updates the undoable state.
      void end_action(); 

      //! Returns true if there is stuff to undo
      bool undoable();

      //! Implements an undo.
      void undo();
    };

  //! Instance of History object to track activity.
  History _history;

  //! Convenience typedef for pointer to member function implementing a kind of spawn.
  typedef void (EvolvotronMain::* SpawnMemberFn)(const MutatableImageNode* image,MutatableImageDisplay* display);

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
  QPopupMenu* _popupmenu_edit;

  //! ID for the undo item (so we can disable it).
  int _popupmenu_edit_undo_id;

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
  std::vector<MutatableImageDisplay*> _displays;

  //! Keeps track of which displays are still available for display (they might have been destroyed while an image was computing).
  std::set<MutatableImageDisplay*> _known_displays;

  //! Keeps track of which displays are still resizing
  std::set<const MutatableImageDisplay*> _resizing;

  //! An owned deepclone of the last image spawned (used to regenerate single displays).
  MutatableImageNode* _last_spawned_image;

  //! Pointer to member function used for last spawn.
  SpawnMemberFn _last_spawn_method;

  //! An owned pointer to the current transform factory (needed for Respawn).
  TransformFactory* _transform_factory;

  //! Accessor.
  History& history()
    {
      return _history;
    }

  //! Accessor.
  const MutatableImageNode*const last_spawned_image() const
    {
      return _last_spawned_image;
    }
  
  //! Accessor.
  const SpawnMemberFn last_spawn_method() const
    {
      return _last_spawn_method;
    }

  //! Not just an accessor.  Takes a deepclone and deletes it when replaced.
  void last_spawned_image(const MutatableImageNode* image,SpawnMemberFn method);

  //! Accessor
  const TransformFactory& transform_factory() const
    {
      return *_transform_factory;
    }

  //! Not just an accessor.  Takes a deepclone and deletes it when replaced.
  void transform_factory(const TransformFactory& tfactory)
    {
      delete _transform_factory;
      _transform_factory=tfactory.clone();
    }

  //@{
  //! Perform a particular type of spawn from an individiual image to an individual display.  (Locking not checked).
  void spawn_normal(const MutatableImageNode* image,MutatableImageDisplay* display);
  void spawn_recoloured(const MutatableImageNode* image,MutatableImageDisplay* display);
  void spawn_warped(const MutatableImageNode* image,MutatableImageDisplay* display);
  //@}

  //! Spawn the specified display using the specified method.
  void spawn_all(MutatableImageDisplay* display,SpawnMemberFn method);

 public:
  //! Constructor.
  EvolvotronMain(QWidget* parent,const QSize& grid_size,uint n_threads);

  //! Accessor.
  std::vector<MutatableImageDisplay*>& displays()
    {
      return _displays;
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

  //! Called by History when performing undo.
  void restore(MutatableImageDisplay* display,MutatableImageNode* image);

  //! Called by History to change undo menu status.
  void set_undoable(bool v);

  //! Regenerates a single display using last spawn method and source.
  void respawn(MutatableImageDisplay* display);

  //! Mutates the image held by the given display to all the other displays owned.
  void spawn_normal(MutatableImageDisplay* spawning_display);

  //! Similar to spawn except just changes the colouration of the image.
  void spawn_recoloured(MutatableImageDisplay* spawning_display);

  //! Similar to spawn except just changes the input co-ordinates to the image.
  void spawn_warped(MutatableImageDisplay* spawning_display,const TransformFactory& tfactory);

  //! Called from display constructor to indicate the display is available for the disposal of its completed tasks.
  void hello(MutatableImageDisplay*);

  //! Called from display destructor to indicate the display is no longer available for the disposal of its completed tasks.
  void goodbye(MutatableImageDisplay*);

  //! Returns true if the display is known.
  bool is_known(MutatableImageDisplay* disp) const;

 protected:
  //! Reset the specified display.
  void reset(MutatableImageDisplay* display);

 protected slots:
  //! Signalled by timer.
  void tick();

  //! Signalled by menu item.  Forwards to History object.
  void undo();

  public slots:
   
  //! Signalled by menu item.  Public because called from evolvotron app wrapper.
  void reset(bool reset_mutation_parameters,bool reset_locks);
 
 //! Forwards to reset(false)
 void reset_warm();
 
 //! Forwards to reset(true)
 void reset_cold();
};

#endif

