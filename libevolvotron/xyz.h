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

//! Class to hold vectors in 3D cartesian co-ordinates.
/*! Direct access to the x,y,z members is not permitted.
 */
class XYZ
{
 protected:
  real rep[3];

 public:

  //@{
  //! Accessor.
  real x() const
    {
      return rep[0];
    }
  real y() const
    {
      return rep[1];
    }
  real z() const
    {
      return rep[2];
    }

  void x(real v)
    {
      rep[0]=v;
    }
  void y(real v)
    {
      rep[1]=v;
    }
  void z(real v)
    {
      rep[2]=v;
    }
  //@}

  //! Null constructor.
  /*! NB The components are not cleared to zero. 
   */
  XYZ()
    {}

  //! Copy constructor.
  XYZ(const XYZ& v)
    {
      rep[0]=v.rep[0];
      rep[1]=v.rep[1];
      rep[2]=v.rep[2];
    }
  
  //! Initialise from separate components.
  XYZ(real vx,real vy,real vz)
    {
      rep[0]=vx;
      rep[1]=vy;
      rep[2]=vz;
    }

  //! Trivial destructor.
  ~XYZ()
    {}

  //! Subtract a vector
  void operator-=(const XYZ& v)
    {
      rep[0]-=v.rep[0];
      rep[1]-=v.rep[1];
      rep[2]-=v.rep[2];
    }

  //! Add a vector
  void operator+=(const XYZ& v)
    {
      rep[0]+=v.rep[0];
      rep[1]+=v.rep[1];
      rep[2]+=v.rep[2];
    }

  //! Multiply by scalar
  void operator*=(real k)
    {
      rep[0]*=k;
      rep[1]*=k;
      rep[2]*=k;
    }

  //! Divide by scalar.
  /*! Implemented assuming one divide and three multiplies is faster than three divides.
   */
  void operator/=(real k)
    {
      const real ik(1.0/k);
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
  const real magnitude2() const
    {
      return x()*x()+y()*y()+z()*z();
    }

  //! Return the magnitude.
  const real magnitude() const
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
  static const XYZ fill(real v)
    {
      return XYZ(v,v,v);
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
inline const real operator%(const XYZ& a,const XYZ& b)
{
  return a.x()*b.x()+a.y()*b.y()+a.z()*b.z();
} 

//! Vector addition.
inline const XYZ operator+(const XYZ& a,const XYZ& b)
{
  return XYZ(a.x()+b.x(),a.y()+b.y(),a.z()+b.z());
}

//! Vector subtraction.
inline const XYZ operator-(const XYZ& a,const XYZ& b)
{
  return XYZ(a.x()-b.x(),a.y()-b.y(),a.z()-b.z());
}

//! Multiplication by scalar.
inline const XYZ operator*(real k,const XYZ& v)
{  
  XYZ ret(v);
  ret*=k;
  return ret;
}

//! Multiplication by scalar.
inline const XYZ operator*(const XYZ& v,real k)
{
  XYZ ret(v);
  ret*=k;
  return ret;
}

//! Division by scalar.
inline const XYZ operator/(const XYZ& v,real k)
{
  return v*(1.0/k);
}

/*! If magnitude is zero we return zero vector.
 */
inline const XYZ XYZ::normalised() const
{
  const real m=magnitude();
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
  RandomXYZInSphere(Random01& rng,real radius);
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
  RandomXYZInXYDisc(Random01& rng,real radius);
};

#endif





