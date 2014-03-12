//#include "stdafx.h"
#include <stdlib.h>
#include <math.h>
#include "complex.h"

/*********************** constructors *****************************/

Complex::Complex(double r, double i)
: re(r), im(i)
{}

Complex::Complex(const Complex& c) {
 
  re = c.re;
  im = c.im;
}

/*********************** members *********************************/

/********************* friends *****************************/

Complex operator*(const double& r, const Complex& c) { 
 
  Complex com;
  com.re = r * c.re;
  com.im = r * c.im;
  return(com);
}

Complex operator+(const double& r, const Complex& c) { 
 
  Complex com;
  com.re = r + c.re;
  com.im =     c.im;
  return(com);
}

Complex operator-(const double& r, const Complex& c) { 
 
  Complex com;
  com.re = r - c.re;
  com.im =   - c.im;
  return(com);
}

Complex operator/(const double& r, const Complex& c) {
 
  Complex com;
  double t, d;
  if(fabs(c.re) <= fabs(c.im)) {
     t = c.re / c.im;
     d = c.im * (1 + t*t);
     com.re = r * t / d;
     com.im = -r / d;    
  } else {
     t = c.im / c.re;
     d = c.re * (1 + t*t); 
     com.re = r / d;
     com.im = -r * t / d; 
  }
  return(com);
  
}

double real(const Complex& c) {
 
  return(c.re);
}

double imag(const Complex& c) {
 
  return(c.im);
}

double arg(const Complex& c) {

  return(c == 0 ? 0 : atan2(c.im, c.re));
}

double cabs(const Complex& c) {
 
  double x, y, ans, temp;
  x = fabs(c.re);
  y = fabs(c.im);
  if (x == 0.0)
     ans = y;
  else if (y == 0.0)    
     ans = x;
  else if (x > y) {
     temp = y/x;
     ans = x*sqrt(1.0+temp*temp);
  } else {
     temp = x/y;
     ans = y*sqrt(1.0+temp*temp);
  }
  return ans;  
}

double cabsm(const Complex& c) {
 
  return(sqrt(c.re*c.re + c.im*c.im));
}

Complex conjg(const Complex& c) {
 
  Complex com;
  com.re =  c.re;
  com.im = -c.im;
  return(com);
}

Complex cmplx(const double re,const double im) {

  Complex com;
  com.re = re;
  com.im = im;
  return(com);
} 

Complex polar(const double& r, const double& theta) {
 
  return(r*cmplx(cos(theta),sin(theta)));  
}

/****************** cosine of Complex number c *******************/
   /*** cos(c) = cos(c.re)*cosh(c.im)-i*sin(c.re)*sinh(c.im) **/

Complex cos(const Complex& c) {
 
  Complex com;
  com = cmplx(cos(c.re)*cosh(c.im),-sin(c.re)*sinh(c.im));
  return(com);
}

/************* hyperbolic cosine of Complex number c *************/
   /** cosh(c) = cosh(c.re)*cos(c.im)+i*sinh(c.re)*sin(c.im) **/  

Complex cosh(const Complex& c) {
 
  Complex com;
  com = cmplx(cosh(c.re)*cos(c.im),sinh(c.re)*sin(c.im));
  return(com);
}

/******************* sine of Complex number c ********************/
   /*** sin(c) = sin(c.re)*cosh*c.im)+i*cos(c.re)*sinh(c.im) **/

Complex sin(const Complex& c) {
 
  Complex com;
  com = cmplx(sin(c.re)*cosh(c.im),cos(c.re)*sinh(c.im));
  return(com);
}

/************** hyperbolic sine of Complex number c **************/
   /** sinh(c) = sinh(c.re)*cos(c.im)+i*cosh(c.re)*sin(c.im) **/  

Complex sinh(const Complex& c) {
 
  Complex com;
  com = cmplx(sinh(c.re)*cos(c.im),cosh(c.re)*sin(c.im));
  return(com);
}

/******************** tan of Complex number c ********************/

Complex tan(const Complex& c) {
 
  return(sin(c)/cos(c));
}

/*************** hyperbolic tan of Complex number c **************/

Complex tanh(const Complex& c) {
 
  return(sinh(c)/cosh(c));
}

/*************** square root of Complex number c *****************/

Complex sqrt(const Complex& c) {
 
  double r = cabs(c);
  double nr, ni;
  if(r == 0.0) nr = ni = r;
  else if(c.re > 0) {
     nr = sqrt(0.5*(r+c.re));  
     ni = c.im/nr/2.0;
  }
  else {
     ni = sqrt(0.5*(r-c.re));  
     if(c.im < 0) ni = -ni; 
     nr = c.im/ni/2.0;
  }
  return(cmplx(nr,ni));
}  

/*************** square root of Complex number c *****************/
   /*** sqrtm(c) = sqrt(abs(c))*(cos(arg/2) + i*sin(arg/2)) ***/
 
Complex sqrtm(const Complex& c) {

  Complex com;
  double   mod, theta;
  mod = cabs(c);
  theta = arg(c)/2.0;           /*****argument [-Pi, +Pi]*********/
  com = sqrt(mod) * cmplx(cos(theta),sin(theta));
  return(com);
}

/*************** exponential of Complex number c *****************/
   /******* exp(c) = exp(c.re)*(cos(c.re)+i*sin(c.im)) ********/

Complex exp(const Complex& c) {

  Complex com;
  com = exp(c.re) * cmplx(cos(c.im),sin(c.im));
  return(com);
}

/*************** natural log of Complex number c *****************/
   /************** log(c) = log(abs(c))+i*arg(c)***************/

Complex log(const Complex& c) {

  Complex com;
  int cs;

  if(c == 0) cs = 1; 
  else cs = 2; 
 
  switch(cs) {
 
   case 1:  cerr << "Problem with log !!!" << endl; com = 0.0; break;
   case 2:  com = cmplx(log(cabs(c)),arg(c));  break;
   default: cerr << "Should nevere get here!" << "\n";
  }

  return(com);
}

/****************** log10 of Complex number c ********************/

Complex log10(const Complex& c) {
 
  return(log10(exp(1.0))*log(c));
}

/********* Complex number c raised to a Complex power y **********/
  
Complex pow(const Complex& c, const Complex& y) {

  if(c == 0) {
     if(y == 0) return(cmplx(1,0)); 
     else return(cmplx(0,0));
  }
  double logr = log(cabs(c));  
  double t = arg(c);
  return(polar(exp(logr*y.re-t*y.im),logr*y.im+t*y.re));
}

/********* Complex number c raised to a double power y  **********/
   /***************** c**y = exp(y * log(c)) ******************/

Complex pow(const Complex& c, const double& y) {

  if(c == 0) {
     if(y == 0) return(cmplx(1,0)); 
     else return(cmplx(0,0));
  }
  return(exp(y*log(c)));
}

/******** Complex number c raised to an integer power y  *********/
   /**** c**y = abs(c)**y * (cos(y*theta)+i*sin(y*theta)) *****/

Complex pow(const Complex& c, int& y) {

  if(c == 0) {
     if(y == 0) return(cmplx(1,0)); 
     else return(cmplx(0,0));
  }
  if(c.im == 0) {
     if(c.re < 0) return(pow(c,cmplx(y,0)));
     else return(cmplx(pow(c.re,double(y)),0.0));
  }
  register double r = pow(cabs(c),double(y));
  register double th = y*arg(c);
  return(cmplx(r*cos(th),r*sin(th)));
}

/********* double number y raised to a Complex power c  **********/
   /****************** y**c = exp(c * log(y)) *****************/

Complex pow(const double& y, const Complex& c) {
 
  if(y == 0) {
     if(c == 0) return(cmplx(1,0)); 
     else return(cmplx(0,0));
  }
  if(y < 0) return(pow(cmplx(y,0),c));
  if(c.im == 0.0) return(cmplx(pow(y,c.re),0.0));
  return(exp(c*log(y)));   
}    
     
istream& operator>>(istream& in, Complex& c) {
 
  in >> c.re >> c.im;
  return(in);
}  

ostream& operator<<(ostream& out, Complex& c) {
 
  out << " ( " << c.re << ',' << " " << c.im << " )";
  return(out);
}  

