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
#include <iostream>

Args* Args::_global=0;

Args::Args(int argc,char* argv[])
  :_argc(argc)
  ,_after(0)
{
  for (int i=0;i<argc;i++)
    _argv.push_back(std::string(argv[i]));
  _global=this;
}

Args::~Args()
{
  delete _after;
}

bool Args::option(const std::string& opt,int n)
{
  delete _after;
  _after=0;

  int i;
  for (i=1;i<_argc;i++)
    {
      if (_argv[i]==opt) break;
    }

  if (i==_argc) return false;

  _after=new std::istringstream;
  
  std::string opts_after;

  for (int o=1;o<=n;o++)
    {
      opts_after+=_argv[i+o];
      if (o!=n) opts_after+=" ";
  
    }
  (*_after).str(opts_after);

  return true;
}
  
std::istringstream& Args::after()
{
  assert(_after!=0);

  return *_after;
}

const std::string& Args::last(int n) const
{
  assert(n!=0);
  if (_argc-n<1)
    {
      std::cerr << "Error: insufficient arguments supplied\n";
      exit(1);
    }
  return _argv[_argc-n];
}
