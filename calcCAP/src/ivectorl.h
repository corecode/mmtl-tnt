#ifndef _IVectorl__h_
#define _IVectorl__h_
#include <iostream.h>

class IVectorl
{
  friend class IMatrix;
  friend class IMatrix3d;
  friend ostream& operator<<(ostream&, const IVectorl&);
  friend IVectorl operator*(int, const IVectorl&); 
  friend int max(const IVectorl&);
  friend int min(const IVectorl&);

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

inline int MAX_L(int x,int y)                  { return (x > y ? x : y); }

int norm(const IVectorl& v);
IVectorl operator^(const IVectorl& v, const IVectorl& w);

#endif
