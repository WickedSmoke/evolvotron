// Source file for evolvotron
// Copyright (C) 2007 Tim Day
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
  \brief Interface for class XY.
*/

#ifndef _xy_h_
#define _xy_h_

//! Class to hold vectors in 2D cartesian co-ordinates.
/*! Direct access to the x,y members is not permitted.
 */
class XY
{
 protected:
  real rep[2];

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

  void x(real v)
    {
      rep[0]=v;
    }
  void y(real v)
    {
      rep[1]=v;
    }
  //@}

  //! Null constructor.
  /*! NB The components are not cleared to zero. 
   */
  XY()
    {}

  //! Copy constructor.
  XY(const XY& v)
    {
      rep[0]=v.rep[0];
      rep[1]=v.rep[1];
    }
  
  //! Initialise from separate components.
  XY(real vx,real vy)
    {
      rep[0]=vx;
      rep[1]=vy;
    }

  //! Trivial destructor.
  ~XY()
    {}

  //! Subtract a vector
  void operator-=(const XY& v)
    {
      rep[0]-=v.rep[0];
      rep[1]-=v.rep[1];
    }

  //! Add a vector
  void operator+=(const XY& v)
    {
      rep[0]+=v.rep[0];
      rep[1]+=v.rep[1];
    }

  //! Multiply by scalar
  void operator*=(real k)
    {
      rep[0]*=k;
      rep[1]*=k;
    }

  //! Divide by scalar.
  /*! Implemented assuming one divide and two multiplies is faster than two divides.
   */
  void operator/=(real k)
    {
      const real ik(1.0/k);
      (*this)*=ik;
    }

  //! Assignment. 
  void assign(const XY& v)
    {
      x(v.x());
      y(v.y());
    }

  //! Negation.
  const XY operator-() const
    {
      return XY(-x(),-y());
    }

  //! Return the square of the magnitude.
  real magnitude2() const
    {
      return x()*x()+y()*y();
    }

  //! Return the magnitude.
  real magnitude() const
    {
      return sqrt(magnitude2());
    }

  //! Returns sum of x and y components.
  real sum_of_components() const
    {
      return x()+y();
    }

  //! Return the vector normalised.
  const XY normalised() const;

  //! Normalise this vector.
  void normalise();

  //! Returns true if an origin centred rectangle with this vectors' semi-axes contains the argument.
  bool origin_centred_rect_contains(const XY& p) const
    {
      return (-x()<=p.x() && p.x()<=x() && -y()<=p.y() && p.y()<=y()); 
    }

  //! Write the vector.
  std::ostream& write(std::ostream&) const;

  //! Helper for common case of creating an instance filled with a common value.
  static const XY fill(real v)
    {
      return XY(v,v);
    }

};

//! Dot product.
/*! Perhaps a curious choice of operator but it works for me.
 */
inline real operator%(const XY& a,const XY& b)
{
  return a.x()*b.x()+a.y()*b.y();
} 

//! Vector addition.
inline const XY operator+(const XY& a,const XY& b)
{
  return XY(a.x()+b.x(),a.y()+b.y());
}

//! Vector subtraction.
inline const XY operator-(const XY& a,const XY& b)
{
  return XY(a.x()-b.x(),a.y()-b.y());
}

//! Multiplication by scalar.
inline const XY operator*(real k,const XY& v)
{  
  XY ret(v);
  ret*=k;
  return ret;
}

//! Multiplication by scalar.
inline const XY operator*(const XY& v,real k)
{
  XY ret(v);
  ret*=k;
  return ret;
}

//! Division by scalar.
inline const XY operator/(const XY& v,real k)
{
  return v*(1.0/k);
}

/*! If magnitude is zero we return zero vector.
 */
inline const XY XY::normalised() const
{
  const real m=magnitude();
  return (m==0.0 ? XY(0.0,0.0) : (*this)/m);
}

inline void XY::normalise()
{
  (*this)=normalised();
}

//! Stream output operator.
/*! Calls write().
 */
inline std::ostream& operator<<(std::ostream& out,const XY& v)
{
  return v.write(out);
}

#endif
