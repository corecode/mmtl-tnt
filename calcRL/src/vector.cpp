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
