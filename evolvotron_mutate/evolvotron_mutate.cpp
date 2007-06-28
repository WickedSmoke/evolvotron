// Source file for evolvotron
// Copyright (C) 2002,2003,2004,2007 Tim Day
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

#include "libevolvotron_precompiled.h"

#include <qstring.h>
#include <qdatetime.h>

extern "C"
{
#include <unistd.h>
}

#include "args.h"
#include "mutatable_image.h"
#include "mutation_parameters.h"
#include "function_top.h"

//! Application code
int main(int argc,char* argv[])
{
  {
    Args args(argc,argv);

    if (args.option("-v")) 
      std::clog.rdbuf(std::cerr.rdbuf());
    else
      std::clog.rdbuf(sink_ostream.rdbuf());
  
    // Normally would use time(0) to seed random number generator
    // but can imagine several of these starting up virtually simultaneously
    // so need something with higher resolution.
    // Adding the process id too to keep things unique.
  
    QTime t(QTime::currentTime());
    uint seed=getpid()+t.msec()+1000*t.second()+60000*t.minute()+3600000*t.hour();
  
    std::clog << "Random seed is " << seed << "\n";
  
    MutationParameters mutation_parameters(seed,0);

    std::string report;
    boost::shared_ptr<const MutatableImage> imagefn_out;

    if (args.option("-g"))
      {
	std::auto_ptr<FunctionTop> fn_top(FunctionTop::initial(mutation_parameters));

	imagefn_out=boost::shared_ptr<const MutatableImage>(new MutatableImage(fn_top,!args.option("-linz"),args.option("-spheremap"),false));
      }
    else
      {

	const boost::shared_ptr<const MutatableImage> imagefn_in(MutatableImage::load_function(mutation_parameters.function_registry(),std::cin,report));

	if (imagefn_in)
	  {
	    std::cerr << "evolvotron_render: Error: Function not loaded due to errors:\n" << report;
	    exit(1);
	  }
	else if (!report.empty())
	  {
	    std::cerr << "evolvotron_render: Warning: Function loaded with warnings:\n" << report;
	  }

	imagefn_out=imagefn_in->mutated(mutation_parameters);
      }

    imagefn_out->save_function(std::cout);
  }

#ifndef NDEBUG
  assert(InstanceCounted::is_clear());
#endif

  exit(0);
}
