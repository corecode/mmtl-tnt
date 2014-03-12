#ifndef _complex__h_
#define _complex__h_

#include <stdlib.h>
#include <math.h>
#include <iostream.h>

class Complex
{
   friend Complex operator*(const double&, const Complex&);
   friend Complex operator+(const double&, const Complex&);
   friend Complex operator-(const double&, const Complex&);
   friend Complex operator/(const double&, const Complex&);

   friend double     real(const Complex&);
   friend double     imag(const Complex&);
   friend double     arg(const Complex&);
   friend double     cabs(const Complex&);
   friend double     cabsm(const Complex&);
   friend Complex   conjg(const Complex&);
   friend Complex   cmplx(const double,const double);  
   friend Complex   polar(const double&, const double&);
   friend Complex   cos(const Complex&);
   friend Complex   cosh(const Complex&);
   friend Complex   sin(const Complex&);
   friend Complex   sinh(const Complex&);
   friend Complex   tan(const Complex&);
   friend Complex   tanh(const Complex&);
   friend Complex   sqrt(const Complex&);
   friend Complex   sqrtm(const Complex&); 
   friend Complex   exp(const Complex&);
   friend Complex   log(const Complex&); 
   friend Complex   log10(const Complex&); 
   friend Complex   pow(const Complex&, const Complex&);
   friend Complex   pow(const Complex&, const double&);
   friend Complex   pow(const Complex&, int&);
   friend Complex   pow(const double&, const Complex&);     
         
   friend ostream& operator<<(ostream&, Complex&);
   friend istream& operator>>(istream&, Complex&);

 public:
   Complex(){}                       // constructors
   Complex(double r, double i = 0.0);
   Complex(const Complex&);

   Complex  operator-(const Complex&) const;
   Complex  operator-(const double&)  const;
   Complex  operator+(const Complex&) const;
   Complex  operator+(const double&)  const;
   Complex  operator*(const Complex&) const;
   Complex  operator*(const double&)  const;
   Complex  operator/(const Complex&) const;
   Complex  operator/(const double&)  const;
   Complex  operator-();
   Complex  operator-() const;
   Complex& operator=(const Complex&);
   Complex& operator=(const double&);
   Complex& operator+=(const Complex&);
   Complex& operator+=(const double&);
   Complex& operator-=(const Complex&);
   Complex& operator-=(const double&);
   Complex& operator*=(const Complex&);
   Complex& operator*=(const double&);
   Complex& operator/=(const Complex&);
   Complex& operator/=(const double&);
   
   int      operator==(const Complex&) const;
   int      operator!=(const Complex&) const;

 private:
   double re, im;
};

inline Complex Complex::operator*(const Complex& c) const {
 
  Complex com;
  com.re = re * c.re - im * c.im;
  com.im = im * c.re + re * c.im;
  return(com);
}

inline Complex Complex::operator*(const double& r) const {
 
  Complex com;
  com.re = re * r;
  com.im = im * r;
  return(com);
}

inline Complex Complex::operator/(const Complex& c) const {
 
  Complex com;
  double t, d;
  if(fabs(c.re) <= fabs(c.im)) {
     t = c.re / c.im;
     d = c.im * (1 + t*t);
     com.re = (re * t + im) / d;
     com.im = (im * t - re) / d;    
  } else {
     t = c.im / c.re;
     d = c.re * (1 + t*t); 
     com.re = (re + im * t) / d;
     com.im = (im - re * t) / d; 
  }
  return(com);
}

inline Complex Complex::operator/(const double& r) const { 
 
  Complex com;
  com.re = re / r;
  com.im = im / r;
  return(com);
}

inline Complex Complex::operator-(const Complex& c) const {
 
  Complex com;
  com.re = re - c.re;
  com.im = im - c.im;
  return(com);
}

inline Complex Complex::operator-(const double& c) const {
 
  Complex com;
  com.re = re - c;
  com.im = im;
  return(com);
}

inline Complex Complex::operator+(const Complex& c) const {
 
  Complex com;
  com.re = re + c.re;
  com.im = im + c.im;
  return(com);
}

inline Complex Complex::operator+(const double& c) const {
 
  Complex com;
  com.re = re + c;
  com.im = im;
  return(com);
}

inline Complex& Complex::operator=(const Complex& c) {
 
  if(this == &c) return(*this);
  re = c.re;
  im = c.im;
  return(*this);
}

inline Complex& Complex::operator=(const double& c) {

  re = c;
  im = 0.0;
  return(*this);
}

inline Complex& Complex::operator+=(const Complex& c) {

  re = re + c.re;
  im = im + c.im;
  return(*this);
}

inline Complex& Complex::operator+=(const double& c) {

  re = re + c;
  im = im;
  return(*this);
}

inline Complex& Complex::operator-=(const Complex& c) {

  re = re - c.re;
  im = im - c.im;
  return(*this);
}

inline Complex& Complex::operator-=(const double& c) {

  re = re - c;
  im = im;
  return(*this);
}

inline Complex& Complex::operator*=(const Complex& c) {

  double rep, imp;
  rep = re * c.re-im * c.im; 
  imp = im * c.re+re * c.im;
  re = rep; im = imp;
  return(*this);
}

inline Complex& Complex::operator*=(const double& c) {

  re = re * c;
  im = im * c;
  return(*this);
}

inline Complex& Complex::operator/=(const Complex& c) {

  double t, d, rep, imp;
  if(fabs(c.re) <= fabs(c.im)) {
     t = c.re / c.im;
     d = c.im * (1 + t*t);
     rep = (re * t + im) / d;
     imp = (im * t - re) / d;    
  } else {
     t = c.im / c.re;
     d = c.re * (1 + t*t); 
     rep = (re + im * t) / d;
     imp = (im - re * t) / d; 
  }
  re = rep; im = imp;  
  return(*this);
}

inline Complex& Complex::operator/=(const double& r) {

  double rep, imp;
  rep = re / r;
  imp = im / r;
  re = rep; im = imp;
  return(*this);
}

inline int Complex::operator==(const Complex& c) const {
  
  return((re==c.re) && (im==c.im));
}

inline int Complex::operator!=(const Complex& c) const {
  
  return((re!=c.re) || (im!=c.im));
}

inline Complex Complex::operator-() {

  Complex result;
  result.re = -re;
  result.im = -im;
  return(result);
}

inline Complex Complex::operator-() const {

  Complex result;
  result.re = -re;
  result.im = -im;
  return(result);
}

#endif

