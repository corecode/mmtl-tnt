#ifndef _complex__h_
#define _complex__h_

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

#endif
