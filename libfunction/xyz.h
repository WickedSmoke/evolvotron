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
  \brief Interface for class XYZ.
*/

#ifndef _xyz_h_
#define _xyz_h_

#include "common.h"

#include "xy.h"

class Random01;

//! Class to hold vectors in 3D cartesian co-ordinates.
/*! Direct access to the x,y,z members is not permitted.
 */
class XYZ
{
 protected:
  boost::array<real,3> _rep;

 public:

  //@{
  //! Accessor.
  real x() const
    {
      return _rep[0];
    }
  real y() const
    {
      return _rep[1];
    }
  real z() const
    {
      return _rep[2];
    }
  const XY xy() const
    {
      return XY(x(),y());
    }

  void x(real v)
    {
      _rep[0]=v;
    }
  void y(real v)
    {
      _rep[1]=v;
    }
  void z(real v)
    {
      _rep[2]=v;
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
      _rep[0]=v._rep[0];
      _rep[1]=v._rep[1];
      _rep[2]=v._rep[2];
    }

  //! Initialise from an XY and a z component.
  XYZ(const XY& p,real vz)
    {
      _rep[0]=p.x();
      _rep[1]=p.y();
      _rep[2]=vz;
    }
  
  //! Initialise from separate components.
  XYZ(real vx,real vy,real vz)
    {
      _rep[0]=vx;
      _rep[1]=vy;
      _rep[2]=vz;
    }

  //! Trivial destructor.
  ~XYZ()
    {}

  //! Subtract a vector
  void operator-=(const XYZ& v)
    {
      _rep[0]-=v._rep[0];
      _rep[1]-=v._rep[1];
      _rep[2]-=v._rep[2];
    }

  //! Add a vector
  void operator+=(const XYZ& v)
    {
      _rep[0]+=v._rep[0];
      _rep[1]+=v._rep[1];
      _rep[2]+=v._rep[2];
    }

  //! Multiply by scalar
  void operator*=(real k)
    {
      _rep[0]*=k;
      _rep[1]*=k;
      _rep[2]*=k;
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
  real magnitude2() const
    {
      return x()*x()+y()*y()+z()*z();
    }

  //! Return the magnitude.
  real magnitude() const
    {
      return sqrt(magnitude2());
    }

  //! Returns sum of x, y and z components.
  real sum_of_components() const
    {
      return x()+y()+z();
    }

  //! Return the vector normalised.
  const XYZ normalised() const;

  //! Normalise this vector.
  void normalise();

  //! Returns true if an origin centred cuboid with this vectors semi-axes contains the argument.
  bool origin_centred_rect_contains(const XYZ& p) const
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
inline real operator%(const XYZ& a,const XYZ& b)
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

//! Modulus all components by 1.0
inline const XYZ modulusf(const XYZ& p)
{
  return XYZ
    (
     modulusf(p.x(),1.0),
     modulusf(p.y(),1.0),
     modulusf(p.z(),1.0)
     );
}

//! Componentwise modulus
inline const XYZ modulusf(const XYZ& p,const XYZ& q)
{
  return XYZ
    (
     modulusf(p.x(),q.x()),
     modulusf(p.y(),q.y()),
     modulusf(p.z(),q.z())
     );
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
