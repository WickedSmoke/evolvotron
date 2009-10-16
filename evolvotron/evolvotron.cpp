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
  
  // General options
  bool autocool;
  bool fullscreen;
  std::string grid;
  bool help;
  bool jitter;
  bool menuhide;
  uint multisample;
  bool spheremap;
  boost::program_options::options_description options_desc("General options");
  {
    using namespace boost::program_options;
    options_desc.add_options()
      ("autocool,a"   ,bool_switch(&autocool)                         ,"Enable autocooling")
      ("grid,g"       ,value<std::string>(&grid)->default_value("5x6"),"Rows x columns in image grid")
      ("fullscreen,S" ,bool_switch(&fullscreen)                       ,"Fullscreen window")
      ("help,h"       ,bool_switch(&help)                             ,"Print command-line options help message and exit")
      ("jitter,j"     ,bool_switch(&jitter)                           ,"Enable rendering jitter")
      ("menuhide,M"   ,bool_switch(&menuhide)                         ,"Hide menus")
      ("multisample,m",value<uint>(&multisample)->default_value(1)    ,"Multisampling grid (NxN)")
      ("spheremap,p"  ,bool_switch(&spheremap)                        ,"Generate spheremaps")
      ;
  }

  // Animation options
  int framerate;
  int frames;
  bool linear;
  boost::program_options::options_description animation_options_desc("Animation options");
  {
    using namespace boost::program_options;
    animation_options_desc.add_options()
      ("fps,s"        ,value<int>(&framerate)->default_value(8)   ,"Animation speed (frames-per-second)")
      ("frames,f"     ,value<int>(&frames)->default_value(1)      ,"Frames in an animation")
      ("linear,l"     ,bool_switch(&linear)                       ,"Sweep z linearly in animations")
      ;
  }

  // Advanced options
  bool debug;
  bool enlargement_threadpool;
  std::string favourite;
  int niceness_enlargement;
  int niceness_grid;
  uint threads;
  bool unwrapped;
  bool verbose;

  boost::program_options::options_description advanced_options_desc("Advanced options");
  {
    using namespace boost::program_options;
    advanced_options_desc.add_options()
      ("debug,D"                 ,bool_switch(&debug)                    ,"Enable function debug mode")
      ("enlargement-threadpool,E",bool_switch(&enlargement_threadpool)   ,"Enlargements computed using a separate threadpool")
      ("favourite,F"             ,value<std::string>(&favourite)         ,"Favourite function")
      ("nice,n"                  ,value<int>(&niceness_grid)->default_value(4)
       ,"Niceness of compute threads for image grid")
      ("Nice,N"                  ,value<int>(&niceness_enlargement)->default_value(8)
       ,"Niceness of compute threads for enlargements (if separate pool)")
      ("threads,t"               ,value<uint>(&threads)->default_value(get_number_of_processors())
       ,"Number of threads in a thread pool")
      ("unwrapped,u"             ,bool_switch(&unwrapped)                ,"Don't wrap favourite function")
      ("verbose,v"               ,bool_switch(&verbose)                  ,"Log some details to stderr")
      ;
  }

  options_desc.add(animation_options_desc);
  options_desc.add(advanced_options_desc);
  boost::program_options::variables_map options;
  boost::program_options::store(boost::program_options::parse_command_line(argc,argv,options_desc),options);
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
  const std::string::size_type p=grid.find("x");
  if (p==std::string::npos || p==0 || p==grid.size()-1)
    {
      std::cerr << "--grid option argument isn't in <rows>x<cols> format\n";
      return 1;
    }
  else
    {
      grid[p]=' ';
    }
  int rows=5;
  int cols=6;
  std::stringstream(grid) >> rows >> cols;

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
       multisample,
       debug,
       linear,
       spheremap
       );

  main_widget->mutation_parameters().function_registry().status(std::clog);

  if (!favourite.empty())
    {
      std::clog
	<< "Selected specific function: "
	<< favourite
	<< (unwrapped ? " (unwrapped)" : " (wrapped)")
	<< "\n";

      if (!main_widget->favourite_function(favourite))
	{
	  std::cerr << "Unrecognised function name specified for -x/-X option\n";
	  return 1;
	}

      main_widget->favourite_function_unwrapped(unwrapped);
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
