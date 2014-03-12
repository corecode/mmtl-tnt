#ifndef __matrix_h__
#define __matrix_h__

#include <stdlib.h>
#include <math.h>
#include "vector.h"

class Matrix
{
  friend class Matrix3d;
  friend ostream& operator<<(ostream&, const Matrix&);
  friend istream& operator>>(istream&, Matrix&);
  friend Matrix operator*(double, const Matrix&);
  friend double max(const Matrix&);
  friend double min(const Matrix&);
  friend double norm(const Matrix&);
  friend double norm1(const Matrix&);
  friend double norm2(const Matrix&);
  friend double norm2_sqv(const Matrix&);
  friend void   del(Matrix&);
 
public:

  Matrix(int n=0, int m=0, double = 0.0);
  Matrix(const Matrix&);
  Matrix(const Vector&);

  Matrix& operator=(const Matrix&);

  ~Matrix();

  void free();

  int  dim_i()  const  {  return d_i; }
  int  dim_j()  const  {  return d_j; }

  Vector& row(int) const;
  Vector  col(int) const;
  Vector  diag() const;
  Matrix  trans() const;
  Matrix  abs() const;
  Matrix& resize(int,int);
  void swap_rows(int,int);  

  operator Vector() const;

  int  operator==(const Matrix&)  const;
  int  operator!=(const Matrix& x)  const { return !(*this == x); }

  Vector& operator[](int i)       const { return row(i); }
  double& operator()(int, int);
  double& operator()(int, int) const;

  Matrix  operator+(const Matrix&);
  Matrix  operator-(const Matrix&);
  Matrix  operator-();
  Matrix& operator+=(const Matrix&);
  Matrix& operator-=(const Matrix&);
  Matrix& operator/=(double);
  Matrix& operator*=(double);
  Matrix  operator*(double);
  Matrix  operator*(double) const;
  Matrix  operator*(const Matrix&);
  Vector  operator*(const Vector& v)    { return Vector(*this * Matrix(v)); }
  Matrix  operator/(double);
  Matrix  operator^(const Matrix&) const;

protected:
  double&  elem(int i, int j) const     { return v[i]->v[j]; }

private:
  Vector** v;
  int  d_i;
  int  d_j;
  void     flip_rows(int,int);
  void     check_dimensions(const Matrix&) const;
};


inline Matrix& Matrix::operator=(const Matrix& mat) {

  int i, j;
  if (d_i != mat.d_i || d_j != mat.d_j) {
    for(i = 0; i < d_i; i++) delete v[i];
    delete v;
    d_i = mat.d_i;
    d_j = mat.d_j;
    v = new Vector*[d_i];
    for(i = 0; i < d_i; i++) v[i] = new Vector(d_j);
  }
  for(i = 0; i < d_i; i++)
    for(j = 0; j < d_j; j++) elem(i,j) = mat.elem(i,j);
  return (*this);
}

inline int Matrix::operator==(const Matrix& x) const {
 
  int i, j;
  if (d_i != x.d_i || d_j != x.d_j) return (0);

  for(i = 0; i < d_i; i++)
    for(j = 0; j < d_j; j++)
      if (elem(i,j) != x.elem(i,j)) return (0);
  return (1);
}

inline Vector& Matrix::row(int i) const {

  if (i < 0 || i >= d_i)  {
    cerr << "Matrix: row index out of range" << endl;
    exit(1);
  }
  return (*v[i]);
}

inline double& Matrix::operator()(int i, int j) {

  if (i < 0 || i >= d_i) {
    cerr << "Matrix: row index out of range" << endl;
    exit(1);
  }
  if (j < 0 || j >= d_j) {
    cerr << "Matrix: col index out of range" << endl;
    exit(1);
  }
  return (elem(i,j));
}

inline double& Matrix::operator()(int i, int j) const {

  if (i < 0 || i >= d_i) {
    cerr << "Matrix: row index out of range" << endl;
    exit(1);
  }
  if (j < 0 || j >= d_j) {
    cerr << "Matrix: col index out of range" << endl;
    exit(1);
  }
  return (elem(i,j));
}

inline Vector Matrix::col(int i)  const {

  if (i < 0 || i >= d_j) {
    cerr << "Matrix: col index out of range" << endl;
    exit(1);
  }
  Vector result(d_i);
  int j = d_i;
  while (j--) result.v[j] = elem(j,i);
  return (result);
}

inline Vector Matrix::diag()  const {

  if (d_i != d_j) {
    cerr << "Matrix: diag defined only if d_i = d_j" << endl;
    exit(1);
  }
  Vector result(d_i);
  int j = d_i;
  while (j--) result.v[j] = elem(j,j);
  return (result);
}

inline Matrix::operator Vector() const {

  if (d_j != 1) {
    cerr << "error: cannot make vector from matrix" << endl;
    exit(1);
  }
  return (col(0));
}

inline Matrix Matrix::operator+(const Matrix& mat) {

  int i, j;
  check_dimensions(mat);
  Matrix result(d_i,d_j);
  for(i = 0; i < d_i; i++)
    for(j = 0; j < d_j; j++)
      result.elem(i,j) = elem(i,j) + mat.elem(i,j);
  return (result);
}

inline Matrix Matrix::operator-(const Matrix& mat) {

  int i, j;
  check_dimensions(mat);
  Matrix result(d_i,d_j);
  for(i = 0; i < d_i; i++)
    for(j = 0; j < d_j; j++)
      result.elem(i,j) = elem(i,j) - mat.elem(i,j);
  return (result);
}

inline Matrix Matrix::operator-( ) {

  int i, j;
  Matrix result(d_i,d_j);
  for(i = 0; i < d_i; i++)
    for(j= 0; j < d_j; j++)
      result.elem(i,j) = -elem(i,j);
  return (result);
}

inline Matrix& Matrix::operator+=(const Matrix& mat) {

  int i, j;
  check_dimensions(mat);
  for(i = 0; i < d_i; i++)
    for(j = 0; j < d_j; j++)
      elem(i,j) += mat.elem(i,j);
  return (*this);
}

inline Matrix& Matrix::operator-=(const Matrix& mat) {

  int i, j;
  check_dimensions(mat);
  for(i = 0; i < d_i; i++)
    for(j = 0; j < d_j; j++)
      elem(i,j) -= mat.elem(i,j);
  return (*this);
}

inline Matrix& Matrix::operator/=(double x) {

  int i, j;
  if (x == 0) {
    cerr << "Matrix/=: divided by zero" << endl;
    exit(1);
  }
  for(i = 0; i < d_i; i++)
    for(j = 0; j < d_j; j++)
      elem(i,j) /= x;
  return (*this);
}

inline Matrix& Matrix::operator*=(double x) {

  int i, j;
  for(i = 0; i < d_i; i++)
    for(j = 0; j < d_j; j++)
      elem(i,j) *= x;
  return (*this);
}

inline Matrix Matrix::operator*(double f) {

  int i, j;
  Matrix result(d_i,d_j);
  for(i = 0; i < d_i; i++)
    for(j = 0; j < d_j; j++)
      result.elem(i,j) = elem(i,j) * f;
  return (result);
}

inline Matrix Matrix::operator*(double f) const {

  int i, j;
  Matrix result(d_i,d_j);
  for(i = 0; i < d_i; i++)
    for(j = 0; j < d_j; j++)
      result.elem(i,j) = elem(i,j) * f;
  return (result);
}

inline Matrix Matrix::operator*(const Matrix& mat) {

  if (d_j != mat.d_i) {
    cerr << "matrix multiplication: incompatible matrix types" << endl;
    exit(1);
  }
  Matrix result(d_i, mat.d_j);
  int i,j;
  for (i = 0; i < mat.d_j; i++)
    for (j = 0; j < d_i; j++) result.elem(j,i) = *v[j] * mat.col(i);
 return (result);
}

inline Matrix Matrix::operator/(double a) {

  if(a == 0) {
    cerr << "Matrix: divided by zero" << endl;
    exit(1);
  }
  a = 1. / a;
  
  return (Matrix(*this *a ));
}

inline Matrix Matrix::operator^(const Matrix& mat) const {

  if (d_i != mat.d_i || d_j != mat.d_j) {
    cerr << "Matrix=: matrixes have different sizes" << endl;
    exit(1);
  }
  Matrix res(d_i,d_j); 
  for(int i = 0; i < d_i; i++)
     for(int j = 0; j < d_j; j++)
        res.elem(i,j) = elem(i,j)*mat.elem(i,j);
  return (res);
}

inline Matrix Matrix::abs() const {

  Matrix result(d_i,d_j);
  for(int i = 0; i < d_i; i++)
    for(int j = 0; j < d_j; j++)
      result(i,j) = fabs(elem(i,j));
  return (result);
}


inline void Print(const Matrix& m, ostream& out=cout) { out << m; }
inline void Read(Matrix& m, istream& in=cin)          { in >> m;  }

#endif
