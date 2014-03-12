//#include "stdafx.h"
#include <math.h>
#include "cmplxvec.h"
#include <stdlib.h>


/******************************* constructors ******************************/

CmplxVector::CmplxVector(int n) {
 
 Complex zero(0.,0.);
 if (n < 0) {
   cerr << "complex vector: negative dimension." << endl;
   exit(1);
 }
 d = n;
 if (d > 0) {
   v = new Complex[d];
   while (n--) v[n] = zero;
 }
 else v = NULL;
}

CmplxVector::CmplxVector(int n, double a) {

  if (n <= 0) {
    cerr << "complex vector: undefined dimension." << endl;
    exit(1);
  }
  d  = n;
  v  = new Complex[d];
  while (n--) v[n] = a;
}

CmplxVector::CmplxVector(int n, Complex a) {

 if (n < 0) {
   cerr << "complex vector: negative dimension." << endl;
   exit(1);
 }
 d  = n;
 v  = new Complex[d];
 while (n--) v[n] = a;
}

CmplxVector::CmplxVector(const CmplxVector& p) {
 
 d = p.d;
 if (d > 0) {
   v = new Complex [d];
   for(int i = 0; i < d; i++) v[i] = p.v[i];
 }
  else v = NULL;
}

CmplxVector::CmplxVector(const Vector& v1) {
 
 d = v1.dim();
 v = new Complex[d];
 for(int i = 0; i < d; i++) v[i] = Complex(v1[i]);
}

/****************************** members ******************************/

void CmplxVector::check_dimensions(const CmplxVector& vec) const {
 
  if (d != vec.d) {
   cerr << "Complex vector arguments have different dimensions." << endl;
   exit(1);
 }
}

/***********************************************************
Complex CmplxVector::operator[](int i) const {
 
 if (i < 0 || i >= d) {
   cerr << "Complex vector: index out of range" << endl;
   exit(1);
 }
 return (v[i]);
}

Complex& CmplxVector::operator[](int i) {
 
 if (i < 0 || i >= d) {
   cerr << "Complex vector: index out of range" << endl;
   exit(1);
 }
 return (v[i]);
}

CmplxVector CmplxVector::operator+(const CmplxVector& vec) const {
 
 check_dimensions(vec);
 int n = d;
 CmplxVector result(n);
 while (n--) result.v[n] = v[n] + vec.v[n];
 return (result);
}

CmplxVector CmplxVector::operator+(const Vector& vec) const {
 
 check_dimensions(vec);
 int n = d;
 CmplxVector result(n);
 while (n--) result.v[n] = v[n] + vec[n];
 return (result);
}

CmplxVector CmplxVector::operator-(const CmplxVector& vec) const {
 
 check_dimensions(vec);
 int n = d;
 CmplxVector result(n);
 while (n--) result.v[n] = v[n] - vec.v[n];
 return (result);
}

CmplxVector CmplxVector::operator-(const Vector& vec) const {
 
 check_dimensions(vec);
 int n = d;
 CmplxVector result(n);
 while (n--) result.v[n] = v[n] - vec[n];
 return (result);
}

CmplxVector& CmplxVector::operator+=(const CmplxVector& vec) {
 
 check_dimensions(vec);
 int n = d;
 while (n--) v[n] += vec.v[n];
 return (*this);
}

CmplxVector& CmplxVector::operator+=(const Vector& vec) {
 
 check_dimensions(vec);
 int n = d;
 while (n--) v[n] += vec[n];
 return (*this);
}

CmplxVector& CmplxVector::operator-=(const CmplxVector& vec) {
 
 check_dimensions(vec);
 int n = d;
 while (n--) v[n] -= vec.v[n];
 return (*this);
}

CmplxVector& CmplxVector::operator-=(const Vector& vec) {
 
 check_dimensions(vec);
 int n = d;
 while (n--) v[n] -= vec[n];
 return (*this);
}

CmplxVector& CmplxVector::operator*=(double a) {
 
 int n = d;
 while (n--) v[n] *= a;
 return (*this);
}

CmplxVector& CmplxVector::operator*=(Complex a) {
 
 int n = d;
 while (n--) v[n] *= a;
 return (*this);
}

CmplxVector CmplxVector::operator-() {
 
  int n = d;
  CmplxVector result(n);
  while (n--) result.v[n] = -v[n];
  return (result);
}

CmplxVector CmplxVector::operator*(double x) const {
 
 int n = d;
 CmplxVector result(n);
 while (n--) result.v[n] = v[n] * x;
 return (result);
}

CmplxVector CmplxVector::operator/(double x) const {
 
 int n = d;
 CmplxVector result(n);
 while (n--) result.v[n] = v[n] / x;
 return (result);
}

CmplxVector CmplxVector::operator*(const Complex& x) const {
 
 int n = d;
 CmplxVector result(n);
 while (n--) result.v[n] = v[n] * x;
 return (result);
}

CmplxVector CmplxVector::operator/(const Complex& x) const {
 
 int n = d;
 CmplxVector result(n);
 while (n--) result.v[n] = v[n] / x;
 return (result);
}

Complex CmplxVector::operator*(const CmplxVector& vec) const {
  
 check_dimensions(vec);

 Complex result(0.,0.);
 int n = d;

 while (n--)  result += v[n] * vec.v[n]; 
 return (result);
}

Complex CmplxVector::operator*(const Vector& vec) const {
 
 check_dimensions(vec);
 Complex result(0.,0.);
 int n = d;
 while (n--) result += v[n] * vec[n];
 return (result);
}

CmplxVector& CmplxVector::operator=(const CmplxVector& vec) {

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

CmplxVector& CmplxVector::operator=(const Vector& vec) {

 int n = vec.dim();
 if (n != d) {
   delete v;
   v = new Complex [n];
   d = n;
 }
 while (n--) v[n] = vec[n];

 return (*this);
}

CmplxVector& CmplxVector::operator=(double a) {

 int n = d;

 while (n--) v[n] = a;

 return (*this);
}

CmplxVector& CmplxVector::operator=(Complex a) {

 int n = d;

 while (n--) v[n] = a;

 return (*this);
}

int CmplxVector::operator==(const CmplxVector& vec)  const {
 
 if (vec.d != d) return (0);
 int i = 0;
 while ((i < d) && (v[i]==vec.v[i])) i++;
 return ((i == d) ? 1 : 0);
}
***********************************/

CmplxVector CmplxVector::conjg() const {
 
 int n = d;
 CmplxVector tmp(n);
 for(int i = 0; i < n; i++)  tmp[i] = ::conjg(v[i]);
 return (tmp);
}

Vector CmplxVector::real() const {
 
 Vector tmp(d);
 int n = d;
 while(n--) tmp[n] = ::real(v[n]);
 return (tmp);
}

Vector CmplxVector::imag() const {
 
 Vector tmp(d);
 int n = d;
 while(n--) tmp[n] = ::imag(v[n]);
 return (tmp);
}

Vector CmplxVector::abs() const {

  Vector result(d);
  for(int i = 0; i < d; i++)
      result[i]=::cabs(v[i]);
  return (result);
}

CmplxVector& CmplxVector::resize(int new_dim) {

  if(d != new_dim) {
    delete v;
    v = new Complex [new_dim];
    d = new_dim;
  }
  while (new_dim--) v[new_dim] = 0.0;
  return (*this);
}

/************            friends          ***********************/

CmplxVector operator*(double f, const CmplxVector& v)
{
 return v*f;
}

CmplxVector operator*(const Complex& f, const CmplxVector& v)
{
 return v*f;
}

Complex inner(const CmplxVector& x, const CmplxVector& x0) 
{
  return(x.conjg()*x0); 
}

ostream& operator<<(ostream& out, const CmplxVector& v)
{
 out.precision(10);
 for (int i = 0; i < v.d; i++)  
 //out << v.v[i] << endl;
   out << " [" << i << "]=" << v.v[i] << endl; 
 return (out);
}

istream& operator>>(istream& in, CmplxVector& x)
{
 int i = 0;
 while (i < x.d && in >> x.v[i++]);
 return (in);
}

double norm(const CmplxVector& x)
{
 double sum = 0.;
 int n = x.dim();
 for(int i = 0; i < n; i++) sum += cabs(x[i]);
 return (sum);
}

double norm2(const CmplxVector& x) 
{
  double sum = 0;
  int n = x.dim();
  //while (n--) sum += real(x[n])*real(x[n])+imag(x[n])*imag(x[n]);
  for(int i = 0; i < n; i++) sum += cabs(x[i])*cabs(x[i]);  
  return (sqrt(sum));
}

double normsa(const CmplxVector& x)
{
 Complex sum = 0.;
 int n = x.dim();
 for(int i = 0; i < n; i++) sum += x[i];
 return (cabs(sum));
}

CmplxVector operator^(const CmplxVector& v,const CmplxVector& w) {

  if (v.dim() != w.dim()) {
    cerr << "CmplxVector=: vectors have different length" << endl;
    exit(1);
  }
  int n = v.dim();
  CmplxVector result(n, 0.0);
  while (n--) result[n] = v[n] * w[n];
  return (result);
}
