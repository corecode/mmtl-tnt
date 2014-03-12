#ifndef complex_numbers_h
#define complex_numbers_h

/* definitions required for use of complex numbers */

typedef struct 
{
  float real;                    /* the real part of the complex number */
  float imaginary;		/* the imaginary part of hte complex number */
} COMPLEX;

typedef struct
{
  double real;                    /* the real part of the complex number */
  double imaginary;                   /* the imaginary part of the
				     complex number 
				  */
} DOUBLE_COMPLEX;

/* these are the prototypes for the functions that perform
*  operations on the complex numbers
*  using these requires that the object be linked against
*  complex_numbers.c
*/

/* make a complex number from two floats*/
COMPLEX cplx(double x,double y);

/* the square root of a complex number */
COMPLEX csqrt(COMPLEX);

/* the complex exponential */
COMPLEX cexp(COMPLEX);

/* divide  the first number by the second */
COMPLEX cdiv(COMPLEX,COMPLEX);

/* multiply  two complex numbers */
COMPLEX cmult(COMPLEX,COMPLEX);

/* find the absolute value of a complex number */
float cmag(COMPLEX);

/* find the complex conjagate of a complex number */
COMPLEX conj(COMPLEX);

#endif
