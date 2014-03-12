#include <stdlib.h>
#include <math.h>
#include "ivectorl.h"
//#include "stdafx.h"

/*************************** constructors **************************/

IVectorl::IVectorl(int n)  {

  if (n < 0) {
    cerr << "IVectorl: negative dimension." << endl;
    exit(1);
  }
  d  = n;
  if (d > 0) {
    v  = new int[d];
    while (n--) v[n] = 0;
  }
  else v = NULL;
}

IVectorl::IVectorl(int n, int v0) {

  if (n <= 0) {
    cerr << "IVectorl: undefined dimension." << endl;
    exit(1);
  }
  d  = n;
  v  = new int[d];
  while (n--) v[n] = v0;
}

IVectorl::IVectorl(const IVectorl& v0) {

  d = v0.d;
  if (d > 0) {
    v = new int[d];
    for(int i = 0; i < d; i++) v[i] = v0.v[i];
  }
  else v = NULL;
}

/************************* members ****************************/

/************************************************

int IVectorl::operator[](int i) const {

  if (i < 0 || i > d - 1) {
    cerr << "IVectorl: index " << i << " out of range" << endl;
    exit(1);
  }
  return (v[i]);
}

int& IVectorl::operator[](int i) {

  if (i < 0 || i > d - 1) {
    cerr << "IVectorl: index " << i << " out of range" << endl;
    exit(1);
  }
  return (v[i]);
}

IVectorl IVectorl::operator+(const IVectorl& vec) const {

  IVectorl result(d);
  int n = d;
  if (d != vec.d) {
    cerr << "IVectorl+: IVectorls have different length" << endl;
    exit(1);
  }
  while (n--) result.v[n] = v[n] + vec.v[n];
  return (result);
}

IVectorl IVectorl::operator-(const IVectorl& vec) const {

  IVectorl result(d);
  int n = d;
  if (d != vec.d) {
    cerr << "IVectorl-: IVectorls have different length" << endl;
    exit(1);
  }
  while (n--) result.v[n] = v[n] - vec.v[n];
  return (result);
}

IVectorl IVectorl::operator*(int x) const {

  IVectorl result(d);
  int n = d;
  while (n--) result.v[n] = v[n] * x;
  return (result);
}

IVectorl IVectorl::operator/(int x) const {

  IVectorl result(d);
  int n = d;
  while (n--) result.v[n] = v[n] / x;
  return (result);
}

IVectorl& IVectorl::operator+=(const IVectorl& vec) {

  int n = d;
  if (d != vec.d) {
    cerr << "IVectorl+=: IVectorls have different length" << endl;
    exit(1);
  }
  while (n--) v[n] += vec.v[n];
  return (*this);
}

IVectorl& IVectorl::operator-=(const IVectorl& vec) {

  int n = d;
  if (d != vec.d) {
    cerr << "IVectorl-=: IVectorls have different length" << endl;
    exit(1);
  }
  while (n--) v[n] -= vec.v[n];
  return (*this);
}

IVectorl IVectorl::operator-() const {

  IVectorl result(d);
  int n = d;
  while (n--) result.v[n] = -v[n];
  return (result);
}

int IVectorl::operator*(const IVectorl& vec) const {

  if (d != vec.d) {
    cerr << "IVectorl*: IVectorls have different length" << endl;
    exit(1);
  }
  int result  = 0;
  int n = d;
  while (n--) result = result + v[n] * vec.v[n];
  return (result);
}

IVectorl& IVectorl::operator=(const IVectorl& vec) {

  //if (this != &vec) {
  //int n = vec.d;
  //if (d != vec.d) {
  //cerr << "IVectorl=: IVectorls have different length" << endl;
  //exit(1);
  //}
  //while (n--) v[n] = vec.v[n];
  //}

  if (this != &vec) {
   int n = vec.d;
   if (n != d) {
     delete v;
     v = new int [n];
     d = n;
   }
   while (n--) v[n] = vec.v[n];
  }
  return (*this);
}

IVectorl& IVectorl::operator=(int a) {

  int n = d;
  while (n--) v[n] = a;
  return (*this);
}

int IVectorl::operator==(const IVectorl& vec)  const {

  if (vec.d != d) return (0);
  int i = 0;
  while ((i < d) && (v[i]==vec.v[i])) i++;
  return ((i == d) ? 1: 0);
}
****************************************************/

void IVectorl::flip() {

 int el;
 int    half;

 half = d / 2;

 for (int i = 0; i < half; i++) {

   el = v[i]; v[i] = v[d - 1 - i]; v[d - 1 - i] = el;

 }
}

IVectorl& IVectorl::resize(int nn) {

 if (d != nn) {
   delete []v;
   d = nn;
   v = new int[d];
   while (nn--) v[nn] = 0;
 }
 return (*this);
}

/**************        friends        *********************/

ostream& operator<<(ostream& out, const IVectorl& v) {

  for (int i = 0; i < v.d; i++)
    //out << v.v[i] << ' ' << endl;
    out << " [" << i << "] = " << v.v[i] << ' ' << endl;
  return (out);
}

/**************     nonfriends        *********************/

int norm(const IVectorl& v) {

  int result  = 0;
  int n = v.dim();
  while (n--) result = result + v[n];
  return (result);
}

IVectorl operator^(const IVectorl& v,const IVectorl& w) {

  if (v.dim() != w.dim()) {
    cerr << "IVectorl=: IVectorls have different length" << endl;
    exit(1);
  }
  int n = v.dim();
  IVectorl result(n, 0.0);
  while (n--) result[n] = v[n] * w[n];
  return (result);
}
