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
  \brief Interface for class Transform.
*/

#ifndef _transform_h_
#define _transform_h_

#include "useful.h"
#include "xyz.h"
#include "matrix.h"

//! Class representing 3d linear transforms.
/*! Not much functionality currently because is used mainly to pass info around for warp functionality
 */
class Transform 
{
 public:
  //! Default constructor sets up identity;
  Transform();

  //! Constructor specifying column vectors.
  Transform(const XYZ& t,const XYZ& x,const XYZ& y,const XYZ& z);

  //@{
  //! Accessor
  const XYZ& translate() const
    {return _translate;}
  const XYZ& basis_x() const
    {return _basis_x;}
  const XYZ& basis_y() const    
    {return _basis_y;}
  const XYZ& basis_z() const    
    {return _basis_z;}
  void translate(const XYZ &t)
     {_translate=t;}
  void basis_x(const XYZ &x)
     {_basis_x=x;}
  void basis_y(const XYZ &y)
     {_basis_y=y;}
  void basis_z(const XYZ &z)
     {_basis_z=z;}
  //@}

 protected:
  //@{
  //! Translation component (column vector in matrix).
  XYZ _translate;
  XYZ _basis_x;
  XYZ _basis_y;
  XYZ _basis_z;
};

#endif





