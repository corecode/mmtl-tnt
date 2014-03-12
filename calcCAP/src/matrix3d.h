#ifndef __matrix3d_h__
#define __matrix3d_h__

//#include "stdafx.h"
#include "cmplxvec.h"
#include "vector.h"
#include "matrix.h"

class Matrix3d  
{ 
  friend ostream& operator<<(ostream&, const Matrix3d&);
  friend Matrix3d operator*(double, const Matrix3d&);
  friend double max(const Matrix3d&);
  friend double min(const Matrix3d&);
  friend double norm(const Matrix3d&);
  friend double norm1(const Matrix3d&);
  friend double norm2(const Matrix3d&);
  friend double norm2_sqv(const Matrix3d&);

public:

  Matrix3d(int n=0, int m=0, int l=0, double = 0.0);
  Matrix3d(const Matrix3d&);
  Matrix3d(const Matrix&);
  Matrix3d(const Vector&);

  Matrix3d& operator=(const Matrix3d&);

  ~Matrix3d();

  void free();

  int  dim_i()  const  {  return d_i; }
  int  dim_j()  const  {  return d_j; }
  int  dim_k()  const  {  return d_k; }
  
  Vector    vec(int, int) const;
  Vector    row(int) const;
  Vector    col(int) const;
  Matrix    mtr(int) const;
  Matrix3d  abs() const;
  Matrix3d& zeros();
  Matrix3d& resize(int,int,int);

  int  operator==(const Matrix3d&)  const;
  int  operator!=(const Matrix3d& x)  const { return !(*this == x); }

  double& operator()(int, int, int);
  double& operator()(int, int, int) const;

  Matrix3d  operator+(const Matrix3d&);
  Matrix3d  operator-(const Matrix3d&);
  Matrix3d  operator-();
  Matrix3d& operator+=(const Matrix3d&);
  Matrix3d& operator-=(const Matrix3d&);
  Matrix3d  operator*(double);
  Matrix3d  operator*(double) const;
  Matrix3d  operator/(double);
  Matrix3d  operator^(const Matrix3d&) const;
  double    operator*(const Matrix3d&) const;
  
protected:
  double&  elem(int i, int j, int k) const  { return *(*(*(v+i)+j)+k); }
                     
private:
  double*** v;
  int  d_i;
  int  d_j;
  int  d_k;
  void     check_dimensions(const Matrix3d&) const;
};

inline void Print(const Matrix3d& m, ostream& out=cout) { out << m; }

#endif
