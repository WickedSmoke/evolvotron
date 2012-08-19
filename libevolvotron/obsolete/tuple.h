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
  \brief Interface for class Tuple.
*/

#ifndef _tuple_h_
#define _tuple_h_

#include "useful.h"

//! Class to hold a fixed size tuple of elements
/*! Maybe Array would have been a better name.
  NB No destructor defined because gcc generates less efficient code for matrices.
 */
template <uint N,class T> class Tuple
{
 protected:

  //! The elements.
  T _element[N];
    
 public:
  
  //! Null constructor.
  Tuple<N,T>()
    {}
  
  //! Copy constructor.
  Tuple<N,T>(const Tuple<N,T>& t)
    {
      for (uint i=0;i<N;i++)
	_element[i]=t[i];
    }

  ////! Destructor.
  //~Tuple<N,T>()
  //  {}

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
	_element[i]-=t[i];
    }

  //! Multiplication by scalar.
  void operator*=(const T& k)
    {
      for (uint i=0;i<N;i++)
	_element[i]*=k;
    }

  //! Write method (delimits with curly braces, elements are whitespace separated).
  std::ostream& write(std::ostream& out) const
    {
      out << "{";
      for (uint i=0;i<N;i++)
	{
	  out<< _element[i] << " ";
	}
      out << "}";
      return out;
    }

  //! Helper for common case of creating an instance filled with a common value.
  static const Tuple<N,T> fill(T v)
    {
      Tuple<N,T> ret;
      for (uint i=0;i<N;i++)
	ret[i]=v;
      return ret;
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

//! Test whether this will expand to sensible straight-through code.
/*! Call TupleHelperCopyEliminate<N-1,SKIP,N,T>::compute(Tuple<N,T>& m_to,const Tuple<N+1,T>& m_from);
  This _does_ seem to generate a nice straight-through run of move instructions. 
 */
template<uint I,uint SKIP,uint N,class T> class TupleHelperCopyEliminate
{
 public:
  static void execute(Tuple<N,T>& m_to,const Tuple<N+1,T>& m_from)
    {
      m_to[I]=m_from[(I>=SKIP ? I+1 : I)];
      TupleHelperCopyEliminate<I-1,SKIP,N,T>::execute(m_to,m_from);
    }
};

//! Specialisation to end recursion
template<uint SKIP,uint N,class T> class TupleHelperCopyEliminate<0,SKIP,N,T>
{
 public:
  static void execute(Tuple<N,T>& m_to,const Tuple<N+1,T>& m_from)
    {
      m_to[0]=m_from[(SKIP==0 ? 1 : 0)];
    }
};

//! Development of class TupleHelperCopyEliminate to provide elimination of a row and column.
/*! Used by matrix class to generate minor matrices efficiently
 */
template<uint I,uint SKIP_R,uint SKIP_C,uint N_R,uint N_C,class T> class TupleHelperDoubleCopyEliminate
{
 public:
  static void execute(Tuple<N_R,Tuple<N_C,T> >& m_to,const Tuple<N_R+1,Tuple<N_C+1,T> >& m_from)
    {
      TupleHelperCopyEliminate<N_R-1,SKIP_C,N_C,T>::execute(m_to[I],m_from[(I>=SKIP_R ? I+1 : I)]);
      
      TupleHelperDoubleCopyEliminate<I-1,SKIP_R,SKIP_C,N_R,N_C,T>::execute(m_to,m_from);
    }
};

//! Specialisation to end recursion
template<uint SKIP_R,uint SKIP_C,uint N_R,uint N_C,class T> class TupleHelperDoubleCopyEliminate<0,SKIP_R,SKIP_C,N_R,N_C,T>
{
 public:
  static void execute(Tuple<N_R,Tuple<N_C,T> >& m_to,const Tuple<N_R+1,Tuple<N_C+1,T> >& m_from)
    {
      TupleHelperCopyEliminate<N_R-1,SKIP_C,N_C,T>::execute(m_to[0],m_from[(SKIP_R==0 ? 1 : 0)]);
    }
};

#endif

