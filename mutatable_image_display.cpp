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
  \brief Implementation of class MutatableImageDisplay.
*/

#include <iostream>

#include <qimage.h>
#include <qcursor.h>
#include <qapplication.h>

#include "mutatable_image_display.h"
#include "evolvotron_main.h"
#include "mutatable_image_computer_task.h"

/*! The constructor is passed the owning widget (probably either a QGrid or null if top-level),
  the EvolvotronMain providing spawn and farm services, and a flag to specify whether this is
  a fully functional display or a restricted one (e.g no spawning for a single top pleve display).
 */
MutatableImageDisplay::MutatableImageDisplay(QWidget* parent,EvolvotronMain* mn,bool full)
  :QWidget(parent)
  ,_main(mn)
  ,_full_functionality(full)
  ,_resize_in_progress(false)
  ,_locked(false)
  ,_current_display_level(0)
  ,_offscreen_buffer(0)
  ,_image(0)
  ,_menu(0)
{
  _offscreen_buffer=new QPixmap();

  // This will stop the widget being cleared to grey before paintEvent is called (supposed to reduce flicker; not 100% though).
  setBackgroundMode(Qt::NoBackground);

  _menu=new QPopupMenu(this);  

  // Most items on the context menu aren't appropriate for a window displaying a single big image
  if (_full_functionality)
    {
      // We want to use a checkmark for "Locked"
      _menu->setCheckable(true);

      _menu_item_number_spawn=_menu->insertItem("&Spawn",this,SLOT(menupick_spawn()));
      _menu->insertSeparator();
      _menu_item_number_lock =_menu->insertItem("&Lock",this,SLOT(menupick_lock()));
      _menu->insertSeparator();
      _menu_item_number_big  =_menu->insertItem("&Big",this,SLOT(menupick_bigwin()));
    }

  _menu_item_number_save =_menu->insertItem("&Save");

  //! \todo: Add save functionality and re-enable Save menu item
  _menu->setItemEnabled(_menu_item_number_save,false);

  main()->hello(this);
}

/*! Destructor signs off from EvolvotronMain to prevent further attempts at completed task delivery.
 */
MutatableImageDisplay::~MutatableImageDisplay()
{
  main()->farm()->abort_for(this);
  main()->goodbye(this);
  delete _image;
  delete _offscreen_buffer;
}

void MutatableImageDisplay::image(MutatableImageNode* i)
{
  // This might have already been done (e.g by resizeEvent), but it can't hurt to be sure.
  main()->farm()->abort_for(this);

  // Careful: we could be passed the same image again (used by resize to trigger recompute & redisplay)
  if (i!=_image)
    {
      delete _image;
      _image=i;
    }

  // If we start recomputing again we need to accept any delivered images.
  _current_display_level=(uint)(-1);
  
  if (i!=0)
    {
      // Allow for displays up to 4096 pixels high or wide
      for (int level=12;level>=0;level--)
	{
	  const int s=(1<<level);
	  
	  if (size().width()>=s && size().height()>=s)
	    {
	      /*! \todo The image tree has no pixel or task specific state, so deepcloning is way overkill.
		Would just need some referencing counting wrapper
		(mainly to take ensure images aren't deleted while still being used by tasks) 
		to avoid having to deepclone.
		On the other hand this seems to work very robustly for now and isn't a killer.
		*/
	      main()->farm()->push_todo(new MutatableImageComputerTask(this,_image->deepclone(),size()/s,level));
	    }
	}
    }
}

void MutatableImageDisplay::deliver(MutatableImageComputerTask* task)
{
  // Ignore tasks which were aborted or which have somehow got out of order (not impossible with multiple compute threads).
  if (!task->aborted() && task->level()<_current_display_level)
    {
      QImage new_image
	(
	 (uchar*)&(task->image_data()[0]),
	 task->size().width(),
	 task->size().height(),
	 32,
	 0,
	 0,
	 QImage::LittleEndian  // Seems to be right for hi[-/R/G/B]lo packing
	 );
		  
      //! \todo Pick a scaling mode: smooth or not (or put it under GUI control).  Curiously, although smoothscale seems to be noticeably slower but doesn't look any better.
      _offscreen_buffer->convertFromImage(new_image.scale(size()));
		  
      //! Note the resolution we've displayed so out-of-order low resolution images are dropped
      _current_display_level=task->level();

      // Update what's on the screen.
      //! \todo Any case for calling update() instead of repaint() ?
      repaint();
    }

  // This will also delete the tasks deepcloned image
  delete task;
}

void MutatableImageDisplay::paintEvent(QPaintEvent*)
{
  // Repaint the screen from the offscreen buffer
  // (If there hasve be resizes it will be black)
  bitBlt(this,0,0,_offscreen_buffer);

  // If this is the first paint event after a resize we can start computing images again.
  if (_resize_in_progress)
    {
      image(_image);
      _resize_in_progress=false;
    }
}

/*! In the resize event we just focus on shutting down existing compute threads
  because they'll all have to be restarted.
  NB Multiple resize events can be received before a repaint occurs.
 */
void MutatableImageDisplay::resizeEvent(QResizeEvent* event)
{
  // Resize and reset our offscreen buffer (something to do while we wait)
  _offscreen_buffer->resize(event->size());
  _offscreen_buffer->fill(black);

  // Abort all current tasks because they'll be the wrong size.
  main()->farm()->abort_for(this);

  // Flag for the next paintEvent to tell it a recompute can be started now.
  _resize_in_progress=true;
}

void MutatableImageDisplay::mousePressEvent(QMouseEvent* event)
{
  if (event->button()==RightButton)
    {
      _menu->exec(QCursor::pos());
    }
  else
    {
      if (_full_functionality)
	{
	  menupick_spawn();
	}
    }
}

/*! This slot is called by selecting the "Spawn" context menu item, or by clicking the image
 */
void MutatableImageDisplay::menupick_spawn()
{
  main()->spawn(this);
}

/*! This slot is called by selecting the "Big" context menu item.
  It creates an independent top level display area with its own copy of the image.
  \todo: There should be an option to make a bigwin with scrollbars.
 */
void MutatableImageDisplay::menupick_bigwin()
{
  // Create an image display with no parent: becomes a top level window 
  // Disable full menu functionality because there's less we can do with a single image (e.g no spawn_target)
  MutatableImageDisplay* window=new MutatableImageDisplay(0,main(),false);

  window->resize(800,800);
  window->show();

  window->image(_image->deepclone());
}

/*! This slot is called by selecting the "Lock" context menu item.
  It stops the image from being overwritten by a new image.
 */
void MutatableImageDisplay::menupick_lock()
{
  _locked=!_locked;

  _menu->setItemChecked(_menu_item_number_lock,_locked);
}
