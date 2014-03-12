#ifndef __matrix_h__
#define __matrix_h__

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
  Matrix& zeros();
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

inline void Print(const Matrix& m, ostream& out=cout) { out << m; }
inline void Read(Matrix& m, istream& in=cin)          { in >> m;  }

#endif
