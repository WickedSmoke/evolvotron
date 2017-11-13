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
  \brief Implementation of class MutatableImageDisplay.
*/

#include "mutatable_image_display.h"

#include "mutatable_image_display_big.h"
#include "evolvotron_main.h"
#include "mutatable_image_computer_task.h"
#include "transform_factory.h"
#include "function_pre_transform.h"
#include "function_top.h"

/*! The constructor is passed:
    - the owning widget (probably either a QGrid or null if top-level),
    - the EvolvotronMain providing spawn and farm services, 
    - a flag to specify whether this is a fully functional display or a restricted one (e.g no spawning for a single top level display),
    - a flag to specify whether the offscreen buffer has fixed size (in which case scrollbars may be used),
    - and the size of the offscreen buffer (only used if fixed_size is true).
    Note that we use Qt's WDestructiveCode flag to ensure the destructor is called on close
 */
MutatableImageDisplay::MutatableImageDisplay(EvolvotronMain* mn,bool full_functionality,bool fixed_size,const QSize& sz,uint f,uint fr)
  :_main(mn)
  ,_full_functionality(full_functionality)
  ,_fixed_size(fixed_size)
  ,_image_size(sz)
  ,_frames(f)
  ,_framerate(fr)
  ,_current_frame(0)
  ,_animate_reverse(false)
  ,_timer(0)
  ,_resize_in_progress(false)
  ,_current_display_level(0)
  ,_current_display_multisample_grid(0)
  ,_icon_serial(0LL)
  ,_properties(0)
  ,_menu(0)
  ,_menu_big(0)
  ,_menu_item_action_lock(0)
  ,_serial(0LL)
{
  setAttribute(Qt::WA_DeleteOnClose,true);

  setFocusPolicy(Qt::StrongFocus);

  _properties=new DialogMutatableImageDisplay(this);

  _menu=new QMenu(this);  

  // Most items on the context menu aren't appropriate for a window displaying a single big image
  if (_full_functionality)
    {
      _menu->addAction("Respawn",this,SLOT(menupick_respawn()));

      _menu->addSeparator();

      _menu->addAction("Spawn",this,SLOT(menupick_spawn()));
      _menu->addAction("Spawn recoloured",this,SLOT(menupick_spawn_recoloured()));

      _menu_warped=_menu->addMenu("Spawn warped");
      _menu_warped->addAction("Random Mix",this,SLOT(menupick_spawn_warped_random())  );
      _menu_warped->addAction("Zoom In"   ,this,SLOT(menupick_spawn_warped_zoom_in()) );
      _menu_warped->addAction("Zoom Out"  ,this,SLOT(menupick_spawn_warped_zoom_out()));
      _menu_warped->addAction("Rotate"    ,this,SLOT(menupick_spawn_warped_rotate())  );
      _menu_warped->addAction("Pan XY"    ,this,SLOT(menupick_spawn_warped_pan_xy())  );
      _menu_warped->addAction("Pan X"     ,this,SLOT(menupick_spawn_warped_pan_x())   );
      _menu_warped->addAction("Pan Y"     ,this,SLOT(menupick_spawn_warped_pan_y())   );
      _menu_warped->addAction("Pan Z"     ,this,SLOT(menupick_spawn_warped_pan_z())   );

      _menu->addSeparator();

      _menu_item_action_lock=_menu->addAction("Lock",this,SLOT(menupick_lock()));
      _menu_item_action_lock->setCheckable(true);

      _menu->addSeparator();
    }
  
  _menu_big=_menu->addMenu("Enlarge");
  _menu_big->addAction("Resizable",this,SLOT(menupick_big_resizable()));
  _menu_big->addSeparator();		
  _menu_big->addAction("256x256"  ,this,SLOT(menupick_big_256x256()));
  _menu_big->addAction("512x512"  ,this,SLOT(menupick_big_512x512())  );
  _menu_big->addAction("768x768"  ,this,SLOT(menupick_big_768x768())  );
  _menu_big->addAction("1024x1024",this,SLOT(menupick_big_1024x1024()));
  _menu_big->addAction("2048x2048",this,SLOT(menupick_big_2048x2048()));
  _menu_big->addAction("4096x4096",this,SLOT(menupick_big_4096x4096()));
  _menu_big->addSeparator();
  _menu_big->addAction("640x480"  ,this,SLOT(menupick_big_640x480())  );
  _menu_big->addAction("1024x768" ,this,SLOT(menupick_big_1024x768()) );
  _menu_big->addAction("1280x960" ,this,SLOT(menupick_big_1280x960()) );
  _menu_big->addAction("1600x1200",this,SLOT(menupick_big_1600x1200()));

  _menu->addSeparator();
  
  _menu->addAction("Save image",this,SLOT(menupick_save_image()));
  _menu->addAction("Save function",this,SLOT(menupick_save_function()));

  if (_full_functionality)
    {
      _menu->addAction("Load function",this,SLOT(menupick_load_function()));
    }

  _menu->addSeparator();
  _menu->addAction("Simplify function",this,SLOT(menupick_simplify()));
  _menu->addAction("Properties...",this,SLOT(menupick_properties()));

  main().hello(this);

  if (_fixed_size)
    {
      setGeometry(0,0,image_size().width(),image_size().height());
    }

  for (uint f=0;f<_frames;f++)
    {
      _offscreen_pixmaps.push_back(QPixmap());
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
  assert(!_image_function.get() || _image_function->ok());

  // During app shutdown EvolvotronMain might have already been destroyed so only call it if it's there.
  // Don't use main() because it asserts non-null.
  if (_main)
    {
      farm().abort_for(this);
      main().goodbye(this);
    }

  _image_function.reset();
  _offscreen_pixmaps.clear();

  _offscreen_images.clear();
}

uint MutatableImageDisplay::simplify_constants(bool single_action)
{
  if (single_action) main().history().begin_action("simplify");

  uint old_nodes;
  uint old_parameters;
  uint old_depth;
  uint old_width;
  real old_const;
  _image_function->get_stats(old_nodes,old_parameters,old_depth,old_width,old_const);

  main().history().replacing(this);

  image_function(_image_function->simplified(),!single_action);

  uint new_nodes;
  uint new_parameters;
  uint new_depth;
  uint new_width;
  real new_const;
  _image_function->get_stats(new_nodes,new_parameters,new_depth,new_width,new_const);

  const uint nodes_eliminated=old_nodes-new_nodes;

  if (single_action) main().history().end_action();

  if (single_action)
    {
      if (_icon.get()) _main->setWindowIcon(*_icon);

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
  repaint();  // Use repaint rather than update because we really do want this to happen immediately.
}

void MutatableImageDisplay::image_function(const boost::shared_ptr<const MutatableImage>& i,bool one_of_many)
{
  assert(_image_function.get()==0 || _image_function->ok());
  assert(i.get()==0 || i->ok());

  // New image, so increment serial number so any old incoming stuff which somehow avoids abort is ignored.
  _serial++;

  // This might have already been done (e.g by resizeEvent), but it can't hurt to be sure.
  farm().abort_for(this);

  // Careful: we could be passed our own existing (and already owned) image
  // (a trick used by resize to trigger recompute & redisplay)
  if (i.get()==0 || _image_function.get()==0 || i->serial()!=_image_function->serial())
    {
      _image_function=i;

      // If we're part of a fullscale change then better to display back rather than something misleading
      // but for one-offs (e.g middle mouse drag) is better not to clear.
      if (one_of_many)
	{
	  // Clear any existing image data - stops old animations continuing to play 
	  for (uint f=0;f<_offscreen_pixmaps.size();f++)
	    _offscreen_pixmaps[f].fill(QColor(0,0,0));
	  
	  // Queue a redraw
	  update();
	}
    }

  // If we start recomputing again we need to accept any delivered images.
  _current_display_level=static_cast<uint>(-1);
  _current_display_multisample_grid=static_cast<uint>(-1);

  // Clear up staging area... its contents are now useless
  _offscreen_images_inbox.clear();

  // Update lock status displayed in menu
  if (_menu_item_action_lock)
    _menu_item_action_lock->setChecked(_image_function.get() ? _image_function->locked() : false);
  
  if (_image_function.get())
    {
      // Allow for displays up to 4096 pixels high or wide
      for (int level=12;level>=0;level--)
	{
	  const int s=(1<<level);
	  const QSize render_size(image_size()/s);
	  
	  // Don't bother rendering anything less than 4x4 unless that's all there is
	  if ((render_size.width()>=4 && render_size.height()>=4) || level==0)
	    {
	      const int fragments
		=(
		  one_of_many
		  ?
		  1
		  :
		  std::min
		  (
		   2*farm().num_threads(),
		   static_cast<uint>(render_size.height())
		   )
		  );
	      
	      std::vector<uint> multisample_grid;
	      multisample_grid.push_back(1);
	      if (level==0)
		{
		  // Only the final full resolution level gets an additional multisampling task.
		  // For 4x4 sampling, do an initial 2x2 too.
		  if (main().render_parameters().multisample_grid()==4) multisample_grid.push_back(2);
		  if (main().render_parameters().multisample_grid()>1) multisample_grid.push_back(main().render_parameters().multisample_grid());
		}

	      for (std::vector<uint>::const_iterator multisample_it=multisample_grid.begin();multisample_it!=multisample_grid.end();multisample_it++)
		{
		  //! \todo Should computed animation frames be constant or reduced c.f spatial resolution ?  (Do full z resolution for now)
		  const boost::shared_ptr<const MutatableImage> task_image(_image_function);
		  assert(task_image->ok());
		  
		  // Use number of samples in unfragmented image as priority
		  const uint task_priority=render_size.width()*render_size.height()*(*multisample_it)*(*multisample_it);
		  
		  int fragment_start_row=0;
		  for (int f=0;f<fragments;f++)
		    {
		      const int fragment_end_row=(render_size.height()*(f+1))/fragments;
		      const boost::shared_ptr<MutatableImageComputerTask> task
			(
			 new MutatableImageComputerTask
			 (
			  this,
			  task_image,
			  task_priority,
			  QSize(0,fragment_start_row),
			  QSize(render_size.width(),fragment_end_row-fragment_start_row),
			  render_size,
			  _frames,
			  level,
			  f,
			  fragments,
			  main().render_parameters().jittered_samples(),
			  (*multisample_it),
			  _serial
			  )
			 );
		      farm().push_todo(task);
		      fragment_start_row=fragment_end_row;
		    }
		}
	    }
	}
    }
}

void MutatableImageDisplay::deliver(const boost::shared_ptr<const MutatableImageComputerTask>& task)
{
  // Ignore tasks which were aborted or which have somehow got out of order 
  // (entirely possible with multiple compute threads).
  if (
      task->aborted()
      || task->serial()!=_serial
      || task->level()>_current_display_level
      || (task->level()==_current_display_level && task->multisample_grid()<=_current_display_multisample_grid)
      )
    return;

  // Record the fragment in the inbox
  const OffscreenImageInbox::key_type inbox_key(task->level(),task->multisample_grid());  
  OffscreenImageInbox::mapped_type& inbox_level=_offscreen_images_inbox[inbox_key];
  assert(inbox_level.find(task->fragment())==inbox_level.end());
  inbox_level[task->fragment()]=task;
  
  if (inbox_level.size()!=task->number_of_fragments())
    return;

  // If the level is complete, we can proceed to displaying it

  // Note that obsolete levels will never complete once a better one is displayed.
  // So clear up previous levels; they're now irrelevant
  _offscreen_images_inbox.erase
    (
     _offscreen_images_inbox.upper_bound(inbox_key),  // upper_bound is the NEXT key from the one we're about to display
     _offscreen_images_inbox.end()
     );
  
  const QSize render_size(task->whole_image_size());
  
  if (task->number_of_fragments()==1)
    {
      // If there's only one fragment in the task, just use it
      _offscreen_images=task->images();
    }
  else
    {
      // Otherwise we need to assemble the fragments together
      _offscreen_images.resize(0);
      for (uint f=0;f<_frames;f++)
	{
	  _offscreen_images.push_back(QImage(render_size,QImage::Format_RGB32));
	  
	  for (OffscreenImageInbox::mapped_type::const_iterator it=inbox_level.begin();it!=inbox_level.end();++it)
	    {
	      QPainter painter(&_offscreen_images.back());
	      painter.drawImage
		(
		 QPoint((*it).second->fragment_origin().width(),(*it).second->fragment_origin().height()),
		 (*it).second->images()[f]
		 );
	    }
	}
    }
  
  for (uint f=0;f<_frames;f++)
    {
      //! \todo Pick a scaling mode: Qt::SmoothTransformation vs Qt::FastTransformation (default) (and put it under GUI control). 
      //! \todo Expose dither mode control: Qt::DiffuseDither vs Qt::ThresholdDither
      _offscreen_pixmaps[f]=QPixmap::fromImage(_offscreen_images[f].scaled(image_size()),(Qt::ColorOnly|Qt::ThresholdDither));
    }
  
  //! Note the resolution we've displayed so out-of-order low resolution images are dropped
  _current_display_level=task->level();
  _current_display_multisample_grid=task->multisample_grid();
  
  // For an icon, take the first image big enough to (hopefully) be filtered down nicely.
  // The (Qt3) converter seems to auto-create an alpha mask sometimes (images with const-color areas), which is quite cool.
  const QSize icon_size(32,32);
  if (task->serial()!=_icon_serial && (task->level()==0 || (render_size.width()>=2*icon_size.width() && render_size.height()>=2*icon_size.height())))
    {
      const QImage icon_image(_offscreen_images[_offscreen_images.size()/2].scaled(icon_size));
      
      if (!_icon.get()) _icon=std::unique_ptr<QPixmap>(new QPixmap(icon_size));
      (*_icon)=QPixmap::fromImage(icon_image,Qt::ColorOnly);
      
      _icon_serial=task->serial();
    }

  // Update what's on the screen.
  update();
}

void MutatableImageDisplay::lock(bool l,bool record_in_history)
{
  // This might be called (with l=false) with null _image during start-up reset.
  if (_image_function && _image_function->locked()!=l)
    {
      if (record_in_history)
	{
	  main().history().begin_action(l ? "lock" : "unlock");
	  main().history().replacing(this);
	}
      const boost::shared_ptr<const MutatableImage> new_image_function(_image_function->deepclone(l));
      image_function(new_image_function,false);
      if (record_in_history)
	{
	  main().history().end_action();
	}
    }

  _menu_item_action_lock->setChecked(l);
}

/*! Enlargements are implied by a non-full-functionality displays.
 */
MutatableImageComputerFarm& MutatableImageDisplay::farm() const
{
  return main().farm(!_full_functionality);
}

void MutatableImageDisplay::paintEvent(QPaintEvent*)
{
  // Repaint the screen from the offscreen pixmaps
  QPainter painter(this);
  painter.drawPixmap(0,0,_offscreen_pixmaps[_current_frame]);

  // If this is the first paint event after a resize we can start computing images for the new size.
  if (_resize_in_progress)
    {
      image_function(_image_function,false);  // A resize should really be considered one-of-many, but because the image doesn't change we seem to be able to get away with it
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
      farm().abort_for(this);
      
      // Resize and reset our offscreen pixmap (something to do while we wait)
      for (uint f=0;f<_offscreen_pixmaps.size();f++)
	{
	  _offscreen_pixmaps[f]=QPixmap(image_size()); 
	  _offscreen_pixmaps[f].fill(QColor(0,0,0));           
	}
      
      // Flag for the next paintEvent to tell it a recompute can be started now.
      _resize_in_progress=true;
    }
}

void MutatableImageDisplay::mousePressEvent(QMouseEvent* event)
{
  if (event->button()==Qt::RightButton)
    {
      _menu->exec(QCursor::pos());
    }
  else if (event->button()==Qt::MidButton)
    {
      // Take a snapshot to undo back to.
      main().history().begin_action("middle-button drag");
      main().history().replacing(this);
      main().history().end_action();

      _mid_button_adjust_start_pos=event->pos();
      _mid_button_adjust_last_pos=event->pos();
    }
  else if (_full_functionality && event->button()==Qt::LeftButton)
    {
      if (_icon.get()) _main->setWindowIcon(*_icon);

      menupick_spawn();
    }
}

void MutatableImageDisplay::mouseMoveEvent(QMouseEvent* event)
{
  if (event->buttons()&Qt::MidButton)
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
	  if (event->modifiers()&Qt::ShiftModifier && !(event->modifiers()&Qt::ControlModifier))
	    {
	      // Alt-Shift is anisotropic
	      if (event->modifiers()&Qt::AltModifier)
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
	  else if (event->modifiers()&Qt::ControlModifier)
	    {
	      // Control-alt is shear
	      if (event->modifiers()&Qt::AltModifier)
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
	  
	  std::unique_ptr<FunctionTop> new_root(image_function()->top().typed_deepclone());
	  new_root->concatenate_pretransform_on_right(transform);

	  // Install new image (triggers recompute).
	  const boost::shared_ptr<const MutatableImage> new_image_function(new MutatableImage(new_root,image_function()->sinusoidal_z(),image_function()->spheremap(),false));
	  image_function(new_image_function,false);

	  // Finally, record position of this event as last event
	  _mid_button_adjust_last_pos=event->pos();
	}
    }
}

/*! This slot is called by selecting the "Respawn" context menu item
 */
void MutatableImageDisplay::menupick_respawn()
{
  main().respawn(this);
}

/*! This slot is called by selecting the "Spawn" context menu item, or by clicking the image
 */
void MutatableImageDisplay::menupick_spawn()
{
  main().spawn_normal(this);
}

/*! This slot is called by selecting the "Spawn Recoloured" context menu item
 */
void MutatableImageDisplay::menupick_spawn_recoloured()
{
  main().spawn_recoloured(this);
}

/*! This slot is called by selecting the "Spawn Warped/Random" context menu item
 */
void MutatableImageDisplay::menupick_spawn_warped_random()
{
  TransformFactoryRandomWarpXY transform_factory;
  
  main().spawn_warped(this,transform_factory);
}

void MutatableImageDisplay::menupick_spawn_warped_zoom_in()
{
  TransformFactoryRandomScaleXY transform_factory(-2.0,0.0);

  main().spawn_warped(this,transform_factory);
}

void MutatableImageDisplay::menupick_spawn_warped_zoom_out()
{
  TransformFactoryRandomScaleXY transform_factory(0.0,2.0);

  main().spawn_warped(this,transform_factory);
}

void MutatableImageDisplay::menupick_spawn_warped_rotate()
{
  TransformFactoryRandomRotateZ transform_factory;

  main().spawn_warped(this,transform_factory);
}

void MutatableImageDisplay::menupick_spawn_warped_pan_xy()
{
  TransformFactoryRandomTranslateXYZ transform_factory(XYZ(0.0,0.0,0.0),XYZ(1.0,1.0,0.0));

  main().spawn_warped(this,transform_factory);
}

void MutatableImageDisplay::menupick_spawn_warped_pan_x()
{
  TransformFactoryRandomTranslateXYZ transform_factory(XYZ(0.0,0.0,0.0),XYZ(1.0,0.0,0.0));

  main().spawn_warped(this,transform_factory);
}

void MutatableImageDisplay::menupick_spawn_warped_pan_y()
{
  TransformFactoryRandomTranslateXYZ transform_factory(XYZ(0.0,0.0,0.0),XYZ(0.0,1.0,0.0));

  main().spawn_warped(this,transform_factory);
}

void MutatableImageDisplay::menupick_spawn_warped_pan_z()
{
  TransformFactoryRandomTranslateXYZ transform_factory(XYZ(0.0,0.0,0.0),XYZ(0.0,0.0,1.0));

  main().spawn_warped(this,transform_factory);
}

/*! This slot is called by selecting the "Lock" context menu item.
  It stops the image from being overwritten by a new image.
 */
void MutatableImageDisplay::menupick_lock()
{
  lock(!_image_function->locked(),true);
}

void MutatableImageDisplay::menupick_simplify()
{
  simplify_constants(true);
}
 

/*! Saves image (unless the image is not full resolution yet, in which case an informative dialog is generated.
 */
void MutatableImageDisplay::menupick_save_image()
{
  if (_icon.get()) _main->setWindowIcon(*_icon);

  std::clog << "Save requested...\n";

  if (_current_display_level!=0 || _current_display_multisample_grid!=main().render_parameters().multisample_grid())
    {
      QMessageBox::information(this,"Evolvotron","The selected image has not yet been generated at maximum resolution.\nPlease try again later.");
    }
  else
    {
      const QString save_filename=QFileDialog::getSaveFileName
	(
	 this,
	 "Save image to a PNG or PPM file",
	 ".",
	 "Images (*.png *.ppm)"
	 );

      if (!save_filename.isEmpty())
	{
	  QString save_format="PNG";
	  if (save_filename.toUpper().endsWith(".PPM"))
	    {
	      save_format="PPM";
	    }
	  else if (save_filename.toUpper().endsWith(".PNG"))
	    {
	      save_format="PNG";
	    }
	  else
	    {
	      QMessageBox::warning
		(
		 this,
		 "Evolvotron",
		 QString("Unrecognised file suffix.\nFile will be written in ")+save_format+QString(" format.")
		 );
	    }
	  
	  for (uint f=0;f<_offscreen_images.size();f++)
	    {
	      QString actual_save_filename(save_filename);
	      
	      if (_offscreen_images.size()>1)
		{
		  QString frame_component;
		  frame_component.sprintf(".f%06d",f);
		  int insert_point=save_filename.lastIndexOf(QString("."));
		  if (insert_point==-1)
		    {
		      actual_save_filename.append(frame_component);
		    }
		  else
		    {
		      actual_save_filename.insert(insert_point,frame_component);
		    }
		}
	      
	      if (!_offscreen_images[f].save(actual_save_filename,save_format.toLocal8Bit()))
		{
		  QMessageBox::critical(this,"Evolvotron","Failed to write file "+actual_save_filename);
		  if (f<_offscreen_images.size()-1)
		    {
		      QMessageBox::critical(this,"Evolvotron","Not attempting to save remaining images in animation");		    
		    }
		  break;
		}
	    }
	}
    }
  std::clog << "...save done\n";
}

void MutatableImageDisplay::menupick_save_function()
{
  if (_icon.get()) _main->setWindowIcon(*_icon);

  const QString save_filename=QFileDialog::getSaveFileName
    (
     this,
     "Save image function to an XML file",
     ".",
     "Functions (*.xml)"
     );

  if (!save_filename.isEmpty())
    {
      std::ofstream file(save_filename.toLocal8Bit());
      _image_function->save_function(file);
      file.flush();
      if (!file)
	{
	  QMessageBox::critical(this,"Evolvotron","File write failed");
	}
    }
}

void MutatableImageDisplay::load_function_file(const QString& load_filename)
{
  const std::string filename(load_filename.toLocal8Bit());
  std::ifstream file(filename.c_str());

  if (!file)
  {
    QMessageBox::critical(
      this,
      "Evolvotron",
      ("Filename '"+filename+"' could not be opened\n").c_str()
    );
  }
  else
  {
    std::string report;
    boost::shared_ptr<const MutatableImage> new_image_function(MutatableImage::load_function(_main->mutation_parameters().function_registry(),file,report));

    if (new_image_function.get()==0)
    {
      QMessageBox::critical(
	this,
	"Evolvotron",
	("Function in filename '"+filename+"' not loaded due to errors:\n"+report).c_str()
      );
    }
    else
    {
      if (!report.empty())
      {
	QMessageBox::warning(
	  this,
	  "Evolvotron",
	  ("Function in filename '"+filename+"' +loaded with warnings:\n"+report).c_str(),
	  QMessageBox::Ok,
	  QMessageBox::NoButton
	);
      }
      
      main().history().begin_action("load");
      main().history().replacing(this);
      main().history().end_action();
      image_function(new_image_function,false);
    }
  }
}

void MutatableImageDisplay::menupick_load_function()
{
  const QString load_filename=QFileDialog::getOpenFileName
    (
     this,
     "Load image function from an XML file",
     ".",
     "Functions (*.xml)"
     );

  if (!load_filename.isEmpty())
    {
      load_function_file(load_filename);
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

  image_function()->get_stats(total_nodes,total_parameters,depth,width,proportion_constant);

  std::stringstream msg;
  msg << " " << total_nodes      << "\t function nodes\n";
  msg << " " << total_parameters << "\t parameters\n";
  msg << " " << depth            << "\t maximum depth\n";
  msg << " " << width            << "\t width\n";
  msg << " " << std::setprecision(3) << 100.0*proportion_constant << "%\t constant\n";

  std::stringstream xml;
  image_function()->save_function(xml);

  _properties->set_content(msg.str(),xml.str());
  if (_icon.get()) _properties->setWindowIcon(*_icon);
  _properties->exec();
}

/*! Create an image display with no parent: becomes a top level window 
  Disable full menu functionality because there's less we can do with a single image (e.g no spawn_target)
*/
void MutatableImageDisplay::spawn_big(bool scrollable,const QSize& sz)
{
  MutatableImageDisplayBig*const top_level_widget=new MutatableImageDisplayBig(&main());
  top_level_widget->setLayout(new QVBoxLayout);
  if (_icon.get()) top_level_widget->setWindowIcon(*_icon);

  MutatableImageDisplay* display=0;

  if (scrollable)
    {
      QScrollArea*const scrollview=new QScrollArea;
      top_level_widget->layout()->addWidget(scrollview);
      display=new MutatableImageDisplay(&main(),false,true,sz,_frames,_framerate);
      scrollview->setWidget(display);
    }
  else
    {
      display=new MutatableImageDisplay(&main(),false,false,QSize(0,0),_frames,_framerate);
      top_level_widget->layout()->addWidget(display);
    }

  top_level_widget->show();

  //Propagate full screen mode 
  if (main().isFullScreen()) top_level_widget->showFullScreen();

  // Fire up image calculation
  display->image_function(_image_function,false);
}
