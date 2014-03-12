#ifndef _cmplxvec__h_
#define _cmplxvec__h_

#include <stdlib.h>
#include <math.h>
#include "complex.h"
#include "vector.h"

class CmplxVector
{
  friend class Complex;
  friend class CmplxMatrix;
  friend ostream& operator<<(ostream&, const CmplxVector&);
  friend istream& operator>>(istream&, CmplxVector&);
  friend CmplxVector operator*(double, const CmplxVector&);
  friend CmplxVector operator*(const Complex&, const CmplxVector&);
  friend Complex inner(const CmplxVector&, const CmplxVector&);

public:

  CmplxVector(int=0);
  CmplxVector(int, double);
  CmplxVector(int, Complex);
  CmplxVector(const CmplxVector&);
  CmplxVector(const Vector&);

  ~CmplxVector() { if (v) delete []v; }

  void free()   { if (v) delete []v; }

  int dim() const { return d; }

  CmplxVector conjg() const;
  Vector real() const ;
  Vector imag() const ;
  Vector  abs() const;
  CmplxVector& resize(int);

  CmplxVector& operator=(const CmplxVector&);
  CmplxVector& operator=(const Vector&);
  CmplxVector& operator=(double);
  CmplxVector& operator=(Complex);

  Complex& operator[](int);

  Complex  operator[](int) const;

  CmplxVector  operator+(const CmplxVector&) const;
  CmplxVector  operator+(const Vector&) const;

  CmplxVector  operator-(const CmplxVector&) const;
  CmplxVector  operator-(const Vector&) const;

  CmplxVector& operator+=(const CmplxVector&);
  CmplxVector& operator+=(const Vector&);

  CmplxVector& operator-=(const CmplxVector&);
  CmplxVector& operator-=(const Vector&);

  CmplxVector& operator*=(double);
  CmplxVector& operator*=(Complex);

  Complex      operator*(const CmplxVector&) const;
  Complex      operator*(const Vector&) const;

  CmplxVector  operator-();
  CmplxVector  operator*(double)          const;
  CmplxVector  operator/(double)          const;
  CmplxVector  operator*(const Complex&)  const;
  CmplxVector  operator/(const Complex&)  const;

  int     operator==(const CmplxVector&) const;
  int     operator!=(const CmplxVector& w)  const { return !(*this == w); }

 private:

  Complex* v;
  int d;

  void check_dimensions(const CmplxVector&) const;
};

inline void Print(const CmplxVector& v, ostream& out=cout) { out << v; }
inline void Read(CmplxVector& v, istream& in=cin)          { in >> v;  }

inline Vector      real(const CmplxVector& x) { return x.real(); }
inline Vector      imag(const CmplxVector& x) { return x.imag(); }
inline CmplxVector conjg(const CmplxVector& x) { return x.conjg(); }

inline Complex CmplxVector::operator[](int i) const {
 
 if (i < 0 || i >= d) {
   cerr << "Complex vector: index out of range" << endl;
   exit(1);
 }
 return (v[i]);
}

inline Complex& CmplxVector::operator[](int i) {
 
 if (i < 0 || i >= d) {
   cerr << "Complex vector: index out of range" << endl;
   exit(1);
 }
 return (v[i]);
}

inline CmplxVector CmplxVector::operator+(const CmplxVector& vec) const {
 
 check_dimensions(vec);
 int n = d;
 CmplxVector result(n);
 while (n--) result.v[n] = v[n] + vec.v[n];
 return (result);
}

inline CmplxVector CmplxVector::operator+(const Vector& vec) const {
 
 check_dimensions(vec);
 int n = d;
 CmplxVector result(n);
 while (n--) result.v[n] = v[n] + vec[n];
 return (result);
}


inline CmplxVector CmplxVector::operator-(const CmplxVector& vec) const {
 
 check_dimensions(vec);
 int n = d;
 CmplxVector result(n);
 while (n--) result.v[n] = v[n] - vec.v[n];
 return (result);
}

inline CmplxVector CmplxVector::operator-(const Vector& vec) const {
 
 check_dimensions(vec);
 int n = d;
 CmplxVector result(n);
 while (n--) result.v[n] = v[n] - vec[n];
 return (result);
}

inline CmplxVector& CmplxVector::operator+=(const CmplxVector& vec) {
 
 check_dimensions(vec);
 int n = d;
 while (n--) v[n] += vec.v[n];
 return (*this);
}

inline CmplxVector& CmplxVector::operator+=(const Vector& vec) {
 
 check_dimensions(vec);
 int n = d;
 while (n--) v[n] += vec[n];
 return (*this);
}

inline CmplxVector& CmplxVector::operator-=(const CmplxVector& vec) {
 
 check_dimensions(vec);
 int n = d;
 while (n--) v[n] -= vec.v[n];
 return (*this);
}

inline CmplxVector& CmplxVector::operator-=(const Vector& vec) {
 
 check_dimensions(vec);
 int n = d;
 while (n--) v[n] -= vec[n];
 return (*this);
}

inline CmplxVector& CmplxVector::operator*=(double a) {
 
 int n = d;
 while (n--) v[n] *= a;
 return (*this);
}

inline CmplxVector& CmplxVector::operator*=(Complex a) {
 
 int n = d;
 while (n--) v[n] *= a;
 return (*this);
}

inline CmplxVector CmplxVector::operator-() {
 
  int n = d;
  CmplxVector result(n);
  while (n--) result.v[n] = -v[n];
  return (result);
}

inline CmplxVector CmplxVector::operator*(double x) const {
 
 int n = d;
 CmplxVector result(n);
 while (n--) result.v[n] = v[n] * x;
 return (result);
}

inline CmplxVector CmplxVector::operator/(double x) const {
 
 int n = d;
 CmplxVector result(n);
 while (n--) result.v[n] = v[n] / x;
 return (result);
}

inline CmplxVector CmplxVector::operator*(const Complex& x) const {
 
 int n = d;
 CmplxVector result(n);
 while (n--) result.v[n] = v[n] * x;
 return (result);
}

inline CmplxVector CmplxVector::operator/(const Complex& x) const {
 
 int n = d;
 CmplxVector result(n);
 while (n--) result.v[n] = v[n] / x;
 return (result);
}

inline Complex CmplxVector::operator*(const CmplxVector& vec) const {
  
 check_dimensions(vec);

 Complex result(0.,0.);
 int n = d;

 while (n--)  result += v[n] * vec.v[n]; 
 return (result);
}

inline Complex CmplxVector::operator*(const Vector& vec) const {
 
 check_dimensions(vec);
 Complex result(0.,0.);
 int n = d;
 while (n--) result += v[n] * vec[n];
 return (result);
}

inline CmplxVector& CmplxVector::operator=(const CmplxVector& vec) {

 if (this != &vec) {
   int n = vec.d;
   if (n != d) {
     delete v;
     v = new Complex [n];
     d = n;
   }
   while (n--) v[n] = vec.v[n];
 }
 return (*this);
}

inline CmplxVector& CmplxVector::operator=(const Vector& vec) {

 int n = vec.dim();
 if (n != d) {
   delete v;
   v = new Complex [n];
   d = n;
 }
 while (n--) v[n] = vec[n];

 return (*this);
}

inline CmplxVector& CmplxVector::operator=(double a) {

 int n = d;

 while (n--) v[n] = a;

 return (*this);
}

inline CmplxVector& CmplxVector::operator=(Complex a) {

 int n = d;

 while (n--) v[n] = a;

 return (*this);
}

inline int CmplxVector::operator==(const CmplxVector& vec)  const {
 
 if (vec.d != d) return (0);
 int i = 0;
 while ((i < d) && (v[i]==vec.v[i])) i++;
 return ((i == d) ? 1 : 0);
}

double norm(const CmplxVector& x);
double norm2(const CmplxVector& x);
double normsa(const CmplxVector& x);


CmplxVector operator^(const CmplxVector& v, const CmplxVector& w);


#endif
