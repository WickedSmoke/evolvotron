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
  \brief Interfaces for class Random and derived classes.
*/

#ifndef _random_h_
#define _random_h_

#include "useful.h"

//! Abstract base class for random number generation
class Random
{
protected:
public:
  
  //! Constructor (nothing to do in base class)
  Random()
    {}

  //! Trivial destructor.
  virtual ~Random()
    {}
  
  //! Return a random number.
  /*! \warning Returns double instead of real because suspect NegExp can return Inf otherwise.
   */
  virtual const double operator()()
    =0;
};

//! Generates random numbers in the range [0,1).
/*! Code lifted from 1999/10/28 version of MT19937 (Mersenne Twister)
  at http://www.math.keio.ac.jp/~nisimura/random/int/mt19937int.c
  (which is LGPL-ed code)
 */
class Random01 : public Random   
{
protected:
  static const unsigned long N=624;
  static const unsigned long M=397;

  //! Constant vector a
  static const unsigned long MATRIX_A=0x9908b0df;   

  //! Most significant w-r bits
  static const unsigned long UPPER_MASK=0x80000000; 

  //! Least significant r bits
  static const unsigned long LOWER_MASK=0x7fffffff; 

  static const unsigned long TEMPERING_MASK_B=0x9d2c5680;
  static const unsigned long TEMPERING_MASK_C=0xefc60000;

  static const unsigned long TEMPERING_SHIFT_U(unsigned long y) {return (y >> 11);}
  static const unsigned long TEMPERING_SHIFT_S(unsigned long y) {return (y << 7);}
  static const unsigned long TEMPERING_SHIFT_T(unsigned long y) {return (y << 15);}
  static const unsigned long TEMPERING_SHIFT_L(unsigned long y) {return (y >> 18);}

  //! the array for the state vector  */
  unsigned long mt[N]; 

  //! mti==N+1 means mt[N] is not initialized 
  uint mti; 

public:
  //! Constructor
  Random01(uint seed);

  //! Trivial destructor
  virtual ~Random01();
  
  //! Return next number in sequence.
  virtual const double operator()();
};

//! Return negative-exponentially distributed random numbers.
class RandomNegExp : public Random
{
protected:

  //! Underlying generator.
  Random01 _generator;

  //! Mean value of distribution.
  double _mean;

public:
  
  //! Construct generator of numbers with mean value m.
  RandomNegExp(uint seed,double m)
    :_generator(seed)
    ,_mean(m)
    {}

  //! Trivial destructor.
  virtual ~RandomNegExp()
    {}
  
  //! Return next number in sequence.
  virtual const double operator()()
    {
      return -_mean*log(1.0-_generator());
    }  
};

#endif






