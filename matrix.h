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
  \brief Interface for class Matrix.
*/

#ifndef _matrix_h_
#define _matrix_h_

#include "useful.h"
#include "tuple.h"

// Fwd declaration of helper class.
template <uint FC,uint R,uint C,class T> class MatrixHelperSumCofactorDeterminantProducts;


//! Common base for general and specialised cases.
/*! Avoids having to reimplement some functions in Matrix<1,1,T> specialisation.
 */
template <uint R,uint C,class T> class MatrixBase : public Tuple<R,Tuple<C,T> >
{
 public:
  //! Null constructor.
  MatrixBase<R,C,T>()
    {}
  
  //! Copy constructor.
  MatrixBase<R,C,T>(const MatrixBase<R,C,T>& t)
    :Tuple<R,Tuple<C,T> >(t)
    {}

  ////! Destructor.
  //~MatrixBase<R,C,T>()
  //  {}

  uint rows() const
    {
      return R;
    }
  
  uint cols() const
    {
      return C;
    }

  static int cofactor_sign(uint mr,uint mc)
    {
      return ( ((mr+mc)&1) ? -1 : 1);
    }

  static const T cofactor_sign(uint mr,uint mc,const T v)
    {
      return ( ((mr+mc)&1) ? -v : v);
    }

  std::ostream& write(std::ostream& out) const
    {
      for (uint r=0;r<R;r++)
	{
	  (*this)[r].write(out);
	  out << "\n";
	}
      return out;
    }
};

//! Class to hold a fixed size matrix of elements
template <uint R,uint C,class T> class Matrix : public MatrixBase<R,C,T>
{
 protected:


 public:
  
  //! Null constructor.
  Matrix<R,C,T>()
    :MatrixBase<R,C,T>()
    {}
  
  //! Copy constructor.
  Matrix<R,C,T>(const Matrix<R,C,T>& m)
    :MatrixBase<R,C,T>(m)
    {}

  //! Construct minor matrix (from a larger matrix)
  Matrix<R,C,T>(uint mr,uint mc,const Matrix<R+1,C+1,T>& m)
    :MatrixBase<R,C,T>()
    {
      m.extract_minor(mr,mc,*this);
    }

  ////! Destructor.
  //~Matrix<R,C,T>()
  //  {}

  void operator*=(const T& v)
    {
      for (uint r=0;r<R;r++)
	{
	  (*this)[r]*=v;
	}
    }

  Matrix<C,R,T> transposed() const
    {
      Matrix<C,R,T> ret;
      for (uint r=0;r<R;r++)
	{
	  for (uint c=0;c<C;c++)
	    {
	      ret[c][r]=(*this)[r][c];
	    }
	}
      return ret;
    }

  //! Puts the minor matrix into an argument
  /*! Would have preferred to call this just "minor" but it's a macro.
   */
  void extract_minor(uint mr,uint mc,Matrix<R-1,C-1,T>& m) const
    {
      assert(mr<R);
      assert(mc<C);

      uint r;
      uint rm;
      for (r=0,rm=0;r<R;r++)
	{
	  if (r!=mr)
	    {
	      uint c;
	      uint cm;
	      for (c=0,cm=0;c<C;c++)
		{
		  if (c!=mc)
		    {
		      m[rm][cm]=(*this)[r][c];
		      cm++;
		    }
		}
	      rm++;
	    }
	}
    }

  //! Template member for extracting minors when row and column to be eliminated are known at compile time.
  template <uint SKIP_R,uint SKIP_C> void extract_minor(Matrix<R-1,C-1,T>& m) const
    {
      TupleHelperDoubleCopyEliminate<R-2,SKIP_R,SKIP_C,R-1,C-1,T>::execute(m,*this);
    }

  T determinant() const
    {
      /* Old code calls runtime minor generator: not efficient code.
      T ret(0);
      for (uint c=0;c<C;c++)
	{
	  ret+=(*this)[0][c]*cofactor_sign(0,c,Matrix<R-1,C-1,T>(0,c,*this).determinant());
	}
      return ret;
      */

      return MatrixHelperSumCofactorDeterminantProducts<C-1,R,C,T>::execute(*this);
    }

  Matrix<R,C,T> inverted() const
    {
      Matrix<C,R,T> ret;
      for (uint r=0;r<R;r++)
	{
	  for (uint c=0;c<C;c++)
	    {
	      ret[c][r]=cofactor_sign(r,c,Matrix<R-1,C-1,T>(r,c,(*this)).determinant());
	    }
	}
      
      ret*=(T(1.0)/determinant());

      return ret;
    }
};

template <uint AR,uint N,uint BC,class T> inline const Matrix<AR,BC,T> operator*(const Matrix<AR,N,T>& a,const Matrix<N,BC,T>& b)
{
  Matrix<AR,BC,T> ret;
  for (uint r=0;r<AR;r++)
    for (uint c=0;c<BC;c++)
      {
	T t(0);
	for (uint i=0;i<N;i++)
	  {
	    t+=a[r][i]*b[i][c];
	  }
	ret[r][c]=t;
      }
  return ret;
}

//! (Partial) specialisation for 1x1 matrix
/*! NB Has no extract_minor method because doesn't make sense for 1x1 matrix.
 */
template <class T> class Matrix<1,1,T> : public MatrixBase<1,1,T>
{
 protected:


 public:
  
  //! Null constructor.
  Matrix<1,1,T>()
    :MatrixBase<1,1,T>()
    {}
  
  //! Copy constructor.
  Matrix<1,1,T>(const Matrix<1,1,T>& t)
    :MatrixBase<1,1,T>(t)
    {}

  //! Construct minor matrix
  Matrix<1,1,T>(uint mr,uint mc,const Matrix<2,2,T>& m)
    :MatrixBase<R,C,T>()
    {
      m.extract_minor(mr,mc,*this);
    }

  //! Convenient constructor
  Matrix<1,1,T>(T v00)
    :MatrixBase<1,1,T>()
    {
      (*this)[0][0]=v00;
    }

  ////! Destructor.
  //~Matrix<1,1,T>()
  //  {}

  Matrix<1,1,T> transposed() const
    {
      return (*this);
    }

  //NB minor of 1x1 matrix makes no sense.
  //void extract_minor(uint mr,uint mc,Matrix<R-1,C-1,T>& m) const

  T determinant() const
    {
      return (*this)[0][0];
    }

  Matrix<1,1,T> inverted() const
    {
      return Matrix<1,1,T>(T(1.0)/(*this)[0][0]);
    }
};

//! (Partial) specialisation for 2x2 matrix
template <class T> class Matrix<2,2,T> : public MatrixBase<2,2,T>
{
 protected:


 public:
  
  //! Null constructor.
  Matrix<2,2,T>()
    :MatrixBase<2,2,T>()
    {}
  
  //! Copy constructor.
  Matrix<2,2,T>(const Matrix<2,2,T>& t)
    :MatrixBase<2,2,T>(t)
    {}

  //! Construct minor matrix
  Matrix<2,2,T>(uint mr,uint mc,const Matrix<3,3,T>& m)
    :MatrixBase<2,2,T>()
    {
      m.extract_minor(mr,mc,*this);
    }

  //! Convenient constructor
  Matrix<2,2,T>(T v00,T v01,T v10,T v11)
    :MatrixBase<2,2,T>()
    {
      (*this)[0][0]=v00;
      (*this)[0][1]=v01;
      (*this)[1][0]=v10;
      (*this)[1][1]=v11;
    }

  ////! Destructor.
  //~Matrix<1,1,T>()
  //  {}

  Matrix<2,2,T> transposed() const
    {
      return Matrix<2,2,T>((*this)[0][0],(*this)[1][0],(*this)[0][1],(*this)[1][1]);
    }

  void extract_minor(uint mr,uint mc,Matrix<1,1,T>& m) const
    {
      assert(mr==0 || mr==1);
      assert(mc==0 || mc==1);
      m[0][0]=(*this)[1-mr][1-mc];
    }

  //! Template member for extracting minors when row and column to be eliminated are known at compile time.
  template <uint SKIP_R,uint SKIP_C> void extract_minor(Matrix<1,1,T>& m) const
    {
      assert(SKIP_R==0 || SKIP_R==1);
      assert(SKIP_C==0 || SKIP_C==1);
      m[0][0]=(*this)[1-SKIP_R][1-SKIP_C];
    }

  T determinant() const
    {
      return (*this)[0][0]*(*this)[1][1]-(*this)[0][1]*(*this)[1][0];
    }

  Matrix<2,2,T> inverted() const
    {
      Matrix<2,2,T> ret((*this)[1][1],-(*this)[0][1],-(*this)[1][0],(*this)[0][0]);
      ret*=(T(1.0)/determinant());
      return ret;
    }
};

template <uint FC,uint R,uint C,class T> class MatrixHelperSumCofactorDeterminantProducts
{
 public:
  static T execute(const Matrix<R,C,T>& m)
    {
      Matrix<R-1,C-1,T> minor_matrix;

      // Would prefer to use
      //m.extract_minor<0,FC>(minor_matrix);
      // but compiler doesn't seem to like it (problem with partial specialisation?)
      TupleHelperDoubleCopyEliminate<R-2,0,FC,R-1,C-1,T>::execute(minor_matrix,m);

      return 
	m[0][FC]*((FC&1) ? -1.0f : 1.0f)*minor_matrix.determinant()
	+
	MatrixHelperSumCofactorDeterminantProducts<FC-1,R,C,T>::execute(m);
	;
    }
};

template <uint R,uint C,class T> class MatrixHelperSumCofactorDeterminantProducts<0,R,C,T>
{
 public:
  static float execute(const Matrix<R,C,float>& m)
    {
      Matrix<R-1,C-1,T> minor_matrix;
      TupleHelperDoubleCopyEliminate<R-2,0,0,R-1,C-1,T>::execute(minor_matrix,m);
      return m[0][0]*minor_matrix.determinant();
    }
};

//! \todo: Add Helper class to do inverse matrix efficiently.

//! Tests basic matrix functionality.
extern void testmatrix();

#endif

