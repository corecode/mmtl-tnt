#include <math.h>
#include <stdlib.h>
#include "vector.h"
#include "myfunc.h"


//double sqr( double a ) {
//
//    return( a * a );
//}


double power( double a, int n ) {
 
    double ans = 1.0;

    if( n == 0 ) 
       return 1.0;
    else if( n > 0 )
       for( int i = 0; i < n; i++ ) 
           ans *= a;
    else
       for( int i = 0; i < -n; i++ ) 
           ans /= a;

    return ans;
}


double power( int a, int n ) {
 
    double ans = 1.0;
 
    if( n == 0 ) 
       return 1.0;
    else if( n > 0 )
       for( int i = 0; i < n; i++ ) 
           ans *= a;
    else
       for( int i = 0; i < -n; i++ ) 
           ans /= double( a );

    return ans;
}  


double power( double a, double n ) {
 
    if( a == 0.0 ) {

       if( n == 0.0 ) 
          return 1.0;
       else 
          return 0.0;
    }

    return exp( n * log( a ) );
}


double integ( Vector& w ) {

    double  x, sum=0.0;
    int N = w.dim();

    for( int i = 0; i < N; i++ )
        sum += w[i];
    
    sum = (sum - (w[0] + w[N-1]) / 2.0);

    return sum;
}


