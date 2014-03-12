#ifndef _IVectorl__h_
#define _IVectorl__h_
#include <stdlib.h>
#include <math.h>
#include <iostream.h>

class IVectorl
{
  friend class IMatrix;
  friend ostream& operator<<(ostream&, const IVectorl&);
  friend IVectorl operator*(int, const IVectorl&);

public:

  IVectorl(int = 0);
  IVectorl(int, int);
  IVectorl(const IVectorl&);

 ~IVectorl()                            { if(v) delete []v; }

  int        dim()                        const { return d; }
  void       flip();
  IVectorl&  resize(int nn);

  IVectorl&   operator=(const IVectorl&);
  IVectorl&   operator=(int);
  int&        operator[](int);
  int         operator[](int)              const;
  IVectorl    operator+(const IVectorl&)    const;
  IVectorl    operator-(const IVectorl&)    const;
  IVectorl&   operator+=(const IVectorl&);
  IVectorl&   operator-=(const IVectorl&);
  IVectorl    operator*(int)               const;
  IVectorl    operator/(int)               const;
  int         operator*(const IVectorl&)    const;
  IVectorl    operator-()                  const;
  int         operator==(const IVectorl&)   const;
  int         operator!=(const IVectorl& w) const { return !(*this == w); }

private:

  int* v;      //            v(0:d-1) 
  int        d;
};

inline int IVectorl::operator[](int i) const {

  if (i < 0 || i > d - 1) {
    cerr << "IVectorl: index " << i << " out of range" << endl;
    exit(1);
  }
  return (v[i]);
}

inline int& IVectorl::operator[](int i) {

  if (i < 0 || i > d - 1) {
    cerr << "IVectorl: index " << i << " out of range" << endl;
    exit(1);
  }
  return (v[i]);
}

inline IVectorl IVectorl::operator+(const IVectorl& vec) const {

  IVectorl result(d);
  int n = d;
  if (d != vec.d) {
    cerr << "IVectorl+: IVectorls have different length" << endl;
    exit(1);
  }
  while (n--) result.v[n] = v[n] + vec.v[n];
  return (result);
}

inline IVectorl IVectorl::operator-(const IVectorl& vec) const {

  IVectorl result(d);
  int n = d;
  if (d != vec.d) {
    cerr << "IVectorl-: IVectorls have different length" << endl;
    exit(1);
  }
  while (n--) result.v[n] = v[n] - vec.v[n];
  return (result);
}

inline IVectorl IVectorl::operator*(int x) const {

  IVectorl result(d);
  int n = d;
  while (n--) result.v[n] = v[n] * x;
  return (result);
}

inline IVectorl IVectorl::operator/(int x) const {

  IVectorl result(d);
  int n = d;
  while (n--) result.v[n] = v[n] / x;
  return (result);
}

inline IVectorl& IVectorl::operator+=(const IVectorl& vec) {

  int n = d;
  if (d != vec.d) {
    cerr << "IVectorl+=: IVectorls have different length" << endl;
    exit(1);
  }
  while (n--) v[n] += vec.v[n];
  return (*this);
}

inline IVectorl& IVectorl::operator-=(const IVectorl& vec) {

  int n = d;
  if (d != vec.d) {
    cerr << "IVectorl-=: IVectorls have different length" << endl;
    exit(1);
  }
  while (n--) v[n] -= vec.v[n];
  return (*this);
}

inline IVectorl IVectorl::operator-() const {

  IVectorl result(d);
  int n = d;
  while (n--) result.v[n] = -v[n];
  return (result);
}

inline int IVectorl::operator*(const IVectorl& vec) const {

  if (d != vec.d) {
    cerr << "IVectorl*: IVectorls have different length" << endl;
    exit(1);
  }
  int result  = 0;
  int n = d;
  while (n--) result = result + v[n] * vec.v[n];
  return (result);
}

inline IVectorl& IVectorl::operator=(const IVectorl& vec) {

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

inline IVectorl& IVectorl::operator=(int a) {

  int n = d;
  while (n--) v[n] = a;
  return (*this);
}

inline int IVectorl::operator==(const IVectorl& vec)  const {

  if (vec.d != d) return (0);
  int i = 0;
  while ((i < d) && (v[i]==vec.v[i])) i++;
  return ((i == d) ? 1: 0);
}

inline int MAX_L(int x,int y)                  { return (x > y ? x : y); }

int norm(const IVectorl& v);
IVectorl operator^(const IVectorl& v, const IVectorl& w);



#endif
