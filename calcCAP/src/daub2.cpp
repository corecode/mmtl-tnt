/*****************************************************************/
/****** Daubechies' wavelet and scaling function ( N = 2 ) *******/
/*****************************************************************/

//#include "stdafx.h"
#include <math.h>
#include <stdlib.h>
#include "vector.h"
#include "myfunc.h"
#include "daub2.h"


/*****************************************************************/ 

void getdaub2( Vector& sx, Vector& sc, Vector& wx, Vector& wl, int nit ) {

    const int nh = int((sc.dim()-1)/3.0);
    const int nd = 3*nh+1;
    const int nw = nh;
    int       i, j, m, n;
    double    h, x, sum;
    Vector    c(4);
    Vector    scal(nd,0.0), wavl(nd,0.0), ftmp(nd,0.0);

   
    /*************************************************/
  
    h = 1.0/nh;
     

    /***************** filter weghts *****************/   

    c[0] = (1.0+sqrt(3.0))/4.0;
    c[1] = (3.0+sqrt(3.0))/4.0;
    c[2] = (3.0-sqrt(3.0))/4.0;
    c[3] = (1.0-sqrt(3.0))/4.0;

   
    /************ starting of the iteration ***********/

    for( i = 0; i <= nh; i++ ) scal[i]    = double(i)*h;
    for( i = 1; i <= nh; i++ ) scal[i+nh] = 1.0-double(i)*h;  

      
    /***** iterative procedure for scaling function ***/
   
    for( i = 0; i <= nit; i++ ) {

        for( j = 0; j < nd; j++ ) {

            sum = 0.0;           		    
	       
  	    for( n = 0; n <= 3; n++ ) {

	        m = 2*j-n*nh;

                if( m >= 0 && m <= nd )
                   sum += c[n]*scal[m];	       
            }

	    ftmp[j] = sum; 
        } 

        scal = ftmp; 
    }      

   
    /******************** wavelet  ********************/

    for( j = -nw; j < nd-nw; j++ ) {
  
        sum = 0.0;

        for( n = -2; n <= 1; n++ ) {
 	   
            m = 2*j-n*nh;

	    if( m >= 0 && m < nd )
   	       sum += c[1-n]*scal[m]*pow( -1.0, double(n) );	       		 
        }

        wavl[j+nw] = sum;
    }

    
    /************* to create data vectors *************/
   
    for( i = 0; i< nd; i++ ) {

        x = double(i)*h;
        sx[i] = x;
        sc[i] = scal[i];

    }	  

    for( i = 0; i < nd; i++ ) {

        x = double(i-nw)*h;
        wx[i] = x;
        wl[i] = wavl[i];
    } 
}


/*****************************************************************/ 

double scaldaub2( double x, int m, int n, Vector& sc ) {

    const int ns = sc.dim()-1; 
    int i;
    double xt, a, b, h, res;

    xt = pow( 2.0, double(m) )*x-double(n);
   
    if( xt >= 0.0 && xt <= 3.0 ) {

       h = 3.0/double(ns);
       i = int(xt*double(ns)/3.0); 
      
       if( i+1 == ns+1 ) a = -sc[i]/h;
       else a = (sc[i+1]-sc[i])/h;

       b = sc[i]-a*double(i)*h;
       res = a*xt+b;      
    } 

    else res = 0.0;

    res *= pow( sqrt(2.0), double(m) );

    return( res );
}


/*****************************************************************/ 

double wavldaub2( double x, int m, int n, Vector& wl ) {

    const int ns = wl.dim()-1; 
    int i;
    double xt, a, b, h, res;

    xt = pow( 2.0, double(m) )*x-double(n);

    if( xt >= -1.0 && xt <= 2.0 ) {

       h = 3.0/double(ns);
       i = int((xt+1.0)*double(ns)/3.0); 
      
       if( i+1 == ns+1 ) a = -wl[i]/h;
       else a = (wl[i+1]-wl[i])/h;

       b = wl[i]-a*double(i)*h;
       res = a*(xt+1.0)+b;      
    } 

    else res = 0.0;

    res *= pow( sqrt(2.0), double(m) );

    return( res );
}


/*****************************************************************/ 

void getdaub2new( Vector& sxn, Vector& scn, Vector& scd ) {

    const int  nh  = int( ( scd.dim() - 1 ) / 3.0 );
    const int  ns  = 8 * nh;
    int        i, j;
    double     gm, cf0, cf1, h;
    
    gm  = -1.0 / sqrt( 3.0 );
    cf0 =  2.0 * gm / ( gm - 1.0 );
    cf1 =  ( 1.0 + gm ) / ( 1.0 - gm );
    h   =  1.0 / double( nh );
    
    sxn.resize(ns+1);

    scn.resize(ns+1);

    for( i = 0; i <= ns; i++ ) {
 
        sxn[i] = -1.0 + double( i ) * h;

        for( j = 0; j <= 100; j++ ) {

            scn[i] += power( cf1, double(j) )* 
                      scaldaub2( sxn[i]+double(1-j), 0, 0, scd );
	}
    }

    scn *= cf0;
}


double scaldaub2new( double x, int n, Vector& sc ) {
    
    const int ns = sc.dim()-1; 
    int       i;
    double    xt, a, b, h, res;

    xt = x - double( n );
    
    if( xt >= -1.0 && xt <= 7.0 ) {

       h = 8.0 / double( ns );    
       i = int( ( xt + 1.0 ) * double( ns ) / 8.0 ); 
      
       if( i+1 == ns+1 ) a = -sc[i] / h;
       else a = ( sc[i+1] - sc[i] ) / h;

       b = sc[i] - a * double( i ) * h;
       res = a * ( xt + 1.0 ) + b;      
    } 

    else res = 0.0;

    return( res );
}


// void getdaub2( Vector& sx, Vector& sc, Vector& wx, Vector& wl, int nit ) {

//     const int nh = int((sc.dim()-1)/3.0);
//     const int ns = 5*nh;
//     int i, j, i0, n;
//     double h, x, sum;
//     Vector c(4), F1(10*nh+1,0.0), F2(10*nh+1,0.0);

   
//     /*************************************************/
  
//     h = 1.0/double(nh);
     

//     /***************** filter weghts *****************/   

//     c[0] = (1.0+sqrt(3.0))/4.0;
//     c[1] = (3.0+sqrt(3.0))/4.0;
//     c[2] = (3.0-sqrt(3.0))/4.0;
//     c[3] = (1.0-sqrt(3.0))/4.0;

   
//     /************ starting of the iteration ***********/

//     for( i = 0; i <= nh; i++ ) F1[i+ns] = double(i)*h;
//     for( i = 1; i <= nh; i++ ) F1[i+ns+nh] = 1.0-double(i)*h;  

      
//     /***** iterative procedure for scaling function ***/
   
//     for( i = 0; i <= nit; i++ ) {

//         for( j = 0; j <= 3*nh; j++ ) {

//             sum = 0.0;           		    
	       
//   	    for( n = 0; n <= 3; n++ ) {

// 	        i0 = 2*j-n*nh;

//                 if( i0 >= 0 && i0 <= 3*nh )
//                    sum += c[n]*F1[i0+ns];	       
//             }

// 	    F2[j+ns] = sum; 
//         } 

//         F1 = F2; 
//     }      

   
//     /******************** wavelet  ********************/

//     for( j = -nh; j <= 2*nh; j++ ) {
  
//         sum = 0.0;

//         for( n = -2; n <= 1; n++ ) {
 	   
//             i0 = 2*j-n*nh;

// 	    if( i0 >= 0 && i0 <= 3*nh )
//    	       sum += c[1-n]*F1[i0+ns]*pow( -1.0, double(n) );	       		 
//         }

//         F2[j+ns] = sum;
//     }

    
//     /************* to create data vectors *************/
   
//     for( i = 0; i<= 3*nh; i++ ) {

//         x = double(i)*h;
//         sx[i] = x;
//         sc[i] = F1[i+ns];
//     }	  

//     for( i = -nh; i <= 2*nh; i++ ) {

//         x = double(i)*h;
//         wx[i+nh] = x;
//         wl[i+nh] = F2[i+ns]; 
//     } 
// }

