// Source file for evolvotron
// Copyright (C) 2003 Tim Day
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
  \brief Interface for class Args.
*/

#ifndef _args_h_
#define _args_h_

#include <sstream>

#include "useful.h"
#include <vector>
#include <string>

//! Class for sanitizing access to commandline arguments.
/*! The main purpose is to allow >> operators to be used to read args into parameters.
 */
class Args
{
 protected:

  int _argc;
  
  std::vector<std::string> _argv;
  
  //! The stringstream after the selected option.
  std::istringstream* _after;

 public:
  //! Construct from usual arg vector
  Args(int argc,char* argv[]);

  //! Destructor.
  ~Args();

  //! Return true if option is present, and set up in anticipation of subsequent after()
  bool option(const std::string& opt,int n=0);

  //! Return a stream containing the n option arguments requested by the last
  std::istringstream& after();

  //! Return the n-th last argument
  const std::string& last(int n) const;
};

#endif
