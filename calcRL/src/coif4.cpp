/*****************************************************************/
/******   Coifman wavelet and scaling function ( L = 4 )   *******/
/*****************************************************************/

//#include "stdafx.h"
#include <math.h>
#include <stdlib.h>
#include "vector.h"
#include "matrix.h"
#include "myfunc.h"
#include "coif4.h"


/*****************************************************************/

void getcoif4( Vector& sxc, Vector& scc, Vector& wxc, Vector& wlc, int nit ) {

    const int nh = int((scc.dim()-1)/11.0);
    const int nd = 11*nh+1;
    const int ns = 4*nh;
    const int nw = 5*nh;
    int i, j, k, m;
    double h, x, sum;
    Vector scal(nd,0.0), wavl(nd,0.0), ftmp(nd,0.0);
  

    /**********************************************************/
  
    h = 1.0/double(nh);
   

    /********************* filter weghts **********************/   

    double fw[] = { 0.011587596739,-0.029320137980,-0.047639590310,
                    0.273021046535, 0.574682393857, 0.294867193696,
                   -0.054085607092,-0.042026480461, 0.016744410163,
                    0.003967883613,-0.001289203356,-0.000509505399 };

   
    /*************** starting of the iteration ****************/

    for(i = 0; i <= nh; i++) {
      double ih = double (i) * h;

      scal[i+ns-nh] = ih;
      //    for(i = 1; i <= nh; i++) 
      if ( i > 0 )
	scal[i+ns] = 1.0 - ih;  
    }

    /******** iterative procedure for scaling function ********/
   
    
    for( i = 0; i <= nit; i++ ) {

        for( j = -ns; j < nd-ns; j++ ) {
	  
            sum = 0.0;
	    
	    for( k = -4; k <= 7; k++ ) {

                m = 2*j-k*nh+ns;
				
                if( m >= 0 && m < nd ) 
		   sum += fw[k+4]*scal[m];
	    }

	    ftmp[j+ns] = sum*2.0;
	}

        scal = ftmp;
    }      


    /************************* wavelet ************************/
    for( j = -nw; j < nd-nw; j++ ) {

        sum = 0.0;

        for( k = -6; k <= 5; k++ ) {

       	    m = 2*j-k*nh+ns;

            if( m >= 0 && m < nd ) 
               sum += fw[1-k+4]*scal[m]*pow( -1.0, double(k+1) );
        }       
       
        wavl[j+nw] = sum*2.0;
    }


    /**************** to print obtained data  *****************/
   
    for( i = 0; i < nd; i++ ) {
     
        x = double(i-ns)*h;
        sxc[i] = x;        
        scc[i] = scal[i];
 
	//    } 
	//
	//	for( i = 0; i < nd; i++ ) {
  
        x = double(i-nw)*h;
	wxc[i] = x;
        wlc[i] = wavl[i]; 
    }
}


/*****************************************************************/ 

double scalcoif4( double x, int m, int n, Vector& sc ) {

    const int ns = sc.dim()-1; 
    int i;
    double xt, a, b, h, res;

    xt = pow( 2.0, double(m) )*x-double(n);

    if( xt >= -4.0 &&  xt <= 7.0 ) {

        h = 11.0/double(ns);
        i = int((xt+4.0)*double(ns)/11.0); 
      
        if( i+1 == ns+1 ) a = -sc[i]/h;
        else a = (sc[i+1]-sc[i])/h;

        b = sc[i]-a*double(i)*h;
        res = a*(xt+4.0)+b;
    } 

    else res = 0.0;

    res *= pow( sqrt(2.0), double(m) );

    return( res );
}


/*****************************************************************/ 

double wavlcoif4( double x, int m, int n, Vector& wl ) {

    const int ns = wl.dim()-1; 
    int i;
    double xt, a, b, h, res;

    xt = pow( 2.0, double(m) )*x-double(n);

    if( xt >= -5.0 && xt <= 6.0 ) {

        h = 11.0/double(ns);
        i = int((xt+5.0)*double(ns)/11.0); 
      
        if( i+1 == ns+1 ) a = -wl[i]/h;
        else a = (wl[i+1]-wl[i])/h;

        b = wl[i]-a*double(i)*h;
        res = a*(xt+5.0)+b;      
    } 

    else res = 0.0;

    res *= pow( sqrt(2.0), double(m) );

    return( res );
}


/*****************************************************************/
/******         Coifman  scaling functions (L = 4)          ******/
/******                 on the real line                    ******/
/*****************************************************************/

void coiflet_line( const   int J,     // scaling functions level
                   Vector& qbase ) {  // contains scaling functions 
                                      // and wavelets
 
    Vector wvlt(qbase), scln(qbase);
    int    k;
    int    nit = 40;                      // number of iterations 
    int    nh  = (qbase.dim() - 1) / 11;  // number of subdivisions 
                                          // of the unit 

    coifman4( nh, nit, J, scln, wvlt );
    qbase = scln;
} 


void coifman4( const int nh, const int NIT, const int J, Vector& sbase, Vector& wbase ) {

    double h[]= { 0.011587596739,-0.029320137980,-0.047639590310,
                  0.273021046535, 0.574682393857, 0.294867193696,
                 -0.054085607092,-0.042026480461, 0.016744410163,
                  0.003967883613,-0.001289203356,-0.000509505399 };
 
    Vector hf(12,h);
    Matrix wvlt(11*nh+1,2);

    double sum;

    int i, it, n;
    int nit1, nit2, i0, i0p, i0m, nh4 = 4*nh, nh7 = 7*nh;
   
    
    /********* initial values for the iteration process *******/

    wvlt(nh4,0) = 1.0;

    double dnh = (double) nh;
    for( i = 1; i <= nh; i++ ) {

        wvlt((i + nh4),0) = wvlt((-i + nh4),0) = 1.0 - (double) i / dnh;
    }  


    /********* iteration procedure for scaling function *******/

    for( it = 1; it <= NIT; it++ ) {

         nit2 = (int) fmod(double(it),2);
         nit1 = 1 - nit2;

         for( i = -nh4; i <= nh7; i++ ) {

             sum = 0.0;
	     int i2 = 2 * i;

             for( n = -4; n <= 7; n++ ) {

                 i0 = i2 - n*nh;

                 if( ( i0 <= nh7 ) && ( i0 >= -nh4 ) ) 
                    sum+= hf[n+4] * wvlt(nh4+i0,nit1) * 2.0; 
             }

             wvlt(nh4+i,nit2) = sum;
         }
    }


    /********* set scaling function ***************************/

    sbase = wvlt.col(nit2) * pow( 2.0, double(J)/2.0 );  


    /********* set wavelet ************************************/
    double vl = pow ( 2.0, double (J) / 2.0);
    for( i = -6*nh; i <= 5*nh; i++ ) {

        sum = 0.0;  
	int i2 = 2 * i;

        for( n = -6; n <= 5; n++ ) {

            i0 = i2 - n*nh;
	    
            if( ( i0 <= nh7 ) && ( i0 >= -nh4 ) ) 
               sum+= hf[1-n+4] * wvlt(nh4+i0,nit2) * 2.0 * pow( -1.0, double(n+1) ); 
        }

        wbase[6*nh+i] = sum * vl;
    }
}

/*****************************************************************/

void coiflet_line_new( const int J,   
                       int&      n0, 
                       Vector&   qbase ) {  
                                      
 
    Vector wvlt(qbase), scln(qbase);
    int    k;
    int    nit = 40;                     
    int    nh  = (qbase.dim() - 1) / 11;

    n0 = int( power( 2, J ) * nh );

    coifman4( nh, nit, J, scln, wvlt );
    qbase = scln;
} 


/*****************************************************************/ 

double scalcoif4_new( double x, int& n0, double& ax, double& bx, Vector& sc ) {
    
    int    ix1, ix2;  
    double xa, a, b, ba;
    double res = 0.0;

    xa = x  - ax;
    ba = bx - ax;

    if( ( xa < ba ) && ( xa > 0.0 ) ) {

       ix1 = (int) ( xa * n0 );
       ix2 = ix1 + 1;

       if( ix2 == sc.dim() ) a = -sc[ix1] * n0; 
       else a = (sc[ix2] - sc[ix1]) * n0;

       b   =  sc[ix1] - a * (double) ix1 / n0 ;
       res = a * xa + b;
    }

    return( res );
}


