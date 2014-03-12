
/*
  
  FACILITY:  NMMTL
  
  MODULE DESCRIPTION:
  
  Contains nmmtl_find_nu - to find an edge effects constant
  and an #ifdefed main to test it
  
  AUTHOR(S):
  
  Kevin J. Buchs
  
  CREATION DATE:  Tue Feb 11 09:35:04 1992
  
  COPYRIGHT:   Copyright (C) 1992 by Mayo Foundation. All rights reserved.
  
  */


/*
 *******************************************************************
 **  INCLUDE FILES
 *******************************************************************
 */

#include "nmmtl.h"
#ifdef FORTRAN_UNDERBARS
#define FMIN fmin_
#else
#define FMIN fmin
#endif
/*
 *******************************************************************
 **  STRUCTURE DECLARATIONS AND TYPE DEFINTIONS
 *******************************************************************
 */
/*
 *******************************************************************
 **  MACRO DEFINITIONS
 *******************************************************************
 */
/*
 *******************************************************************
 **  PREPROCESSOR CONSTANTS
 *******************************************************************
 */
/*
 *******************************************************************
 **  GLOBALS
 *******************************************************************
 */

static float EPSILON_TERM; /* the constant term in nu equation */
static float THETA2;       /* constant in nu equation */
static float THETA_TERM;   /* constant in nu equation 2*theta1 - theta2 */

/*
 *******************************************************************
 **  FUNCTION DECLARATIONS
 *******************************************************************
 */
extern "C" void FMIN(float (*nmmtl_nu_function)(float *), float *a,
	float *b, float *x, float *w, float *aerr, float *rerr,
	float *error, int *ind);
/*
 *******************************************************************
 **  FUNCTION DEFINITIONS
 *******************************************************************
 */


/*
  
  FUNCTION NAME:  nmmtl_find_nu
  
  
  FUNCTIONAL DESCRIPTION:
  
  Solve the transcendental equation:
  
  0 = ( sin( nu * theta2 ) - sin( nu * (2 * theta1 - theta2) 
  * ( epsilon1 - epsilon2 ) / ( epsilon1 + epsilon2 ) )**2
  
  The second term is given by the variable EPSILON_TERM.
  
  for nu, given the value of theta1, theta2, epsilon1 and epsilon2.
  We will make an attempt to use the NSWC function FMIN to find the 
  minimum of this function.
  
  FORMAL PARAMETERS:
  
  float epsilon1     epsilon value passed in
  float epsilon2     epsilon value passed in
  float theta1       angle passed in
  float theta2       angle passed in
  
  RETURN VALUE:
  
  value of nu satisfying equation, -1.0 if no accurate solution, since 
  nu should be a positive constant.
  
  CALLING SEQUENCE:
  
  nu = nmmtl_find_nu(epsilon1,epsilon2,theta1,theta2);
  
  */

float nmmtl_find_nu(float epsilon1,float epsilon2,float theta1,float theta2)
{
  float a,b;
  float nu;
  float fofnu;
  float AERR,RERR;
  float ERROR;
  int IND;
  char msg[150];
  
  /* setup constants to call to FMIN */
  
  a = 1.0e-4F;	    /* lower endpoint of interval */
  b = 1.0F;	    /* upper endpoint of interval */
  AERR = 10.0e-7F;  /* absolute error tolerance- 7 significant digits */
  RERR = 0.0F;	    /* relative error tolerance- to machine precision */
  
  
  /* setup globals for nmmtl_nu_function() */
  
  EPSILON_TERM = (epsilon1 - epsilon2)/(epsilon1 + epsilon2);
  THETA2 = theta2;
  THETA_TERM = 2*theta1 - theta2;
  
  FMIN(nmmtl_nu_function,&a,&b,&nu,&fofnu,&AERR,&RERR,&ERROR,&IND);
  
#ifdef TEST_FIND_NU
  printf("nu=%f, f(nu)=%f, error=%f\n",nu,fofnu,ERROR);
#endif    
  
  if(IND == 0) return(nu);
  else
  {
    fprintf(stderr,"ELECTRO-F-NOSOLUTION No solution for transcendental equation for edge effects was found, error=%f\n",ERROR);
    
    return(-1.0F);
  }
}


/*
  
  FUNCTION NAME:  nmmtl_nu_function(float *nu);
  
  
  FUNCTIONAL DESCRIPTION:
  
  evaluate the equation to minimize for determining the value of nu.
  
  0 = ( sin( nu * theta2 ) / sin( nu * (2 * theta1 - theta2) )
  - ( epsilon1 - epsilon2 ) / ( epsilon1 + epsilon2 ) )**2
  
  this equation divides by zero when theta1 = 1/2 theta2, therefore multiply 
  both sides by sin( nu * (2 * theta1 - theta2) ) and use the new equation:
  
  0 = ( sin( nu * theta2 ) - sin( nu * (2 * theta1 - theta2) 
  * ( epsilon1 - epsilon2 ) / ( epsilon1 + epsilon2 ) )**2
  
  FORMAL PARAMETERS:
  
  float *nu;  the value of nu to use 
  Note: this function accepts a pointer to nu since this function
  must be called from FORTRAN which by default passes arguments by 
  reference
  
  RETURN VALUE:
  
  the value of the function evaluation.
  
  CALLING SEQUENCE:
  
  */
float nmmtl_nu_function(float *nu)
{
  float x;
  x = sin(*nu * THETA2) - sin(*nu * THETA_TERM) * EPSILON_TERM;
  return( x * x );
}


/*
  
  FUNCTION NAME:  main (test this module)
  
  
  FUNCTIONAL DESCRIPTION:
  
  Runs a couple of calls to nmmtl_find_nu function in this module to test it
  
  FORMAL PARAMETERS:
  
  None.
  
  RETURN VALUE:
  
  None
  
  CALLING SEQUENCE:
  
  None
  
  */

#ifdef TEST_FIND_NU

main()
{
  float nu;
  float theta1;
  
  
  printf("symmetry test1\n");
  printf("theta1 = %f, theta2 = %f, epsilon1 = %f, epsilon2 = %f, nu = %f\n",
	 PI,3*PI/2,1.0F,2.0F,nmmtl_find_nu(1.0F,2.0F,PI,3*PI/2));
  printf("theta1 = %f, theta2 = %f, epsilon1 = %f, epsilon2 = %f, nu = %f\n",
	 PI/2,3*PI/2,1.0F,2.0F,nmmtl_find_nu(1.0F,2.0F,PI/2,3*PI/2));
  printf("symmetry test2\n");
  printf("theta1 = %f, theta2 = %f, epsilon1 = %f, epsilon2 = %f, nu = %f\n",
	 2*PI/3,3*PI/2,1.0F,2.0F,nmmtl_find_nu(1.0F,2.0F,2*PI/3,3*PI/2));
  printf("theta1 = %f, theta2 = %f, epsilon1 = %f, epsilon2 = %f, nu = %f\n",
	 5*PI/6,3*PI/2,1.0F,2.0F,nmmtl_find_nu(1.0F,2.0F,5*PI/6,3*PI/2));
  printf("15/8 PI test3\n");
  printf("theta1 = %f, theta2 = %f, epsilon1 = %f, epsilon2 = %f, nu = %f\n",
	 PI/3,15*PI/8,1.0F,2.0F,nmmtl_find_nu(1.0F,2.0F,PI/3,15*PI/8));
  printf("31/16 PI test3\n");
  printf("theta1 = %f, theta2 = %f, epsilon1 = %f, epsilon2 = %f, nu = %f\n",
	 PI/3,31*PI/16,1.0F,2.0F,nmmtl_find_nu(1.0F,2.0F,PI/3,31*PI/16));
  
  printf("now entering evaluation phase:\nenter theta1 and theta2\n");
  scanf("%f %f",&theta1,&THETA2);
  
  THETA_TERM = 2*theta1 - THETA2;
  
  nu = 0.0;
  while(nu >= 0.0F)
  {
    printf("enter nu:\n");
    scanf("%f",&nu);
    if(nu < 0.0F) break;
    printf("f(nu)=%f\n\n",nmmtl_nu_function(&nu));
  }
  
}

#endif






