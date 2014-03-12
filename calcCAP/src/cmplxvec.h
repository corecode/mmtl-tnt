#ifndef _cmplxvec__h_
#define _cmplxvec__h_

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
  CmplxVector& zeros();

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


double norm(const CmplxVector& x);
double norm2(const CmplxVector& x);
double normsa(const CmplxVector& x);


CmplxVector operator^(const CmplxVector& v, const CmplxVector& w);


#endif
