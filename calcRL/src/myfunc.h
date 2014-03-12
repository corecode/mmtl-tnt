#ifndef _myfunc__h
#define _myfunc__h


double sqr( double );
double power( double, int );
double power( int, int );
double power( double, double );

double integ( Vector& );

inline double power( double a, int n ) {
 
    double ans = 1.0;
 
    for( int i = 0; i < n; i++ ) 
        ans *= a;

    return ans;
}


inline double power( int a, int n ) {
 
    double ans = 1.0;
    double fa = (double) a;

    if( n == 0 ) 
       return 1.0;
    else if( n > 0 )
       for( int i = 0; i < n; i++ ) 
           ans *= fa;
    else
       for( int i = 0; i < -n; i++ ) 
           ans *= 1.0 / fa;

    return ans;
}  


inline double power( double a, double n ) {
 
    if( a == 0.0 ) {

       if( n == 0.0 ) 
          return 1.0;
       else 
          return 0.0;
    }

    return exp( n * log(a) );
}


inline double integ( Vector& w ) {

    double  x, sum=0.0;
    int N = w.dim();

    for( int i = 0; i < N; i++ )
        sum += w[i];
    
    sum = (sum - (w[0] + w[N-1]) / 2.0);

    return sum;
}



#endif 
