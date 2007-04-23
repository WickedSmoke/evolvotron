// Source file for evolvotron
// Copyright (C) 2002,2003 Tim Day
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

#include <iostream>
#include <fstream>
#include <string>

#include <qimage.h>
#include <qstring.h>

#include "args.h"
#include "mutatable_image.h"

//! Application code
int main(int argc,char* argv[])
{
  Args args(argc,argv);

  if (args.option("-v")) 
    std::clog.rdbuf(std::cerr.rdbuf());
  else
    std::clog.rdbuf(sink_ostream.rdbuf());

  FunctionRegistry function_registry;

  std::string report;
  const std::auto_ptr<const MutatableImage> imagefn(MutatableImage::load_function(function_registry,std::cin,report));

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

  for (uint frame=0;frame<frames;frame++)
    {
      std::vector<uint> image_data;
      image_data.reserve(width*height);
  
      for (int row=0;row<height;row++)
	for (int col=0;col<width;col++)
	  {
	    // xy co-ords vary over -1.0 to 1.0.  In the one frame case z will be 0
	    const XYZ p(
			imagefn->sampling_coordinate(col,row,frame,width,height,frames)
			);
	    
	    uint c[3];
	    imagefn->get_rgb(p,c);
	    
	    image_data.push_back((c[0]<<16)|(c[1]<<8)|(c[2]));
	  }

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
  
  exit(0);
}
