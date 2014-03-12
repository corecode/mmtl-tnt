#ifndef _Hankel__h_
#define _Hankel__h_

double  recurrence( int, int, double, double* );
int     hank01( double&, double&, double, int );
int     hank11( double&, double&, double, int );

Complex Hank20( double );
Complex Hank21( double );

Complex Hank10( double );
Complex Hank11( double );

#endif
