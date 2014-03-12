#ifndef _adfunc__h_
#define _adfunc__h_

#include "kelvin.h"

double   sqv(double);
double   csft(double);
double   sidelen(double, double, double, double);
double   traparea(int);
int      recgeom(int, double&, double&, double&);
int      cirgeom(int, double&, double&, double& , double&);
int      trageom(int, double&, double&, double&);
void     getsbs(Matrix&);
void     intpnt(double, Vector&, Vector&, Vector&, Vector&,
                double, Vector&, Vector&, Vector&, Vector&, 
                double); 
Complex  kernel_v0(double&, double&, int, int);
Complex  kernel_u0(double&, double&, int, int); 
Complex  kernel_vi(double&, double&, int, int); 
Complex  kernel_ui(double&, double&, int, int); 
double   g0(double, double, double, double); 
double   g0x(double, double, double, double); 
double   g0y(double, double, double, double); 
double   g0mn(int, int, double, double);
double   g0pmn(int, int, double, double); 
double   gir(double, double, double, double);
double   girx(double, double, double, double);
double   giry(double, double, double, double); 
double   girmn(int, int, double, double); 
double   girpmn(int, int, double, double);
double   gii(double, double, double, double); 
double   giix(double, double, double, double);
double   giiy(double, double, double, double); 
double   giimn(int, int, double, double); 
double   giipmn(int, int, double, double); 
Complex  gimn(int, int, double, double); 
Complex  gipmn(int, int, double, double); 
Vector   tangle( double, double, double, double, double, double);

inline double sqv( double x ) {

    return( x*x );

}

inline double csft( double x ) {

    if( x < 0.0 ) {
      return( x + 1.0 );
    } else if( x >= 1.0 ) {
      return( x - 1.0 );
    } else {
      return( x );
    }

}

/* To calculate the length of a side */
inline double sidelen(double x1, double y1, double x2, double y2 ) {

  return(sqrt(sqv(x1-x2)+sqv(y1-y2)));

}


#endif
