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
  \brief Interface for class Matrix.
*/

#ifndef _matrix_h_
#define _matrix_h_

#include "useful.h"
#include "tuple.h"

// Fwd declaration of helper class.
template <uint FC,uint R,uint C,class T> class MatrixHelperSumCofactorDeterminantProducts;
template <uint R,uint C,uint ROWS,uint COLS,class T> class MatrixHelperInvert;


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

      // Better version uses metaprogramming to expand to straight-line code.
      return MatrixHelperSumCofactorDeterminantProducts<C-1,R,C,T>::execute(*this);
    }

  Matrix<R,C,T> inverted() const
    {
      Matrix<C,R,T> ret;

      /* Old code calls runtime minor generator: not efficient code.
      for (uint r=0;r<R;r++)
	{
	  for (uint c=0;c<C;c++)
	    {
	      ret[c][r]=cofactor_sign(r,c,Matrix<R-1,C-1,T>(r,c,(*this)).determinant());
	    }
	}
      */

      // Better version uses metaprogramming to expand to straight-line code.
      MatrixHelperInvert<R-1,C-1,R,C,T>::execute(ret,(*this));
      
      ret*=(T(1.0)/determinant());

      return ret;
    }
};

//! Matrix multiplication
/*! \todo Check assembler code.  Want loops to unroll - use template recursion ?
 */
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

//! Matrix x tuple multiplication
/*! \todo Check assembler code.  Want loops to unroll - use template recursion ?
 */
template <uint R,uint C,class T> inline const Tuple<R,T> operator*(const Matrix<R,C,T>& m,const Tuple<C,T>& v)
{
  Tuple<R,T> ret;
  for (uint r=0;r<R;r++)
    {
      T t(0);
      for (uint c=0;c<C;c++)
	{
	  t+=m[r][c]*v[c];
	}
      ret[r]=t;
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
  static float execute(const Matrix<R,C,T>& m)
    {
      Matrix<R-1,C-1,T> minor_matrix;
      TupleHelperDoubleCopyEliminate<R-2,0,0,R-1,C-1,T>::execute(minor_matrix,m);
      return m[0][0]*minor_matrix.determinant();
    }
};


template <uint R,uint C,uint ROWS,uint COLS,class T> class MatrixHelperInvert
{
 public:
  static void execute(Matrix<COLS,ROWS,T>& m_out,const Matrix<ROWS,COLS,T>& m_in)
    {
      Matrix<ROWS-1,COLS-1,T> minor_matrix;
      TupleHelperDoubleCopyEliminate<ROWS-2,R,C,ROWS-1,COLS-1,T>::execute(minor_matrix,m_in);
      m_out[C][R]=Matrix<ROWS,COLS,T>::cofactor_sign(R,C,minor_matrix.determinant());
      MatrixHelperInvert<R,C-1,ROWS,COLS,T>::execute(m_out,m_in);
    }
};

template <uint R,uint ROWS,uint COLS,class T> class MatrixHelperInvert<R,0,ROWS,COLS,T>
{
 public:
  static void execute(Matrix<COLS,ROWS,T>& m_out,const Matrix<ROWS,COLS,T>& m_in)
    {
      Matrix<ROWS-1,COLS-1,T> minor_matrix;
      TupleHelperDoubleCopyEliminate<ROWS-2,R,0,ROWS-1,COLS-1,T>::execute(minor_matrix,m_in);
      m_out[0][R]=Matrix<ROWS,COLS,T>::cofactor_sign(R,0,minor_matrix.determinant());
      MatrixHelperInvert<R-1,COLS-1,ROWS,COLS,T>::execute(m_out,m_in);
    }
};

template <uint ROWS,uint COLS,class T> class MatrixHelperInvert<0,0,ROWS,COLS,T>
{
 public:
  static void execute(Matrix<COLS,ROWS,T>& m_out,const Matrix<ROWS,COLS,T>& m_in)
    {
      Matrix<ROWS-1,COLS-1,T> minor_matrix;
      TupleHelperDoubleCopyEliminate<ROWS-2,0,0,ROWS-1,COLS-1,T>::execute(minor_matrix,m_in);
      m_out[0][0]=Matrix<ROWS,COLS,T>::cofactor_sign(0,0,minor_matrix.determinant());
    }
};

//! 3x3 matrix class
class Matrix33 : public Matrix<3,3,float>
{
 public:
  Matrix33()
    {}
  Matrix33(const Matrix33& m)
    :Matrix<3,3,float>(m)
    {}
};

class Matrix33RotateX : public Matrix33
{
 public:
  Matrix33RotateX(float a)
    {
      const float sa=sin(a);
      const float ca=cos(a);
      (*this)[0][0]=1.0f;(*this)[0][1]=0.0f;(*this)[0][2]=0.0f;
      (*this)[1][0]=0.0f;(*this)[1][1]=  ca;(*this)[1][2]= -sa;
      (*this)[2][0]=0.0f;(*this)[2][1]=  sa;(*this)[2][2]=  ca;
    }
};

class Matrix33RotateY : public Matrix33
{
 public:
  Matrix33RotateY(float a)
    {
      const float sa=sin(a);
      const float ca=cos(a);
      (*this)[0][0]=  ca;(*this)[0][1]=0.0f;(*this)[0][2]=  sa;
      (*this)[1][0]= -sa;(*this)[1][1]=1.0f;(*this)[1][2]=  ca;
      (*this)[2][0]=0.0f;(*this)[2][1]=0.0f;(*this)[2][2]=0.0f;
    }
};

class Matrix33RotateZ : public Matrix33
{
 public:
  Matrix33RotateZ(float a)
    {
      const float sa=sin(a);
      const float ca=cos(a);
      (*this)[0][0]=  ca;(*this)[0][1]= -sa;(*this)[0][2]=0.0f;
      (*this)[1][0]=  sa;(*this)[1][1]=  ca;(*this)[1][2]=0.0f;
      (*this)[2][0]=0.0f;(*this)[2][1]=0.0f;(*this)[2][2]=1.0f;
    }
};

//! Tests basic matrix functionality.
extern void testmatrix();

#endif

