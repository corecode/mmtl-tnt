/****************GAUSS-LEGENDRE QUADRATURE METHOD*****************/
     /*****Source - "NUMERICAL RECIPES IN C", www.nr.com*****/


//#include "stdafx.h"
#include <math.h>
#include <stdlib.h>
#include "vector.h"
#include "gauleg.h"
#define  EPS  1.0e-12 


/*****Abscissas and weights for the Gauss-Legendre quadrature*****/

void gauleg( double x1, double x2, Vector& p, Vector& w )
{
   int m, i, j;
   int const n = p.dim();
   double z1, z, xm, xl, pp, p3, p2, p1;
 
   m = (n+1)/2;
   xm = (x2+x1)/2;
   xl = (x2-x1)/2;

   double atn = 4.0 * atan(1.0);

   for(i=1; i<=m; i++)
     {
        z = cos(atn*(i-0.25)/(n+0.5));
        do {
              p1 = 1.0;
              p2 = 0.0;
              for(j=1; j<=n; j++)
                {
                   p3 = p2;
                   p2 = p1;
                   p1 = ((2.0*j-1.0)*z*p2-(j-1.0)*p3)/j;
                } 
              pp = n*(z*p1-p2)/(z*z-1.0);            
              z1 = z;
              z = z1 - p1/pp;
	   }  while(fabs(z-z1) > EPS);
        p[i-1] = xm-xl*z;
        p[n-i] = xm+xl*z;
        w[i-1] = 2.0*xl/((1.0-z*z)*pp*pp);           
        w[n-i] = w[i-1];
     }      
   if(double(n)/2>floor(double (n)/2)) p[(n-1)/2] = (x2+x1)/2; 
}
