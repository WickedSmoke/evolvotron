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
  \brief Interface for class Transform.
*/

#ifndef _transform_h_
#define _transform_h_

#include "common.h"

#include "xyz.h"

//! Class representing 3d linear transforms.
/*! Not much functionality currently because is used mainly to pass info around for warp functionality
 */
class Transform 
{
 public:
  //! Default constructor.  NB Doesn't set up identity or anything.
  Transform();

  //! Copy constructor.
  Transform(const Transform&);

  //! Constructor specifying column vectors.
  Transform(const XYZ& t,const XYZ& x,const XYZ& y,const XYZ& z);

  //! Constructor specifying column-wise elements.
  Transform(const std::vector<real>& v,uint starting_element=0);

  //! virtual destructor in case of extension
  virtual ~Transform();

  //@{
  //! Accessor
  const XYZ& translate() const
    {
      return _translate;
    }
  const XYZ& basis_x() const
    {
      return _basis_x;
    }
  const XYZ& basis_y() const    
    {
      return _basis_y;
    }
  const XYZ& basis_z() const    
    {
      return _basis_z;
    }

  void translate(const XYZ &t)
     {
       _translate=t;
     }
  void basis_x(const XYZ &x)
     {
       _basis_x=x;
     }
  void basis_y(const XYZ &y)
     {
       _basis_y=y;
     }
  void basis_z(const XYZ &z)
     {
       _basis_z=z;
     }
  //@}

  //! Get column-wise element values as a vector
  const std::vector<real> get_columns() const;

  //! Transform a point
  const XYZ transformed(const XYZ& p) const;

  //! Transform a point with no translation
  const XYZ transformed_no_translate(const XYZ& p) const;

  //! Concatenate transforms
  Transform& concatenate_on_right(const Transform& t);

  //! Concatenate transforms
  Transform& concatenate_on_left(const Transform& t);

 protected:
  //@{
  //! Translation component (column vector in matrix).
  XYZ _translate;
  XYZ _basis_x;
  XYZ _basis_y;
  XYZ _basis_z;
};

inline const XYZ operator*(const Transform& t,const XYZ& p)
{
  return 
     t.basis_x()*p.x()
    +t.basis_y()*p.y()
    +t.basis_z()*p.z()
    +t.translate();
}

inline std::ostream& operator<<(std::ostream& out,const Transform& t)
{
  return out << t.translate() << ";" << t.basis_x() << "," << t.basis_y() << "," << t.basis_z();
}

class TransformIdentity : public Transform
{
 public:
  TransformIdentity()
    {
      translate(XYZ(0.0,0.0,0.0));
      basis_x(XYZ(1.0,0.0,0.0));
      basis_y(XYZ(0.0,1.0,0.0));
      basis_z(XYZ(0.0,0.0,1.0));
    }
};

class TransformTranslate : public Transform
{
 public:
  TransformTranslate(const XYZ& t)
    {
      translate(t);
      basis_x(XYZ(1.0,0.0,0.0));
      basis_y(XYZ(0.0,1.0,0.0));
      basis_z(XYZ(0.0,0.0,1.0));
    }
};

class TransformScale : public Transform
{
 public:
  TransformScale(const XYZ& s)
    {
      translate(XYZ(0.0,0.0,0.0));
      basis_x(XYZ(s.x(),0.0,0.0));
      basis_y(XYZ(0.0,s.y(),0.0));
      basis_z(XYZ(0.0,0.0,s.z()));
    }
  TransformScale(real s)
    {
      translate(XYZ(0.0,0.0,0.0));
      basis_x(XYZ(s,0.0,0.0));
      basis_y(XYZ(0.0,s,0.0));
      basis_z(XYZ(0.0,0.0,s));
    }
};


class TransformRotateX : public Transform
{
 public:
  TransformRotateX(real a)
    {
      const real sa=sin(a);
      const real ca=cos(a);

      translate(XYZ(0.0,0.0,0.0));
      basis_x(XYZ(1.0,0.0,0.0));
      basis_y(XYZ(0.0, ca , sa ));
      basis_z(XYZ(0.0,-sa , ca ));
    }
};

class TransformRotateY : public Transform
{
 public:
  TransformRotateY(real a)
    {
      const real sa=sin(a);
      const real ca=cos(a);
      
      translate(XYZ(0.0,0.0,0.0));
      basis_x(XYZ( ca ,0.0,-sa ));
      basis_y(XYZ(0.0,1.0,0.0));
      basis_z(XYZ( sa ,0.0, ca ));
    }
};

class TransformRotateZ : public Transform
{
 public:
  TransformRotateZ(real a)
    {
      const real sa=sin(a);
      const real ca=cos(a);
      
      translate(XYZ(0.0,0.0,0.0));
      basis_x(XYZ( ca , sa ,0.0));
      basis_y(XYZ(-sa , ca ,0.0));
      basis_z(XYZ(0.0,0.0,1.0));
    }
};

#endif
