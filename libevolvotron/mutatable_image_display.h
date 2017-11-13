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
  \brief Interface for class MutatableImageDisplay
*/

#ifndef _mutatable_image_display_h_
#define _mutatable_image_display_h_

#include "common.h"

#include "mutatable_image.h"
#include "mutatable_image_computer.h"
#include "dialog_mutatable_image_display.h"

class EvolvotronMain;
class MutatableImageComputerTask;

//! Widget responsible for displaying a MutatableImage.
/*! A MutatableImageDisplay is responsible for displaying the image computed from the MutatableImage it owns.
  Computations are split off into separate threads to take advantage of multiprocessor machines.
 */
class MutatableImageDisplay : public QWidget
{
  Q_OBJECT

 protected:
  //! Pointer back to the application object to access services.
  EvolvotronMain* _main;

  //! Flag for whether context menu should display all options.
  /*! false also implies a standalone window
   */
  const bool _full_functionality;

  //! Flag for whether the offscreen buffer has fixed size
  const bool _fixed_size;

  //! Size of offscreen buffer
  QSize _image_size;

  //! Number of frames in image
  uint _frames;

  //! Framerate for animation.
  uint _framerate;

  //! Currently displaying frame.
  uint _current_frame;

  //! Direction to play
  bool _animate_reverse;

  //! Timer for animating frames
  QTimer* _timer;

  //! Flag indicating resize is in progress (between resizeEvent and subsequent paintEvent).
  /*! Used to supress unnecessary task spawning.
   */
  bool _resize_in_progress;

  //! The resolution level currently displaying (0=1-for-1 pixels, 1=half resolution etc).
  /*! Needed to handle possible out of order task returns from multiple compute threads.
   */
  uint _current_display_level;

  //! Similar to _current_display_level, but for tracking multisample grids within a resolution level.
  uint _current_display_multisample_grid;

  //! An image suitable for setting as an icon.
  std::unique_ptr<QPixmap> _icon;

  //! Track which image the icon is actually of.
  unsigned long long int _icon_serial;

  //! Offscreen image buffer.
  std::vector<QPixmap> _offscreen_pixmaps;

  //! Offscreen image buffer in sensible image format (used for save, as pixmap is in display format which might be less bits).
  std::vector<QImage> _offscreen_images;

  //! Type for staging area for incoming fragments.
  /*! Key is level and multisampling, mapped type is also itself a map from fragment number to tasks.
   */
  typedef std::map<std::pair<uint,uint>,std::map<uint,boost::shared_ptr<const MutatableImageComputerTask> > > OffscreenImageInbox;

  //! Staging area for incoming fragments.
  /*! Fragments are accumulated for each (level,multisample) key, and completed levels passed on for display
   */
  OffscreenImageInbox _offscreen_images_inbox;

  //! The image function being displayed (its root node).
  /*! The held image is const because references to it could be held by history archive, compute tasks etc,
    so it should be completely replaced rather than manipulated.
   */
  boost::shared_ptr<const MutatableImage> _image_function;

  //! Properties dialog.
  DialogMutatableImageDisplay* _properties;

  //! Context (right-click) menu.
  QMenu* _menu;

  //! Submenu for spawn warped options.
  QMenu* _menu_warped;

  //! Submenu for Big image options.
  QMenu* _menu_big;

  //! Position of item in menu.
  /*! This is the only menu item we need to retain this information for becuase we need it to set the lock check-mark.
   */
  QAction* _menu_item_action_lock;

  //! Coordinate of mouse event which started mid-button adjustment
  QPoint _mid_button_adjust_start_pos;

  //! Coordinate of last mouse event when mid-button adjusting
  QPoint _mid_button_adjust_last_pos;

  //! Serial number to kill some rare problems with out-of-order tasks being returned
  unsigned long long int _serial;

 public:
  //! Constructor.  
  MutatableImageDisplay(EvolvotronMain* mn,bool full_functionality,bool fixed_size,const QSize& image_size,uint f,uint fr);

  //! Destructor.
  virtual ~MutatableImageDisplay();

  //! Accessor.
  const boost::shared_ptr<const MutatableImage>& image_function()
    {
      return _image_function;
    }

  //! Accessor.
  bool locked() const
    {
      return (_image_function.get()!=0 ? _image_function->locked() : false);
    }

  //! Accessor.
  EvolvotronMain& main() const
    {
      assert(_main!=0);
      return *_main;
    }
  
  //! Accessor.
  void main(EvolvotronMain* m)
    {
      _main=m;
    }

  //! Accessor.
  const QSize& image_size() const
    {
      return _image_size;
    }

  //! Load a new image (clears up old image, starts new compute tasks).
  /*! When the one_of_many parameter is true, it implies many other images are also being updated
    (affects fragmentation strategy for multithreading).
   */
  void image_function(const boost::shared_ptr<const MutatableImage>& image_fn,bool one_of_many);

  //! Evolvotron main calls this with completed (but possibly aborted) tasks.
  void deliver(const boost::shared_ptr<const MutatableImageComputerTask>& task);

  //! Set the lock state.
  void lock(bool l,bool record_in_history);

 protected:

  //! Which farm this display should use.
  MutatableImageComputerFarm& farm() const;

  //! Usual handler for repaint events.
  virtual void paintEvent(QPaintEvent* event);

  //! Usual handler for resize events.
  virtual void resizeEvent(QResizeEvent* event);

  //! Handler for mouse events.
  virtual void mousePressEvent(QMouseEvent* event);

  //! Handler for mouse events.
  virtual void mouseMoveEvent(QMouseEvent* event);

  public slots:

  //! Simplify the held image, return the number of nodes eliminated
  uint simplify_constants(bool single);

  //! Load a function from the given filename.
  void load_function_file(const QString&);

  protected slots:

  //! Called by timer
  void frame_advance();

  //! Called from context menu.
  void menupick_respawn();

  //! Called from context menu and also by click event.
  void menupick_spawn();

  //! Called from context menu.
  void menupick_spawn_recoloured();

  //! Called from context menu.
  void menupick_spawn_warped_random();

  //! Called from context menu.
  void menupick_spawn_warped_zoom_in();

  //! Called from context menu.
  void menupick_spawn_warped_zoom_out();

  //! Called from context menu.
  void menupick_spawn_warped_rotate();

  //! Called from context menu.
  void menupick_spawn_warped_pan_xy();

  //! Called from context menu.
  void menupick_spawn_warped_pan_x();

  //! Called from context menu.
  void menupick_spawn_warped_pan_y();

  //! Called from context menu.
  void menupick_spawn_warped_pan_z();

  //! Called from context menu.
  void menupick_lock();

  //! Trivial wrapper for simplify_constants
  void menupick_simplify();

  //! Called from context menu.
  void menupick_save_image();

  //! Called from context menu.
  void menupick_save_function();

  //! Called from context menu.
  void menupick_load_function();

  //! Called from "Big" submenu of context menu.
  void menupick_big_resizable();

  //! Called from "Big" submenu of context menu.
  void menupick_big_256x256();

  //! Called from "Big" submenu of context menu.
  void menupick_big_512x512();

  //! Called from "Big" submenu of context menu.
  void menupick_big_768x768();

  //! Called from "Big" submenu of context menu.
  void menupick_big_1024x1024();

  //! Called from "Big" submenu of context menu.
  void menupick_big_640x480();

  //! Called from "Big" submenu of context menu.
  void menupick_big_1024x768();

  //! Called from "Big" submenu of context menu.
  void menupick_big_1280x960();

  //! Called from "Big" submenu of context menu.
  void menupick_big_1600x1200();

  //! Called from "Big" submenu of context menu.
  void menupick_big_2048x2048();

  //! Called from "Big" submenu of context menu.
  void menupick_big_4096x4096();

  //! Called from "Properties" on context menu
  void menupick_properties();

 protected:
  //! Common code for big slots.
  void spawn_big(bool scrollable,const QSize& sz);
};

#endif
