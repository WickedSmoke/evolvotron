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
  \brief Implementation of class Args.
*/

#include "args.h"

/*! We pad _args with a space at each end to make it easier to search for options.
 */
Args::Args(int argc,char* argv[])
:_after(0)
{
  _args.append(" ");
  for (int i=1;i<argc;i++)
    {
      _args.append(argv[i]);
      _args.append(" ");
    }
}

Args::~Args()
{}

bool Args::option(const std::string& opt)
{
  const uint opt_pos=_args.find(" "+opt+" ");

  if (opt_pos<_args.size())
    {
      const uint opt_after=opt_pos+opt.size()+2;

      delete _after;
      _after=new std::istringstream(_args.substr(opt_after,_args.size()-opt_after));

      return true;
    }
  else
    {
      return false;
    }
}
  
std::istringstream& Args::after()
{
  assert(_after!=0);

  return *_after;
}
