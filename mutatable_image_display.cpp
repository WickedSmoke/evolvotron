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

#include <qstring.h>
#include <qimage.h>
#include <qcursor.h>
#include <qapplication.h>
#include <qscrollview.h>
#include <qmessagebox.h>
#include <qfiledialog.h>

#include "mutatable_image_display.h"
#include "evolvotron_main.h"
#include "mutatable_image_computer_task.h"
#include "transform_factory.h"
#include "function_pre_transform.h"

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
   ,_offscreen_image(0)
   ,_offscreen_image_data(0)
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

      _menu->insertItem("&Respawn",this,SLOT(menupick_respawn()));

      _menu->insertSeparator();

      _menu->insertItem("&Spawn",this,SLOT(menupick_spawn()));
      _menu->insertItem("Spawn re&coloured",this,SLOT(menupick_spawn_recoloured()));

      _menu_warped=new QPopupMenu(this);
      _menu_warped->insertItem("Random &Mix",this,SLOT(menupick_spawn_warped_random()));
      _menu_warped->insertItem("Zoom &In",this,SLOT(menupick_spawn_warped_zoom_in()));
      _menu_warped->insertItem("Zoom &Out",this,SLOT(menupick_spawn_warped_zoom_out()));
      _menu_warped->insertItem("&Rotate",this,SLOT(menupick_spawn_warped_rotate()));
      _menu_warped->insertItem("&Pan XY",this,SLOT(menupick_spawn_warped_pan_xy()));
      _menu_warped->insertItem("Pan &X",this,SLOT(menupick_spawn_warped_pan_x()));
      _menu_warped->insertItem("Pan &Y",this,SLOT(menupick_spawn_warped_pan_y()));
      _menu_warped->insertItem("Pan &Z",this,SLOT(menupick_spawn_warped_pan_z()));

      _menu->insertItem("Spawn &warped",_menu_warped);

      _menu->insertSeparator();

      _menu_item_number_lock =_menu->insertItem("&Lock",this,SLOT(menupick_lock()));

      _menu->insertSeparator();
    }
  
  _menu_big=new QPopupMenu(this);
  _menu_big->insertItem("&Resizable",this,SLOT(menupick_big_resizable()));
  _menu_big->insertSeparator();
  _menu_big->insertItem("&256x256",this,SLOT(menupick_big_256x256()));
  _menu_big->insertItem("&512x512",this,SLOT(menupick_big_512x512()));
  _menu_big->insertItem("&1024x1024",this,SLOT(menupick_big_1024x1024()));
  _menu_big->insertItem("&2048x2048",this,SLOT(menupick_big_2048x2048()));
  _menu_big->insertItem("&4096x4096",this,SLOT(menupick_big_4096x4096()));
  _menu_big->insertSeparator();
  _menu_big->insertItem("640x&480",this,SLOT(menupick_big_640x480()));
  _menu_big->insertItem("1024x&768",this,SLOT(menupick_big_1024x768()));
  _menu_big->insertItem("1280x&960",this,SLOT(menupick_big_1280x960()));
  _menu_big->insertItem("1&600x1200",this,SLOT(menupick_big_1600x1200()));

  _menu->insertItem("&Big",_menu_big);

  _menu->insertItem("&Save",this,SLOT(menupick_save()));

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
  assert(_image->ok());

  // During app shutdown EvolvotronMain might have already been destroyed so only call it if it's there.
  // Don't use main() because it asserts non-null.
  if (_main)
    {
      main()->farm()->abort_for(this);
      main()->goodbye(this);
    }

  delete _image;
  delete _offscreen_buffer;
  delete _offscreen_image;
  delete _offscreen_image_data;
}

void MutatableImageDisplay::image(MutatableImage* i)
{
  assert(_image==0 || _image->ok());
  assert(i==0 || i->ok());

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
	      const MutatableImage*const task_image=_image->deepclone();
	      assert(task_image->ok());

	      MutatableImageComputerTask* task=new MutatableImageComputerTask(this,task_image,image_size()/s,level);

	      main()->farm()->push_todo(task);
	    }
	}
    }
}

void MutatableImageDisplay::deliver(MutatableImageComputerTask* task)
{
  // Ignore tasks which were aborted or which have somehow got out of order (not impossible with multiple compute threads).
  if (!task->aborted() && task->level()<_current_display_level)
    {
      delete _offscreen_image;
      delete _offscreen_image_data;

      // Copy image data out of task
      _offscreen_image_data=new std::vector<uint>(task->image_data());
      
      _offscreen_image
	=new QImage
	(
	 (uchar*)&((*_offscreen_image_data)[0]),
	 task->size().width(),
	 task->size().height(),
	 32,
	 0,
	 0,
	 QImage::LittleEndian  // Seems to be right for hi[-/R/G/B]lo packing
	 );
  
      //! \todo Pick a scaling mode: smooth or not (or put it under GUI control). 
      // Curiously, although smoothscale seems to be noticeably slower, it doesn't look any better.
      _offscreen_buffer->convertFromImage(_offscreen_image->scale(image_size()));
		  
      //! Note the resolution we've displayed so out-of-order low resolution images are dropped
      _current_display_level=task->level();

      // Update what's on the screen.
      //! \todo Any case for calling update() instead of repaint() ?  Repaint maybe feels smoother.
      repaint();
    }

  // This will also delete the task's deepcloned image
  delete task;
}

void MutatableImageDisplay::lock(bool l)
{
  _locked=l;

  _menu->setItemChecked(_menu_item_number_lock,_locked);
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
  else if (event->button()==MidButton)
    {
      // Take a snapshot to undo back to.
      main()->history().begin_action("middle-button drag");
      main()->history().replacing(this);
      main()->history().end_action();

      _mid_button_adjust_start_pos=event->pos();
      _mid_button_adjust_last_pos=event->pos();
    }
  else if (_full_functionality && event->button()==LeftButton)
    {
      menupick_spawn();
    }
}

void MutatableImageDisplay::mouseMoveEvent(QMouseEvent* event)
{
  if (event->state()&MidButton)
    {
      const QPoint pixel_delta(event->pos()-_mid_button_adjust_last_pos);
      
      // Build the transform caused by the adjustment
      Transform transform;

      // Shift button (no ctrl) is various zooms
      if (event->state()&ShiftButton && !(event->state()&ControlButton))
	{
	  // Alt-Shift is anisotropic
	  if (event->state()&AltButton)
	    {
	      // Only scale in non-degenerate cases
	      if (
		  event->pos().x()!=image_size().width()/2
		  && event->pos().y()!=image_size().height()/2
		  && _mid_button_adjust_last_pos.x()!=image_size().width()/2
		  && _mid_button_adjust_last_pos.y()!=image_size().height()/2
		  )
		{
		  XYZ scale(
			    (event->pos().x()-image_size().width() /2) / static_cast<float>(_mid_button_adjust_last_pos.x()-image_size().width() /2),
			    (event->pos().y()-image_size().height()/2) / static_cast<float>(_mid_button_adjust_last_pos.y()-image_size().height()/2),
			    0.0f
			    );
	      
		  transform.basis_x(XYZ(1.0f/scale.x(),          0.0f,0.0f));
		  transform.basis_y(XYZ(          0.0f,1.0f/scale.y(),0.0f));
		}
	    }
	  // Shift button alone is isotropic zoom
	  else 
	    {
	      const float cx=image_size().width()/2.0f;
	      const float cy=image_size().width()/2.0f;

	      const float dx=event->pos().x()-cx;
	      const float dy=event->pos().y()-cy;
	      
	      const float last_dx=_mid_button_adjust_last_pos.x()-cx;
	      const float last_dy=_mid_button_adjust_last_pos.y()-cy;

	      const float radius=sqrt(dx*dx+dy*dy);
	      const float last_radius=sqrt(last_dx*last_dx+last_dy*last_dy);

	      // Only scale in non-degenerate cases
	      if (radius!=0.0f && last_radius!=0.0f)
		{
		  const float scale=radius/last_radius;
		  transform.basis_x(XYZ(1.0f/scale,          0.0f,0.0f));
		  transform.basis_y(XYZ(      0.0f,1.0f/scale,0.0f));
		}
	    }
	}
      else if (event->state()&ControlButton)
	{
	  // Control-alt is shear
	  if (event->state()&AltButton)
	    {
	      const float cx=image_size().width()/2.0f;
	      const float cy=image_size().width()/2.0f;

	      const float dx=(event->pos().x()-_mid_button_adjust_last_pos.x())/cx;
	      const float dy=(event->pos().y()-_mid_button_adjust_last_pos.y())/cy;
	      
	      transform.basis_x(XYZ(1.0f, -dy,0.0f));
	      transform.basis_y(XYZ(  dx,1.0f,0.0f));
	    }
	  // Control button only is rotate
	  else
	    {
	      const float cx=image_size().width()/2.0f;
	      const float cy=image_size().width()/2.0f;
	      
	      const float dx=event->pos().x()-cx;
	      const float dy=event->pos().y()-cy;
	      
	      const float last_dx=_mid_button_adjust_last_pos.x()-cx;
	      const float last_dy=_mid_button_adjust_last_pos.y()-cy;
	      
	      const float a=atan2(dy,dx);
	      const float last_a=atan2(last_dy,last_dx);
	  
	      const float rot=a-last_a;
	  
	      const float sr=sin(rot);
	      const float cr=cos(rot);

	      transform.basis_x(XYZ( cr,sr,0.0f));
	      transform.basis_y(XYZ(-sr,cr,0.0f));
	    }
	}
      // Default (no interesting modifier keys detected) is panning around
      else
	{
	  XYZ translate(
			(-2.0f*pixel_delta.x())/image_size().width(),
			( 2.0f*pixel_delta.y())/image_size().height(),
			0.0f
			);
	  transform.translate(translate);
	}

      FunctionNode* new_root;
      if (image()->root()->is_a_FunctionNodeUsingFunctionPreTransform())
	{
	  // If the image root is a transform wrapper then we can just concatentate transforms.
	  new_root=image()->root()->deepclone();
	  
	  FunctionNodeUsing<FunctionPreTransform>*const new_root_as_transform=new_root->is_a_FunctionNodeUsingFunctionPreTransform();
	  assert(new_root_as_transform!=0);

	  // Have to access params() through this to avoid protected scope of non-const version creating an error.
	  const FunctionNodeUsing<FunctionPreTransform>*const const_new_root_as_transform=new_root_as_transform;

	  Transform current_transform(const_new_root_as_transform->params());
	  current_transform.concatenate_on_right(transform);
	  new_root_as_transform->params(current_transform.get_columns());
	}
      else
	{
	  // Otherwise have to create a new wrapper for the transform:

	  std::vector<float> params=transform.get_columns();

	  std::vector<FunctionNode*> args;	  
	  args.push_back(image()->root()->deepclone());
	  
	  new_root=new FunctionNodeUsing<FunctionPreTransform>(params,args);
	}

      // Install new image (triggers recompute).
      image(new MutatableImage(new_root));

      // Finally, record position of this event as last event
      _mid_button_adjust_last_pos=event->pos();
    }
}

/*! This slot is called by selecting the "Respawn" context menu item
 */
void MutatableImageDisplay::menupick_respawn()
{
  main()->respawn(this);
}

/*! This slot is called by selecting the "Spawn" context menu item, or by clicking the image
 */
void MutatableImageDisplay::menupick_spawn()
{
  main()->spawn_normal(this);
}

/*! This slot is called by selecting the "Spawn Recoloured" context menu item
 */
void MutatableImageDisplay::menupick_spawn_recoloured()
{
  main()->spawn_recoloured(this);
}

/*! This slot is called by selecting the "Spawn Warped/Random" context menu item
 */
void MutatableImageDisplay::menupick_spawn_warped_random()
{
  TransformFactoryRandomWarpXY transform_factory;
  
  main()->spawn_warped(this,transform_factory);
}

void MutatableImageDisplay::menupick_spawn_warped_zoom_in()
{
  TransformFactoryRandomScaleXY transform_factory(-2.0f,0.0f);

  main()->spawn_warped(this,transform_factory);
}

void MutatableImageDisplay::menupick_spawn_warped_zoom_out()
{
  TransformFactoryRandomScaleXY transform_factory(0.0f,2.0f);

  main()->spawn_warped(this,transform_factory);
}

void MutatableImageDisplay::menupick_spawn_warped_rotate()
{
  TransformFactoryRandomRotateZ transform_factory;

  main()->spawn_warped(this,transform_factory);
}

void MutatableImageDisplay::menupick_spawn_warped_pan_xy()
{
  TransformFactoryRandomTranslateXYZ transform_factory(XYZ(0.0f,0.0f,0.0f),XYZ(1.0f,1.0f,0.0f));

  main()->spawn_warped(this,transform_factory);
}

void MutatableImageDisplay::menupick_spawn_warped_pan_x()
{
  TransformFactoryRandomTranslateXYZ transform_factory(XYZ(0.0f,0.0f,0.0f),XYZ(1.0f,0.0f,0.0f));

  main()->spawn_warped(this,transform_factory);
}

void MutatableImageDisplay::menupick_spawn_warped_pan_y()
{
  TransformFactoryRandomTranslateXYZ transform_factory(XYZ(0.0f,0.0f,0.0f),XYZ(0.0f,1.0f,0.0f));

  main()->spawn_warped(this,transform_factory);
}

void MutatableImageDisplay::menupick_spawn_warped_pan_z()
{
  TransformFactoryRandomTranslateXYZ transform_factory(XYZ(0.0f,0.0f,0.0f),XYZ(0.0f,0.0f,1.0f));

  main()->spawn_warped(this,transform_factory);
}

/*! This slot is called by selecting the "Lock" context menu item.
  It stops the image from being overwritten by a new image.
 */
void MutatableImageDisplay::menupick_lock()
{
  lock(!_locked);
}

/*! Saves image (unless the image is not full resolution yet, in which case an informative dialog is generated.
 */
void MutatableImageDisplay::menupick_save()
{
  if (_current_display_level!=0)
    {
      QMessageBox::information(this,"Evolvotron","The selected image has not yet been generated at maximum resolution.\nPlease try again later.");
    }
  else
    {
      QString save_filename=QFileDialog::getSaveFileName(".","Images(*.ppm *.png)",this,"Save image","Save image to a .ppm or .png file");
      if (!save_filename.isEmpty())
	{
	  QString save_format("PPM");
	  if (save_filename.upper().endsWith(".PPM"))
	    {
	      save_format="PPM";
	    }
	  else if (save_filename.upper().endsWith(".PNG"))
	    {
	      save_format="PNG";
	    }
	  else
	    {
	      QMessageBox::warning(this,"Evolvotron","Unrecognised file suffix.\nFile will be written in "+save_format+" format.");
	    }
	  
	  if (!save_filename.isEmpty())
	    {
	      if (!_offscreen_image->save(save_filename,save_format))
		{
		  QMessageBox::critical(this,"Evolvotron","File write failed");
		}
	    }
	}
    }
}

void MutatableImageDisplay::menupick_big_resizable()
{
  spawn_big(false,QSize(0,0));
}

void MutatableImageDisplay::menupick_big_640x480()
{
  spawn_big(true,QSize(640,480));
}

void MutatableImageDisplay::menupick_big_1024x768()
{
  spawn_big(true,QSize(1024,768));
}

void MutatableImageDisplay::menupick_big_1280x960()
{
  spawn_big(true,QSize(1280,960));
}

void MutatableImageDisplay::menupick_big_1600x1200()
{
  spawn_big(true,QSize(1600,1200));
}

void MutatableImageDisplay::menupick_big_256x256()
{
  spawn_big(true,QSize(256,256));
}

void MutatableImageDisplay::menupick_big_512x512()
{
  spawn_big(true,QSize(512,512));
}

void MutatableImageDisplay::menupick_big_1024x1024()
{
  spawn_big(true,QSize(1024,1024));
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
  
  //Used to the size explicitly here, but it seems to work better without
  //top_level_widget->resize(512,512);

  top_level_widget->show();
      
  display->image(_image->deepclone());
}


