#ifndef _coif4__h_
#define _coif4__h_


void getcoif4( Vector&, Vector&, Vector&, Vector&, int );
double scalcoif4( double, int, int, Vector& );
double wavlcoif4( double, int, int, Vector& ); 


void coiflet_line( const int, Vector& );
void coifman4( const int, const int, const int, Vector&, Vector& );


void coiflet_line_new( const int, int&, Vector& );
double scalcoif4_new( double, int&, double&, double&, Vector& );


#endif
