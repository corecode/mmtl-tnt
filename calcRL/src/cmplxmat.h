#ifndef _cmplxmat__h_
#define _cmplxmat__h_

#include <stdlib.h>
#include <math.h>
#include "cmplxvec.h"
#include "vector.h"
#include "matrix.h"

class CmplxMatrix
{
  friend class Matrix;
  friend ostream& operator<<(ostream&, const CmplxMatrix&);
  friend istream& operator>>(istream&, CmplxMatrix&);

public:

  CmplxMatrix(int=0, int=0, double=0.0);
  CmplxMatrix(int, int, Complex);
  CmplxMatrix(const CmplxMatrix&);
  CmplxMatrix(const Matrix&);
  CmplxMatrix(const CmplxVector&);
  CmplxMatrix(const Vector&);

  CmplxMatrix& operator=(const CmplxMatrix&);
  CmplxMatrix& operator=(const Matrix&);

  ~CmplxMatrix();

  void free();

int     dim_i()  const  {  return d_i; }
int     dim_j()  const  {  return d_j; }

CmplxVector& row(int) const;
CmplxVector  col(int i) const;
CmplxMatrix  trans() const;
Matrix real() const;
Matrix imag() const;
Matrix  abs() const;
CmplxMatrix conjg() const;
CmplxMatrix& resize(int,int);

operator CmplxVector() const;

int     operator==(const CmplxMatrix&)    const;
int     operator!=(const CmplxMatrix& x)  const { return !(*this == x); }

CmplxVector& operator[](int i)            const { return row(i); }
Complex& operator()(int, int);
Complex& operator()(int, int) const;

CmplxMatrix operator+(const CmplxMatrix&);
CmplxMatrix operator-(const CmplxMatrix&);
CmplxMatrix& operator+=(const CmplxMatrix&);
CmplxMatrix& operator-=(const CmplxMatrix&);
CmplxMatrix operator+(const Matrix&);
CmplxMatrix operator-(const Matrix&);
CmplxMatrix operator-();

CmplxMatrix operator*(double);
CmplxMatrix operator*(const Complex&);
CmplxMatrix operator/(double);
CmplxMatrix operator/(const Complex&);
CmplxMatrix operator*(const CmplxMatrix&);
CmplxMatrix operator*(const Matrix&);
CmplxVector operator*(CmplxVector& w)             { return CmplxVector(*this * CmplxMatrix(w)); }
//CmplxVector operator*(CmplxVector& w)       const { return CmplxVector(*this * CmplxMatrix(w)); }
//CmplxVector operator*(const CmplxVector& w)       { return CmplxVector(*this * CmplxMatrix(w)); }
//CmplxVector operator*(const CmplxVector& w) const { return CmplxVector(*this * CmplxMatrix(w)); }
CmplxVector operator*(Vector& w)                  { return CmplxVector(*this * CmplxMatrix(w)); }
//CmplxVector operator*(Vector& w)            const { return CmplxVector(*this * CmplxMatrix(w)); }
CmplxVector operator*(const Vector& w)            { return CmplxVector(*this * CmplxMatrix(w)); }
//CmplxVector operator*(const Vector& w)      const { return CmplxVector(*this * CmplxMatrix(w)); }


private:
  CmplxVector** v;
  int  d_i;
  int  d_j;

  void     flip_rows(int,int);
  void     check_dimensions(const CmplxMatrix&) const;
  Complex&  elem(int i, int j) const { return v[i]->v[j]; }
};


inline CmplxMatrix& CmplxMatrix::operator=(const CmplxMatrix& mat) {
  
  int i, j;
  if (d_i != mat.d_i || d_j != mat.d_j) {
    for(i = 0; i < d_i; i++) delete v[i];
    delete v;
    d_i = mat.d_i;
    d_j = mat.d_j;
    v = new CmplxVector*[d_i];
    for(i = 0; i < d_i; i++) v[i] = new CmplxVector(d_j);
  }
  for(i = 0; i < d_i; i++)
    for(j = 0; j < d_j; j++) elem(i,j) = mat.elem(i,j);
  return (*this);
}

inline CmplxMatrix& CmplxMatrix::operator=(const Matrix& mat) {
  
  int i, j;
  CmplxMatrix tmp(mat);
  if (d_i != mat.dim_i() || d_j != mat.dim_j()) {
    for(i = 0; i < d_i; i++) delete v[i];
    delete v;
    d_i = mat.dim_i();
    d_j = mat.dim_j();
    v = new CmplxVector*[d_i];
    for(i = 0; i < d_i; i++) v[i] = new CmplxVector(d_j);
  }
  for(i = 0; i < d_i; i++)
    for(j = 0; j < d_j; j++) elem(i,j) = tmp.elem(i,j);
  return (*this);
}

inline int CmplxMatrix::operator==(const CmplxMatrix& x) const {

  int i, j;
  if (d_i != x.d_i || d_j != x.d_j) return (0);
  for(i = 0; i < d_i; i++)
    for(j = 0; j < d_j; j++)
      if (elem(i,j) != x.elem(i,j)) return (0);
  return (1);
}

inline CmplxVector& CmplxMatrix::row(int i) const {

  if (i < 0 || i >= d_i) {  
    cerr << "complex matrix: row index out of range" << endl;
    exit(1);
  }
  return (*v[i]);
}

inline Complex& CmplxMatrix::operator()(int i, int j) {

  if (i < 0 || i >= d_i) { 
    cerr << "complex matrix: row index out of range" << endl;
    exit(1);
  }
  if (j < 0 || j >= d_j) { 
    cerr << "complex matrix: col index out of range" << endl;
    exit(1);
  }
  return (elem(i,j));
}

inline Complex& CmplxMatrix::operator()(int i, int j) const {

  if (i < 0 || i >= d_i) { 
    cerr << "complex matrix: row index out of range" << endl;
    exit(1);
  }
  if (j < 0 || j >= d_j) { 
    cerr << "complex matrix: col index out of range" << endl;
    exit(1);
  }
  return (elem(i,j));
}

inline CmplxVector CmplxMatrix::col(int i) const {
 
  if (i < 0 || i >= d_j) {  
    cerr << "complex matrix: col index out of range" << endl;
    exit(1);
  }
  CmplxVector result(d_i);
  int j = d_i;
  while (j--) result.v[j] = elem(j,i);
  return (result);
}

inline CmplxMatrix::operator CmplxVector() const {

  if (d_j != 1) { 
    cerr << "error: cannot make complex vector from complex matrix" << endl;
    exit(1);
  }
  return (col(0));
}

inline CmplxMatrix CmplxMatrix::operator+(const CmplxMatrix& mat) {

  int i, j;
  check_dimensions(mat);
  CmplxMatrix result(d_i,d_j);
  for(i = 0; i < d_i; i++)
    for(j = 0; j < d_j; j++)
      result.elem(i,j) = elem(i,j) + mat.elem(i,j);
  return (result);
}

inline CmplxMatrix CmplxMatrix::operator+(const Matrix& mat) {

  int i, j;
  if (d_i != mat.dim_i() || d_j != mat.dim_j()) {
    cerr << "incompatible complex matrix types. +" << endl;
    exit(1);
  }
  CmplxMatrix result(d_i,d_j);
  CmplxMatrix tmp(mat);
  for(i = 0; i < d_i; i++)
    for(j = 0; j < d_j; j++)
      result.elem(i,j) = elem(i,j) + tmp.elem(i,j);
  return (result);
}

inline CmplxMatrix CmplxMatrix::operator-(const CmplxMatrix& mat) {

  int i, j;
  check_dimensions(mat);
  CmplxMatrix result(d_i,d_j);
  for(i = 0; i < d_i; i++)
    for(j = 0; j < d_j; j++)
      result.elem(i,j) = elem(i,j) - mat.elem(i,j);
  return (result);
}

inline CmplxMatrix CmplxMatrix::operator-(const Matrix& mat) {

  int i, j;
  if (d_i != mat.dim_i() || d_j != mat.dim_j()) {
    cerr << "incompatible complex matrix types in -." << endl;
    exit(1);
  }
  CmplxMatrix result(d_i,d_j);
  CmplxMatrix tmp(mat);
  for(i = 0; i < d_i; i++)
    for(j = 0; j < d_j; j++)
      result.elem(i,j) = elem(i,j) - tmp.elem(i,j);
  return (result);
}

inline CmplxMatrix& CmplxMatrix::operator+=(const CmplxMatrix& mat) {

  int i, j;
  check_dimensions(mat);
  for(i = 0; i < d_i; i++)
    for(j = 0; j < d_j; j++)
      elem(i,j) += mat.elem(i,j);
  return (*this);
}

inline CmplxMatrix& CmplxMatrix::operator-=(const CmplxMatrix& mat) {

  int i, j;
  check_dimensions(mat);
  for(i = 0; i < d_i; i++)
    for(j = 0; j < d_j; j++)
      elem(i,j) -= mat.elem(i,j);
  return (*this);
}

inline CmplxMatrix CmplxMatrix::operator-() {

  int i, j;
  CmplxMatrix result(d_i,d_j);
  for(i = 0; i < d_i; i++)
    for(j = 0; j < d_j; j++)
      result.elem(i,j) = -elem(i,j);
  return (result);
}

inline CmplxMatrix CmplxMatrix::operator*(double f) {

  int i, j;
  CmplxMatrix result(d_i,d_j);
  for(i = 0; i < d_i; i++)
    for(j = 0; j < d_j; j++)
      result.elem(i,j) = elem(i,j) * f;
  return (result);
}

inline CmplxMatrix CmplxMatrix::operator*(const Complex& f) {

  int i, j;
  CmplxMatrix result(d_i,d_j);
  for(i = 0; i < d_i; i++)
    for(j = 0; j < d_j; j++)
      result.elem(i,j) = elem(i,j) * f;
  return (result);
}

inline CmplxMatrix CmplxMatrix::operator*(const Matrix& mat) {

  if (d_j != mat.dim_i()) {
    cerr << "complex matrix: incompatible matrix types" << endl;
    exit(1);
  }
  CmplxMatrix result(d_i, mat.dim_j());
  int i, j;
  for(i = 0;i < mat.dim_j(); i++)
    for(j = 0; j < d_i; j++) result.elem(j,i) = *v[j] * mat.col(i);
  return (result);
}

inline CmplxMatrix CmplxMatrix::operator*(const CmplxMatrix& mat) {

  if (d_j != mat.d_i) {
    cerr << "complex matrix: incompatible matrix types" << endl;
    exit(1);
  }
  CmplxMatrix result(d_i, mat.d_j);
  int i,j;
  for(i = 0; i < mat.d_j; i++)
    for(j = 0; j < d_i; j++) result.elem(j,i) = *v[j] * mat.col(i);
  return (result);
}

inline CmplxMatrix CmplxMatrix::operator/(double a) {
 
  if (a==0) {
    cerr << "complex matrix: divided by zero" << endl;
    exit(1);
  }
  a = 1. / a;
  return (CmplxMatrix(*this * a));
}

inline CmplxMatrix CmplxMatrix::operator/(const Complex& a) {

  if (a==0) { 
    cerr << "complex matrix:  divided by zero" << endl;
    exit(1);
  }
  Complex tmp = 1. / a;
  return (CmplxMatrix(*this * a));
}

inline void CmplxMatrix::free()
{
  if (v) {
    while(d_i--) delete v[d_i];
    delete v;
  }
}

inline void CmplxMatrix::check_dimensions(const CmplxMatrix& mat) const {

  if (d_i != mat.d_i || d_j != mat.d_j) {
    cerr << "incompatible complex matrix types." << endl;
    exit(1);
  }
}

inline void CmplxMatrix::flip_rows(int i,int j) {
 
  CmplxVector* p = v[i];
  v[i] = v[j];
  v[j] = p;
}

inline void Print(const CmplxMatrix& m, ostream& out=cout) { out << m; }
inline void Read(CmplxMatrix& m, istream& in=cin)          { in >> m;  }
#endif
