#ifndef _complex__h_
#define _complex__h_

#include <iostream>

using namespace std;

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


Complex operator*(const double&, const Complex&);
Complex operator+(const double&, const Complex&);
Complex operator-(const double&, const Complex&);
Complex operator/(const double&, const Complex&);

double     real(const Complex&);
double     imag(const Complex&);
double     arg(const Complex&);
double     cabs(const Complex&);
double     cabsm(const Complex&);
Complex   conjg(const Complex&);
Complex   cmplx(const double,const double);
Complex   polar(const double&, const double&);
Complex   cos(const Complex&);
Complex   cosh(const Complex&);
Complex   sin(const Complex&);
Complex   sinh(const Complex&);
Complex   tan(const Complex&);
Complex   tanh(const Complex&);
Complex   sqrt(const Complex&);
Complex   sqrtm(const Complex&);
Complex   exp(const Complex&);
Complex   log(const Complex&);
Complex   log10(const Complex&);
Complex   pow(const Complex&, const Complex&);
Complex   pow(const Complex&, const double&);
Complex   pow(const Complex&, int&);
Complex   pow(const double&, const Complex&);

ostream& operator<<(ostream&, Complex&);
istream& operator>>(istream&, Complex&);

#endif
