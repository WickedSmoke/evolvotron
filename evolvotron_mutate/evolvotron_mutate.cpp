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
  \brief Standalone mutator for evolvotron function files.
*/

#include <iostream>
#include <fstream>
#include <string>

#include <qstring.h>
#include <qdatetime.h>

extern "C"
{
#include <unistd.h>
}

#include "args.h"
#include "mutatable_image.h"
#include "mutation_parameters.h"

//! Application code
int main(int argc,char* argv[])
{
  Args args(argc,argv);

  std::ofstream sink("/dev/null");

  if (args.option("-v")) 
    std::clog.rdbuf(std::cerr.rdbuf());
  else
    std::clog.rdbuf(sink.rdbuf());
  
  // Normally would use time(0) to seed random number generator
  // but can imagine several of these starting up virtually simultaneously
  // so need something with higher resolution.
  // Adding the process id too to keep things unique.
  
  QTime t(QTime::currentTime());
  uint seed=getpid()+t.msec()+1000*t.second()+60000*t.minute()+3600000*t.hour();
  
  std::clog << "Random seed is " << seed << "\n";
  
  MutationParameters mutation_parameters(seed);

  std::string report;
  MutatableImage* imagefn;
  if (args.option("-g"))
    {
      FunctionNode*const fn=FunctionNode::initial(mutation_parameters);
      imagefn=new MutatableImage(fn);
    }
  else
    {

      imagefn=MutatableImage::load_function(std::cin,report);

      if (imagefn==0)
	{
	  std::cerr << "evolvotron_render: Error: Function not loaded due to errors:\n" << report;
	  exit(1);
	}
      else if (!report.empty())
	{
	  std::cerr << "evolvotron_render: Warning: Function loaded with warnings:\n" << report;
	}

      imagefn->mutate(mutation_parameters);
    }

  imagefn->save_function(std::cout);

  exit(0);
}