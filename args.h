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

//! Class for sanitizing access to commandline arguments.
/*! The main purpose is to allow >> operators to be used to read args into parameters.
  Note that any information inherent in the vector partitioning of the arguments is disregarded;
  for example '-f file name' and '-f "file name"' on the command line will look the same to users of this class.
 */
class Args
{
 protected:

  //! The arguments all concatenated together.
  std::string _args;

  //! The stringstream after the selected option.
  std::istringstream* _after;

 public:
  //! Construct from usual arg vector
  Args(int argc,char* argv[]);

  //! Destructor.
  ~Args();

  //! Return true if option is present, and set up in anticipation of subsequent after()
  bool option(const std::string& opt);

  //! Return a stream from which arguments
  std::istringstream& after();
};

#endif
