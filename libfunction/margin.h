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
  \brief Interface for class Margin
*/

#ifndef _margin_h_
#define _margin_h_

#include "common.h"

//! Utility class for outputting the margin for a given level of indentation.
class Margin
{
  //! Levels of indentation
  const uint _n;
 public:

  //! Constructor.
  Margin(uint n)
    :_n(n)
    {}

  //! Output some spaces for each level of indentation.
  std::ostream& write(std::ostream& out) const;
};

//! Make Margin usable using << operator.
inline std::ostream& operator<<(std::ostream& out,const Margin& m)
{
  return m.write(out);
}

#endif

