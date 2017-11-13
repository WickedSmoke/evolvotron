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
  \brief Standalone mutator for evolvotron function files.
*/

#include "mutatable_image.h"
#include "mutation_parameters.h"
#include "function_top.h"

#include <boost/program_options.hpp>

//! Application code
int main(int argc,char* argv[])
{
  {
    bool genesis;
    bool help;
    bool linear;
    bool spheremap;
    bool verbose;

    boost::program_options::options_description options_desc("Options");
    {
      using namespace boost::program_options;
      options_desc.add_options()
	("genesis,g"  ,bool_switch(&genesis)  ,"Create a new function to stdout (without this option, a function will be read from stdin)")
	("help,h"     ,bool_switch(&help)     ,"Print command-line options help message and exit")
	("linear,l"   ,bool_switch(&linear)   ,"Sweep z linearly in animations")
	("spheremap,p",bool_switch(&spheremap),"Generate spheremap")
	("verbose,v"  ,bool_switch(&verbose)  ,"Log some details to stderr")
	;
    }
    
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
    
    // Normally would use time(0) to seed random number generator
    // but can imagine several of these starting up virtually simultaneously
    // so need something with higher resolution.
    // Adding the process id too to keep things unique.
    
    QTime t(QTime::currentTime());
    uint seed=getpid()+t.msec()+1000*t.second()+60000*t.minute()+3600000*t.hour();
    
    std::clog << "Random seed is " << seed << "\n";
    
    MutationParameters mutation_parameters(seed,false,false);
    
    std::string report;
    boost::shared_ptr<const MutatableImage> imagefn_out;
    
    if (genesis)
      {
	std::unique_ptr<FunctionTop> fn_top(FunctionTop::initial(mutation_parameters));
	
	imagefn_out=boost::shared_ptr<const MutatableImage>(new MutatableImage(fn_top,!linear,spheremap,false));
      }
    else
      {
	const boost::shared_ptr<const MutatableImage> imagefn_in
	  (
	   MutatableImage::load_function(mutation_parameters.function_registry(),std::cin,report)
	   );
	
	if (imagefn_in.get()==0)
	  {
	    std::cerr << "evolvotron_mutate: Error: Function not loaded due to errors:\n" << report;
	    return 1;
	  }
	else if (!report.empty())
	  {
	    std::cerr << "evolvotron_mutate: Warning: Function loaded with warnings:\n" << report;
	  }
	
	imagefn_out=imagefn_in->mutated(mutation_parameters);
      }
    
    imagefn_out->save_function(std::cout);
  }
    
  return 0;
}
