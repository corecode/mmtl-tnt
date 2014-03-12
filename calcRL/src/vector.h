#ifndef _Vector__h_
#define _Vector__h_

#include <stdlib.h>
#include <math.h>
#include <iostream.h>
#include <iomanip.h>

class Vector
{
  friend class Matrix;
  friend class Matrix3d;
  friend ostream& operator<<(ostream&, const Vector&);
  friend istream& operator>>(istream&, Vector&);
  friend Vector operator*(double, const Vector&);
  friend double max(const Vector&);
  friend double min(const Vector&);

public:

  Vector(int = 0);
  Vector(int, double);
  Vector(int, double*);
  Vector(const Vector&);

  ~Vector()    { if(v) delete []v; }

  void free() { if(v) delete []v; }

  int     dim()  const { return d; }
  double* vec()  const { return v; }
  Vector  abs()  const;
  Vector& resize(int);
  void    swap_elem(int,int);

  Vector& operator=(const Vector&);
  double& operator[](int);
  double  operator[](int) const;
  Vector  operator+(const Vector&) const;
  Vector  operator-(const Vector&) const;
  Vector& operator+=(const Vector&);
  Vector& operator-=(const Vector&);
  Vector& operator/=(double);
  Vector& operator*=(double);
  Vector  operator*(double)        const;
  Vector  operator/(double)        const;
  double  operator*(const Vector&) const;
  Vector  operator-() const;

  int     operator==(const Vector&) const;
  int     operator!=(const Vector& w)  const { return !(*this == w); }

private:

  double* v;
  int     d;
};


inline double Vector::operator[](int i) const {

  if (i < 0 || i > d - 1) {
    cerr << "Vector: index out of range" << endl;
    exit(1);
  }
  return (v[i]);
}

inline double& Vector::operator[](int i) {

  if (i < 0 || i > d - 1) {
    cerr << "Vector: index out of range" << endl;
    exit(1);
  }
  return (v[i]);
}

inline Vector Vector::operator+(const Vector& vec) const {

  Vector result(d);
  int n = d;
  if (d != vec.d) {
    cerr << "Vector+: vectors have different length" << endl;
    exit(1);
  }
  while (n--) result.v[n] = v[n] + vec.v[n];
  return (result);
}

inline Vector Vector::operator-(const Vector& vec) const {

  Vector result(d);
  int n = d;
  if (d != vec.d) {
    cerr << "Vector-: vectors have different length" << endl;
    exit(1);
  }
  while (n--) result.v[n] = v[n] - vec.v[n];
  return (result);
}

inline Vector Vector::operator*(double x) const {

  Vector result(d);
  int n = d;
  while (n--) result.v[n] = v[n] * x;
  return (result);
}

inline Vector Vector::operator/(double x) const {

  Vector result(d);
  int n = d;
  while (n--) result.v[n] = v[n] / x;
  return (result);
}

inline Vector& Vector::operator+=(const Vector& vec) {

  int n = d;
  if (d != vec.d) {
    cerr << "Vector+=: vectors have different length" << endl;
    exit(1);
  }
  while (n--) v[n] += vec.v[n];
  return (*this);
}

inline Vector& Vector::operator-=(const Vector& vec) {

  int n = d;
  if (d != vec.d) {
    cerr << "Vector-=: vectors have different length" << endl;
    exit(1);
  }
  while (n--) v[n] -= vec.v[n];
  return (*this);
}

inline Vector& Vector::operator/=(double x) {

  int n = d;
  if (x == 0) {
    cerr << "Vector/=: divided by zero" << endl;
    exit(1);
  }
  while (n--) v[n] /= x;
  return (*this);
}

inline Vector& Vector::operator*=(double x) {

  int n = d;
  while (n--) v[n] *= x;
  return (*this);
}

inline Vector Vector::operator-() const {

  Vector result(d);
  int n = d;
  while (n--) result.v[n] = -v[n];
  return (result);
}

inline double Vector::operator*(const Vector& vec) const {

  if (d != vec.d) {
    cerr << "Vector*: vectors have different length" << endl;
    exit(1);
  }
  double result  = 0;
  int n = d;
  while (n--) result = result + v[n] * vec.v[n];
  return (result);
}

inline Vector& Vector::operator=(const Vector& vec) {

  //if (this != &vec) {
  //int n = vec.d;
  //if (d != vec.d) {
  //cerr << "Vector=: vectors have different length" << endl;
  //exit(1);
  //}
  //while (n--) v[n] = vec.v[n];
  //}
  //return (*this);

  if (this != &vec) {
   int n = vec.d;
   if (n != d) {
     delete v;
     v = new double [n];
     d = n;
   }
   while (n--) v[n] = vec.v[n];
 }
 return (*this);
}

inline int Vector::operator==(const Vector& vec)  const {

  if (vec.d != d) return (0);
  int i = 0;
  while ((i < d) && (v[i]==vec.v[i])) i++;
  return ((i == d) ? 1: 0);
}

inline Vector Vector::abs() const {

  Vector result(d);
  for(int i = 0; i < d; i++)
      result[i]=fabs(v[i]);
  return (result);
}

inline Vector& Vector::resize(int new_dim) {

  if(d != new_dim) {
    delete v;
    v = new double [new_dim];
    d = new_dim;
  }
  while (new_dim--) v[new_dim] = 0.0;
  return (*this);
}

inline void  Vector::swap_elem(int m,int n) {

  double tmp = v[m];
  v[m] = v[n];
  v[n] = tmp; 
}


inline void Print(const Vector& v, ostream& out=cout) { out << v; }
inline void Read(Vector& v, istream& in=cin)          { in >> v;  }
inline double max(double x,double y)                  { return (x > y ? x : y); }

double norm(const Vector& v);
double norm2(const Vector& v);
Vector operator^(const Vector& v, const Vector& w);

#endif
