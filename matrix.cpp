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
  \brief Implementation for class Matrix.

  No Matrix:: code because of usual template instantiation problem.
*/

#include "matrix.h"
#include "random.h"

#include <iostream>

void testmatrix_extract_runtime(Matrix<2,2,float>& m_out,const Matrix<3,3,float>& m_in)
{
  m_in.extract_minor(1,1,m_out);
}

void testmatrix_extract_compiletime(Matrix<2,2,float>& m_out,const Matrix<3,3,float>& m_in)
{
  m_in.extract_minor<1,1>(m_out);
}

float testmatrix_across(const Matrix<3,3,float>& m_in)
{
  return MatrixHelperSumCofactorDeterminantProducts<2,3,3>::execute(m_in);
}

void testmatrix_inverse(Matrix<3,3,float>& m_out,const Matrix<3,3,float>& m_in)
{
  m_out=m_in.inverted();
}

float testmatrix_determinant(const Matrix<3,3,float>& m_in)
{
  return m_in.determinant();
}

void testmatrix()
{
  std::cout << "Testing matrix class:\n";
  Random01 r01(time(0));

  for (uint i=0;i<3;i++)
    {
      Matrix<3,3,float> m0;

      for (uint r=0;r<m0.rows();r++)
	for (uint c=0;c<m0.cols();c++)
	  {
	    if (i==0) 
	      m0[r][c]=(r==c);
	    else if (i==1)
	      m0[r][c]=2*(r==c);
	    else
	      m0[r][c]=2.0*r01()-1.0;
	  }

      m0.write(std::cout);

      std::cout << "\n";

      std::cout << m0.determinant();
      
      std::cout << "\n";

      Matrix<3,3,float> m1(m0.inverted());

      testmatrix_inverse(m1,m0);
  
      m1.write(std::cout);

      std::cout << "\n";

      Matrix<3,3,float> m2=m0*m1;
      
      m2.write(std::cout);

      std::cout << "-----\n";
    }
}

