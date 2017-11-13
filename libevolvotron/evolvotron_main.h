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
  \brief Interface for class EvolvotronMain.
*/

#ifndef _evolvotron_main_h_
#define _evolvotron_main_h_

#include "common.h"

#include "function_registry.h"
#include "transform_factory.h"

#include "mutatable_image.h"
#include "mutatable_image_display.h"
#include "mutatable_image_computer_farm.h"
#include "mutation_parameters_qobject.h"
#include "render_parameters.h"

class DialogAbout;
class DialogHelp;
class DialogMutationParameters;
class DialogRenderParameters;
class DialogFunctions;
class DialogFavourite;

//! Utility class to expand "restart with" menu picks
/*! A boost::bind kind of thing
 */
class SignalExpanderRestartWith : public QObject
{
 private:
  Q_OBJECT

  const FunctionRegistration*const _fn;
 public:
  SignalExpanderRestartWith(QObject* parent,const FunctionRegistration* fn)
    :QObject(parent)
    ,_fn(fn)
    {}
  public slots:
    void restart_with()
    {
      emit restart_with(_fn);
    }
 signals:
  void restart_with(const FunctionRegistration*);
};

//! Top level GUI component for evolvotron application
class EvolvotronMain : public QMainWindow
{
 private:
  Q_OBJECT
    
 protected:

  //! Class encapsulating everything needed for undo functionality.
  /*! \todo This is too big to be a nested class.
   */
  class History
    {
    protected:
      //! Pointer to main app.
      EvolvotronMain*const _main;

      typedef std::map<MutatableImageDisplay*,boost::shared_ptr<const MutatableImage> > ArchiveRecordEntries;
      typedef std::pair<std::string,ArchiveRecordEntries> ArchiveRecord;
      typedef std::deque<ArchiveRecord> Archive;

      //! Each deque slot contains the collection of display-image pairs replaced by an single action (and a string naming that action).
      /*! We use a deque rather than a stack because we want to clean up the tail end (limited number of Undos).
       */
      Archive _archive;

      //! Number of slots retained for history.
      const uint max_slots;

      //! Clean up the last slot in the queue.
      void purge();

      //! Write some info to std::clog.
      void log_status() const;

    public:

      //! Constructor.
      History(EvolvotronMain*);

      //! Destructor.
      ~History();

      //! Eliminate any references to the display (and clean up any undo actions which are empty as a result).
      void goodbye(MutatableImageDisplay*);
      
      //! Record that we are overwriting the given display.
      void replacing(MutatableImageDisplay* display);

      //! Starts a new action slot
      void begin_action(const std::string& action_name);

      //! Ends an action slot and updates the undoable state.
      void end_action(); 

      //! Returns true if there is stuff to undo
      bool undoable();

      //! Implements an undo.
      void undo();
    };

 protected:
  //! Convenience typedef for pointer to member function implementing a kind of spawn.
  typedef void (EvolvotronMain::* SpawnMemberFn)(const boost::shared_ptr<const MutatableImage>& image,MutatableImageDisplay* display,bool one_of_many);

  //! Instance of History object to track activity.
  std::unique_ptr<History> _history;

  //! Sweep z linearly through animations
  /*! \todo Move to mutation or render paraemeters ?
   */
  const bool _linear_zsweep;

  //! Generate spheremaps
  /*! \todo Move to mutation or render paraemeters ?
   */
  const bool _spheremap;

  //! Name of files to load on a reset.
  std::vector<std::string> _startup_filenames;

  //! Whether to shuffle startup files (if any).
  const bool _startup_shuffle;
  
  //! Instance of mutation parameters for the app
  /*! This used to be held by DialogMutationParameters, but now we want to share it around a bit
    (although modifications should always be via the dialog slots, to keep the dialogs up to date)
   */
  MutationParametersQObject _mutation_parameters;

  //! Instance of render parameters for the app
  RenderParameters _render_parameters;

  //! Somewhere to report what's going on
  QStatusBar* _statusbar;

  //! Label for displaying number of tasks running (more permanent than StatusBar's message method).
  QLabel* _statusbar_tasks_label;

  //! Number of main tasks the statusbar is currently reporting as active
  /*! Cached to avoid unnecessarily regenerating message
   */
  uint _statusbar_tasks_main;

  //! Number of enlargement tasks the statusbar is currently reporting as active
  /*! Cached to avoid unnecessarily regenerating message
   */
  uint _statusbar_tasks_enlargement;

  //! The "About" dialog widget.
  DialogAbout* _dialog_about;

  //! The "Help" dialog widget (quick reference text)
  DialogHelp* _dialog_help_short;

  //! The "Help" dialog widget (full manual text)
  DialogHelp* _dialog_help_long;

  //! The dialog for adjusting MutationParameters.
  DialogMutationParameters* _dialog_mutation_parameters;

  //! The dialog for adjusting RenderParameters.
  DialogRenderParameters* _dialog_render_parameters;

  //! Dialog for controlling which functions are in use.
  DialogFunctions* _dialog_functions;

  //! Dialog for selecting a favourite function (also holds the state for favourite stuff)
  DialogFavourite* _dialog_favourite;

  //! The file menu.
  QMenu* _popupmenu_file;

  //! The edit menu.
  QMenu* _popupmenu_edit;

  //! ID for the undo item (so we can disable it).
  QAction* _popupmenu_edit_undo_action;

  //! The settings menu
  QMenu* _popupmenu_settings;

  //! Action for setting fullscreen
  QAction* _menu_action_fullscreen;

  //! Action for hiding menubar
  QAction* _menu_action_hide_menu;

  //! The help menu.
  QMenu* _popupmenu_help;

  //! Select autocooling (also serves to reset the generation count).
  QCheckBox* _checkbox_autocool_enable;

  //! Report number of generations.
  QLabel* _label_autocool_enable;

  //! Button to reheat
  QPushButton* _button_autocool_reheat;

  //! Grid for image display areas
  QWidget* _grid;

  //! Timer to drive tick() slot
  QTimer* _timer;

  //! Two farms of compute threads.  One for the main display, one for enlargements.
  std::unique_ptr<MutatableImageComputerFarm> _farm[2];

  //! All the displays in the grid.
  std::vector<MutatableImageDisplay*> _displays;

  //! Keeps track of which displays are still available for display (they might have been destroyed while an image was computing).
  /*! Non-const because we might need to notify them about various things
   */
  std::set<MutatableImageDisplay*> _known_displays;

  //! Keeps track of which displays are still resizing
  std::set<const MutatableImageDisplay*> _resizing;

  //! The last image spawned (used to regenerate single displays).
  boost::shared_ptr<const MutatableImage> _last_spawned_image;

  //! Pointer to member function used for last spawn.
  SpawnMemberFn _last_spawn_method;

  //! An owned pointer to the current transform factory (needed for Respawn).
  std::unique_ptr<TransformFactory> _transform_factory;

  //! Accessor.
  const boost::shared_ptr<const MutatableImage> last_spawned_image() const
    {
      return _last_spawned_image;
    }
  
  //! Accessor.
  SpawnMemberFn last_spawn_method() const
    {
      return _last_spawn_method;
    }

  //! Not just an accessor.  Takes ownership of a deepclone of the image
  void last_spawned_image(const boost::shared_ptr<const MutatableImage>& image,SpawnMemberFn method);

  //! Accessor
  const TransformFactory& transform_factory() const
    {
      // We shouldn't be here unless transform_factory has been set to something.
      assert(_transform_factory.get()!=0);

      return *_transform_factory;
    }

  //! Not just an accessor.  Takes ownership of a deepclone of the argument.
  void transform_factory(const TransformFactory& tfactory)
    {
      _transform_factory=tfactory.clone();
    }

  //@{
  //! Perform a particular type of spawn from an individiual image to an individual display.  (Locking not checked).
  void spawn_normal(const boost::shared_ptr<const MutatableImage>& image,MutatableImageDisplay* display,bool one_of_many);
  void spawn_recoloured(const boost::shared_ptr<const MutatableImage>& image,MutatableImageDisplay* display,bool one_of_many);
  void spawn_warped(const boost::shared_ptr<const MutatableImage>& image,MutatableImageDisplay* display,bool one_of_many);
  //@}

  //! Spawn the specified display using the specified method.
  void spawn_all(MutatableImageDisplay* display,SpawnMemberFn method,const std::string& action_name);

 public:
  //! Constructor.
  EvolvotronMain
    (
     QWidget* parent,
     const QSize& grid_size,
     uint frames,
     uint framerate,
     uint n_threads,
     bool separate_farm_for_enlargements,
     int niceness_grid,
     int niceness_enlargements,
     bool start_fullscreen,
     bool start_menuhidden,
     bool autocool,
     bool jitter,
     uint multisample_level,
     bool function_debug_mode,
     bool linear_zsweep,
     bool spheremap,
     const std::vector<std::string>& startup_filenames,
     bool startup_shuffle
     );

  //! Destructor.
  ~EvolvotronMain();

  //! Accessor. Returns true if function name recognised.  Forwards to DialogFavourite.
  bool favourite_function(const std::string& f);

  //! Accessor.  Forwards to DialogFavourite.
  void favourite_function_unwrapped(bool v);

  //! Accessor.  
  std::vector<MutatableImageDisplay*>& displays()
    {
      return _displays;
    }

  //! Accessor.
  /*! NB Only const version made available publicly as modifications should be through an appropriate dialog slot.
   */
  const MutationParameters& mutation_parameters() const
    {
      return _mutation_parameters;
    }

  //! Accessor.
  /*! NB Only const version made available publicly as modifications should be through an appropriate dialog slot.
   */
  const RenderParameters& render_parameters() const
    {
      return _render_parameters;
    }

  //! Returns which farm to use for purpose.
  MutatableImageComputerFarm& farm(bool enlargement)
    {
      return *_farm[enlargement && _farm[1].get()];
    }

  //! Accessor.
  History& history()
    {
      return *_history;
    }

  //! Called by History when performing undo.
  void restore(MutatableImageDisplay* display,const boost::shared_ptr<const MutatableImage>&,bool one_of_many);

  //! Called by History to change undo menu status.
  void set_undoable(bool v,const std::string& name);

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

  //! Write a list of known displays (for debugging)
  void list_known(std::ostream& out) const;

 protected:
  //! Handle key-presses
  void keyPressEvent(QKeyEvent* e);

  //! Reset the specified display.
  void reset(MutatableImageDisplay* display);

 protected slots:
  //! Signalled by timer.
  void tick();

  //! Signalled by menu item.  Forwards to History object.
  void undo();

  //! Signalled by menu item.  Simplifies all functions.
  void simplify_constants();
 public slots:

  //! Signalled by menu item.
  void toggle_hide_menu();

  //! Signalled by menu item
  void toggle_fullscreen();
  
  //! Signalled by menu item.  Public because called from evolvotron app wrapper.
  void reset(bool reset_mutation_parameters,bool reset_locks);

  //! Forwards to reset(false)
  void reset_warm();
  
  //! Forwards to reset(true)
  void reset_cold();

  //! Resets and randomizes function weightings
  void reset_randomized();

  //! So we can update any exposed mutation parameters (e.g autocool enable, generation count)
  void mutation_parameters_changed();

  //! So we can re-render when render parameters change
  void render_parameters_changed();
};

#endif

