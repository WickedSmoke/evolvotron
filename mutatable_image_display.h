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

class EvolvotronMain;
class MutatableImageComputerTask;

//! Widget responsible for displaying a MutatableImage (which is the same as a MutatableImageNode currently).
/*! A MutatableImageDisplay is responsible for displaying the image computed from the MutatableImageNode it owns.
  Computations are split off into separate threads to take advantage of multiprocessor machines.
 */
class MutatableImageDisplay : public QWidget
{
  Q_OBJECT

 protected:
  //! Pointer back to the application object to access services.
  EvolvotronMain*const _main;

  //! Flag for whether context menu should display all options.
  bool _full_functionality;

  //! Flag for whether the offscreen buffer has fixed size
  bool _fixed_size;

  //! Size of offscreen buffer
  QSize _image_size;

  //! Flag indicating resize is in progress (between resizeEvent and subsequent paintEvent).
  /*! Used to supress unnecessary task spawning.
   */
  bool _resize_in_progress;

  //! Flag indicating the display is considered locked and shouldn't be loaded with a new image.
  bool _locked;

  //! The resolution level currently displaying (0=1-for-1 pixels, 1=half resolution etc).
  /*! Needed to handle possible (but unlikely?) out of order task returns from multiple compute threads.
   */
  uint _current_display_level;

  //! Offscreen image buffer.
  QPixmap* _offscreen_buffer;

  //! Offscreen image buffer in sensible image format (used for save, as pixmap is in display format).
  QImage* _offscreen_image;

  //! Offscreen image data for _offscreen_image.  This must remain alive longer than the QImage.
  std::vector<uint>* _offscreen_image_data;

  //! The image being displayed (its root node).
  MutatableImageNode* _image;

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

  //@{
  //! Position of item in menu_big.
  uint _menu_big_item_number_resizable;
  uint _menu_big_item_number_640x480;
  uint _menu_big_item_number_1024x768;
  uint _menu_big_item_number_1280x960;
  uint _menu_big_item_number_1600x1200;
  uint _menu_big_item_number_512x512;
  uint _menu_big_item_number_1024x1024;
  uint _menu_big_item_number_2048x2048;
  uint _menu_big_item_number_4096x4096;
  //@}

 public:
  //! Constructor.
  MutatableImageDisplay(QWidget* parent,EvolvotronMain* mn,bool full,bool fixed_size,const QSize& image_size);

  //! Destructor.
  virtual ~MutatableImageDisplay();

  //! Accessor.
  MutatableImageNode*const image()
    {
      return _image;
    }

  //! Accessor.
  const bool locked() const
    {
      return _locked;
    }

  //! Accessor.
  EvolvotronMain*const main() const
    {
      return _main;
    }

  //! Accessor.
  const QSize& image_size() const
    {
      return _image_size;
    }

  //! Load a new image (clears up old image, starts new compute tasks).
  void image(MutatableImageNode* image);

  //! Evolvotron main calls this with completed (but possibly aborted) tasks.
  void deliver(MutatableImageComputerTask* task);

  //! Set the lock state.
  void lock(bool l);

 protected:
  //! Usual handler for repaint events.
  virtual void paintEvent(QPaintEvent* event);

  //! Usual handler for resize events.
  virtual void resizeEvent(QResizeEvent* event);

  //! Handler for mouse events.
  virtual void mousePressEvent(QMouseEvent* event);

  protected slots:

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
  void menupick_lock();

  //! Called from context menu.
  void menupick_save();

  //! Called from "Big" submenu of context menu.
  void menupick_big_resizable();

  //! Called from "Big" submenu of context menu.
  void menupick_big_512x512();

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

 protected:
  //! Common code for big slots.
  void spawn_big(bool scrollable,const QSize& sz);
};

#endif
