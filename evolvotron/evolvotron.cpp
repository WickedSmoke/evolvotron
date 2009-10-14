/**************************************************************************/
/*  Copyright 2009 Tim Day                                                */
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
  \brief Application code for evolvotron executable (contains "main").
*/

/*! \mainpage Evolvotron : image evolver

  \author Tim Day 

  \section introduction Introduction
  "Evolvotron" is an interactive tool for producing "generative art".
  Images are generated from function trees, which are then mutated and evolved through a process of user selection.

  \todo For new features to be added, see the TODO file.
 */

#include "evolvotron_precompiled.h"

#include "platform_specific.h"

#include "evolvotron_main.h"

//! Application code
int main(int argc,char* argv[])
{
  QApplication app(argc,argv);

  // A 4:3 ratio would nice to get square images on most screens, but isn't many images so default to 6:5
  uint cols;
  uint rows;
  uint multisample_level;
  boost::program_options::options_description options_desc("Recognized options");
  options_desc.add_options()
    ("autocool,a","enable autocooling")
    ("cols,c",boost::program_options::value<uint>(&cols)->default_value(6),"columns in image grid")
    ("fullscreen,F","fullscreen window")
    ("help,h","produce command-line options help message and exit")
    ("jitter,j","enable rendering jitter")
    ("menuhide,M","hide menus")
    ("multisample,m",boost::program_options::value<uint>(&multisample_level)->default_value(1),"multisampling grid (NxN)")
    ("rows,r",boost::program_options::value<uint>(&rows)->default_value(5),"rows in image grid")
    ("spheremap,p","generate spheremaps")
    ;

  int frames;
  int framerate;
  boost::program_options::options_description animation_options_desc("Animation options");
  animation_options_desc.add_options()
    ("frames,f",boost::program_options::value<int>(&frames)->default_value(1),"frames in an animation")
    ("linear,l","sweep z linearly in animations")
    ("fps,s",boost::program_options::value<int>(&framerate)->default_value(8),"animation speed (frames per second)")
    ;

  std::string favourite_function;
  int niceness_grid;
  int niceness_enlargement;
  uint threads;
  boost::program_options::options_description advanced_options_desc("Advanced options");
  advanced_options_desc.add_options()
    ("debug,D","enable function debug mode")
    ("enlargement-threadpool,E","Enlargements computed using a separate threadpool")
    ("favourite,x",boost::program_options::value<std::string>(&favourite_function),"favourite function, wrapped")
    ("favourite-unwrapped,X",boost::program_options::value<std::string>(&favourite_function),"favourite function, no wrapper")
    ("nice,n",boost::program_options::value<int>(&niceness_grid)->default_value(4),"niceness of compute threads for image grid")
    ("Nice,N",boost::program_options::value<int>(&niceness_enlargement)->default_value(8),"niceness of compute threads for enlargements (if separate pool)")
    ("threads,t",boost::program_options::value<uint>(&threads)->default_value(get_number_of_processors()),"threads in thread pools")
    ("verbose,v","log some details to stderr")  // bool
    ;

  options_desc.add(animation_options_desc);
  options_desc.add(advanced_options_desc);

  boost::program_options::variables_map options;
  boost::program_options::store(boost::program_options::parse_command_line(argc,argv,options_desc),options);
  boost::program_options::notify(options);

  // General options
  const bool autocool=options.count("autocool");
  const bool fullscreen=options.count("fullscreen");
  const bool help=options.count("help");
  const bool jitter=options.count("jitter");
  const bool menuhide=options.count("menuhide");
  const bool spheremap=options.count("spheremap");

  // Animation options
  const bool linear=options.count("linear");

  // Advanced options
  const bool debug=options.count("debug");
  const bool enlargement_threadpool=options.count("enlargement-threadpool");
  const bool favourite_unwrapped=options.count("favourite-unwrapped");
  const bool verbose=options.count("verbose");

  if (help)
    {
      std::cerr << options_desc;
      return 0;
    }
  
  if (verbose)
    std::clog.rdbuf(std::cerr.rdbuf());
  else
    std::clog.rdbuf(sink_ostream.rdbuf());

  if (cols*rows<2)
    {
      std::cerr << "Must be at least 2 display grid cells (options: -g <cols> <rows>)\n";
      return 1;
    }

  if (frames<1)
    {
      std::cerr << "Must specify at least 1 frame (option: -f <frames>)\n";
      return 1;
    }

  if (framerate<1)
    {
      std::cerr << "Must specify framerate of at least 1 (option: -s <framerate>)\n";
      return 1;
    }

  std::clog
    << "Evolvotron version "
    << stringify(EVOLVOTRON_BUILD)
    << " starting with " 
    << cols 
    << " by " 
    << rows 
    << " display cells and " 
    << threads
    << " compute threads per farm (niceness "
    << niceness_grid
    << " and "
    << niceness_enlargement
    << ")\n";

  EvolvotronMain*const main_widget=new EvolvotronMain
      (
       0,
       QSize(cols,rows),
       frames,
       framerate,
       threads,
       enlargement_threadpool,
       niceness_grid,
       niceness_enlargement,
       fullscreen,
       menuhide,
       autocool,
       jitter,
       multisample_level,
       debug,
       linear,
       spheremap
       );

  main_widget->mutation_parameters().function_registry().status(std::clog);

  if (!favourite_function.empty())
    {
      std::clog
	<< "Selected specific function: "
	<< favourite_function
	<< (favourite_unwrapped ? " (unwrapped)" : " (wrapped)")
	<< "\n";

      if (!main_widget->favourite_function(favourite_function))
	{
	  std::cerr << "Unrecognised function name specified for -x/-X option\n";
	  return 1;
	}

      main_widget->favourite_function_unwrapped(favourite_unwrapped);
    }
  
  main_widget->show();
  
  // NB Do this here rather than in constructor so that compute threads aren't being fired off during general GUI set-up
  
  std::clog << "Resetting main widget...\n";
  main_widget->reset_cold();
  
  // NB No need to worry about deleting EvolvotronMain... QApplication seems to do it for us.
  std::clog << "Commencing main loop...\n";
  return app.exec();
  std::clog << "...returned from main loop\n";
  
  return 0;
}
