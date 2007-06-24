// Source file for evolvotron
// Copyright (C) 2002,2003,2007 Tim Day
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

#include <boost/ptr_container/ptr_map.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/shared_array.hpp>
#include <boost/shared_ptr.hpp>
#include <cassert>
#include <cmath>
#include <cstdlib>
#include <iosfwd>
#include <map>
#include <memory>
#include <string>
#include <sstream>

//! Convenience typedef.
typedef unsigned int uint;

//! Convenience typedef.
typedef unsigned short ushort;

//! Convenience typedef.
typedef unsigned char uchar;

//! float turns out to not be sufficient for some noise-amplifying ops (e.g gradient type things) so migrate to double.
/*! Unfortunately probably kills any potential speed-ups from pre-SSE2 hardware, but it didn't provide much benefit anyway.
 */
typedef double real;

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

//! Used when some small sampling step is required (e.g gradient operators).
#define epsilon (1e-6)
#define epsilon2 (2.0*epsilon)
#define inv_epsilon (1.0/epsilon)
#define inv_epsilon2 (1.0/epsilon2)
#define big_epsilon (sqrt(epsilon))

//! Sane modulus function always returning a number in the range [0,y)
inline real modulusf(real x,real y)
{
  if (y<0.0) y=-y;
  real r=fmod(x,y);
  if (r<0.0) r+=y;
  return r;
}

//! Sane modulus function always returning a number in the range [0,y-1]
inline uint modulusi(int x,int y)
{
  if (y<0) y=-y;
  int r=x%y;
  if (r<0) r+=y;
  assert(r>=0);
  return r;
}

//! Triangle function: like modulus, but ramps down instead of discontinuity at y
/*! Setting y=1 ensures x in [0,1]
 */
inline real trianglef(real x,real y)
{
  if (y<0.0) y=-y;
  if (x<0.0) x=-x;
  real r=fmod(x,2.0*y);
  if (r>y) r=2.0*y-r;
  return r;
}

//! Use this to divert clog to supress verbose logging.  Needs longer life than scope of main().
extern std::ofstream sink_ostream;

#ifndef NDEBUG 

//! Inherit from this class (in debug only) and then assert(InstanceCounted::is_clean) at progam exit to check all instances are gone.
class InstanceCounted
{
 public:
  InstanceCounted(const std::string& what,bool verbose);
  virtual ~InstanceCounted();

  static const bool is_clear();

 private:
  const std::string _what;
  const bool _verbose;

  static std::auto_ptr<std::map<std::string,uint> > _instance_counts;
};


#endif

#endif