// Source file for evolvotron
// Copyright (C) 2002 Tim Day
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
  \brief Application boilerplate for evolvotron executable (contains "main").
*/

/*! \mainpage Evolvotron : image evolver

  \author Tim Day 

  \section introduction Introduction
  "Evolvotron" is an interactive tool for producing "generative art".
  Images are generated from function trees, which are then mutated and evolved through a process of user selection.

  \todo For new features to be added, see the TODO file.
 */

#include <iostream>
#include <fstream>

#include <qapplication.h>

#include "args.h"

#include "evolvotron_main.h"

#include "tuple.h"
#include "matrix.h"

//! Application code
int main(int argc,char* argv[])
{
  // Call test of matrix functionality
  //testmatrix();

  QApplication app(argc,argv);

  Args args(argc,argv);

  uint rows=6;
  uint cols=8;
  uint threads=2;

  std::ofstream sink("/dev/null");

  if (args.option("-v")) 
    std::clog.rdbuf(std::cerr.rdbuf());
  else
    std::clog.rdbuf(sink.rdbuf());

  if (args.option("-g")) args.after() >> cols >> rows;
  
  if (args.option("-t")) args.after() >> threads;

  std::clog
    << "Evolvotron version "
    << EVOLVOTRON_VERSION_STRING
    << " starting with " 
    << cols 
    << " by " 
    << rows 
    << " display cells and " 
    << threads 
    << " compute threads\n";

  FunctionRegistry::get().status(std::clog);

  if (threads<1)
    {
      std::cerr << "Must specify at least one compute thread (option: -t <threads>)\n";
      exit(1);
    }

  if (cols*rows<2)
    {
      std::cerr << "Must be at least 2 display grid cells (options: -g <cols> <rows>)\n";
      exit(1);
    }
  
  // Columns, rows, threads
  EvolvotronMain*const main_widget=new EvolvotronMain(0,QSize(cols,rows),threads);
  
  app.setMainWidget(main_widget);
  main_widget->show();
    
  std::clog << "Commencing main loop...\n";

  // NB Do this here rather than in constructor so that compute threads aren't being fired off during general GUI set-up
  main_widget->reset_cold();
    
  // NB No need to worry about deleting EvolvotronMain... QApplication seems to do it for us.
  return app.exec();
}
