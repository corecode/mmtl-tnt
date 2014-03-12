#include "stdafx.h"
#include <math.h>
#include <iomanip.h>
#include <stdlib.h>
#include "complex.h"
#include "vector.h"
#include "matrix.h"
#include "cmplxvec.h"
#include "cmplxmat.h"  
#include "systsolv.h"

extern double  pi, k0, Z0, eps0, mu0;
extern double  mm, epsr, epse, ke;
extern double  w, d, h;
extern Complex Ic;
extern int     N;

void prony( Vector xx, Vector yy, CmplxVector& a, CmplxVector& c ) {

  int M = xx.dim()/2, i, j, n;
  double dx = fabs(xx[1]-xx[0]), EPS= 0.0, dlu;
  CmplxVector f, xp, cp, alpha, xc, mu;
  CmplxMatrix sys;
  Vector Indx;

  f.resize(M);
  cp.resize(M);
  xp.resize(M);
  sys.resize(M,M);

  for( i = 0; i < M; i ++ ) {
    for( j = 0; j < M; j ++ ) {
      sys(i,j) = yy[M-1+i-j];
    }
    f[i] = -yy[M+i];
  }

  ludcmp( sys, Indx, dlu );
  lubksb( sys, f, Indx );

  alpha.resize(M+1);
  alpha[0] = 1.0;
  for( i = 0; i < M; i ++ ) {
    alpha[i+1] = f[i];
  }
  
  for( n = 0; n < M; n ++ ) {
    CmplxVector ini(3);
    ini[0] = -3.0;
    ini[2] = 3.0;
    ini[1] = (ini[0] + ini[2])/2.0;
    muller( fun, ini, mu, alpha );
  }

  if(mu.dim() != M) {
    cerr << "M=" << M << endl;
    cerr << mu.dim() << endl;
    cerr << mu << endl;
    cerr << "root finding error!" << endl;
    exit(1);
  }
  
  for( i = 0; i < M; i ++ ) {
    for( j = 0; j < M; j ++ ) {
      sys(i,j) = pow(mu[j],double(i));
    }
    f[i] = yy[i];
  }

  ludcmp( sys, Indx, dlu );
  lubksb( sys, f, Indx );
 
  cp = f;

  for( i = 0; i < M; i ++ ) {
    a[i] = log(mu[i])/dx;
    c[i] = cp[i]/exp(a[i]*dx);
  }

}

/*************Function muller - to find complex roots*************/

void muller( Complex (*fn)( CmplxVector, Complex ), CmplxVector v, CmplxVector& zero, CmplxVector& alpha )
{
   const double er = 1.0e-10;
   Complex q, a, b, c, d1, d2, d, tmp;
   CmplxVector x, fx(3), fv(3), tmpz;
   int i, j, noz, cc = 0, nit = 1000;

   if( v[0] == v[1] || v.dim() != 3 ) {

       cerr << "Something is wrong in muller !!!" << endl;
       exit(1); 
   }

   x = v;
   
   noz = zero.dim();

   for(i = 0; i < 3; i++) {
     fx[i] = fn(alpha, v[i]);
     for(j = 0; j < noz; j++) {
       fx[i] /= (v[i]-zero[j]);
     }
   }

   while((cabs(x[2]-x[1]) >= er*cabs(x[2])) || (cc > nit)) {

      q = (x[2]-x[1])/(x[1]-x[0]);
      a = q*fx[2]-q*(1+q)*fx[1]+q*q*fx[0];
      b = (2*q+1)*fx[2]-(1+q)*(1+q)*fx[1]+q*q*fx[0];
      c = (1+q)*fx[2];
      d1 = b+sqrt(b*b-4*a*c);
      d2 = b-sqrt(b*b-4*a*c);
      d = ( cabs(d1) >= cabs(d2) ) ? d1 : d2;
      tmp = x[2]-(x[2]-x[1])*2.0*c/d;
      
      x[2] = tmp;
      x[1] = v[2];
      x[0] = v[1];
      v = x;

      fv[2] = fn(alpha, x[2]);
      for(j = 0; j < noz; j++) {
	fv[2] /= (x[2]-zero[j]);
      }
      fv[1] = fx[2];
      fv[0] = fx[1];
      fx = fv;
      
      cc ++;
      
   }

   tmpz.resize(zero.dim()+1);
   for(i=0; i<zero.dim(); i++) {
     tmpz[i] = zero[i];
   }
   tmpz[zero.dim()] = x[2];
   zero = tmpz;

}

Complex fun( CmplxVector alpha, Complex xx ) {

  int     s = alpha.dim(), i;
  Complex res = 0.0;

  for(i = 0; i < s; i ++) {
    res += alpha[i]*pow(xx,s-i-1);
  }

  return(res);

}

