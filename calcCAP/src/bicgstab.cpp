/*****************************************************************/
/***                                                           ***/
/***                 Bi_CGSTAB iterative method                ***/
/***                                                           ***/
/***  A - system matrix, xi - initial guess and output vector, ***/ 
/***  b - right hand side                                      ***/
/***                                                           ***/
/*****************************************************************/


//#include "stdafx.h"
#include <iostream.h>
#include <math.h>

#include "vector.h"
#include "matrix.h"
#include "complex.h"
#include "cmplxvec.h"
#include "cmplxmat.h"
#include "ivectorl.h"

#include "sparse.h"
#include "diffrm.h"

#include "bicgstab.h"


int bi_cgstab_d( Matrix& A, Vector& xi, Vector& b, double EPS ) {

    const int  ns = A.dim_i();
    int        i, maxit  = 2000; 
    int        iflag  = 1, icount = 0;
    double     delta0, deltai;
    double     bet, roi;
    double     roim1 = 1.0, al = 1.0, wi = 1.0;
    Vector     ri, roc; 
    Vector     s(ns,0.0), t(ns,0.0), vi(ns,0.0), pi(ns,0.0);
  
    ri = b - A * xi; 
    roc = ri;   
    delta0 = norm2( ri );
 
    while( iflag != 0 && icount < maxit ) {

       icount += 1;
           
       roi = roc * ri; 
       bet = ( roi / roim1 ) * ( al / wi );  
       pi = ri + ( pi - vi * wi ) * bet;
       vi = A * pi;        
       al = roi / ( roc * vi );  
       s = ri - vi * al;   
       t = A * s;
       wi = ( t * s ) / ( t * t );
       xi += pi * al + s * wi;
       ri = s - t * wi;

       deltai = norm2( ri );        

//        cerr << "   " << icount << "   " 
//             << setprecision(10) 
//             << deltai          << "   "  
//             << delta0          << "   "
//             << deltai/delta0   << endl;      

       if( deltai < EPS * delta0 ) 
          iflag = 0;
       else  
          roim1 = roi;
    }


    cout << endl
         << endl 
         << " Number of iterations = " 
         << icount 
         << endl << endl;

    return( 0 );
}


int bi_cgstab_c( CmplxMatrix& A, CmplxVector& xi, CmplxVector& b, double EPS ) {

    const int    ns = A.dim_i();
    int          i, maxit  = 2000; 
    int          iflag  = 1, icount = 0;
    double       delta0, deltai;
    Complex      bet, roi;
    Complex      roim1 = 1.0, al = 1.0, wi = 1.0;
    CmplxVector  ri, roc; 
    CmplxVector  s(ns,0.0), t(ns,0.0), vi(ns,0.0), pi(ns,0.0);
  
    ri = b - A * xi; 
    roc = ri;   
    delta0 = norm2( ri );
 
    while( iflag != 0 && icount < maxit ) {

       icount += 1;
           
       roi = inner( roc, ri ); 
       bet = ( roi / roim1 ) * ( al / wi );  
       pi = ri + ( pi - vi * wi ) * bet;
       vi = A * pi;        
       al = roi / inner( roc, vi );  
       s = ri - vi * al;   
       t = A * s;
       wi = inner( t, s ) / inner( t, t );
       xi += pi * al + s * wi;
       ri = s - t * wi;

       deltai = norm2( ri );        

 //       cerr << "   " << icount << "   " 
//             << setprecision(10) 
//             << deltai          << "   "  
//             << delta0          << "   "
//             << deltai/delta0   << endl;      

       if( deltai < EPS * delta0 ) 
          iflag = 0;
       else  
          roim1 = roi;
    }


    cerr << endl
	 << endl 
	 << " Number of iterations = " 
	 << icount 
	 << endl << endl;

    return( 0 );
}


int bi_cgstab_d_sparse( Matrix& A, Vector& xi, Vector& b, double EPS, double thresh ) {

    const int  ns = A.dim_i();
    int        i, maxit  = 2000; 
    int        iflag  = 1, icount = 0;
    double     delta0, deltai;
    double     bet, roi;
    double     roim1 = 1.0, al = 1.0, wi = 1.0;
    Vector     ri, roc; 
    Vector     s(ns,0.0), t(ns,0.0), vi(ns,0.0), pi(ns,0.0);

    Vector     sa;
    IVectorl   ija;
  
    sprsin_d( A, thresh, sa, ija ); 

    ri = b - sprsax_d_v( sa, ija, xi ); 
    roc = ri;   
    delta0 = norm2( ri );
 
    while( iflag != 0 && icount < maxit ) {

       icount += 1;
           
       roi = roc * ri; 
       bet = ( roi / roim1 ) * ( al / wi );  
       pi = ri + ( pi - vi * wi ) * bet;
       vi = sprsax_d_v( sa, ija, pi );        
       al = roi / ( roc * vi );  
       s = ri - vi * al;   
       t = sprsax_d_v( sa, ija, s );
       wi = ( t * s ) / ( t * t );
       xi += pi * al + s * wi;
       ri = s - t * wi;

       deltai = norm2( ri );        

       if( deltai < EPS * delta0 ) 
          iflag = 0;
       else  
          roim1 = roi;
    }


    cerr << endl
         << endl 
         << " Number of iterations = " 
         << icount 
         << endl << endl;

    return( 0 );
}


int bi_cgstab_c_sparse( CmplxMatrix& A, CmplxVector& xi, CmplxVector& b, double EPS, double thresh ) {

    const int    ns = A.dim_i();
    int          i, maxit  = 2000; 
    int          iflag  = 1, icount = 0;
    double       delta0, deltai;
    Complex      bet, roi;
    Complex      roim1 = 1.0, al = 1.0, wi = 1.0;
    CmplxVector  ri, roc; 
    CmplxVector  s(ns,0.0), t(ns,0.0), vi(ns,0.0), pi(ns,0.0);

    CmplxVector  sa;
    IVectorl     ija;
  
    sprsin_c( A, thresh, sa, ija ); 
  
    ri = b - sprsax_c_v( sa, ija, xi ); 
    roc = ri;   
    delta0 = norm2( ri );
 
    while( iflag != 0 && icount < maxit ) {

       icount += 1;
           
       roi = inner( roc, ri ); 
       bet = ( roi / roim1 ) * ( al / wi );  
       pi = ri + ( pi - vi * wi ) * bet;
       vi = sprsax_c_v( sa, ija, pi );      
       al = roi / inner( roc, vi );  
       s = ri - vi * al;   
       t =  sprsax_c_v( sa, ija, s );
       wi = inner( t, s ) / inner( t, t );
       xi += pi * al + s * wi;
       ri = s - t * wi;

       deltai = norm2( ri );        

       if( deltai < EPS * delta0 ) 
          iflag = 0;
       else  
          roim1 = roi;
    }


    cerr << endl
         << endl 
         << " Number of iterations = " 
         << icount 
         << endl << endl;

    return( 0 );
}


int bi_cg_d( Matrix& A, Vector& xi, Vector& b, double EPS ) {

    const int  ns = A.dim_i();
    int        i, maxit  = 2000; 
    int        iflag  = 1, icount = 0;
    double     delta0, deltai;
    double     bet, roi;
    double     roim1 = 1.0, al;
    Vector     ri, roc; 
    Vector     vi(ns,0.0), pi(ns,0.0), pih(ns,0.0);
    Matrix     At = A.trans();
  
    ri = b - A * xi; 
    roc = ri;   
    delta0 = norm2( ri );
 
    while( iflag != 0 && icount < maxit ) {
          
       icount += 1;
           
       roi = roc * ri; 
       bet = roi / roim1;  
       pi = ri + pi * bet;
       pih = roc + pih * bet;
       vi = A * pi;        
       al = roi / ( pih * vi );  
       xi += pi * al;
       ri -= vi * al;
       roc -= ( At * pih ) * al;

       deltai = norm2( ri );        

       if( deltai < EPS * delta0 ) 
          iflag = 0;
       else  
          roim1 = roi;         
    }


    cerr << endl
         << endl 
         << " Number of iterations = " 
         << icount 
         << endl << endl;

    return( 0 );
}


int bi_cg_c( CmplxMatrix& A, CmplxVector& xi, CmplxVector& b, double EPS ) {

    const int    ns = A.dim_i();
    int          i, maxit  = 2000; 
    int          iflag  = 1, icount = 0;
    double       delta0, deltai;
    Complex      bet, roi;
    Complex      roim1 = 1.0, al;
    CmplxVector  ri, roc; 
    CmplxVector  vi(ns,0.0), pi(ns,0.0), pih(ns,0.0);
    CmplxMatrix  At = A.trans();
  
    ri = b - A * xi; 
    roc = ri;   
    delta0 = norm2( ri );
 
    while( iflag != 0 && icount < maxit ) {

       icount += 1;
           
       roi = inner( roc, ri ); 
       bet = roi / roim1;  
       pi = ri + pi * bet;
       pih = roc + pih * bet;
       vi = A * pi;        
       al = roi / inner( pih, vi );  
       xi += pi * al;
       ri -= vi * al;
       roc -= ( At * pih ) * al;

       deltai = norm2( ri );        

       if( deltai < EPS * delta0 ) 
          iflag = 0;
       else  
          roim1 = roi;
    }


    cerr << endl
         << endl 
         << " Number of iterations = " 
         << icount 
         << endl << endl;

    return( 0 );
}


int bi_cgstab_d_precond( Matrix& A, Vector& xi, Vector& b, double EPS ) {

    const int  ns = A.dim_i();
    int        i, maxit  = 2000; 
    int        iflag  = 1, icount = 0;
    double     delta0, deltai;
    double     bet, roi;
    double     roim1 = 1.0, al = 1.0, wi = 1.0;
    Vector     mp(ns), y(ns), z(ns), ri, roc; 
    Vector     s(ns,0.0), t(ns,0.0), vi(ns,0.0), pi(ns,0.0);
  
    for( i = 0; i < ns; i++ )
        mp[i] = 1.0 / A(i,i);
    
    ri = b - A * xi; 
    roc = ri;   
    delta0 = norm2( ri );
 
    while( iflag != 0 && icount < maxit ) {

       icount += 1;
           
       roi = roc * ri; 
       bet = ( roi / roim1 ) * ( al / wi );  
       pi = ri + ( pi - vi * wi ) * bet;
       y = mp ^ pi;
       vi = A * y;        
       al = roi / ( roc * vi );  
       s = ri - vi * al;   
       z = mp ^ s;
       t = A * z;
       wi = ( t * s ) / ( t * t );
       xi += y * al + z * wi;
       ri = s - t * wi;

       deltai = norm2( ri );        

       if( deltai < EPS * delta0 ) 
          iflag = 0;
       else  
          roim1 = roi;
    }


    cerr << endl
         << endl 
         << " Number of iterations = " 
         << icount 
         << endl << endl;

    return( 0 );
}


int bi_cgstab_c_precond( CmplxMatrix& A, CmplxVector& xi, CmplxVector& b, double EPS ) {

    const int    ns = A.dim_i();
    int          i, maxit  = 2000; 
    int          iflag  = 1, icount = 0;
    double       delta0, deltai;
    Complex      bet, roi;
    Complex      roim1 = 1.0, al = 1.0, wi = 1.0;
    CmplxVector  mp(ns), y(ns), z(ns), ri, roc; 
    CmplxVector  s(ns,0.0), t(ns,0.0), vi(ns,0.0), pi(ns,0.0);

   
    for( i = 0; i < ns; i++ )
        mp[i] = 1.0 / A(i,i);
  
    ri = b - A * xi; 
    roc = ri;   
    delta0 = norm2( ri );
 
    while( iflag != 0 && icount < maxit ) {

       icount += 1;
           
       roi = inner( roc, ri ); 
       bet = ( roi / roim1 ) * ( al / wi );  
       pi = ri + ( pi - vi * wi ) * bet;
       y = mp ^ pi;
       vi = A * y;        
       al = roi / inner( roc, vi );  
       s = ri - vi * al;   
       z = mp ^ s;
       t = A * z;
       wi = inner( t, s ) / inner( t, t );
       xi += y * al + z * wi;
       ri = s - t * wi;

       deltai = norm2( ri );           

       if( deltai < EPS * delta0 ) 
          iflag = 0;
       else  
          roim1 = roi;
    }


    cerr << endl
         << endl 
         << " Number of iterations = " 
         << icount 
         << endl << endl;

    return( 0 );
}


int bi_cgstab_d_sparse_precond( Matrix& A, Vector& xi, Vector& b, double EPS, double thresh ) {

    const int  ns = A.dim_i();
    int        i, maxit  = 2000; 
    int        iflag  = 1, icount = 0;
    double     delta0, deltai;
    double     bet, roi;
    double     roim1 = 1.0, al = 1.0, wi = 1.0;
    Vector     mp(ns), y(ns), z(ns), ri, roc; 
    Vector     s(ns,0.0), t(ns,0.0), vi(ns,0.0), pi(ns,0.0);
  
    for( i = 0; i < ns; i++ )
        mp[i] = 1.0 / A(i,i);

    Vector     sa;
    IVectorl   ija;
  
    sprsin_d( A, thresh, sa, ija );     

    ri = b - sprsax_d_v( sa, ija, xi ); ; 
    roc = ri;   
    delta0 = norm2( ri );
 
    while( iflag != 0 && icount < maxit ) {

       icount += 1;
           
       roi = roc * ri; 
       bet = ( roi / roim1 ) * ( al / wi );  
       pi = ri + ( pi - vi * wi ) * bet;
       y = mp ^ pi;
       vi = sprsax_d_v( sa, ija, y ); 
       al = roi / ( roc * vi );  
       s = ri - vi * al;   
       z = mp ^ s;
       t = sprsax_d_v( sa, ija, z ); ;
       wi = ( t * s ) / ( t * t );
       xi += y * al + z * wi;
       ri = s - t * wi;

       deltai = norm2( ri );        

       if( deltai < EPS * delta0 ) 
          iflag = 0;
       else  
          roim1 = roi;
    }


    cerr << endl
         << endl 
         << " Number of iterations = " 
         << icount 
         << endl << endl;

    return( 0 );
}


int bi_cgstab_c_sparse_precond( CmplxMatrix& A, CmplxVector& xi, CmplxVector& b, double EPS, double thresh ) {

    const int    ns = A.dim_i();
    int          i, maxit  = 2000; 
    int          iflag  = 1, icount = 0;
    double       delta0, deltai;
    Complex      bet, roi;
    Complex      roim1 = 1.0, al = 1.0, wi = 1.0;
    CmplxVector  mp(ns), y(ns), z(ns), ri, roc; 
    CmplxVector  s(ns,0.0), t(ns,0.0), vi(ns,0.0), pi(ns,0.0);

    for( i = 0; i < ns; i++ )
        mp[i] = 1.0 / A(i,i);

    CmplxVector  sa;
    IVectorl     ija;
  
    sprsin_c( A, thresh, sa, ija ); 
  
    ri = b - sprsax_c_v( sa, ija, xi ); 
    roc = ri;   
    delta0 = norm2( ri );
 
    while( iflag != 0 && icount < maxit ) {

       icount += 1;
           
       roi = inner( roc, ri ); 
       bet = ( roi / roim1 ) * ( al / wi );  
       pi = ri + ( pi - vi * wi ) * bet;
       y = mp ^ pi;
       vi = sprsax_c_v( sa, ija, y );      
       al = roi / inner( roc, vi );  
       s = ri - vi * al;   
       z = mp ^ s;
       t =  sprsax_c_v( sa, ija, z );
       wi = inner( t, s ) / inner( t, t );
       xi += y * al + z * wi;
       ri = s - t * wi;

       deltai = norm2( ri );        

       if( deltai < EPS * delta0 ) 
          iflag = 0;
       else  
          roim1 = roi;
    }


    cerr << endl
         << endl 
         << " Number of iterations = " 
         << icount 
         << endl << endl;

    return( 0 );
}


int bi_cgstab_c_sparse_simple( CmplxMatrix& A, CmplxVector& xi, CmplxVector& b, double EPS ) {

    const int    ns = A.dim_i();
    int          i, maxit  = 2000; 
    int          iflag  = 1, icount = 0;
    double       delta0, deltai;
    Complex      bet, roi;
    Complex      roim1 = 1.0, al = 1.0, wi = 1.0;
    CmplxVector  ri, roc; 
    CmplxVector  s(ns,0.0), t(ns,0.0), vi(ns,0.0), pi(ns,0.0);
  
    ri     = b - sprsax_c_v_simple( A, xi ); 
    roc    = ri;   
    delta0 = norm2( ri );
 
    while( iflag != 0 && icount < maxit ) {

       icount += 1;
           
       roi = inner( roc, ri ); 
       bet = ( roi / roim1 ) * ( al / wi );  
       pi = ri + ( pi - vi * wi ) * bet;
       vi = sprsax_c_v_simple( A, pi );        
       al = roi / inner( roc, vi );  
       s = ri - vi * al;   
       t = sprsax_c_v_simple( A, s );
       wi = inner( t, s ) / inner( t, t );
       xi += pi * al + s * wi;
       ri = s - t * wi;

       deltai = norm2( ri );        

//        cerr << "   " << icount << "   " 
//             << setprecision(10) 
//             << deltai          << "   "  
//             << delta0          << "   "
//             << deltai/delta0   << endl;      

       if( deltai < EPS * delta0 ) 
          iflag = 0;
       else  
          roim1 = roi;
    }


 //    cerr << endl
//          << endl 
//          << " Number of iterations = " 
//          << icount 
//          << endl << endl;

    return( 0 );
}


int bi_cgstab_c_sparse_new( CmplxVector& sa, IVectorl& ija, CmplxVector& xi, CmplxVector& b, double EPS ) {

    const int    ns = b.dim();
    int          i, maxit  = 2000; 
    int          iflag  = 1, icount = 0;
    double       delta0, deltai;
    Complex      bet, roi;
    Complex      roim1 = 1.0, al = 1.0, wi = 1.0;
    CmplxVector  ri, roc; 
    CmplxVector  s(ns,0.0), t(ns,0.0), vi(ns,0.0), pi(ns,0.0);
  
    ri = b - sprsax_c_v( sa, ija, xi ); 
    roc = ri;   
    delta0 = norm2( ri );
 
    while( iflag != 0 && icount < maxit ) {

       icount += 1;
           
       roi = inner( roc, ri ); 
       bet = ( roi / roim1 ) * ( al / wi );  
       pi = ri + ( pi - vi * wi ) * bet;
       vi = sprsax_c_v( sa, ija, pi );      
       al = roi / inner( roc, vi );  
       s = ri - vi * al;   
       t =  sprsax_c_v( sa, ija, s );
       wi = inner( t, s ) / inner( t, t );
       xi += pi * al + s * wi;
       ri = s - t * wi;

       deltai = norm2( ri );        

       if( deltai < EPS * delta0 ) 
          iflag = 0;
       else  
          roim1 = roi;
    }


//     cerr << endl
//          << endl 
//          << " Number of iterations = " 
//          << icount 
//          << endl << endl;

    return( 0 );
}






