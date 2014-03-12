/*****************************************************************/
/******   Coifman wavelet and scaling function ( L = 6 )   *******/
/*****************************************************************/

//#include "stdafx.h"
#include <math.h>
#include <stdlib.h>
#include "vector.h"
#include "coif6.h"


/*****************************************************************/

void getcoif6( Vector& sxc, Vector& scc, Vector& wxc, Vector& wlc, int nit ) {

    const int nh = int((scc.dim()-1)/17.0);
    const int nd = 17*nh+1;
    const int ns = 6*nh;
    const int nw = 8*nh;
    int i, j, k, m;
    double h, x, sum;
    Vector scal(nd,0.0), wavl(nd,0.0), ftmp(nd,0.0); 
  

    /**********************************************************/
  
    h = 1.0/double(nh);
   
    
    /********************* filter weights *********************/   

    double fw[] = { -0.002682418671, 0.005503126709, 0.016583560479,
                    -0.046507764479,-0.043220763560, 0.286503335274,
                     0.561285256870, 0.302983571773,-0.050770140755,
                    -0.058196250762, 0.024434094321, 0.011229240962,
                    -0.006369601011,-0.001820458916, 0.000790205101,
                     0.000329665174,-0.000050192775,-0.000024465734 };

    
    /*************** starting of the iteration ****************/

    for(i = 0; i <= nh; i++) scal[i+ns-nh] = double(i)*h;
    for(i = 1; i <= nh; i++) scal[i+ns] = 1.0-double(i)*h;  
   

    /******** iterative procedure for scaling function ********/
   
    for( i = 0; i <= nit; i++ ) {

        for( j = -ns; j < nd-ns; j++ ) {
	  
            sum = 0.0;
		 
	    for( k = -6; k <= 11; k++ ) {

                m = 2*j-k*nh+ns;     
				
                if( m >= 0 && m < nd ) 
		   sum += fw[k+6]*scal[m];
	    }

	    ftmp[j+ns] = sum*2.0;
	}

        scal = ftmp;
    }      

   
    /************************* wavelet ************************/
   
    for( j = -nw; j < nd-nw; j++ ) {

        sum = 0.0;

        for( k = -10; k <= 7; k++ ) {

       	    m = 2*j-k*nh+ns;

            if( m >= 0 && m < nd ) 
               sum += fw[1-k+6]*scal[m]*pow( -1.0, double(k+1) );
        }       
       
        wavl[j+nw] = sum*2.0;
    }

    
    /**************** to print obtained data  *****************/
   
    for( i = 0; i < nd; i++ ) {
     
        x = double(i-ns)*h;
        sxc[i] = x;        
        scc[i] = scal[i];
    } 

    for( i = 0; i < nd; i++ ) {
  
        x = double(i-nw)*h;
	wxc[i] = x;
        wlc[i] = wavl[i]; 
    }
}


/*****************************************************************/ 

double scalcoif6( double x, int m, int n, Vector& sc ) {

    const int ns = sc.dim()-1; 
    int i;
    double xt, a, b, h, res;

    xt = pow( 2.0, double(m) )*x-double(n);

    if( xt >= -6.0 &&  xt <= 11.0 ) {

        h = 17.0/double(ns);
        i = int((xt+6.0)*double(ns)/17.0); 
      
        if( i+1 == ns+1 ) a = -sc[i]/h;
        else a = (sc[i+1]-sc[i])/h;

        b = sc[i]-a*double(i)*h;
        res = a*(xt+6.0)+b;      
    } 

    else res = 0.0;

    res *= pow( sqrt(2.0), double(m) );

    return( res );
}


/*****************************************************************/ 

double wavlcoif6( double x, int m, int n, Vector& wl ) {

    const int ns = wl.dim()-1; 
    int i;
    double xt, a, b, h, res;

    xt = pow( 2.0, double(m) )*x-double(n);

    if( xt >= -8.0 && xt <= 9.0 ) {

        h = 17.0/double(ns);
        i = int((xt+8.0)*double(ns)/17.0); 
      
        if( i+1 == ns+1 ) a = -wl[i]/h;
        else a = (wl[i+1]-wl[i])/h;

        b = wl[i]-a*double(i)*h;
        res = a*(xt+8.0)+b;      
    } 

    else res = 0.0;

    res *= pow( sqrt(2.0), double(m) );

    return( res );
}

