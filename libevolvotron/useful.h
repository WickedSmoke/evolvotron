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
  \brief File containing all the author's favourite little helpers.
*/

#ifndef _useful_h_
#define _useful_h_

extern "C"
{
#include <stdlib.h>
#include <assert.h>
}

#include <math.h>
#include <iosfwd>
#include <string>
#include <sstream>

//! Convenience typedef.
typedef unsigned int uint;

//! Convenience typedef.
typedef unsigned short ushort;

//! Convenience typedef.
typedef unsigned char uchar;

//! Maximum of 2 arguments.
template <class T> inline const T maximum(T a,T b) {return (a>b ? a : b);}

//! Maximum of 3 arguments.
template <class T> inline const T maximum(T a,T b,T c) {return maximum(a,maximum(b,c));}

//! Maximum of 4 arguments.
template <class T> inline const T maximum(T a,T b,T c,T d) {return maximum(maximum(a,b),maximum(c,d));}

//! Minimum of 2 arguments
template <class T> inline const T minimum(T a,T b) {return (a<b ? a : b);}

//! Minimum of 3 arguments
template <class T> inline const T minimum(T a,T b,T c) {return minimum(a,minimum(b,c));}

//! Minimum of 4 arguments
template <class T> inline const T minimum(T a,T b,T c,T d) {return minimum(minimum(a,b),minimum(c,d));}

//! Returns argument multiplied by itself.
template <class T> inline const T sqr(T a) {return a*a;}

//! Returns result of clamping first argument to range specified by second and third.
template <class T> inline const T clamped(T v,T lo,T hi) {return (v<lo ? lo : (v>hi ? hi : v));}

//! Clamps v to lie between lo and hi
template <class T> inline void clamp(T& v,T lo,T hi) {v=(v<lo ? lo : (v>hi ? hi : v));}

//! Exchanges values of a and b.
template <class T> inline void exchange(T& a,T& b) {const T x(a);a=b;b=x;}

//! Print fatal error message and exit.
extern void fatal_error(const char*);

//! Print fatal error message and exit.
inline void fatal_error(const std::string& s)
{
  fatal_error(s.c_str());
}

//! Call this for fatal internal error type messages.
extern void fatal_internal_error(const char* src_file,uint src_line);

//! Called if constraint macro fails.
extern void constraint_violation(const char* test,const char* src_file,uint src_line);

//! Use this to provide assert-like behaviour which is never disabled.
#define constraint(TEST) {if (!TEST) {constraint_violation(#TEST,__FILE__,__LINE__);}}

template <typename T> class NiftyCounter
{
 private:
  //! \warning Derived classes must explicitly declare nulled instance of this in appropriate .cpp
  static uint _count;
 public:
  NiftyCounter()
    {
      if (_count++==0) T::singleton_create();
    }
  ~NiftyCounter()
    {
      if (--_count==0) T::singleton_destroy();
    }
};

template <typename T> class Singleton
{
 public:
  static void singleton_create()
    {
      if (!_singleton_instance) _singleton_instance=new T();
    }
  static void singleton_destroy()
    {
      delete _singleton_instance;
      _singleton_instance=0;
    }
  static T*const get()
    {
      if (!_singleton_instance) singleton_create(); 
      return _singleton_instance;
    }
    
 protected:
  Singleton()
    {}

  //! \warning Derived classes must explicitly declare nulled instance of this in appropriate .cpp
  static T* _singleton_instance;  
};

#endif


