// Source file for evolvotron
// Copyright (C) 2002,2003 Tim Day
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
  \brief Interface for class XYZ.
*/

#ifndef _xyz_h_
#define _xyz_h_

#include "useful.h"
#include "random.h"
#include "tuple.h"

//! Class to hold vectors in 3D cartesian co-ordinates.
/*! Direct access to the x,y,z members is not permitted.
 */
class XYZ : public Tuple<3,float>
{
 public:

  //@{
  //! Access to underlying tuple representation.  Needed to avoid infinite recursion in symetric binary operators.
  Tuple<3,float>& rep()
    {
      return static_cast<Tuple<3,float>& > (*this);
    }
  const Tuple<3,float>& rep() const
    {
      return static_cast<const Tuple<3,float>& > (*this);
    }
  //@}

  //@{
  //! Accessor.
  float x() const
    {
      return (*this)[0];
    }
  float y() const
    {
      return (*this)[1];
    }
  float z() const
    {
      return (*this)[2];
    }

  void x(float v)
    {
      (*this)[0]=v;
    }
  void y(float v)
    {
      (*this)[1]=v;
    }
  void z(float v)
    {
      (*this)[2]=v;
    }
  //@}

  //! Null constructor.
  /*! NB The components are not cleared to zero. 
   */
  XYZ()
    {}

  //! Copy constructor.
  XYZ(const XYZ& v)
    :Tuple<3,float>(v.rep())
    {}
  
  //! Construct from base tuple class, but only when explicit.
  explicit XYZ(const Tuple<3,float>& v)
    :Tuple<3,float>(v)
    {}

  //! Initialise from separate components.
  XYZ(float vx,float vy,float vz)
    :Tuple<3,float>()
    {
      x(vx);
      y(vy);
      z(vz);
    }

  //! Trivial destructor.
  ~XYZ()
    {}

  //! Divide by scalar.
  /*! Implemented assuming one divide and three multiplies is faster than three divides.
   */
  void operator/=(float k)
    {
      const float ik(1.0/k);
      (*this)*=ik;
    }

  //! Assignment. 
  void assign(const XYZ& v)
    {
      x(v.x());
      y(v.y());
      z(v.z());
    }

  //! Negation.
  const XYZ operator-() const
    {
      return XYZ(-x(),-y(),-z());
    }

  //! Return the square of the magnitude.
  const float magnitude2() const
    {
      return x()*x()+y()*y()+z()*z();
    }

  //! Return the magnitude.
  const float magnitude() const
    {
      return sqrt(magnitude2());
    }

  //! Return the vector normalised.
  const XYZ normalised() const;

  //! Normalise this vector.
  void normalise();

  //! Returns true if an origin centred cuboid with this vectors semi-axes contains the argument.
  const bool origin_centred_rect_contains(const XYZ& p) const
    {
      return (-x()<=p.x() && p.x()<=x() && -y()<=p.y() && p.y()<=y() && -z()<=p.z() && p.z()<=z()); 
    }

  //! Write the vector.
  std::ostream& write(std::ostream&) const;

  //! Helper for common case of creating an instance filled with a common value.
  static const XYZ fill(float v)
    {
      return XYZ(Tuple<3,float>::fill(v));
    }

};

//! Cross product.
inline const XYZ operator*(const XYZ& a,const XYZ& b)
{
  return XYZ(
	     a.y()*b.z()-a.z()*b.y(),
	     a.z()*b.x()-a.x()*b.z(),
	     a.x()*b.y()-a.y()*b.x()
	     );
} 

//! Dot product.
/*! Perhaps a curious choice of operator but it works for me.
 */
inline const float operator%(const XYZ& a,const XYZ& b)
{
  return a.x()*b.x()+a.y()*b.y()+a.z()*b.z();
} 

//! Vector addition.
inline const XYZ operator+(const XYZ& a,const XYZ& b)
{
  return XYZ(a.rep()+b.rep());
}

//! Vector subtraction.
inline const XYZ operator-(const XYZ& a,const XYZ& b)
{
  return XYZ(a.rep()-b.rep());
}

//! Multiplication by scalar.
inline const XYZ operator*(float k,const XYZ& v)
{  
  XYZ ret(v);
  ret*=k;
  return ret;
}

//! Multiplication by scalar.
inline const XYZ operator*(const XYZ& v,float k)
{
  XYZ ret(v);
  ret*=k;
  return ret;
}

//! Division by scalar.
inline const XYZ operator/(const XYZ& v,float k)
{
  return (1.0/k)*v;
}

/*! If magnitude is zero we return zero vector.
 */
inline const XYZ XYZ::normalised() const
{
  const float m=magnitude();
  return (m==0.0 ? XYZ(0.0,0.0,0.0) : (*this)/m);
}

inline void XYZ::normalise()
{
  (*this)=normalised();
}

//! Stream output operator.
/*! Calls write().
 */
inline std::ostream& operator<<(std::ostream& out,const XYZ& v)
{
  return v.write(out);
}

//! Generates a random point in the cube bounded by (0,0,0) and (1.0,1.0,1.0)
class RandomXYZInUnitCube : public XYZ
{
 public:
  //! Constructor.
  RandomXYZInUnitCube(Random01&);
};

//! Generates random points in a recnangular box centred on the origin
class RandomXYZInBox : public XYZ
{
 public:
  //! Constructor.
  RandomXYZInBox(Random01& rng,const XYZ& bounds);
};

//! Generates a random point in or on a unit-radius sphere centred on the origin.
class RandomXYZInSphere : public XYZ
{
 public:
  //! Constructor.
  RandomXYZInSphere(Random01& rng,float radius);
};

//! Generates a random point on the surface of a unit-radius sphere
class RandomXYZSphereNormal : public XYZ
{
 public:
  //! Constructor.
  RandomXYZSphereNormal(Random01& rng);
};


//! Generates a random point in or on an origin-centred ellipsoid with semi-axes of the specified size.
class RandomXYZInEllipsoid : public XYZ
{
 public:
  //! Constructor.
  RandomXYZInEllipsoid(Random01& rng,const XYZ& axes);
};

//! Generates a random point in or on a disc in the XY plane of the specified radius.
class RandomXYZInXYDisc : public XYZ
{
 public:
  //! Constructor.
  RandomXYZInXYDisc(Random01& rng,float radius);
};

#endif





