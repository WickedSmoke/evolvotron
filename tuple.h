// Source file for evolvotron
// Copyright (C) 2003 Tim Day
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
  \brief Interface for class Tuple.
*/

#ifndef _tuple_h_
#define _tuple_h_

#include "useful.h"

//! Class to hold a fixed size tuple of elements
template <uint N,class T> class Tuple
{
 protected:

  //! The elements.
  T _element[N];
    
 public:
  
  //! Null constructor.
  Tuple()
    {}
  
  //! Copy constructor.
  Tuple<N,T>(const Tuple<N,T>& t)
    {
      for (uint i=0;i<N;i++)
	_element[i]=t[i];
    }

  //! Destructor.
  ~Tuple()
    {}

  //! Accessor.
  T& operator[](uint i)
    {
      assert(i<N);
      return _element[i];
    }

  //! Constant accessor.
  const T& operator[](uint i) const
    {
      assert(i<N);
      return _element[i];
    }

  //! Elementwise addition operator.
  void operator+=(const Tuple<N,T>& t)
    {
      for (uint i=0;i<N;i++)
	_element[i]+=t[i];
    }

  //! Elementwise subtraction operator.
  void operator-=(const Tuple<N,T>& t)
    {
      for (uint i=0;i<N;i++)
	_element[i]+=t[i];
    }

  //! Multiplication by scalar.
  void operator*=(const T& k)
    {
      for (uint i=0;i<N;i++)
	_element[i]*=k;
    }
};

//! Equality operator.
template <uint N,class T> inline const bool operator==(const Tuple<N,T>& a,const Tuple<N,T>& b)
{
  for (uint i=0;i<N;i++)
    {
      if (a[i]!=b[i]) return false;
    }
  return true;
}

//! Inequality operator.
template <uint N,class T> inline const bool operator!=(const Tuple<N,T>& a,const Tuple<N,T>& b)
{
  return !(a==b);
}

//! Element-wise addition.
template <uint N,class T> inline const Tuple<N,T> operator+(const Tuple<N,T>& a,const Tuple<N,T>& b)
{
  Tuple<N,T> r(a);
  r+=b;
  return r;
}

//! Element-wise subtraction.
template <uint N,class T> inline const Tuple<N,T> operator-(const Tuple<N,T>& a,const Tuple<N,T>& b)
{
  Tuple<N,T> r(a);
  r-=b;
  return r;
}

#endif

