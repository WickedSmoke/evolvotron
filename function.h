// Source file for evolvotron
// Copyright (C) 2002 Tim Day
/*
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
  \brief Interfaces for Function classes.
  NB There is no class heirarchy here as all virtualisation and boilerplate services are supplied when the functions are plugged into the MutatableImageNode template.
*/

#ifndef _function_h_
#define _function_h_

#include "mutatable_image.h"
#include "xyz.h"

namespace Function
{

class Identity
{
 public:
  
  static const uint parameters()
    {
      return 0;
    }
  static const uint arguments()
    {
      return 0;
    }
  static const XYZ evaluate(const MutatableImageNode*const our,const XYZ& p);
};

}

#endif