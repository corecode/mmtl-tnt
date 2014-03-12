/******* functions, needed for sparse matrices  *******/

#include <math.h>
#include <iostream.h>
#include <stdlib.h>
#include <iomanip.h>

#include "complex.h" 
#include "vector.h" 
#include "matrix.h" 
#include "cmplxvec.h" 
#include "cmplxmat.h"
#include "ivectorl.h"

#include "sparse.h"

//#include "stdafx.h"


void  sprsin_d( Matrix& a, double thresh, Vector& sa, IVectorl& ija ) {

    const int  ns = a.dim_i();
    int        i, j, k, nh;
     
    nh = ns + 1;

    for( i = 0; i < ns; i++ ) {

        for( j = 0; j < ns; j++ ) {

            if( fabs( a(i,j) ) > thresh && i != j )
               nh += 1;
        }
    }

    sa.resize( nh );
    ija.resize( nh );
    
    for( j = 0; j < ns; j++ )
        sa[j] = a(j,j);

    ija[0] = ns + 1;
    k = ns;

    for( i = 0; i < ns; i++ ) {

        for( j = 0; j < ns; j++ ) {

	    if( fabs( a(i,j) ) > thresh && i != j ) {

               k += 1;
               sa[k] = a(i,j);
               ija[k] = j;
            }               
        }

        ija[i+1] = k + 1;
    }

    a.resize(0,0);             
}    
   

void  sprsax_d( Vector& sa, IVectorl& ija, Vector& x, Vector& b ) {

    const int  ns = x.dim();
    int        i, k; 

    b.resize(ns); 

    if( ija[0] != ns + 1 ) {

       cerr << " Something is wrong in sprsax_d !!! " << endl;
       exit(1);
    }

    for( i = 0; i < ns; i++ ) {
        
      b[i] = sa[i] * x[i]; //cerr << i << endl;

        for( k = ija[i]; k <= ija[i+1]-1; k++ ) {

	  //cerr << k << endl;
 	    b[i] += sa[k] * x[ija[k]]; 
        }
    }
}


void  sprsin_c( CmplxMatrix& a, double thresh, CmplxVector& sa, IVectorl& ija ) {

    const int  ns = a.dim_i();
    int        i, j, k, nh;
     
    nh = ns + 1;

    for( i = 0; i < ns; i++ ) {

        for( j = 0; j < ns; j++ ) {

            if( cabs( a(i,j) ) > thresh && i != j )
               nh += 1;
        }
    }

    sa.resize( nh );
    ija.resize( nh );
    
    for( j = 0; j < ns; j++ )
        sa[j] = a(j,j);

    ija[0] = ns + 1;
    k = ns;

    for( i = 0; i < ns; i++ ) {

        for( j = 0; j < ns; j++ ) {

	    if( cabs( a(i,j) ) > thresh && i != j ) {

               k += 1;
               sa[k] = a(i,j);
               ija[k] = j;
            }               
        }

        ija[i+1] = k + 1;
    }

    a.resize(0,0);             
}    
   

void  sprsax_c( CmplxVector& sa, IVectorl& ija, CmplxVector& x, CmplxVector& b ) {

    const int  ns = x.dim();
    int        i, k; 

    b.resize(ns); 

    if( ija[0] != ns + 1 ) {

       cerr << " Something is wrong in sprsax_c !!! " << endl;
       exit(1);
    }

    for( i = 0; i < ns; i++ ) {
        
        b[i] = sa[i] * x[i];

        for( k = ija[i]; k <= ija[i+1]-1; k++ ) {

	    b[i] += sa[k] * x[ija[k]];
        }
    }
}


Vector  sprsax_d_v( Vector& sa, IVectorl& ija, Vector& x ) {

    Vector res;

    sprsax_d( sa, ija, x, res );
 
    return( res );
}


CmplxVector  sprsax_c_v( CmplxVector& sa, IVectorl& ija, CmplxVector& x ) {

    CmplxVector res;

    sprsax_c( sa, ija, x, res );
 
    return( res );
}
