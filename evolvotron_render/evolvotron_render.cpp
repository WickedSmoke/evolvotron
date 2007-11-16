// Source file for evolvotron_render
// Copyright (C) 2007 Tim Day
/*! \page License License

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
  \brief Standalone renderer for evolvotron function files.
*/

#include "evolvotron_render_precompiled.h"

#include "args.h"
#include "function_registry.h"
#include "mutatable_image.h"
#include "random.h"

//! Application code
int main(int argc,char* argv[])
{
  {
    Args args(argc,argv);
    
    if (args.option("-v")) 
      std::clog.rdbuf(std::cerr.rdbuf());
    else
      std::clog.rdbuf(sink_ostream.rdbuf());
    
    FunctionRegistry function_registry;
    
    std::string report;
    const boost::shared_ptr<const MutatableImage> imagefn(MutatableImage::load_function(function_registry,std::cin,report));

    if (imagefn.get()==0)
      {
	std::cerr << "evolvotron_render: Error: Function not loaded due to errors:\n" << report;
	exit(1);
      }
    else if (!report.empty())
      {
	std::cerr << "evolvotron_render: Warning: Function loaded with warnings:\n" << report;
      }

    int width=512;
    int height=512;
  
    if (args.option("-s",2)) args.after() >> width >> height;

    uint frames=1;
    if (args.option("-f",1)) args.after() >> frames;
    if (frames<1)
      {
	std::cerr << "Must specify at least 1 frame (option: -f <frames>)\n";
	exit(1);
      }

    const bool jitter=args.option("-j");
  
    uint multisample_level=1;
    if (args.option("-m",1)) args.after() >> multisample_level;

    Random01 r01(23);     // Seed pretty unimportant; only used for sample jitter.

    for (uint frame=0;frame<frames;frame++)
      {
	std::vector<uint> image_data;
	image_data.reserve(width*height);
  
	uint pixels=0;
	uint report=1;
	const uint reports=20;
	for (int row=0;row<height;row++)
	  for (int col=0;col<width;col++)
	    {
	      const XYZ v(imagefn->sampling_coordinate(col,row,frame,width,height,frames));
	    
	      const XYZ colour(imagefn->get_rgb(col,row,frame,width,height,frames,(jitter ? &r01 : 0),multisample_level));
	    
	      const uint col0=lrint(clamped(colour.x(),0.0,255.0));
	      const uint col1=lrint(clamped(colour.y(),0.0,255.0));
	      const uint col2=lrint(clamped(colour.z(),0.0,255.0));

	      image_data.push_back(((col0<<16)|(col1<<8)|(col2)));

	      pixels++;
	      if (pixels>=(report*width*height)/reports)
		{
		  std::clog << "[" << (100*report)/reports << "%]";
		  report++;
		}
	    }
	std::clog << "\n";

	{
	  //! \todo If filename is "-", write PPM to stdout (QImage save only supports write-to-a-filenames though)
	  QString save_filename(QString::fromLocal8Bit(args.last(1).c_str()));

	  const char* save_format="PPM";
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
	      std::cerr 
		<< "evolvotron_render: Warning: Unrecognised file suffix.  File will be written in "
		<< save_format
		<< " format.\n";
	    }

	  if (frames>1)
	    {
	      QString frame_component;
	      frame_component.sprintf(".f%06d",frame);
	      int insert_point=save_filename.findRev(QString("."));
	      if (insert_point==-1)
		{
		  save_filename.append(frame_component);
		}
	      else
		{
		  save_filename.insert(insert_point,frame_component);
		}
	    }
    
	  QImage image(
		       (uchar*)&(image_data[0]),
		       width,
		       height,
		       32,
		       0,
		       0,
		       QImage::LittleEndian
		       );

	  if (!image.save(save_filename,save_format))
	    {
	      std::cerr 
		<< "evolvotron_render: Error: Couldn't save file "
		<< save_filename.local8Bit()
		<< "\n";
	      exit(1);
	    }
	
	  std::clog
	    << "Wrote file " 
	    << save_filename.local8Bit()
	    << "\n";
	}
      }
  }

#ifndef NDEBUG
  assert(InstanceCounted::is_clear());
#endif
  
  exit(0);
}
