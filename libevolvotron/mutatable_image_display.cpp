// Source file for evolvotron
// Copyright (C) 2007 Tim Day
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

#include "libevolvotron_precompiled.h"

#include "mutatable_image_display.h"

#include <qscrollview.h>
#include <qmessagebox.h>
#include <qfiledialog.h>
#include <qpngio.h>

#include "mutatable_image_display_big.h"
#include "evolvotron_main.h"
#include "mutatable_image_computer_task.h"
#include "transform_factory.h"
#include "function_pre_transform.h"
#include "function_top.h"

/*! The constructor is passed:
    - the owning widget (probably either a QGrid or null if top-level),
    - the EvolvotronMain providing spawn and farm services, 
    - a flag to specify whether this is a fully functional display or a restricted one (e.g no spawning for a single top pleve display),
    - a flag to specify whether the offscreen buffer has fixed size (in which case scrollbars are used),
    - and the size of the offscreen buffer (only used if fixed_size is true).
    Note that we use Qt's WDestructiveCode flag to ensure the destructor is called on close
 */
MutatableImageDisplay::MutatableImageDisplay(QWidget* parent,EvolvotronMain* mn,bool full,bool fixed_size,const QSize& sz,uint f,uint fr)
  :QWidget(parent,0,Qt::WDestructiveClose)
   ,_main(mn)
   ,_full_functionality(full)
   ,_fixed_size(fixed_size)
   ,_image_size(sz)
   ,_frames(f)
   ,_framerate(fr)
   ,_current_frame(0)
   ,_animate_reverse(false)
   ,_timer(0)
   ,_resize_in_progress(false)
   ,_current_display_level(0)
   ,_icon_serial(0LL)
   ,_properties(0)
   ,_menu(0)
   ,_menu_big(0)
   ,_serial(0LL)
{
  
  // We DO want background drawn for fixed size because window could be bigger than image (not entirely satisfactory however: still flickers)
  if (!fixed_size)
    {
      // This will stop the widget being cleared to grey before paintEvent is called (supposed to reduce flicker; not 100% though).
      setBackgroundMode(Qt::NoBackground);
    }

  _properties=new DialogMutatableImageDisplay(this);

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
  _menu_big->insertItem("&768x768",this,SLOT(menupick_big_768x768()));
  _menu_big->insertItem("&1024x1024",this,SLOT(menupick_big_1024x1024()));
  _menu_big->insertItem("&2048x2048",this,SLOT(menupick_big_2048x2048()));
  _menu_big->insertItem("&4096x4096",this,SLOT(menupick_big_4096x4096()));
  _menu_big->insertSeparator();
  _menu_big->insertItem("640x&480",this,SLOT(menupick_big_640x480()));
  _menu_big->insertItem("1024x&768",this,SLOT(menupick_big_1024x768()));
  _menu_big->insertItem("1280x&960",this,SLOT(menupick_big_1280x960()));
  _menu_big->insertItem("1&600x1200",this,SLOT(menupick_big_1600x1200()));

  _menu->insertItem("&Enlarge",_menu_big);

  _menu->insertSeparator();
  
  _menu->insertItem("Save &image",this,SLOT(menupick_save_image()));
  _menu->insertItem("Save &function",this,SLOT(menupick_save_function()));

  if (_full_functionality)
    {
      _menu->insertItem("L&oad function",this,SLOT(menupick_load_function()));
    }

  _menu->insertSeparator();
  _menu->insertItem("Simplif&y function",this,SLOT(menupick_simplify()));
  _menu->insertItem("&Properties...",this,SLOT(menupick_properties()));

  main()->hello(this);

  if (_fixed_size)
    {
      setGeometry(0,0,image_size().width(),image_size().height());
    }

  for (uint f=0;f<_frames;f++)
    {
      _offscreen_buffer.push_back(new QPixmap());
    }

  _timer=new QTimer(this);
  connect
    (
     _timer,SIGNAL(timeout()),
     this,SLOT(frame_advance())
     );
  if (_frames>1) _timer->start(1000/_framerate);
}

/*! Destructor signs off from EvolvotronMain to prevent further attempts at completed task delivery.
 */
MutatableImageDisplay::~MutatableImageDisplay()
{
  assert(!_image.get() || _image->ok());

  // During app shutdown EvolvotronMain might have already been destroyed so only call it if it's there.
  // Don't use main() because it asserts non-null.
  if (_main)
    {
      main()->farm().abort_for(this);
      main()->goodbye(this);
    }

  _image.reset();
  _offscreen_buffer.clear();

  _offscreen_image.clear();
}

const uint MutatableImageDisplay::simplify_constants(bool single_action)
{
  if (single_action) main()->history().begin_action("simplify");

  uint old_nodes;
  uint old_parameters;
  uint old_depth;
  uint old_width;
  real old_const;
  _image->get_stats(old_nodes,old_parameters,old_depth,old_width,old_const);

  main()->history().replacing(this);

  image(_image->simplified());

  uint new_nodes;
  uint new_parameters;
  uint new_depth;
  uint new_width;
  real new_const;
  _image->get_stats(new_nodes,new_parameters,new_depth,new_width,new_const);

  const uint nodes_eliminated=old_nodes-new_nodes;

  if (single_action) main()->history().end_action();

  if (single_action)
    {
      if (_icon.get()) _main->setIcon(*_icon);

      std::stringstream msg;
      msg << "Eliminated " << nodes_eliminated << " redundant function nodes\n";
      QMessageBox::information(this,"Evolvotron",msg.str().c_str(),QMessageBox::Ok);
    }
  return nodes_eliminated;
}

void MutatableImageDisplay::frame_advance()
{
  assert(!(_current_frame==0         && _animate_reverse==true));
  assert(!(_current_frame==_frames-1 && _animate_reverse==false));

  if (_animate_reverse)
    {
      _current_frame--;
      if (_current_frame==0)
	{
	  _animate_reverse=false;
	}
    }
  else
    {
      _current_frame++;
      if (_current_frame==_frames-1)
	{
	  _animate_reverse=true;
	}
    }
  repaint(false);
}

void MutatableImageDisplay::image(const boost::shared_ptr<const MutatableImage>& i)
{
  assert(_image.get()==0 || _image->ok());
  assert(i.get()==0 || i->ok());

  // New image, so increment serial number so any old incoming stuff which somehow avoids abort is ignored.
  _serial++;

  _properties->set_message(std::string("Not yet implemented"));

  // This might have already been done (e.g by resizeEvent), but it can't hurt to be sure.
  main()->farm().abort_for(this);

  // Careful: we could be passed our own existing (and already owned) image
  // (a trick used by resize to trigger recompute & redisplay)
  // but if the image isn't being changed we might as well leave what's already shown
  if (i.get()==0 || _image.get()==0 || i->serial()!=_image->serial())
    {
      _image=i;

      // Clear any existing image data - stops old animations continuing to play 
      for (uint f=0;f<_offscreen_buffer.size();f++)
	_offscreen_buffer[f].fill(black);

      // Queue a paint so the display will be blacked out
      repaint();
    }

  // If we start recomputing again we need to accept any delivered images.
  _current_display_level=(uint)(-1);

  // Update lock status displayed in menu
  _menu->setItemChecked(_menu_item_number_lock,(_image.get() ? _image->locked() : false));
  
  if (_image.get())
    {
      // Allow for displays up to 4096 pixels high or wide
      for (int level=12;level>=0;level--)
	{
	  const int s=(1<<level);
	  
	  if (image_size().width()>=s && image_size().height()>=s)
	    {
	      /*! \todo Should computed frames be constant or reduced c.f spatial resolution ?  (Do full z resolution for now)
	       */
	      const boost::shared_ptr<const MutatableImage> task_image(_image);
	      assert(task_image->ok());

	      const boost::shared_ptr<MutatableImageComputerTask> task(new MutatableImageComputerTask(this,task_image,image_size()/s,_frames,level,_serial));
	      main()->farm().push_todo(task);
	    }
	}
    }
}

void MutatableImageDisplay::deliver(const boost::shared_ptr<const MutatableImageComputerTask>& task)
{
  // Ignore tasks which were aborted or which have somehow got out of order 
  // (not impossible with multiple compute threads).
  /*! \todo Not entirely sure the level check is sufficient in all possible situations,
      would be best to have a serial number incremented for each recompute.
   */
  if (!task->aborted() && task->level()<_current_display_level && task->serial()==_serial)
    {
      _offscreen_image.clear();

      _offscreen_image_data=task->image_data();
      
      for (uint f=0;f<task->frames();f++)
	{
	  _offscreen_image.push_back
	    (
	     new QImage
	     (
	      (uchar*)&(_offscreen_image_data[f*task->size().width()*task->size().height()]),
	      task->size().width(),
	      task->size().height(),
	      32,
	      0,
	      0,
	      QImage::LittleEndian  // Seems to be right for hi[-/R/G/B]lo packing
	      )
	     );
	  
	  //! \todo Pick a scaling mode: smooth or not (or put it under GUI control). 
	  // Curiously, although smoothscale seems to be noticeably slower, it doesn't look any better.
	  _offscreen_buffer[f].convertFromImage(_offscreen_image.back().scale(image_size()));
	}

      // For an icon, take the first image big enough to (hopefully) be filtered down nicely.
      // The converter seems to auto-create an alpha mask sometimes (images with const-color areas), which is quite cool.
      const QSize icon_size(32,32);
      if (task->serial()!=_icon_serial && (task->level()==0 || (task->size().width()>=2*icon_size.width() && task->size().height()>=2*icon_size.height())))
	{
	  const QImage icon_image(_offscreen_image[_offscreen_image.size()/2].smoothScale(icon_size));
	  
	  _icon=std::auto_ptr<QPixmap>(new QPixmap(icon_size));
	  _icon->convertFromImage(icon_image,QPixmap::Color);

	  _icon_serial=task->serial();
	}
	  
      //! Note the resolution we've displayed so out-of-order low resolution images are dropped
      _current_display_level=task->level();

      // Update what's on the screen.
      //! \todo Any case for calling update() instead of repaint() ?  Repaint maybe feels smoother.
      repaint();
    }
  }

void MutatableImageDisplay::lock(bool l,bool record_in_history)
{
  // This might be called (with l=false) with null _image during start-up reset.
  if (_image && _image->locked()!=l)
    {
      if (record_in_history)
	{
	  main()->history().begin_action(l ? "lock" : "unlock");
	  main()->history().replacing(this);
	}
      const boost::shared_ptr<const MutatableImage> new_image(_image->deepclone(l));
      image(new_image);
      if (record_in_history)
	{
	  main()->history().end_action();
	}
    }

  _menu->setItemChecked(_menu_item_number_lock,l);
}

void MutatableImageDisplay::paintEvent(QPaintEvent*)
{
  // Repaint the screen from the offscreen buffer
  // (If there have been resizes it will be black)
  bitBlt(this,0,0,&_offscreen_buffer[_current_frame]);

  // If this is the first paint event after a resize we can start computing images for the new size.
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
      
      // Abort all current tasks because they'll be the wrong size.
      main()->farm().abort_for(this);
      
      // Resize and reset our offscreen buffer (something to do while we wait)
      for (uint f=0;f<_offscreen_buffer.size();f++)
	{
	  _offscreen_buffer[f].resize(image_size());
	  _offscreen_buffer[f].fill(black);
	}
      
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
      if (_icon.get()) _main->setIcon(*_icon);

      menupick_spawn();
    }
}

void MutatableImageDisplay::mouseMoveEvent(QMouseEvent* event)
{
  if (event->state()&MidButton)
    {
      if (locked())
	{
	  QMessageBox::warning(this,"Evolvotron","Cannot middle-mouse adjust a locked image.\nUnlock and try again.");
	}
      else
	{
	  const QPoint pixel_delta(event->pos()-_mid_button_adjust_last_pos);
	  
	  // Build the transform caused by the adjustment
	  Transform transform=TransformIdentity();
	  
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
				(event->pos().x()-image_size().width() /2) / static_cast<real>(_mid_button_adjust_last_pos.x()-image_size().width() /2),
				(event->pos().y()-image_size().height()/2) / static_cast<real>(_mid_button_adjust_last_pos.y()-image_size().height()/2),
				0.0
				);
		      
		      transform.basis_x(XYZ(1.0/scale.x(),          0.0,0.0));
		      transform.basis_y(XYZ(          0.0,1.0/scale.y(),0.0));
		      
		      std::clog << "[Anisotropic scale]";
		    }
		}
	      // Shift button alone is isotropic zoom
	      else 
		{
		  const real cx=image_size().width()/2.0;
		  const real cy=image_size().width()/2.0;
		  
		  const real dx=event->pos().x()-cx;
		  const real dy=event->pos().y()-cy;
		  
		  const real last_dx=_mid_button_adjust_last_pos.x()-cx;
		  const real last_dy=_mid_button_adjust_last_pos.y()-cy;
		  
		  const real radius=sqrt(dx*dx+dy*dy);
		  const real last_radius=sqrt(last_dx*last_dx+last_dy*last_dy);
		  
		  // Only scale in non-degenerate cases
		  if (radius!=0.0 && last_radius!=0.0)
		    {
		      const real scale=radius/last_radius;
		      transform.basis_x(XYZ(1.0/scale,          0.0,0.0));
		      transform.basis_y(XYZ(      0.0,1.0/scale,0.0));
		      
		      std::clog << "[Isotropic scale]";
		    }
		}
	    }
	  else if (event->state()&ControlButton)
	    {
	      // Control-alt is shear
	      if (event->state()&AltButton)
		{
		  const real cx=image_size().width()/2.0;
		  const real cy=image_size().width()/2.0;
		  
		  const real dx=(event->pos().x()-_mid_button_adjust_last_pos.x())/cx;
		  const real dy=(event->pos().y()-_mid_button_adjust_last_pos.y())/cy;
		  
		  transform.basis_x(XYZ(1.0, -dy,0.0));
		  transform.basis_y(XYZ(  dx,1.0,0.0));
		  
		  std::clog << "[Shear]";
		}
	      // Control button only is rotate
	      else
		{
		  const real cx=image_size().width()/2.0;
		  const real cy=image_size().width()/2.0;
		  
		  const real dx=event->pos().x()-cx;
		  const real dy=event->pos().y()-cy;
		  
		  const real last_dx=_mid_button_adjust_last_pos.x()-cx;
		  const real last_dy=_mid_button_adjust_last_pos.y()-cy;
		  
		  const real a=atan2(dy,dx);
		  const real last_a=atan2(last_dy,last_dx);
		  
		  const real rot=a-last_a;
		  
		  const real sr=sin(rot);
		  const real cr=cos(rot);
		  
		  transform.basis_x(XYZ( cr,sr,0.0));
		  transform.basis_y(XYZ(-sr,cr,0.0));
	      
		  std::clog << "[Rotate]";
		}
	    }
	  // Default (no interesting modifier keys detected) is panning around
	  else
	    {
	      XYZ translate(
			    (-2.0*pixel_delta.x())/image_size().width(),
			    ( 2.0*pixel_delta.y())/image_size().height(),
			    0.0
			    );
	      transform.translate(translate);
	      
	      std::clog << "[Pan]";
	    }
	  
	  std::auto_ptr<FunctionTop> new_root(image()->top().typed_deepclone());
	  new_root->concatenate_pretransform_on_right(transform);

	  // Install new image (triggers recompute).
	  const boost::shared_ptr<const MutatableImage> new_image(new MutatableImage(new_root,image()->sinusoidal_z(),image()->spheremap(),false));
	  image(new_image);

	  // Finally, record position of this event as last event
	  _mid_button_adjust_last_pos=event->pos();
	}
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
  TransformFactoryRandomScaleXY transform_factory(-2.0,0.0);

  main()->spawn_warped(this,transform_factory);
}

void MutatableImageDisplay::menupick_spawn_warped_zoom_out()
{
  TransformFactoryRandomScaleXY transform_factory(0.0,2.0);

  main()->spawn_warped(this,transform_factory);
}

void MutatableImageDisplay::menupick_spawn_warped_rotate()
{
  TransformFactoryRandomRotateZ transform_factory;

  main()->spawn_warped(this,transform_factory);
}

void MutatableImageDisplay::menupick_spawn_warped_pan_xy()
{
  TransformFactoryRandomTranslateXYZ transform_factory(XYZ(0.0,0.0,0.0),XYZ(1.0,1.0,0.0));

  main()->spawn_warped(this,transform_factory);
}

void MutatableImageDisplay::menupick_spawn_warped_pan_x()
{
  TransformFactoryRandomTranslateXYZ transform_factory(XYZ(0.0,0.0,0.0),XYZ(1.0,0.0,0.0));

  main()->spawn_warped(this,transform_factory);
}

void MutatableImageDisplay::menupick_spawn_warped_pan_y()
{
  TransformFactoryRandomTranslateXYZ transform_factory(XYZ(0.0,0.0,0.0),XYZ(0.0,1.0,0.0));

  main()->spawn_warped(this,transform_factory);
}

void MutatableImageDisplay::menupick_spawn_warped_pan_z()
{
  TransformFactoryRandomTranslateXYZ transform_factory(XYZ(0.0,0.0,0.0),XYZ(0.0,0.0,1.0));

  main()->spawn_warped(this,transform_factory);
}

/*! This slot is called by selecting the "Lock" context menu item.
  It stops the image from being overwritten by a new image.
 */
void MutatableImageDisplay::menupick_lock()
{
  lock(!_image->locked(),true);
}

void MutatableImageDisplay::menupick_simplify()
{
  simplify_constants(true);
}
 

/*! Saves image (unless the image is not full resolution yet, in which case an informative dialog is generated.
 */
void MutatableImageDisplay::menupick_save_image()
{
  if (_icon.get()) _main->setIcon(*_icon);

  std::clog << "Save requested...\n";

  if (_current_display_level!=0)
    {
      QMessageBox::information(this,"Evolvotron","The selected image has not yet been generated at maximum resolution.\nPlease try again later.");
    }
  else
    {
      QString save_filename=QFileDialog::getSaveFileName(".","Images (*.ppm *.png *.qt-mng)",this,"Save image","Save image to a PPM, PNG or QT-MNG file");
      if (!save_filename.isEmpty())
	{
	  const char* save_format="PPM";
	  if (save_filename.upper().endsWith(".PPM"))
	    {
	      save_format="PPM";
	    }
	  else if (save_filename.upper().endsWith(".PNG"))
	    {
	      save_format="PNG";
	    }
	  else if (save_filename.upper().endsWith(".QT-MNG"))
	    {
	      save_format="QT-MNG";
	    }
	  else
	    {
	      QMessageBox::warning
		(
		 this,
		 "Evolvotron",
		 QString("Unrecognised file suffix.\nFile will be written in ")+QString(save_format)+QString(" format.")
		 );
	    }
	  
	  if (!save_filename.isEmpty())
	    {
	      std::auto_ptr<QFile> mng_file;
	      if (save_format=="QT-MNG")
		{
		  mng_file=std::auto_ptr<QFile>(new QFile(save_filename));
		  if (!mng_file->open(IO_WriteOnly|IO_Truncate))
		    {
		      QMessageBox::critical(this,"Evolvotron","Failed to open file "+save_filename);
		    }
		  else 
		    {
		      QPNGImagePacker packer(mng_file.get(),32,0);
		      
		      for (uint f=0;f<_offscreen_image.size();f++)
			{
			  if (!packer.packImage(_offscreen_image[f]))
			    {
			      QMessageBox::critical(this,"Evolvotron","Failed while writing file "+save_filename+"\nFile will be removed");
			      mng_file->close();
			      if (!mng_file->remove())
				{
				  QMessageBox::critical(this,"Evolvotron","Failed to remove file "+save_filename);
				}
			      break;
			    }
			  std::clog << "Appended frame " << f << " to " << save_filename.local8Bit() << "\n";
			}

		      mng_file->close();
		      if (mng_file->status()!=IO_Ok)
			{
			  QMessageBox::critical(this,"Evolvotron","Failed while closing file "+save_filename+"\nFile will be removed");
			  if (!mng_file->remove())
			    {
			      QMessageBox::critical(this,"Evolvotron","Failed to remove file "+save_filename);
			    }
			}
		    }
		}
	      else
		{
		  for (uint f=0;f<_offscreen_image.size();f++)
		    {
		      QString actual_save_filename(save_filename);
		      
		      if (_offscreen_image.size()>1)
			{
			  QString frame_component;
			  frame_component.sprintf(".f%06d",f);
			  int insert_point=save_filename.findRev(QString("."));
			  if (insert_point==-1)
			    {
			      actual_save_filename.append(frame_component);
			    }
			  else
			    {
			      actual_save_filename.insert(insert_point,frame_component);
			    }
			}
		      
		      if (!_offscreen_image[f].save(actual_save_filename.local8Bit(),save_format))
			{
			  QMessageBox::critical(this,"Evolvotron","Failed to write file "+actual_save_filename);
			  if (f<_offscreen_image.size()-1)
			    {
			      QMessageBox::critical(this,"Evolvotron","Not attempting to save remaining images in animation");
			    }
			  break;
			}
		    }
		}
	    }
	}
    }
  std::clog << "...save done\n";
}

void MutatableImageDisplay::menupick_save_function()
{
  if (_icon.get()) _main->setIcon(*_icon);

  QString save_filename=QFileDialog::getSaveFileName(".","Functions (*.xml)",this,"Save function","Save image function to an XML file");
  if (!save_filename.isEmpty())
    {
      std::ofstream file(save_filename.local8Bit());
      _image->save_function(file);
      file.flush();
      if (!file)
	{
	  QMessageBox::critical(this,"Evolvotron","File write failed");
	}
    }
}

void MutatableImageDisplay::menupick_load_function()
{
  QString load_filename=QFileDialog::getOpenFileName(".","Functions (*.xml)",this,"Load function","Load image function from an XML file");
  if (!load_filename.isEmpty())
    {
      std::ifstream file(load_filename.local8Bit());
      std::string report;
      boost::shared_ptr<const MutatableImage> new_image(MutatableImage::load_function(_main->mutation_parameters().function_registry(),file,report));
      if (new_image.get()==0)
	{
	  QMessageBox::critical(this,"Evolvotron",("Function not loaded due to errors:\n"+report).c_str());
	}
      else
	{
	  if (!report.empty())
	    {
	      QMessageBox::warning(this,"Evolvotron",("Function loaded with warnings:\n"+report).c_str(),QMessageBox::Ok,QMessageBox::NoButton);
	    }
	  
	  main()->history().begin_action("load");
	  main()->history().replacing(this);
	  main()->history().end_action();
	  image(new_image);
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

void MutatableImageDisplay::menupick_big_768x768()
{
  spawn_big(true,QSize(768,768));
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

void MutatableImageDisplay::menupick_properties()
{
  uint total_nodes;
  uint total_parameters;
  uint depth;
  uint width;
  real proportion_constant;

  image()->get_stats(total_nodes,total_parameters,depth,width,proportion_constant);

  std::stringstream msg;
  msg << total_nodes      << "\t function nodes\n";
  msg << total_parameters << "\t parameters\n";
  msg << depth            << "\t maximum depth\n";
  msg << width            << "\t width\n";
  msg << std::setprecision(3) << 100.0*proportion_constant << "%\t constant\n";

  if (_icon.get()) _properties->setIcon(*_icon);

  _properties->set_message(msg.str());
  _properties->exec();
}

/*! Create an image display with no parent: becomes a top level window 
  Disable full menu functionality because there's less we can do with a single image (e.g no spawn_target)
*/
void MutatableImageDisplay::spawn_big(bool scrollable,const QSize& sz)
{
  MutatableImageDisplayBig*const top_level_widget=new MutatableImageDisplayBig(0,main());
  if (_icon.get()) top_level_widget->setIcon(*_icon);

  MutatableImageDisplay* display=0;
  
  if (scrollable)
    {
      QScrollView*const scrollview=new QScrollView(top_level_widget,0,Qt::WDestructiveClose);
      display=new MutatableImageDisplay(scrollview->viewport(),main(),false,true,sz,_frames,_framerate);
      scrollview->addChild(display);
      top_level_widget->hold(scrollview);
    }
  else
    {
      display=new MutatableImageDisplay(top_level_widget,main(),false,false,QSize(0,0),_frames,_framerate);
      top_level_widget->hold(display);
    }

  top_level_widget->show();

  //Propagate full screen mode 
  if (main()->isFullScreen()) top_level_widget->showFullScreen();

  // Fire up image calculation
  display->image(_image);
}
