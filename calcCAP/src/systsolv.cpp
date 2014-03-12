/*************LU DECOMPOSITION AND ITS APPLICATIONS***************/
     /*****Source - "NUMERICAL RECIPES IN C", www.nr.com*****/

//#include "stdafx.h"

#include <iostream>
#include <math.h>
#include <stdlib.h>
#include "complex.h"
#include "vector.h"
#include "matrix.h"
#include "cmplxvec.h"
#include "cmplxmat.h"
#include "systsolv.h"

#define TINY 1.0e-30;


/*********ludcmp - LU decomposition of a complex matrix***********/

void ludcmp( CmplxMatrix& a, Vector& indx, double& d )
{
   int const  n = a.dim_i();
   int        i, imax, j, k;
   double     big, temp, dum;
   Vector     vv(n), ind(n);
   Complex    sum, dum1;

   d = 1.0;

   for( i=0; i < n; i++ ) {
      big = 0.0;
      for( j=0; j < n; j++ )
         if ( (temp = cabs(a(i,j))) > big ) big = temp;
      if( big == 0.0 ) {
         cerr << "Singular matrix in routine ludcmp !!!" << endl;
         exit(1);
      }
      vv[i] = 1.0/big;
   }
   for( j=0; j < n; j++ ) {
      for( i=0; i < j; i++ ) {
         sum = a(i,j);
         for( k=0; k < i; k++ ) sum -= a(i,k)*a(k,j);
         a(i,j) = sum;
      }
      big = 0.0;
      for( i=j; i < n; i++ ) {
         sum = a(i,j);
         for( k=0; k < j; k++ ) sum -= a(i,k)*a(k,j);
      a(i,j) = sum;
      if( (dum = vv[i]*cabs(sum)) >= big ) {
         big = dum;
         imax = i;
      }
   }
   if( j != imax ) {
      for( k=0; k < n; k++ ) {
         dum1 = a(imax,k);
         a(imax,k) = a(j,k);
         a(j,k) = dum1;
      }
      d = -d;
      vv[imax] = vv[j];
   }
   ind[j] = imax;
   if( a(j,j) == 0.0 ) a(j,j) = TINY;
   if( j != n-1 ) {
      dum1 = 1.0/a(j,j);
      for( i=j+1; i < n; i++ ) a(i,j) *= dum1; }
   }
   indx = ind;
}


/*******lubksb - solution of a system of linear equations*********/

void lubksb( CmplxMatrix& a, CmplxVector& b, Vector& indx )
{
   int const  n = a.dim_i();
   int        i, j, ip;
   int        ii=-1;
   Complex    sum(0.0);

   if (n != indx.dim()) {
      cerr << "Something is wrong in lubksb !!!" << endl;
      exit(1);
   }

   for(i=0; i<n; i++) {
      ip = int(indx[i]);
      sum = b[ip];
      b[ip] = b[i];
      if(ii!=-1)
         for(j=ii; j<=i-1; j++) sum -= a(i,j)*b[j];
      else if(sum!=0.0) ii = i;
      b[i] = sum;
   }
   for(i=n-1; i>=0; i--) {
      sum = b[i];
      for(j=i+1; j<n; j++) sum -= a(i,j)*b[j];
      b[i] = sum/a(i,i);
   }
}


/***********ludetr - determinant of a complex matrix**************/

Complex ludetr( CmplxMatrix& a, double& d )
{
   int const  n = a.dim_i();
   Complex    det(1.0);
   det *= d;
   for(int i=0; i<n; i++) det *= a(i,i);
   return det;
}


/*******************luinvm - inverse  of a matrix*****************/

void luinvm( CmplxMatrix& a, CmplxMatrix& mat, Vector& indx )
{
   int const    n = a.dim_i();
   int          i, j;
   CmplxVector  col(n);
   CmplxMatrix  res(n,n);

   if (n != a.dim_j() || n != indx.dim()) {
      cerr << "Something is wrong in luinvm !!!" << endl;
      exit(1);
   }

   for(j=0; j<n; j++) {
      for(i=0; i<n; i++) col[i] = 0.0;
      col[j] = 1.0;
      lubksb( a, col, indx );
      for(i=0; i<n; i++) res(i,j) = col[i];
   }
   mat = res;
}


/*****improve - improvement of a solution to linear equations*****/

void improve( CmplxMatrix& a, CmplxMatrix& alud, Vector& indx,
              CmplxVector& b, CmplxVector& x )
{
   int const    n = a.dim_i();
   //int i, j;
   CmplxVector  r(n);

   r = a*x-b;
   lubksb( alud, r, indx );
   x -= r;
}


/*********ludcmp - LU decomposition of a real matrix**************/

void ludcmp_real( Matrix& a, Vector& indx, double& d )
{
   int const  n = a.dim_i();
   int        i, imax, j, k;
   double     big, temp, dum;
   Vector     vv(n), ind(n);
   double     sum, dum1;

   d = 1.0;

   for( i=0; i < n; i++ ) {
      big = 0.0;
      for( j=0; j < n; j++ )
         if ( (temp = fabs(a(i,j))) > big ) big = temp;
      if( big == 0.0 ) {
         cerr << "Singular matrix in routine ludcmp !!!" << endl;
         exit(1);
      }
      vv[i] = 1.0/big;
   }
   for( j=0; j < n; j++ ) {
      for( i=0; i < j; i++ ) {
         sum = a(i,j);
         for( k=0; k < i; k++ ) sum -= a(i,k)*a(k,j);
         a(i,j) = sum;
      }
      big = 0.0;
      for( i=j; i < n; i++ ) {
         sum = a(i,j);
         for( k=0; k < j; k++ ) sum -= a(i,k)*a(k,j);
      a(i,j) = sum;
      if( (dum = vv[i]*fabs(sum)) >= big ) {
         big = dum;
         imax = i;
      }
   }
   if( j != imax ) {
      for( k=0; k < n; k++ ) {
         dum1 = a(imax,k);
         a(imax,k) = a(j,k);
         a(j,k) = dum1;
      }
      d = -d;
      vv[imax] = vv[j];
   }
   ind[j] = imax;
   if( a(j,j) == 0.0 ) a(j,j) = TINY;
   if( j != n-1 ) {
      dum1 = 1.0/a(j,j);
      for( i=j+1; i < n; i++ ) a(i,j) *= dum1; }
   }
   indx = ind;
}


/*******lubksb - solution of a system of linear equations*********/

void lubksb_real( Matrix& a, Vector& b, Vector& indx )
{
   int const  n = a.dim_i();
   int        i, j, ip;
   int        ii=-1;
   double     sum=0.0;

   if (n != indx.dim()) {
      cerr << "Something is wrong in lubksb !!!" << endl;
      exit(1);
   }

   for(i=0; i<n; i++) {
      ip = int(indx[i]);
      sum = b[ip];
      b[ip] = b[i];
      if(ii!=-1)
         for(j=ii; j<=i-1; j++) sum -= a(i,j)*b[j];
      else if(sum!=0.0) ii = i;
      b[i] = sum;
   }
   for(i=n-1; i>=0; i--) {
      sum = b[i];
      for(j=i+1; j<n; j++) sum -= a(i,j)*b[j];
      b[i] = sum/a(i,i);
   }
}


/***********ludetr - determinant of a complex matrix**************/

double ludetr_real( Matrix& a, double& d )
{
   int const  n = a.dim_i();
   double     det = 1.0;
   det *= d;
   for(int i=0; i<n; i++) det *= a(i,i);
   return det;
}



/*******************luinvm - inverse  of a matrix*****************/

void luinvm_real( Matrix& a, Matrix& mat, Vector& indx )
{
   int const  n = a.dim_i();
   int        i, j;
   Vector     col(n);
   Matrix     res(n,n);

   if (n != a.dim_j() || n != indx.dim()) {
      cerr << "Something is wrong in luinvm !!!" << endl;
      exit(1);
   }

   for(j=0; j<n; j++) {
      for(i=0; i<n; i++) col[i] = 0.0;
      col[j] = 1.0;
      lubksb_real( a, col, indx );
      for(i=0; i<n; i++) res(i,j) = col[i];
   }
   mat = res;
}


/*****improve - improvement of a solution to linear equations*****/

void improve_real( Matrix& a, Matrix& alud, Vector& indx,
                   Vector& b, Vector& x )
{
   int const  n = a.dim_i();
   //int i, j;
   Vector     r(n);

   r = a*x-b;
   lubksb_real( alud, r, indx );
   x -= r;
}


void get_l_u_real( Matrix& l, Matrix& u, const Matrix& a ) {

   const int  n = a.dim_i();
   int        i, j;
   double     dlu;
   Vector     Indx;
   Matrix     res;

   if( n !=  a.dim_j() ) {

      cerr << " Something is wrong in getl_l_u_real !!! "
           << endl;

      exit(1);
   }

   res = a;
   ludcmp_real( res, Indx, dlu );

   l.resize(n,n);
   u.resize(n,n);

   for( i = 0; i < n; i++ )
       for( j = i; j < n; j++ )
           u(i,j) = res(i,j);

   for( i = 0; i < n; i++ )
       l(i,i) = 1.0;

   for( i = 1; i < n; i++ )
       for( j = 0; j < i; j++ )
           l(i,j) = res(i,j);
}


void get_l_real( Matrix& l, const Matrix& u, const Matrix& a ) {

   const int  n = a.dim_i();
   int        i, j, k;
   double     tmp;

   if( n !=  a.dim_j() || n != u.dim_i() || n != u.dim_j() ) {

      cerr << " Something is wrong in get_l_real !!! "
           << endl;

      exit(1);
   }

   l.resize(n,n);

   for( i = 0; i < n; i++ )
       l(i,0) = a(i,0) / u(0,0);

   for( i = 1; i < n; i++ ) {

       for( j = 1; j <= i; j++ ) {

           tmp = 0.0;

           for( k = 0; k <= j-1; k++ )
               tmp += l(i,k) * u(k,j);

           l(i,j) = (a(i,j) - tmp) / u(j,j);
       }
   }
}


void solve_u_known_real( Matrix& l, const Matrix& u, const Matrix& a ) {

   const int  n = a.dim_i();
   int        i, j, k;
   double     tmp;

   if( n !=  a.dim_j() || n != u.dim_i() || n != u.dim_j() ) {

      cerr << " Something is wrong in solve_u_known_real !!! "
           << endl;

      exit(1);
   }

   l.resize(n,n);

   for( i = 0; i < n; i++ )
       l(i,0) = a(i,0) / u(0,0);

   for( i = 0; i < n; i++ ) {

       for( j = 1; j < n; j++ ) {

           tmp = 0.0;

           for( k = 0; k <= j-1; k++ )
               tmp += l(i,k) * u(k,j);

           l(i,j) = (a(i,j) - tmp) / u(j,j);
       }
   }
}


void solve_l_known_real( Matrix& u, const Matrix& l, const Matrix& a ) {

   const int  n = a.dim_i();
   int        i, j, k;
   double     tmp;

   if( n !=  a.dim_j() || n != l.dim_i() || n != l.dim_j() ) {

      cerr << " Something is wrong in solve_l_known_real !!! "
           << endl;

      exit(1);
   }

   u.resize(n,n);

   for( i = 0; i < n; i++ )
       u(0,i) = a(0,i) / l(0,0);

   for( i = 1; i < n; i++ ) {

       for( j = 0; j < n; j++ ) {

           tmp = 0.0;

           for( k = 0; k <= i-1; k++ )
               tmp += l(i,k) * u(k,j);

           u(i,j) = (a(i,j) - tmp) / l(i,i);
       }
   }
}


/*********ludcmp - LU decomposition of a complex matrix***********/

void ludcmp_sparse( CmplxMatrix& a, Vector& indx, double& d )
{
   int const  n = a.dim_i();
   int        i, imax, j, k;
   double     big, temp, dum;
   Vector     vv(n), ind(n);
   Complex    sum, dum1;

   d = 1.0;


   for( i = 0; i < n; i++ ) {

      big = 0.0;

      for( j = 0; j < n; j++ ) {

          if( ( temp = cabs( a(i,j) ) ) > big )
             big = temp;
      }

      if( big == 0.0 ) {

         cerr << "Singular matrix in routine ludcmp !!!"
              << endl;

         exit(1);
      }

      vv[i] = 1.0 / big;
   }


   for( j = 0; j < n; j++ ) {

       for( i = 0; i < j; i++ ) {

           sum = a(i,j);

           for( k = 0; k < i; k++ ) {

               if( a(i,k) != 0.0 &&  a(k,j) != 0.0 )
                  sum -= a(i,k) * a(k,j);
           }

           a(i,j) = sum;
       }

       big = 0.0;

       for( i = j; i < n; i++ ) {

           sum = a(i,j);

           for( k = 0; k < j; k++ ) {

               if( a(i,k) != 0.0 &&  a(k,j) != 0.0 )
                  sum -= a(i,k) * a(k,j);
           }

           a(i,j) = sum;

           if( ( dum = vv[i] * cabs( sum ) ) >= big ) {

              big = dum;
              imax = i;
           }
       }


       if( j != imax ) {

          for( k = 0; k < n; k++ ) {

              dum1 = a(imax,k);
              a(imax,k) = a(j,k);
              a(j,k) = dum1;
          }

          d = -d;

          vv[imax] = vv[j];
       }


       ind[j] = imax;


       if( a(j,j) == 0.0 )
          a(j,j) = TINY;


       if( j != n-1 ) {

          dum1 = 1.0 / a(j,j);

          for( i = j+1; i < n; i++ )
              a(i,j) *= dum1;
       }
   }


   indx = ind;
}
