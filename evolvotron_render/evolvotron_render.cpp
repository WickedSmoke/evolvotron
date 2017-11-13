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
  \brief Standalone renderer for evolvotron function files.
*/

#include "function_registry.h"
#include "mutatable_image.h"
#include "random.h"

#include <boost/program_options.hpp>

//! Application code
int main(int argc,char* argv[])
{
  {
    uint frames;
    bool help;
    bool jitter;
    int multisample;
    std::string output_filename;
    std::string size;
    bool verbose;
    
    boost::program_options::options_description options_desc("Options");
    boost::program_options::positional_options_description pos_options_desc;
    {
      using namespace boost::program_options;
      options_desc.add_options()
	("frames,f"     ,value<uint>(&frames)->default_value(1)    ,"Frames in an animation")
	("help,h"       ,bool_switch(&help)                        ,"Print command-line options help message and exit")
	("jitter,j"     ,bool_switch(&jitter)                      ,"Enable rendering jitter")
	("multisample,m",value<int>(&multisample)->default_value(1),"Multisampling grid (NxN)")
	("output,o"     ,value<std::string>(&output_filename)      ,"Output filename (.png or .ppm suffix).  (Or use first positional argument.)")
	("size,s"       ,value<std::string>(&size)->default_value("512x515"),"Generated image size")
	("verbose,v"    ,bool_switch(&verbose)                     ,"Log some details to stderr")
	;
      pos_options_desc.add("output",1);
    }

    boost::program_options::variables_map options;
    boost::program_options::store
      (
       boost::program_options::command_line_parser(argc,argv)
       .options(options_desc).positional(pos_options_desc).run()
       ,options
       );
    boost::program_options::notify(options);
    
    if (help)
      {
	std::cerr << options_desc;
	return 0;
      }

    if (verbose)
      std::clog.rdbuf(std::cerr.rdbuf());
    else
      std::clog.rdbuf(sink_ostream.rdbuf());

    //! \todo Could be done better maybe (set 'x' as input separator)
    const std::string::size_type p=size.find("x");
    if (p==std::string::npos || p==0 || p==size.size()-1)
      {
	std::cerr << "--size option argument isn't in <width>x<height> format\n";
	return 1;
      }
    else
      {
	size[p]=' ';
      }
    int width=512;
    int height=512;
    std::stringstream(size) >> width >> height;
    
    if (frames<1)
      {
	std::cerr << "Must specify at least 1 frame (option: -f <frames>)\n";
	return 1;
      }

    if (output_filename.empty())
      {
	std::cerr << "Must specify an output filename\n";
	return 1;
      }
    
    FunctionRegistry function_registry;
    
    std::string report;
    const boost::shared_ptr<const MutatableImage> imagefn(MutatableImage::load_function(function_registry,std::cin,report));

    if (imagefn.get()==0)
      {
	std::cerr << "evolvotron_render: Error: Function not loaded due to errors:\n" << report;
	return 1;
      }
    else if (!report.empty())
      {
	std::cerr << "evolvotron_render: Warning: Function loaded with warnings:\n" << report;
      }

    // Seed value pretty unimportant; only used for sample jitter.
    Random01 r01(23);

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
	    
	      const XYZ colour(imagefn->get_rgb(col,row,frame,width,height,frames,(jitter ? &r01 : 0),multisample));
	    
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
	  QString save_filename(QString::fromLocal8Bit(output_filename.c_str()));

	  const char* save_format="PPM";
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
	      std::cerr 
		<< "evolvotron_render: Warning: Unrecognised file suffix.  File will be written in "
		<< save_format
		<< " format.\n";
	    }

	  if (frames>1)
	    {
	      QString frame_component;
	      frame_component.sprintf(".f%06d",frame);
	      int insert_point=save_filename.lastIndexOf(QString("."));
	      if (insert_point==-1)
		{
		  save_filename.append(frame_component);
		}
	      else
		{
		  save_filename.insert(insert_point,frame_component);
		}
	    }
    
	  QImage image
	    (
	     reinterpret_cast<uchar*>(&(image_data[0])),
	     width,
	     height,
	     QImage::Format_RGB32
	     );

	  if (!image.save(save_filename,save_format))
	    {
	      std::cerr 
		<< "evolvotron_render: Error: Couldn't save file "
		<< save_filename.toLocal8Bit().data()
		<< "\n";
	      return 1;
	    }
	
	  std::clog
	    << "Wrote file " 
	    << save_filename.toLocal8Bit().data()
	    << "\n";
	}
      }
  }
  
  return 0;
}
