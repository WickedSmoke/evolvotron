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
  \brief Interfaces for class Random and derived classes.
*/

#ifndef _random_h_
#define _random_h_

#include <boost/random.hpp>
#include "useful.h"

//! Generates random numbers in the range [0,1).
class Random01
{
public:
  //! Constructor
  Random01(uint seed) : _rng(seed), _dist(0.0,1.0), _gen(_rng,_dist) {}

  //! Return next number in sequence.
  double operator()() { return _gen(); }

private:

  //! Base generator
  boost::mt19937 _rng;

  //! Distribution
  boost::uniform_real<> _dist;

  //! Actual generator
  boost::variate_generator<boost::mt19937,boost::uniform_real<> > _gen;
};

//! Return negative-exponentially distributed random numbers.
class RandomNegExp
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

  //! Return next number in sequence.
  /*! \warning Returns double instead of real because suspect NegExp can return Inf otherwise.
   */
  double operator()()
    {
      return -_mean*log(1.0-_generator());
    }  
};

template <typename T> void random_shuffle(boost::ptr_vector<T>& v,Random01& r01)
{
  boost::ptr_vector<T> nv;
  while (!v.empty())
    {
      const uint n=static_cast<uint>(r01()*v.size());
      nv.transfer(nv.end(),v.begin()+n,v);
    }
  v.transfer(v.end(),nv.begin(),nv.end(),nv);
}

template <typename T> void random_shuffle(std::vector<T>& v,Random01& r01)
{
  size_t a, b;
  for (auto i = v.size(); i > 1; --i) {
    a = i - 1;
    b = static_cast<size_t>(r01() * i);
    if (a != b)
      std::swap(v[a], v[b]);
  }
}

#endif
