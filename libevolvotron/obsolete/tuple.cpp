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
  \brief Implementation for class Tuple.

  No Tuple:: code because of usual template instantiation problem.
*/

#include "tuple.h"

//! Test code
void testtuple_eliminate(Tuple<2,float>& m_to,const Tuple<3,float>& m_from)
{
  // Skip middle element of m_from
  TupleHelperCopyEliminate<1,1,2,float>::execute(m_to,m_from);
}

//! Test code
void testtuple_doubleeliminate(Tuple<2,Tuple<2,float> >& m_to,const Tuple<3,Tuple<3,float> >& m_from)
{
  TupleHelperDoubleCopyEliminate<1,1,1,2,2,float>::execute(m_to,m_from);
}

