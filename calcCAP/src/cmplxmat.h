#ifndef _cmplxmat__h_
#define _cmplxmat__h_

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
CmplxVector operator*(CmplxVector& w)       const { return CmplxVector(*this * CmplxMatrix(w)); }
CmplxVector operator*(const CmplxVector& w)       { return CmplxVector(*this * CmplxMatrix(w)); }
CmplxVector operator*(const CmplxVector& w) const { return CmplxVector(*this * CmplxMatrix(w)); }
CmplxVector operator*(Vector& w)                  { return CmplxVector(*this * CmplxMatrix(w)); }
CmplxVector operator*(Vector& w)            const { return CmplxVector(*this * CmplxMatrix(w)); }
CmplxVector operator*(const Vector& w)            { return CmplxVector(*this * CmplxMatrix(w)); }
CmplxVector operator*(const Vector& w)      const { return CmplxVector(*this * CmplxMatrix(w)); }


private:
  CmplxVector** v;
  int  d_i;
  int  d_j;

  void     flip_rows(int,int);
  void     check_dimensions(const CmplxMatrix&) const;
  Complex&  elem(int i, int j) const { return v[i]->v[j]; }
};

inline void Print(const CmplxMatrix& m, ostream& out=cout) { out << m; }
inline void Read(CmplxMatrix& m, istream& in=cin)          { in >> m;  }
#endif
