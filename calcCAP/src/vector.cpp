//#include "stdafx.h"
#include <stdlib.h>
#include <math.h>
#include "vector.h"

/*************************** constructors **************************/

Vector::Vector(int n)  {

  if (n < 0) {
    cerr << "Vector: negative dimension." << endl;
    exit(1);
  }
  d  = n;
  if (d > 0) {
    v  = new double[d];
    while (n--) v[n] = 0.0;
  }
  else v = NULL;
}

Vector::Vector(int n, double v0) {

  if (n <= 0) {
    cerr << "Vector: undefined dimension." << endl;
    exit(1);
  }
  d  = n;
  v  = new double[d];
  while (n--) v[n] = v0;
}

Vector::Vector(int n, double* v0) {

  if (n <= 0) {
    cerr << "Vector: undefined dimension." << endl;
    exit(1);
  }
  d  = n;
  v  = new double[d];
  while (n--) v[n] = *(v0+n);
}

Vector::Vector(const Vector& v0) {

  d = v0.d;
  if (d > 0) {
    v = new double[d];
    for(int i = 0; i < d; i++) v[i] = v0.v[i];
  }
  else v = NULL;
}

/************************* members ****************************/

double Vector::operator[](int i) const {

  if (i < 0 || i > d - 1) {
    cerr << "Vector: index out of range" << endl;
    exit(1);
  }
  return (v[i]);
}

double& Vector::operator[](int i) {

  if (i < 0 || i > d - 1) {
    cerr << "Vector: index out of range" << endl;
    exit(1);
  }
  return (v[i]);
}

Vector Vector::operator+(const Vector& vec) const {

  Vector result(d);
  int n = d;
  if (d != vec.d) {
    cerr << "Vector+: vectors have different length" << endl;
    exit(1);
  }
  while (n--) result.v[n] = v[n] + vec.v[n];
  return (result);
}

Vector Vector::operator-(const Vector& vec) const {

  Vector result(d);
  int n = d;
  if (d != vec.d) {
    cerr << "Vector-: vectors have different length" << endl;
    exit(1);
  }
  while (n--) result.v[n] = v[n] - vec.v[n];
  return (result);
}

Vector Vector::operator*(double x) const {

  Vector result(d);
  int n = d;
  while (n--) result.v[n] = v[n] * x;
  return (result);
}

Vector Vector::operator/(double x) const {

  Vector result(d);
  int n = d;
  while (n--) result.v[n] = v[n] / x;
  return (result);
}

Vector& Vector::operator+=(const Vector& vec) {

  int n = d;
  if (d != vec.d) {
    cerr << "Vector+=: vectors have different length" << endl;
    exit(1);
  }
  while (n--) v[n] += vec.v[n];
  return (*this);
}

Vector& Vector::operator-=(const Vector& vec) {

  int n = d;
  if (d != vec.d) {
    cerr << "Vector-=: vectors have different length" << endl;
    exit(1);
  }
  while (n--) v[n] -= vec.v[n];
  return (*this);
}

Vector& Vector::operator/=(double x) {

  int n = d;
  if (x == 0) {
    cerr << "Vector/=: divided by zero" << endl;
    exit(1);
  }
  while (n--) v[n] /= x;
  return (*this);
}

Vector& Vector::operator*=(double x) {

  int n = d;
  while (n--) v[n] *= x;
  return (*this);
}

Vector Vector::operator-() const {

  Vector result(d);
  int n = d;
  while (n--) result.v[n] = -v[n];
  return (result);
}

double Vector::operator*(const Vector& vec) const {

  if (d != vec.d) {
    cerr << "Vector*: vectors have different length" << endl;
    exit(1);
  }
  double result  = 0;
  int n = d;
  while (n--) result = result + v[n] * vec.v[n];
  return (result);
}

Vector& Vector::operator=(const Vector& vec) {

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

int Vector::operator==(const Vector& vec)  const {

  if (vec.d != d) return (0);
  int i = 0;
  while ((i < d) && (v[i]==vec.v[i])) i++;
  return ((i == d) ? 1: 0);
}

Vector Vector::abs() const {

  Vector result(d);
  for(int i = 0; i < d; i++)
      result[i]=fabs(v[i]);
  return (result);
}

Vector& Vector::resize(int new_dim) {

  if(d != new_dim) {
    delete v;
    v = new double [new_dim];
    d = new_dim;
  }
  while (new_dim--) v[new_dim] = 0.0;
  return (*this);
}

void  Vector::swap_elem(int m,int n) {

  double tmp = v[m];
  v[m] = v[n];
  v[n] = tmp; 
}

/**************        friends        *********************/

ostream& operator<<(ostream& out, const Vector& v) {

  for (int i = 0; i < v.d; i++)
    //out << v.v[i] << ' ' << endl;
    out << " [" << i << "] = " << v.v[i] << ' ' << endl;
  return (out);
}

istream& operator>>(istream& in, Vector& x) {

  int i = 0;
  while ((i < x.d) && (in >> x.v[i++]));
  return (in);
}

Vector operator*(double f, const Vector& v) {

  return (v*f);
}

double max(const Vector& vec) {
  
  int n = vec.dim();
  double c = vec[n-1];  
  while (n--) c = (c > vec[n] ? c : vec[n]); 
  return c;
}

double min(const Vector& vec) {
  
  int n = vec.dim();
  double c = vec[n-1];
  while (n--) c = (c < vec[n] ? c : vec[n]);
  return c;
}

int max_index(const Vector& vec) {

  int n = vec.dim();
  int res = n-1;
  double c = vec[n-1];  
  while (n--) {
     c = (c > vec[n] ? c : vec[n]); 
     res = (c > vec[n] ? res : n); 
  }
  return res;
}

int min_index(const Vector& vec) {

  int n = vec.dim();
  int res = n-1;
  double c = vec[n-1];  
  while (n--) {
     c = (c < vec[n] ? c : vec[n]);
     res = (c < vec[n] ? res : n); 
  }
  return res;
}

/**************     nonfriends        *********************/

double norm(const Vector& v) {

  double result = 0;
  int n = v.dim();
  while (n--) result += v[n];
  return (result);
}

double norm2(const Vector& v) {

  double result = 0;
  int n = v.dim();
  while (n--) result += v[n]*v[n];
  return (sqrt(result));
}

Vector operator^(const Vector& v,const Vector& w) {

  if (v.dim() != w.dim()) {
    cerr << "Vector=: vectors have different length" << endl;
    exit(1);
  }
  int n = v.dim();
  Vector result(n, 0.0);
  while (n--) result[n] = v[n] * w[n];
  return (result);
}
