// Source file for evolvotron
// Copyright (C) 2002,2003,2007 Tim Day
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
  \brief Interface for class MutatableImageDisplay
*/

#ifndef _mutatable_image_display_h_
#define _mutatable_image_display_h_

#include <qwidget.h>
#include <qpixmap.h>
#include <qpopupmenu.h>

#include "useful.h"
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
  bool _full_functionality;

  //! Flag for whether the offscreen buffer has fixed size
  bool _fixed_size;

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
  /*! Needed to handle possible (but unlikely?) out of order task returns from multiple compute threads.
   */
  uint _current_display_level;

  //! Offscreen image buffer.
  boost::ptr_vector<QPixmap> _offscreen_buffer;

  //! Offscreen image buffer in sensible image format (used for save, as pixmap is in display format which might be less bits).
  boost::ptr_vector<QImage> _offscreen_image;

  //! Offscreen image data for _offscreen_image.  This must remain alive longer than the QImage.
  boost::shared_array<uint> _offscreen_image_data;

  //! The image function being displayed (its root node).
  /*! The held image is const because references to it could be held by history archive, compute tasks etc,
    so it should be completely replaced rather than manipulated.
   */
  boost::shared_ptr<const MutatableImage> _image;

  //! Properties dialog.
  DialogMutatableImageDisplay* _properties;

  //! Context (right-click) menu.
  QPopupMenu* _menu;

  //! Submenu for spawn warped options.
  QPopupMenu* _menu_warped;

  //! Submenu for Big image options.
  QPopupMenu* _menu_big;

  //! Position of item in menu.
  /*! This is the only menu item we need to retain this information for becuase we need it to set the lock check-mark.
   */
  uint _menu_item_number_lock;

  //! Coordinate of mouse event which started mid-button adjustment
  QPoint _mid_button_adjust_start_pos;

  //! Coordinate of last mouse event when mid-button adjusting
  QPoint _mid_button_adjust_last_pos;

  //! Serial number to kill some rare problems with out-of-order tasks being returned
  unsigned long long int _serial;

 public:
  //! Constructor.  
  MutatableImageDisplay(QWidget* parent,EvolvotronMain* mn,bool full,bool fixed_size,const QSize& image_size,uint f,uint fr);

  //! Destructor.
  virtual ~MutatableImageDisplay();

  //! Accessor.
  const boost::shared_ptr<const MutatableImage>& image()
    {
      return _image;
    }

  //! Accessor.
  const bool locked() const
    {
      return (_image.get()!=0 ? _image->locked() : false);
    }

  //! Accessor.
  EvolvotronMain*const main() const
    {
      assert(_main!=0);
      return _main;
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
  void image(const boost::shared_ptr<const MutatableImage>& image);

  //! Evolvotron main calls this with completed (but possibly aborted) tasks.
  void deliver(const boost::shared_ptr<const MutatableImageComputerTask>& task);

  //! Set the lock state.
  void lock(bool l,bool record_in_history);

 protected:
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
  const uint simplify_constants(bool single);

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
