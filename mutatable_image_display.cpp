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
#include <qscrollview.h>

#include "mutatable_image_display.h"
#include "evolvotron_main.h"
#include "mutatable_image_computer_task.h"

/*! The constructor is passed:
    - the owning widget (probably either a QGrid or null if top-level),
    - the EvolvotronMain providing spawn and farm services, 
    - a flag to specify whether this is a fully functional display or a restricted one (e.g no spawning for a single top pleve display),
    - a flag to specify whether the offscreen buffer has fixed size (in which case scrollbars are used),
    - and the size of the offscreen buffer (only used if fixed_size is true).
    Note that we use Qt's WDestructiveCode flag to ensure the destructor is called on close
 */
MutatableImageDisplay::MutatableImageDisplay(QWidget* parent,EvolvotronMain* mn,bool full,bool fixed_size,const QSize& sz)
  :QWidget(parent,0,Qt::WDestructiveClose)
   ,_main(mn)
   ,_full_functionality(full)
   ,_fixed_size(fixed_size)
   ,_image_size(sz)
   ,_resize_in_progress(false)
   ,_locked(false)
   ,_current_display_level(0)
   ,_offscreen_buffer(0)
   ,_image(0)
   ,_menu(0)
   ,_menu_big(0)
{
  _offscreen_buffer=new QPixmap();

  // We DO want background drawn for fixed size because window could be bigger than image (not entirely satisfactory however: flickers)
  if (!fixed_size)
    {
      // This will stop the widget being cleared to grey before paintEvent is called (supposed to reduce flicker; not 100% though).
      setBackgroundMode(Qt::NoBackground);
    }

  _menu=new QPopupMenu(this);  

  // Most items on the context menu aren't appropriate for a window displaying a single big image
  if (_full_functionality)
    {
      // We want to use a checkmark for "Locked"
      _menu->setCheckable(true);

      _menu_item_number_spawn=_menu->insertItem("&Spawn",this,SLOT(menupick_spawn()));
      _menu_item_number_spawn_recoloured=_menu->insertItem("&Recolour",this,SLOT(menupick_spawn_recoloured()));
      _menu_item_number_spawn_warped=_menu->insertItem("&Warp",this,SLOT(menupick_spawn_warped()));
      _menu->insertSeparator();
      _menu_item_number_lock =_menu->insertItem("&Lock",this,SLOT(menupick_lock()));
      _menu->insertSeparator();
      
      _menu_big=new QPopupMenu(this);
      _menu_big_item_number_resizable  =_menu_big->insertItem("&Resizable",this,SLOT(menupick_big_resizable()));
      _menu_big_item_number_1024x1024  =_menu_big->insertItem("&1024x1024",this,SLOT(menupick_big_1024x1024()));
      _menu_big_item_number_2048x2048  =_menu_big->insertItem("&2048x2048",this,SLOT(menupick_big_2048x2048()));
      _menu_big_item_number_4096x4096  =_menu_big->insertItem("&4096x4096",this,SLOT(menupick_big_4096x4096()));
      _menu_big->insertSeparator();
      _menu_big_item_number_1280x960   =_menu_big->insertItem("1280x&960",this,SLOT(menupick_big_1280x960()));
      _menu_big_item_number_1600x1200  =_menu_big->insertItem("1&600x1200",this,SLOT(menupick_big_1600x1200()));

      _menu_item_number_big  =_menu->insertItem("&Big",_menu_big);
    }

  _menu_item_number_save =_menu->insertItem("&Save");

  //! \todo: Add save functionality and re-enable Save menu item
  _menu->setItemEnabled(_menu_item_number_save,false);

  main()->hello(this);

  if (_fixed_size)
    {
      setGeometry(0,0,image_size().width(),image_size().height());
    }
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
	  
	  if (image_size().width()>=s && image_size().height()>=s)
	    {
	      /*! \todo The image tree has no pixel or task specific state, so deepcloning is way overkill.
		Would just need some referencing counting wrapper
		(mainly to take ensure images aren't deleted while still being used by tasks) 
		to avoid having to deepclone.
		On the other hand this seems to work very robustly for now and isn't a killer.
		*/
	      main()->farm()->push_todo(new MutatableImageComputerTask(this,_image->deepclone(),image_size()/s,level));
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
      _offscreen_buffer->convertFromImage(new_image.scale(image_size()));
		  
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
  NB There's nothing to be done for fixed size images (not even setting _resize_in_progress).
 */
void MutatableImageDisplay::resizeEvent(QResizeEvent* event)
{
  // Fixed size images don't need anything doing here.  We don't even set _resize_in_progress.
  if (!_fixed_size)
    {
      _image_size=event->size();
      
      // Resize and reset our offscreen buffer (something to do while we wait)
      _offscreen_buffer->resize(image_size());
      _offscreen_buffer->fill(black);
      
      // Abort all current tasks because they'll be the wrong size.
      main()->farm()->abort_for(this);
      
      // Flag for the next paintEvent to tell it a recompute can be started now.
      _resize_in_progress=true;
    }
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

/*! This slot is called by selecting the "Spawn Recoloured" context menu item
 */
void MutatableImageDisplay::menupick_spawn_recoloured()
{
  main()->spawn_recoloured(this);
}

/*! This slot is called by selecting the "Spawn Warped" context menu item
 */
void MutatableImageDisplay::menupick_spawn_warped()
{
  main()->spawn_warped(this);
}

/*! This slot is called by selecting the "Lock" context menu item.
  It stops the image from being overwritten by a new image.
 */
void MutatableImageDisplay::menupick_lock()
{
  _locked=!_locked;

  _menu->setItemChecked(_menu_item_number_lock,_locked);
}

void MutatableImageDisplay::menupick_big_resizable()
{
  spawn_big(false,QSize(0,0));
}

void MutatableImageDisplay::menupick_big_1024x1024()
{
  spawn_big(true,QSize(1280,960));
}

void MutatableImageDisplay::menupick_big_1280x960()
{
  spawn_big(true,QSize(1280,960));
}

void MutatableImageDisplay::menupick_big_1600x1200()
{
  spawn_big(true,QSize(1600,1200));
}

void MutatableImageDisplay::menupick_big_2048x2048()
{
  spawn_big(true,QSize(2048,2048));
}

void MutatableImageDisplay::menupick_big_4096x4096()
{
  spawn_big(true,QSize(4096,4096));
}

/*! Create an image display with no parent: becomes a top level window 
  Disable full menu functionality because there's less we can do with a single image (e.g no spawn_target)
*/
void MutatableImageDisplay::spawn_big(bool scrollable,const QSize& sz)
{
  QWidget* top_level_widget=0;
  MutatableImageDisplay* display;

  if (scrollable)
    {
      QScrollView* scrollview=new QScrollView(0,0,Qt::WDestructiveClose);

      top_level_widget=scrollview;

      display=new MutatableImageDisplay(scrollview->viewport(),main(),false,true,sz);
      scrollview->addChild(display);
    }
  else
    {
      display=new MutatableImageDisplay(0,main(),false,false,QSize(0,0));
      
      top_level_widget=display;
    }
  
  top_level_widget->resize(512,512);
  top_level_widget->show();
      
  display->image(_image->deepclone());
}
