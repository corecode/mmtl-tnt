/* one letter prefixes in routine names have the following meanings
   (no prefix indicates single precision real)
     c -- complex data is being used
     d -- doubles are being used
     d_c -- double complex data is to be input
*/
/* this is a set of wrappers for the math library 
*  the available routines are
*
*      c_fft
*      c_fft_inv
*          ( internal: c_set_calc_eigenvalues
*          ( internal: c_init_calc_eigenvalues
*      c_calc_eigenvalues
*      c_mult_matricies
*
*          { internal: c_set_invert_matrix
*          { internal: c_init_invert_matrix
*      c_invert_matrix
*          { internal: set_invert_matrix
*          { internal: init_invert_matrix
*      invert_matrix
*      invert_matrix_cond
*          { internal: d_set_invert_matrix
*          { internal: d_init_invert_matrix
*      d_invert_matrix
*
*          { internal: c_set_solve_linear
*          { internal: c_init_solve_linear
*      c_solve_linear
*          { internal: d_c_set_solve_linear
*          { internal: d_c_init_solve_linear
*      d_c_solve_linear
*
*      lu_factor
*      lu_factor_cond
*      dlu_factor
*
*      lu_solve_linear
*      dlu_solve_linear
*  
*/

#include <stdio.h>
#include <stdlib.h>
#include "magicad.h"
#include "math_library.h"



 float *c_calc_eigenvalues_ai = NULL; /* workspace matricies */
 float *c_calc_eigenvalues_ar = NULL;
 float *c_calc_eigenvalues_wr = NULL;
 float *c_calc_eigenvalues_wi = NULL;
 float *c_calc_eigenvalues_zi = NULL;
 float *c_calc_eigenvalues_zr = NULL;
 float *c_calc_eigenvalues_temp = NULL;
 int n_c_calc_eigenvalues = 0;
 int n_c_init_calc_eigenvalues = 0;

 int n_c_invert_matrix = 0;
 int n_c_init_invert_matrix = 0;
 int *c_invert_matrix_ipvt = NULL;
 COMPLEX *c_invert_matrix_wrk = NULL;

 int n_invert_matrix = 0;
 int n_init_invert_matrix = 0;
 int *invert_matrix_ipvt = NULL;
 float *invert_matrix_wrk = NULL;
 double *d_invert_matrix_wrk = NULL;

 int n_c_solve_linear = 0;
 int n_c_init_solve_linear = 0;
 COMPLEX *c_solve_linear_a = NULL;
 COMPLEX *c_solve_linear_b = NULL;
 COMPLEX *c_solve_linear_wrk = NULL;
 int *c_solve_linear_ipvt = NULL;

 int n_d_c_solve_linear = 0;
 int n_d_c_init_solve_linear = 0;
 double *d_c_solve_linear_ai = NULL;
 double *d_c_solve_linear_ar = NULL;
 double *d_c_solve_linear_bi = NULL;
 double *d_c_solve_linear_br = NULL;
 double *d_c_solve_linear_wrk = NULL;
 int *d_c_solve_linear_ipvt = NULL;

//  int n_solve_nonlinear = 0;
//  int m_solve_nonlinear = 0;
//  int n_init_solve_nonlinear = 0;
// 
//  int *solve_nonlinear_iwk;
//  float *solve_nonlinear_wrk = NULL;
//  float *solve_nonlinear_x = NULL;

     
/* ***********************************************************************
 * ROUTINE NAME c_fft 
 *
 *
 * ABSTRACT  performs a forward fourier transform on the input data
 *           
 *            
 *
 * ENVIRONMENT  c_fft
  *
  *
  *
  * 
  * INPUTS  
  *   n                   the number of points in the vector 
  *   *a                  vector of complex numbers to be transformed 
  *          
  * OUTPUTS  
  *   c                   the complex coeff of the transform 
  *   status              indicates if the fft worked
  *                           this will be either SUCCESS or FAIL
  *          
  *         
  * FUNCTIONS CALLED 
  *         FFT
  *         
  *         
  *                                    
  * AUTHOR          Andrew Staniszewski
  *                                                        
  * MODIFICATION HISTORY                                   
  * 1.00     Original                          AJS       12-15-90
  * 2.00     Update to support fortran         AJS        3-8-91   
  *                                                                 
  * ***********************************************************************
  */

 /* wrapper for fft routine */

void c_fft(int *n,COMPLEX *a,COMPLEX *c,int *status)
 /* the values pointed to by c will become the transform, it is necessary 
 *  to copy the data before the transform if the original data is to be kept
 */


 {
   int ierr;                 /* indicates return status of the fft routine */
   int i;                    /* a loop variable */
   int sign = -1;            /* determines the sign of the eponent used in
			     *  calculating the transform 
			     */
 /* clear the error flag */
   ierr = 0;      
   for (i = 0; i < (*n); i++)
     {
       c[i] = a[i];
     }
   FFT(c,n,&sign,&ierr);
   if (ierr != 0)
     {
       (*status) = FAIL;
       fprintf(stderr,"ELECTRO-F-FFTINT Fast Fourier Transform Faileds with code %ld.\n",ierr);
       return;
     }
   (*status) = SUCCESS;
   return;
 }

/* ***********************************************************************
 * ROUTINE NAME  c_fft_inv
 *
 *
 * ABSTRACT  will find the complex coeff of the inverse fourier transform
 *           
 *            
 *
 * ENVIRONMENT  c_fft_inv 
 *
 *
 *
 * 
 * INPUTS  
 *   a              vector of complex numbers to be transformed 
 *   n              the number of points in the vector 
 *          
 * OUTPUTS  
 *   c              the complex coeff of the transform 
 *   status         indicates if the routine succeded
 *                                 will be either SUCCESS or FAIL
 *          
 *         
 * FUNCTIONS CALLED 
 *         
 *         
 *         
 *                                    
 * AUTHOR          Andrew Staniszewski
 *                                                        
 * MODIFICATION HISTORY                                   
 * 1.00     Original                          AJS       12-15-90  
 * 2.00     Update to support fortran         AJS        3-8-91 
 *                                                                 
 * ***********************************************************************
 */

/* inverse of fft */

void c_fft_inv(int *n,COMPLEX *a,COMPLEX *c,int *status)

/* the values pointed to by c will become the transform, it is necessary 
*  to copy the data before the transform if the original data is to be kept
*/


{
  int ierr;                 /* indicates return status of the fft routine */
  int i;                    /* A LOOP VARIABLE */
  int sign = 1;             /* determines the sign of the eponent used in
                            *  calculating the transform 
			    */

/* clear the error flag */
  ierr = 0;      
  for (i = 0; i < (*n); i++)
    {
      c[i] = a[i];
    }
  FFT(c,n,&sign,&ierr);
  if (ierr != 0)
    {
      (*status) = FAIL;
      fprintf(stderr,"ELECTRO-F-FFTINT Fast Fourier Transform Faileds with code %ld.\n",ierr);
      return;
    }
  (*status) = SUCCESS;
  return;
}

/* ***********************************************************************
 * ROUTINE NAME c_set_calc_eigenvalues  
 *
 *
 * ABSTRACT  Will set the size of the workspace created for
 *           the routine c_calc_eigenvalues. This is
 *           designed to allow multiple calls with the
 *           largest size being retained. The routine is optional
 *           
 *            
 *
 * ENVIRONMENT  c_set_calc_eigenvalues
 *
 *
 *
 * 
 * INPUTS  
 *    n                    the size of the square matrix
 *                           that will be input into
 *                           c_calc_eigenvalues
 *          
 * OUTPUTS  
 *         
 * FUNCTIONS CALLED 
 *         
 *         
 *         
 *                                    
 * AUTHOR          Andrew Staniszewski
 *                                                        
 * MODIFICATION HISTORY                                   
 * 1.00     Original                          AJS       3-15-91   
 *                                                                 
 * ***********************************************************************
 */
void c_set_calc_eigenvalues(int *n)

{
  extern int n_c_calc_eigenvalues;

  if (n_c_calc_eigenvalues < (*n))
    n_c_calc_eigenvalues = (*n);

  return;
}

/* ***********************************************************************
 * ROUTINE NAME  c_init_calc_eigenvalues
 *
 *
 * ABSTRACT  this routine sets up the workspace for the routine
 *             c_calc_eigenvalues. It should only be called
 *             from within c_calc_eigenvalues
 *           
 *            
 *
 * ENVIRONMENT  c_init_calc_eigenvalues
 *
 *
 *
 * 
 * INPUTS  
 *          
 * OUTPUTS  
 *    status                       will be either SUCCESS or FAIL
 *         
 * FUNCTIONS CALLED 
 *     calloc
  *         
 *         
 *                                    
 * AUTHOR          Andrew Staniszewski
 *                                                        
 * MODIFICATION HISTORY                                   
 * 1.00     Original                          AJS       3-8-91   
 *                                                                 
 * ***********************************************************************
 */
void c_init_calc_eigenvalues(int *status)
{

  extern float *c_calc_eigenvalues_ai; /* workspace matricies */
  extern float *c_calc_eigenvalues_ar;
  extern float *c_calc_eigenvalues_wr;
  extern float *c_calc_eigenvalues_wi;
  extern float *c_calc_eigenvalues_zi;
  extern float *c_calc_eigenvalues_zr;
  extern float *c_calc_eigenvalues_temp;

  extern int n_c_calc_eigenvalues;
  extern int n_c_init_calc_eigenvalues;

  int n;
  n = n_c_init_calc_eigenvalues = n_c_calc_eigenvalues;

  /* malloc the workspace */
  if (c_calc_eigenvalues_wi != NULL)
    {
      free(c_calc_eigenvalues_wi);
      free(c_calc_eigenvalues_wr);
      free(c_calc_eigenvalues_zi);
      free(c_calc_eigenvalues_zr);
      free(c_calc_eigenvalues_ai);
      free(c_calc_eigenvalues_ar);
      free(c_calc_eigenvalues_temp);
    }

  n = n_c_calc_eigenvalues;
  c_calc_eigenvalues_ai = (float *)calloc(n*n,sizeof(float));
  c_calc_eigenvalues_ar = (float *)calloc(n*n,sizeof(float));
  c_calc_eigenvalues_wi = (float *)calloc(n*n,sizeof(float));
  c_calc_eigenvalues_wr = (float *)calloc(n*n,sizeof(float));
  c_calc_eigenvalues_zi = (float *)calloc(n*n,sizeof(float));
  c_calc_eigenvalues_zr = (float *)calloc(n*n,sizeof(float));
  c_calc_eigenvalues_temp = (float *)calloc(3*n,sizeof(float));

  (*status) = SUCCESS;
  return;
}

/* ***********************************************************************
 * ROUTINE NAME c_calc_eigenvalues  
 *
 *
 * ABSTRACT returns the eigenvalues and eigen vectors of the input
 *          matrix. It will produce a warning if not called after
 *          c_set_calc_eigenvalues
 *           
 *            
 *
 * ENVIRONMENT c_calc_eigenvalues
 *
 *
 *
 * 
 * INPUTS  
 *    c                  input matrix 
 *    n                  the number of rows/columns in c
 *          
 * OUTPUTS  
 *   eval               the eigenvalues of c
 *   evect              the matrix of eigenvectors for c
 *   ldc                number of rows in c
 *   ldevect            number of rows in evect
 *   status             return status of the call
 *   
 *          
 *         
 * FUNCTIONS CALLED 
 *     c_set_calc_eigenvalues
 *     c_init_calc_eigenvalues    
 *         
 *         
 *                                    
 * AUTHOR          Andrew Staniszewski
 *                                                        
 * MODIFICATION HISTORY                                   
 * 1.00     Original                          AJS       12-15-90   
 * 2.00     Update to support fortran         AJS        3-18-91
 * 
 *                                                                 
 * ***********************************************************************
 */

  
//  void c_calc_eigenvalues(COMPLEX *c,int *n,COMPLEX *eval,COMPLEX *evect,
//  			      int *ldc, int *ldevect, int *status) 

/* this  routine will find the eigenvalues and eigenvectors of a complex
*  matrix
*/
//  {
//    extern float *c_calc_eigenvalues_ai; /* workspace matricies */
//    extern float *c_calc_eigenvalues_ar;
//    extern float *c_calc_eigenvalues_wr;
//    extern float *c_calc_eigenvalues_wi;
//    extern float *c_calc_eigenvalues_zi;
//    extern float *c_calc_eigenvalues_zr;
//    extern float *c_calc_eigenvalues_temp;

//    extern int n_c_init_calc_eigenvalues;

//    int ierr;                    /* status flag for the routine called */
//    int i,j;		       /* loop varible */
//    int bal = 0;                 /* indicates if balancing is to be used
//  			       *  see NSWC page 315
//  			       */

//    if ((*n) > n_c_init_calc_eigenvalues)
//      {
//        c_set_calc_eigenvalues(n);
//        c_init_calc_eigenvalues(status);
//        if ((*status) != SUCCESS)
//  	return;
//      }
  
//    /* copy the input data to the workspace */

//    for (i = 0; i < (*n); i++)
//      {
//        for (j = 0; j < (*n); j++)
//  	{
//  	  c_calc_eigenvalues_ai[i*(*n) + j] = c[i*(*ldc) + j].imaginary;
//  	  c_calc_eigenvalues_ar[i*(*n) + j] = c[i*(*ldc) +j].real;
//  	}
//      }
  
//    ierr = 0;
//    CEIGV(&bal,c_calc_eigenvalues_ar,c_calc_eigenvalues_ai,n,n,
//  	c_calc_eigenvalues_wr,c_calc_eigenvalues_wi,
//  	c_calc_eigenvalues_zr,c_calc_eigenvalues_zi,&ierr,
//  	c_calc_eigenvalues_temp);
//    if (ierr != 0)
//      {
//        (*status) = FAIL;
//        fprintf(stderr,"ELECTRO-F-EIGENINT Error on computing eigenvalues, NSWC code %ld\n",ierr);
//        return;
//      }

//    /* copy final matricies to outputs */
//    for (i = 0; i < (*n); i++)
//      {
//        eval[i].real = c_calc_eigenvalues_wr[i];
//        eval[i].imaginary = c_calc_eigenvalues_wi[i];
//      }
//    for (i = 0; i < (*n); i++)
//      {
//        for (j = 0; j < (*n); j++)
//  	{
//  	  evect[i*(*ldevect) + j].real = c_calc_eigenvalues_zr[i*(*n) + j];
//  	  evect[i*(*ldevect) + j].imaginary = c_calc_eigenvalues_zi[i*(*n) + j];
//  	}
//      }
  
//    (*status) = SUCCESS;
//    return;
//  }

/* ***********************************************************************
 * ROUTINE NAME c_mult_matricies  
 *
 *
 * ABSTRACT  multiplies two complex matricies
 *           
 *            
 *
 * ENVIRONMENT c_mult_matricies 
 *             
 *
 *
 * 
 * INPUTS  
 *    ra            number of rows used in a 
 *    carb          number of columns in a / rows in b
 *    cb            number of columns in b 
 *    lda           number of rows in a (when dimensioned)
 *    ldb           number of rows in b (when dimensioned)
 *    ldc           number of rows in c (when dimensioned)
 *    a             complex input matrix 
 *    b             complex input matrix 
 *          
 * OUTPUTS  
 *    c             complex output matrix
 *          
 *         
 * FUNCTIONS CALLED 
 *       CMTMS  
 *         
 *         
 *                                    
 * AUTHOR          Andrew Staniszewski
 *                                                        
 * MODIFICATION HISTORY                                   
 * 1.00     Original                          AJS       12-15-90   
 * 2.00     Update to Support Fortran         AJS        3-18-91 
 *                                                                 
 * ***********************************************************************
 */

/* this routine will multiply two complex matricies */

void c_mult_matricies(int *ra,int *carb, int *cb,
			   COMPLEX *a, COMPLEX *b, COMPLEX *c,
			   int *lda, int *ldb, int *ldc,int *status)
     
{
  CMTMS(ra,carb,cb,a,lda,b,ldb,c,ldc);
  (*status) = SUCCESS;
  return;
}


/* ***********************************************************************
 * ROUTINE NAME mult_matricies  
 *
 *
 * ABSTRACT  multiplies two complex matricies
 *           
 *            
 *
 * ENVIRONMENT mult_matricies 
 *             
 *
 *
 * 
 * INPUTS  
 *    ra            number of rows used in a 
 *    carb          number of columns in a / rows in b
 *    cb            number of columns in b 
 *    lda           number of rows in a (when dimensioned)
 *    ldb           number of rows in b (when dimensioned)
 *    ldc           number of rows in c (when dimensioned)
 *    a             float input matrix 
 *    b             float input matrix 
 *          
 * OUTPUTS  
 *    c             float output matrix
 *          
 *         
 * FUNCTIONS CALLED 
 *       CMTMS  
 *         
 *         
 *                                    
 * AUTHOR          Andrew Staniszewski
 *                                                        
 * MODIFICATION HISTORY                                   
 * 1.00     Original                          AJS       12-15-90   
 * 2.00     Update to Support Fortran         AJS        3-18-91 
 *                                                                 
 * ***********************************************************************
 */

/* this routine will multiply two matricies */

void mult_matricies(int *ra,int *carb, int *cb,
			   float *a, float *b, float *c,
			   int *lda, int *ldb, int *ldc,int *status)
     
{
  MTMS(ra,carb,cb,a,lda,b,ldb,c,ldc);
  (*status) = SUCCESS;
  return;
}


/* ***********************************************************************
 * ROUTINE NAME c_set_invert_matrix  
 *
 *
 * ABSTRACT  Will set the size of the workspace created for
 *           the routine c_invert_matrix. This is
 *           designed to allow multiple calls with the
 *           largest size being retained. 
 *           
 *            
 *
 * ENVIRONMENT  c_set_inv_matrix
 *
 *
 *
 * 
 * INPUTS  
 *    n                    the size of the square matrix
 *                           that will be input into
 *                           c_invert_matrix
 *          
 * OUTPUTS  
 *         
 * FUNCTIONS CALLED 
 *         
 *         
 *         
 *                                    
 * AUTHOR          Andrew Staniszewski
 *                                                        
 * MODIFICATION HISTORY                                   
 * 1.00     Original                          AJS       3-15-91   
 *                                                                 
 * ***********************************************************************
 */
void c_set_invert_matrix(int *n)

{
  extern int n_c_invert_matrix;

  if (n_c_invert_matrix < (*n))
    n_c_invert_matrix = (*n);

  return;
}

/* ***********************************************************************
 * ROUTINE NAME  c_init_invert_matrix
 *
 *
 * ABSTRACT  this routine sets up the workspace for the routine
 *             c_invert_matrix. It should only be called
 *             from within c_invert_matrix
 *           
 *            
 *
 * ENVIRONMENT  c_init_invert_matrix
 *
 *
 *
 * 
 * INPUTS  
 *          
 * OUTPUTS  
 *    status                       will be either SUCCESS or FAIL
 *         
 * FUNCTIONS CALLED 
 *         
 *         
 *         
 *                                    
 * AUTHOR          Andrew Staniszewski
 *                                                        
 * MODIFICATION HISTORY                                   
 * 1.00     Original                          AJS       3-8-91   
 *                                                                 
 * ***********************************************************************
 */
void c_init_invert_matrix(int *status)
{

  extern int *c_invert_matrix_ipvt;
  extern COMPLEX *c_invert_matrix_wrk;

  extern int n_c_invert_matrix;
  extern int n_c_init_invert_matrix;

  int n;
  n = n_c_init_invert_matrix = n_c_invert_matrix;
  /* malloc the workspace */
  if (c_invert_matrix_ipvt != NULL)
    {
      free(c_invert_matrix_ipvt);
      free(c_invert_matrix_wrk);
    }


/* malloc some workspace */  
  c_invert_matrix_ipvt = (int *)calloc(n,sizeof(int));
  c_invert_matrix_wrk = (COMPLEX *)calloc(n*n,sizeof(COMPLEX));

  (*status) = SUCCESS;
  return;
}


/* ***********************************************************************
 * ROUTINE NAME c_invert_matrix  
 *
 *
 * ABSTRACT  will supply the inverse of a complex matrix
 *           
 *            
 *
 * ENVIRONMENT  c_invert_matrix
 *
 *
 * 
 * INPUTS  
 *     n                   the size of the a and b is n*n 
 *     a                   the input matrix
 *     lda                 leading dimension of a
 *     ldb                 leading dimension of b 
 *          
 * OUTPUTS  
 *     b              the inverted matrix 
 *     status         SUCCESS or FAIL
 *          
 *         
 * FUNCTIONS CALLED 
 *         
 *         
 *         
 *                                    
 * AUTHOR          Andrew Staniszewski
 *                                                        
 * MODIFICATION HISTORY                                   
 * 1.00     Original                          AJS       12-15-90   
 * 2.00     Update for use from fortran       AJS        3-18-91
 *                                                                 
 * ***********************************************************************
 */
/* this routine will invert a complex matrix */

void c_invert_matrix(int *n,COMPLEX *a,COMPLEX *b,
		     int *lda, int *ldb, int *status)
     
{
  extern int *c_invert_matrix_ipvt;
  extern COMPLEX *c_invert_matrix_wrk;



  extern int n_c_init_invert_matrix;

  int ierr;             /*status flag for call */
  int i,j;                 /*loop variable */
  int calc_inv = 0;       /* indicates inverse of is to be calculated */
  int zero_dim = 0;	/* indicates that no solutions is needed see
			   NSWC page 211
			*/

  if ((*n) > n_c_init_invert_matrix)
    {
      c_set_invert_matrix(n);
      c_init_invert_matrix(status);
    }



/* first copy a to b */

  for (i = 0; i < (*n); i++)
    {
      for (j = 0; j < (*n); j++)
	{
	  b[i*(*ldb)+j] = a[i*(*lda)+j];
	}
    }
  
  CMSLV1(&calc_inv, n, &zero_dim,
	    b, ldb, a,
	    &zero_dim,
	    &ierr,
	    c_invert_matrix_ipvt,
	    c_invert_matrix_wrk);

  if (ierr != 0) 
    {
      (*status) = FAIL;
      fprintf(stderr,"ELECTRO-F-INVRSINT Error in matrix inversion, NSWC code %ld\n",ierr);
      return;
    }

  (*status) = SUCCESS;
  return;
}

/* ***********************************************************************
 * ROUTINE NAME set_invert_matrix  
 *
 *
 * ABSTRACT  Will set the size of the workspace created for
 *           the routine invert_matrix. This is
 *           designed to allow multiple calls with the
 *           largest size being retained. 
 *           
 *            
 *
 * ENVIRONMENT  set_invert_matrix
 *
 *
 *
 * 
 * INPUTS  
 *    n                    the size of the square matrix
 *                           that will be input into
 *                           c_invert_matrix
 *          
 * OUTPUTS  
 *         
 * FUNCTIONS CALLED 
 *         
 *         
 *         
 *                                    
 * AUTHOR          Andrew Staniszewski
 *                                                        
 * MODIFICATION HISTORY                                   
 * 1.00     Original                          AJS       3-15-91   
 *                                                                 
 * ***********************************************************************
 */
void set_invert_matrix(int *n)

{
  extern int n_invert_matrix;

  if (n_invert_matrix < (*n))
    n_invert_matrix = (*n);

  return;
}

/* ***********************************************************************
 * ROUTINE NAME  init_invert_matrix
 *
 *
 * ABSTRACT  this routine sets up the workspace for the routine
 *             invert_matrix. It should only be called
 *             from within invert_matrix
 *           
 *            
 *
 * ENVIRONMENT  init_invert_matrix
 *
 *
 *
 * 
 * INPUTS  
 *          
 * OUTPUTS  
 *    status                       will be either SUCCESS or FAIL
 *         
 * FUNCTIONS CALLED 
 *         
 *         
 *         
 *                                    
 * AUTHOR          Andrew Staniszewski
 *                                                        
 * MODIFICATION HISTORY                                   
 * 1.00     Original                          AJS       3-8-91   
 *                                                                 
 * ***********************************************************************
 */
void init_invert_matrix(int *status)
{

  extern int *invert_matrix_ipvt;
  extern float *invert_matrix_wrk;

  extern int n_invert_matrix;
  extern int n_init_invert_matrix;

  int n;
  n = n_init_invert_matrix = n_invert_matrix;
  /* malloc the workspace */
  if (invert_matrix_ipvt != NULL)
    {
      free(invert_matrix_ipvt);
      free(invert_matrix_wrk);
    }


/* malloc some workspace */  
  invert_matrix_ipvt = (int *)calloc(n,sizeof(int));
  invert_matrix_wrk = (float *)calloc(n*n,sizeof(float));

  (*status) = SUCCESS;
  return;
}

/* ***********************************************************************
 * ROUTINE NAME invert_matrix  
 *
 *
 * ABSTRACT  will supply the inverse of a matrix
 *           
 *            
 *
 * ENVIRONMENT  invert_matrix
 *
 *
 * 
 * INPUTS  
 *     n                   the size of the a and b is n*n 
 *     a                   the input matrix
 *     lda                 leading dimension of a
 *     ldb                 leading dimension of b 
 *          
 * OUTPUTS  
 *     b              the inverted matrix 
 *     status         SUCCESS or FAIL
 *          
 *         
 * FUNCTIONS CALLED 
 *         
 *         
 *         
 *                                    
 * AUTHOR          Andrew Staniszewski
 *                                                        
 * MODIFICATION HISTORY                                   
 * 1.00     Original                          AJS       12-15-90   
 * 2.00     Update for use from fortran       AJS        3-18-91
 *                                                                 
 * ***********************************************************************
 */

/* this routine will invert a single precision real matrix */

void invert_matrix(int *n,float *a,float *b,
		   int *lda, int *ldb, int *status)
     
{
  extern int *invert_matrix_ipvt;
  extern float *invert_matrix_wrk;

  extern int n_init_invert_matrix;

  float t1[2];           /* workspace matricies */
  float rcond;            /* value indicating condition of input matrix */
  int ierr;             /*status flag for call */
  int i,j;                 /*loop variable */
  int calc_inv = 0;       /* indicates inverse of is to be calculated */
  int zero_dim = 0;	/* indicates that no solutions is needed see
			   NSWC page 211
			*/

  if ((*n) > n_init_invert_matrix)
    {
      set_invert_matrix(n);
      init_invert_matrix(status);
    }



/* first copy a to b */

  for (i = 0; i < (*n); i++)
    {
      for (j = 0; j < (*n); j++)
	{
	  b[i*(*ldb)+j] = a[i*(*lda)+j];
	}
    }
  
  MSLV(&calc_inv,n,&zero_dim,b,ldb,NULL,&zero_dim,&t1[0],
       &rcond,&ierr,invert_matrix_ipvt,invert_matrix_wrk);

  if (ierr != 0) 
    {
      (*status) = FAIL;
      fprintf(stderr,"ELECTRO-F-INVRSINT Error in matrix inversion, NSWC code %ld\n",ierr);
      return;
    }

  (*status) = SUCCESS;
  return;
}




/* ***********************************************************************
 * ROUTINE NAME invert_matrix_cond
 *
 *
 * ABSTRACT  will supply the inverse of a matrix and return the condition
 *           number
 *            
 *
 * ENVIRONMENT  invert_matrix_cond
 *
 *
 * 
 * INPUTS  
 *     n                   the size of the a and b is n*n 
 *     a                   the input matrix
 *     lda                 leading dimension of a
 *     ldb                 leading dimension of b 
 *          
 * OUTPUTS  
 *     b              the inverted matrix 
 *     rcond          condition number
 *     status         SUCCESS or FAIL
 *          
 *         
 * FUNCTIONS CALLED 
 *         
 *         MSLV (NSWC Fortran library)
 *         
 *                                    
 * AUTHOR          Andrew Staniszewski
 *                                                        
 * MODIFICATION HISTORY                                   
 * 1.00     Original                          AJS       12-15-90   
 * 2.00     Update for use from fortran       AJS        3-18-91
 *                                                                 
 * ***********************************************************************
 */

/* this routine will invert a single precision real matrix */

void invert_matrix_cond(int *n,float *a,float *b,
		   int *lda, int *ldb, float *rcond, int *status)
     
{
  extern int *invert_matrix_ipvt;
  extern float *invert_matrix_wrk;

  extern int n_init_invert_matrix;

  float t1[2];           /* workspace matricies */
  int ierr;             /*status flag for call */
  int i,j;                 /*loop variable */
  int calc_inv = 0;       /* indicates inverse of is to be calculated */
  int zero_dim = 0;	/* indicates that no solutions is needed see
			   NSWC page 211 */
  int istatus;

  if ((*n) > n_init_invert_matrix)
    {
      set_invert_matrix(n);
      init_invert_matrix(&istatus);
    }

/* first copy a to b */

  for (i = 0; i < (*n); i++)
    for (j = 0; j < (*n); j++)
      b[i*(*ldb)+j] = a[i*(*lda)+j];

  
  MSLV(&calc_inv,n,&zero_dim,b,ldb,NULL,&zero_dim,&t1[0],
       rcond,&ierr,invert_matrix_ipvt,invert_matrix_wrk);

  if (ierr != 0) 
    {
      (*status) = FAIL ;
      fprintf(stderr,"ELECTRO-F-INVRSINT Error in matrix inversion, NSWC code %ld\n",ierr);
      return;
    }

  (*status) = SUCCESS;
  return;
}



/* ***********************************************************************
 * ROUTINE NAME d_set_invert_matrix  
 *
 *
 * ABSTRACT  Will set the size of the workspace created for
 *           the routine invert_matrix. This is
 *           designed to allow multiple calls with the
 *           largest size being retained. 
 *           
 *            
 *
 * ENVIRONMENT  d_set_invert_matrix
 *
 *
 *
 * 
 * INPUTS  
 *    n                    the size of the square matrix
 *                           that will be input into
 *                           c_invert_matrix
 *          
 * OUTPUTS  
 *         
 * FUNCTIONS CALLED 
 *         
 *         
 *         
 *                                    
 * AUTHOR          Andrew Staniszewski
 *                                                        
 * MODIFICATION HISTORY                                   
 * 1.00     Original                          AJS       3-15-91   
 *                                                                 
 * ***********************************************************************
 */
void d_set_invert_matrix(int *n)

{
  extern int n_invert_matrix;

  if (n_invert_matrix < (*n))
    n_invert_matrix = (*n);

  return;
}

/* ***********************************************************************
 * ROUTINE NAME  d_init_invert_matrix
 *
 *
 * ABSTRACT  this routine sets up the workspace for the routine
 *             d_invert_matrix. It should only be called
 *             from within invert_matrix
 *           
 *            
 *
 * ENVIRONMENT  d_init_invert_matrix
 *
 *
 *
 * 
 * INPUTS  
 *          
 * OUTPUTS  
 *    status                       will be either SUCCESS or FAIL
 *         
 * FUNCTIONS CALLED 
 *         
 *         
 *         
 *                                    
 * AUTHOR          Andrew Staniszewski
 *                                                        
 * MODIFICATION HISTORY                                   
 * 1.00     Original                          AJS       3-8-91   
 *                                                                 
 * ***********************************************************************
 */
void d_init_invert_matrix(int *status)
{

  extern int *invert_matrix_ipvt;
  extern double *d_invert_matrix_wrk;

  extern int n_invert_matrix;
  extern int n_init_invert_matrix;

  int n;
  n = n_init_invert_matrix = n_invert_matrix;
  /* malloc the workspace */
  if (invert_matrix_ipvt != NULL)
    {
      free(invert_matrix_ipvt);
      free(invert_matrix_wrk);
    }


/* malloc some workspace */  
  invert_matrix_ipvt = (int *)calloc(n,sizeof(int));
  d_invert_matrix_wrk = (double *)calloc(n*n,sizeof(double));

  (*status) = SUCCESS;
  return;
}


/* ***********************************************************************
 * ROUTINE NAME d_invert_matrix  
 *
 *
 * ABSTRACT  will supply the inverse of a matrix
 *           
 *            
 *
 * ENVIRONMENT  d_invert_matrix
 *
 *
 * 
 * INPUTS  
 *     n                   the size of the a and b is n*n 
 *     a                   the input matrix
 *     lda                 leading dimension of a
 *     ldb                 leading dimension of b 
 *          
 * OUTPUTS  
 *     b              the inverted matrix 
 *     status         SUCCESS or FAIL
 *          
 *         
 * FUNCTIONS CALLED 
 *         
 *         
 *         
 *                                    
 * AUTHOR          Andrew Staniszewski
 *                                                        
 * MODIFICATION HISTORY                                   
 * 1.00     Original                          AJS       12-15-90   
 * 2.00     Update for use from fortran       AJS        3-18-91
 *                                                                 
 * ***********************************************************************
 */

/* this routine will invert a single precision real matrix */

void d_invert_matrix(int *n,double *a,double *b,
		   int *lda, int *ldb, int *status)
     
{
  extern int *invert_matrix_ipvt;
  extern double *d_invert_matrix_wrk;

  extern int n_init_invert_matrix;

  double t1[2];           /* workspace matricies */
  double rcond;            /* value indicating condition of input matrix */
  int ierr;             /*status flag for call */
  int i,j;                 /*loop variable */
  int calc_inv = 0;       /* indicates inverse of is to be calculated */
  int zero_dim = 0;	/* indicates that no solutions is needed see
			   NSWC page 211
			*/

  if ((*n) > n_init_invert_matrix)
    {
      d_set_invert_matrix(n);
      d_init_invert_matrix(status);
    }



/* first copy a to b */

  for (i = 0; i < (*n); i++)
    {
      for (j = 0; j < (*n); j++)
	{
	  b[i*(*ldb)+j] = a[i*(*lda)+j];
	}
    }
  
  DMSLV(&calc_inv,n,&zero_dim,b,ldb,NULL,&zero_dim,&t1[0],
       &rcond,&ierr,invert_matrix_ipvt,d_invert_matrix_wrk);

  if (ierr != 0) 
    {
      (*status) = FAIL;
      fprintf(stderr,"ELECTRO-F-INVRSINT Error in matrix inversion, NSWC code %ld\n",ierr);
      return;
    }

  (*status) = SUCCESS;
  return;
}


/* ***********************************************************************
 * ROUTINE NAME c_set_solve_linear  
 *
 *
 * ABSTRACT  Will set the size of the workspace created for
 *           the routine c_solve_linear. This is
 *           designed to allow multiple calls with the
 *           largest size being retained. 
 *           
 * ENVIRONMENT  c_set_solve_linear
 *
 * INPUTS  
 *    n                    the size of the square matrix
 *                           that will be input into
 *                           c_solve_linear
 *          
 * OUTPUTS  
 *         
 * FUNCTIONS CALLED 
 *         
 * AUTHOR          Jeff Prentice
 *                                                        
 * ***********************************************************************
 */
void c_set_solve_linear(int *n)

{
  extern int n_c_solve_linear;

  if (n_c_solve_linear < (*n))
    n_c_solve_linear = (*n);

  return;
}

/* ***********************************************************************
 * ROUTINE NAME  c_init_solve_linear
 *
 *
 * ABSTRACT  this routine sets up the workspace for the routine
 *             c_solve_linear. It should only be called
 *             from within c_solve_linear
 *           
 *            
 *
 * ENVIRONMENT  c_init_solve_linear
 * 
 * INPUTS  
 *          
 * OUTPUTS  
 *    status                       will be either SUCCESS or FAIL
 *         
 * FUNCTIONS CALLED 
 *         
 * AUTHOR          Jeff Prentice
 *                                                        
 * ***********************************************************************
 */
void c_init_solve_linear(int *status)
{
  extern COMPLEX *c_solve_linear_a;
  extern COMPLEX *c_solve_linear_b;

  extern int n_c_solve_linear;
  extern int n_c_init_solve_linear;

  int n;

  n = n_c_init_solve_linear = n_c_solve_linear;

  /* free the workspace */
  if (c_solve_linear_a != NULL)
  {
      free(c_solve_linear_a);
      free(c_solve_linear_b);
  }

/* malloc some workspace */  
  c_solve_linear_a = (COMPLEX *)calloc(n*n,sizeof(COMPLEX));
  c_solve_linear_b = (COMPLEX *)calloc(n*n,sizeof(COMPLEX));
  c_solve_linear_wrk = (COMPLEX *)calloc(n,sizeof(COMPLEX));
  c_solve_linear_ipvt = (int *)calloc(n,sizeof(int));

  (*status) = SUCCESS;
  return;
}


/* ***********************************************************************
 * ROUTINE NAME c_solve_linear  
 *
 *
 * ABSTRACT  will solve a complex linear system of equations 
 *	     (A*C = B) without destroying A or B.
 *           
 * ENVIRONMENT  dsovle_linear(n, a, b, c, lda, status)
 * 
 * INPUTS  
 *    int n;                   the size of the a  is n*n 
 *    COMPLEX *a;              the input matrix
 *    COMPLEX *b;              the input vector
 *    int lda;                 leading dimension of a
 *          
 * OUTPUTS  
 *    COMPLEX *c;              the solution vector 
 *    int     status;          SUCCESS or FAIL
 *          
 * FUNCTIONS CALLED 
 *                                    
 * AUTHOR          Jeff Prentice
 *                                                                 
 * ***********************************************************************
 */

void c_solve_linear(int *n, COMPLEX *a, COMPLEX *b,
		      COMPLEX *c, int *lda, int *status)
     
{
  extern COMPLEX *c_solve_linear_a;
  extern COMPLEX *c_solve_linear_b;
  extern COMPLEX *c_solve_linear_wrk;
  extern int *c_solve_linear_ipvt;

  extern int n_c_init_solve_linear;

  int i,j;
  int ierr;                /* status flag for call */
  int calc_inv = 1;        /* indicates not to calculate inverse */
  int one_dim = 1;

  if ((*n) > n_c_init_solve_linear)
    {
      c_set_solve_linear(n);
      c_init_solve_linear(status);
    }
  
  /* copy contents of a and b to temporary work arrays */
  for (i = 0; i < (*n); i++)
    {
      for (j = 0; j < (*n); j++)
	{
	  c_solve_linear_a[i*(*n)+j] = a[i*(*lda)+j];
	}
      c_solve_linear_b[i] = b[i];
    }


  CMSLV1(&calc_inv, n, &one_dim, c_solve_linear_a, lda, c_solve_linear_b, n, 
	 &ierr, c_solve_linear_ipvt, c_solve_linear_wrk);
  
  if (ierr != 0) 
    {
      (*status) = FAIL;
      fprintf(stderr,"ELECTRO-F-LININT Error in solution of linear system, NSWC code %ld\n",ierr);
      return;
    }

  for (i = 0; i < (*n); i++)
      c[i] = c_solve_linear_b[i];

  (*status) = SUCCESS;
  return;
}



/* ***********************************************************************
 * ROUTINE NAME d_c_set_solve_linear  
 *
 *
 * ABSTRACT  Will set the size of the workspace created for
 *           the routine d_c_solve_linear. This is
 *           designed to allow multiple calls with the
 *           largest size being retained. 
 *           
 *            
 *
 * ENVIRONMENT  d_c_set_solve_linear
 *
 *
 *
 * 
 * INPUTS  
 *    n                    the size of the square matrix
 *                           that will be input into
 *                           d_c_solve_linear
 *          
 * OUTPUTS  
 *         
 * FUNCTIONS CALLED 
 *         
 *         
 *         
 *                                    
 * AUTHOR          Andrew Staniszewski
 *                                                        
 * MODIFICATION HISTORY                                   
 * 1.00     Original                          AJS       3-15-91   
 *                                                                 
 * ***********************************************************************
 */
void d_c_set_solve_linear(int *n)

{
  extern int n_d_c_solve_linear;

  if (n_d_c_solve_linear < (*n))
    n_d_c_solve_linear = (*n);

  return;
}

/* ***********************************************************************
 * ROUTINE NAME  d_c_init_solve_linear
 *
 *
 * ABSTRACT  this routine sets up the workspace for the routine
 *             d_c_solve_linear. It should only be called
 *             from within d_c_solve_linear
 *           
 *            
 *
 * ENVIRONMENT  d_c_init_solve_linear
 *
 *
 *
 * 
 * INPUTS  
 *          
 * OUTPUTS  
 *    status                       will be either SUCCESS or FAIL
 *         
 * FUNCTIONS CALLED 
 *         
 *         
 *         
 *                                    
 * AUTHOR          Andrew Staniszewski
 *                                                        
 * MODIFICATION HISTORY                                   
 * 1.00     Original                          AJS       3-8-91   
 *                                                                 
 * ***********************************************************************
 */
void d_c_init_solve_linear(int * status)
{

  extern double *d_c_solve_linear_ai;
  extern double *d_c_solve_linear_ar;
  extern double *d_c_solve_linear_bi;
  extern double *d_c_solve_linear_br;

  extern int n_d_c_solve_linear;
  extern int n_d_c_init_solve_linear;

  int n;

  n = n_d_c_init_solve_linear = n_d_c_solve_linear;

  /* malloc the workspace */
  if (d_c_solve_linear_ai != NULL)
    {
      free(d_c_solve_linear_ai);
      free(d_c_solve_linear_ar);
      free(d_c_solve_linear_bi);
      free(d_c_solve_linear_br);
    }


/* malloc some workspace */  
  d_c_solve_linear_ai = (double *)calloc(n*n,sizeof(double));
  d_c_solve_linear_ar = (double *)calloc(n*n,sizeof(double));
  d_c_solve_linear_bi = (double *)calloc(n,sizeof(double));
  d_c_solve_linear_br = (double *)calloc(n,sizeof(double));
  d_c_solve_linear_wrk = (double *)calloc(2*n,sizeof(double));
  d_c_solve_linear_ipvt = (int *)calloc(n,sizeof(int));

  (*status) = SUCCESS;
  return;
}


/* ***********************************************************************
 * ROUTINE NAME d_c_solve_linear  
 *
 *
 * ABSTRACT  will solve a linear system of equations with doubles for input
 *	     (A*C = B)
 *           
 *            
 *
 * ENVIRONMENT  dsovle_linear(n, a, b, c, lda, status)
 *
 *
 * 
 * INPUTS  
 *    int n;                   the size of the a  is n*n 
 *    DOUBLE_COMPLEX *a;       the input matrix
 *    DOUBLE_COMPLEX *b;       the input vector
 *    int lda;                 leading dimension of a
 *          
 * OUTPUTS  
 *    DOUBLE_COMPLEX *c;       the solution vector 
 *    int     status;          SUCCESS or FAIL
 *          
 *         
 * FUNCTIONS CALLED 
 *         
 *         
 *         
 *                                    
 * AUTHOR          Andrew Staniszewski
 *                                                        
 * MODIFICATION HISTORY                                   
 * 1.00     Original                          AJS       12-15-90   
 *                                                                 
 * ***********************************************************************
 */

void d_c_solve_linear(int *n,DOUBLE_COMPLEX *a,DOUBLE_COMPLEX *b,
		      DOUBLE_COMPLEX *c,int *lda,int *status)
     
{
  extern double *d_c_solve_linear_ai;
  extern double *d_c_solve_linear_ar;
  extern double *d_c_solve_linear_bi;
  extern double *d_c_solve_linear_br;
  extern double *d_c_solve_linear_wrk;
  extern int *d_c_solve_linear_ipvt;

  extern int n_d_c_init_solve_linear;

  DOUBLE_COMPLEX t1[2];           /* workspace matricies */
  int rcond;            /* value indicating condition of input matrix */
  int ierr;             /*status flag for call */
  int i,j;                 /*loop variable */
  int calc_inv = 1;       /* indicates if inverse of is to be calculated */
  int one_dim = 1;

  if ((*n) > n_d_c_init_solve_linear)
    {
      d_c_set_solve_linear(n);
      d_c_init_solve_linear(status);
    }



/* first copy  a to real and imag mat */

  for (i = 0; i < (*n); i++)
    {
      for (j = 0; j < (*n); j++)
	{
	  d_c_solve_linear_ar[i*(*n)+j] = a[i*(*lda)+j].real;
	  d_c_solve_linear_ai[i*(*n)+j] = a[i*(*lda)+j].imaginary;
	}
      d_c_solve_linear_bi[i] = b[i].imaginary;
      d_c_solve_linear_br[i] = b[i].real;
    }
  
  DCMSLV(&calc_inv,n,&one_dim,d_c_solve_linear_ar,d_c_solve_linear_ai,
	 lda,d_c_solve_linear_br,d_c_solve_linear_bi,n,&ierr,
	 d_c_solve_linear_ipvt,d_c_solve_linear_wrk);
  

  if (ierr != 0) 
    {
      (*status) = FAIL;
      fprintf(stderr,"ELECTRO-F-LININT Error in solution of linear system, NSWC code %ld",ierr);
      return;
    }

  for (i = 0; i < (*n); i++)
    {
      c[i].real = d_c_solve_linear_br[i];
      c[i].imaginary = d_c_solve_linear_bi[i];
    }
  (*status) = SUCCESS;
  return;
}

/* ***********************************************************************
 * ROUTINE NAME set_solve_nonlinear  
 *
 *
 * ABSTRACT  Will set the size of the workspace created for
 *           the routine solve_nonlinear. This is
 *           designed to allow multiple calls with the
 *           largest size being retained. 
 *           
 *            
 *
 * ENVIRONMENT  set_solve_nonlinear
 *
 *
 *
 * 
 * INPUTS  
 *    n                   the number of varibles in the nonlinear equation 
 *          
 * OUTPUTS  
 *    status               will be SUCCESS or FAIL
 *         
 * FUNCTIONS CALLED 
 *         
 *         
 *         
 *                                    
 * AUTHOR          Andrew Staniszewski
 *                                                        
 * MODIFICATION HISTORY                                   
 * 1.00     Original                          AJS       3-15-91   
 *                                                                 
 * ***********************************************************************
 */
// void set_solve_nonlinear(int *n)
// 
// {
//   extern int n_solve_nonlinear;
// 
//   if (n_solve_nonlinear < (*n))
//     n_solve_nonlinear = (*n);
// 
//   return;
// }
// 
/* ***********************************************************************
 * ROUTINE NAME  init_solve_nonlinear
 *
 *
 * ABSTRACT  this routine sets up the workspace for the routine
 *             solve_nonlinear. It should only be called
 *             from within solve_nonlinear
 *           
 *            
 *
 * ENVIRONMENT  init_solve_nonlinear
 *
 *
 *
 * 
 * INPUTS  
 *          
 * OUTPUTS  
 *    status                       will be either SUCCESS or FAIL
 *         
 * FUNCTIONS CALLED 
 *         
 *         
 *         
 *                                    
 * AUTHOR          Andrew Staniszewski
 *                                                        
 * MODIFICATION HISTORY                                   
 * 1.00     Original                          AJS       3-8-91   
 *                                                                 
 * ***********************************************************************
 */
// void init_solve_nonlinear(int *status)
// {
//   extern int *solve_nonlinear_iwk;
//   extern float *solve_nonlinear_wrk;
//   extern float *solve_nonlinear_x;
// 
//   
//   extern int n_init_solve_nonlinear;
//   extern int n_solve_nonlinear;
//   extern int m_solve_nonlinear;
//   
//   int n;
//   int m;
//   
//   n = n_init_solve_nonlinear = n_solve_nonlinear;
//   
//   /* malloc the workspace */
//   
//   if (solve_nonlinear_wrk != NULL)
//     {
//       free(solve_nonlinear_wrk);
//     }
//   
//   /* malloc some workspace */
// 
//   m_solve_nonlinear = m = (int ) (ceil(n*(n+1)/2.0) + n*n +7*n) ;
// 
//   solve_nonlinear_wrk = (float *)calloc(m,sizeof(float));
//   solve_nonlinear_iwk = (int *)calloc(n,sizeof(int));
//   solve_nonlinear_x = (float *)calloc(n,sizeof(float));
// 
//   (*status) = SUCCESS;
//   return;
// }
// 
//
/* ***********************************************************************
 * ROUTINE NAME solve_nonlinear  
 *
 *
 * ABSTRACT  will solve a non-linear system of equations 
 *           
 *            
 *
 * ENVIRONMENT  sovle_nonlinear
 *
 *
 * 
 * INPUTS  
 *     void (*f)()              a pointer to a function that
 *                                will evaluate the system of equations
 *                                see page 137 of NSWC
 *     int n;                   the size of the system of equation
 *     float *x                 the initial guess for the solution
 *     float EPS                a statement of the relitive accuracy of (*f)()
 *     float TOL                the desired accuracy of the soln
 *                                s/b less than 1.0e-5
 * OUTPUTS
 *    float *x                 the final solution  
 *    float *fvec              a pointer to a vector that represents
 *                                what (*f)() evaluates to at the final 
 *                                solution
 *         
 * FUNCTIONS CALLED 
 *         
 *         
 *         
 *                                    
 * AUTHOR          Andrew Staniszewski
 *                                                        
 * MODIFICATION HISTORY                                   
 * 1.00     Original                          AJS       12-15-90   
 *                                                                 
 * ***********************************************************************
 */
// 
// void solve_nonlinear(void (*f)(),void (*f2)(), int *n,float *eps,float *tol,
// 		     float *x, float *fvec, int *status)
//      
// {
//   extern float *solve_nonlinear_wrk;
//   extern int *solve_nonlinear_iwk;
//   extern float *solve_nonlinear_x;
//   extern int n_init_solve_nonlinear;
//   int info = 0;
//   int i;
//   int m;
//   int mode = 1;
//   int maxfev = 200;
//   int ml_mu;
//   float factor = 100.0;
//   float gtol = 0.0;
//   int nprint = 0;
//   int nfev = 0;
//   int lr;
//   float sum;
//   float *m1,*m2,*m3,*m4,*m5,*m6,*m7,*mm;
//   
//   ml_mu = (*n) -1;
// 
//   if ((*n) > n_init_solve_nonlinear)
//     {
//       set_solve_nonlinear(n);
//       init_solve_nonlinear(status);
//     }
// 
//   m = n_init_solve_nonlinear;
//   lr = (int )ceil((m*(m+1))/2.0);
//   m1 = solve_nonlinear_wrk + lr;
//   m2 = m1 + m;
//   m3 = m2 + m;
//   m4 = m3 + m;
//   m5 = m4 + m;
//   m6 = m5 + m;
//   m7 = m6 + m;
//   mm = m7 + m;
// 
//   for (i = 0; i < (*n); i++)
//     {
//       solve_nonlinear_x[i] = x[i];
//     }
// /*
//   HYBRD(f,n,solve_nonlinear_x,fvec,tol,&maxfev,&ml_mu,&ml_mu,eps,
// 	m1,&mode,&factor,&nprint,&info,&nfev,mm,&m,solve_nonlinear_wrk,
// 	&lr,m2,m3,m4,m5,m6);
// */
//   if (info != 1)
//     {
//       for (i = 0; i < (*n); i++)
// 	{
// 	  solve_nonlinear_x[i] = x[i];
// 	}
// /*
//       LMDIF(f2,n,n,solve_nonlinear_x,fvec,tol,tol,&gtol,&maxfev,eps,
// 	    m1,&mode,&factor,&nprint,&info,&nfev,mm,&m,m2,m3,m4,m5,
// 	    m6,m7);
// */
//       printf("dummy, you didn't reactivate the non linear problem solver\n");
//       if (info == 4)
// 	{
// 	  for (i = 0,sum=0; i < (*n); i++)
// 	    {
// 	      sum += fvec[i] * fvec[i];
// 	    }
// 	  if (sum == 0)
// 	    info = 3;
// 	}
//  
//      if (info != 1 && info != 2 && info != 3) 
// 
// 	{
// 	  (*status) = FAIL;
//	  fprintf(stderr,"ELECTRO-F-NONLININT Error in solution of nonlinear system, NSWC code %ld\n",info);
// 	  return;
// 	}
//     }
//   for (i = 0; i < (*n); i++)
//     {
//       x[i] = solve_nonlinear_x[i];
//     }
//   (*status) = SUCCESS;
//   return;
// }
// 
/* ***********************************************************************
 * ROUTINE NAME lu_factor
 *
 *
 * ABSTRACT  Factors a real matrix A by gaussian elimination (A = L*U).
 *           
 * ENVIRONMENT  lu_factor(n, a, lu, lda, ipvt, status)
 * 
 * INPUTS  
 *    int *n;               the order of matrix a
 *    float *a;             the matrix to be factored
 *    int *lda;             leading dimension of a
 *          
 * OUTPUTS  
 *    int *ipvt;	    integer vector of pivot indices
 *    float *lu;	    factorization of A (= L*U)
 *                          if a is not needed, pass a or NULL for lu
 *    int     *status;      SUCCESS or FAIL
 *          
 * FUNCTIONS CALLED 
 *    sgefa (NSWC originally from LINPACK)
 *                                    
 * AUTHOR          Jeff Prentice
 *                                                                 
 * ***********************************************************************
 */

void lu_factor(int *n, float *a, float *lu, int *lda, 
		 int *ipvt, int *status)
{
    int i,j;	/* loop indices */
    int info;   /* status flag for call */

    /********************************************************************
    *									*
    * If the matrix lu is passed (not NULL and different than a), the	*
    * matrix a is copied to lu which is used in the factorization and	*
    * will contain the results, preserving the contents of a.		*
    * Otherwise a will contain the results destroying its original	*
    * contents.								*
    *									*
    ********************************************************************/
    if ((lu != NULL) && (lu != a))
    {
	for (i = 0; i < (*n); i++)
	    for (j = 0; j < (*n); j++)
		lu[i*(*n)+j] = a[i*(*lda)+j];
	SGEFA(lu, lda, n, ipvt, &info);
    }	
    else
	SGEFA(a, lda, n, ipvt, &info);
	

    /********************************************************************
    *									*
    * A nonzero value for info indicates that u(info,info) = 0.0.  This *
    * is not necessarily an error condition, however it does indicate	*
    * that using the l*u factorization to solve (sgesl) or to invert	*
    * (sgedi) will divide by zero.					*
    *									*
    ********************************************************************/
    if (info != 0)   
    {
      (*status) = FAIL;
      fprintf(stderr,"ELECTRO-F-LUFACT Error in LU factorization of matrix, NSWC code %ld\n",info);
      return;
    }

  (*status) = SUCCESS;
  return;
}
// 
/* ***********************************************************************
 * ROUTINE NAME lu_factor_cond
 *
 *
 * ABSTRACT  Factors a real matrix A by gaussian elimination (A = L*U).
 *           Returns a condition number.
 *           
 * ENVIRONMENT  lu_factor_cond(n, a, lu, lda, ipvt, rcond, status)
 * 
 * INPUTS  
 *    int *n;               the order of matrix a
 *    float *a;             the matrix to be factored
 *    int *lda;             leading dimension of a
 *          
 * OUTPUTS  
 *    int *ipvt;	    integer vector of pivot indices
 *    float *lu;	    factorization of A (= L*U)
 *                          if a is not needed, pass a or NULL for lu
 *    float *rcond          condition number
 *    int *status;      SUCCESS or LUFACTCN
 *          
 * FUNCTIONS CALLED 
 *    sgeco (NSWC originally from LINPACK)
 *                                    
 * AUTHOR          Jeff Prentice
 *                                                                 
 * ***********************************************************************
 */

void lu_factor_cond(int *n, float *a, float *lu, int *lda, 
		 int *ipvt, float *rcond, int *status)
{
    int i,j;	           /* loop indices */
    static int *z;         /* workspace vector */
    static int old_n = 0;  /* previous value of n used to size z */
    float check;           /* used for doing a check of rcond */
    double condition;      /* used to pass rcond to message */
    
        
    // z is a vector of size n - enlarge if needed
    if(*n > old_n)
      {
	free(z);
	z = (int *)malloc(sizeof(int) * *n);
	old_n = *n; // save largest n
      }
    
    
    /********************************************************************
    *									*
    * If the matrix lu is passed (not NULL and different than a), the	*
    * matrix a is copied to lu which is used in the factorization and	*
    * will contain the results, preserving the contents of a.		*
    * Otherwise a will contain the results destroying its original	*
    * contents.								*
    *									*
    ********************************************************************/
    if ((lu != NULL) && (lu != a))
    {
	for (i = 0; i < (*n); i++)
	    for (j = 0; j < (*n); j++)
		lu[i*(*n)+j] = a[i*(*lda)+j];
	SGECO(lu, lda, n, ipvt, rcond, z);
    }	
    else
	SGECO( a, lda, n, ipvt, rcond, z);
	
    /********************************************************************
    *                                                                   *
    * What about the rcond condition number?  Here is the recommended   *
    * way to check this number, if ( rcond + 1.0 == 1.0 ) then the      *
    * matrix should be treated as singular.  This is different, though  *
    * probably not independent of the z value which is discussed in the *
    * NSWC documentation.                                               *
    *                                                                   *
    ********************************************************************/    

    check = 1.0 + *rcond;
    if (check == 1.0)   
    {
      (*status) = FAIL ;
      condition = *rcond;
      fprintf(stderr,"ELECTRO-E-LUFACTCN LU factorization shows matrix is singular, condition number %e\n",condition);
      return;
    }

  (*status) = SUCCESS;
  return;
}

/* ***********************************************************************
 * ROUTINE NAME dlu_factor
 *
 *
 * ABSTRACT  Factors a real matrix A by gaussian elimination (A = L*U).
 *           
 * ENVIRONMENT  dlu_factor(n, a, lu, lda, ipvt, status)
 * 
 * INPUTS  
 *    int *n;               the order of matrix a
 *    double *a;             the matrix to be factored
 *    int *lda;             leading dimension of a
 *          
 * OUTPUTS  
 *    int *ipvt;	    integer vector of pivot indices
 *    double *lu;	    factorization of A (= L*U)
 *                          if a is not needed, pass a or NULL for lu
 *    int     *status;      SUCCESS or FAIL
 *          
 * FUNCTIONS CALLED 
 *    dgefa (NSWC originally from LINPACK)
 *                                    
 * AUTHOR          Jeff Prentice
 *                                                                 
 * ***********************************************************************
 */

void dlu_factor(int *n, double *a, double *lu, int *lda, 
		 int *ipvt, int *status)
{
    int i,j;	/* loop indices */
    int info;   /* status flag for call */

    /********************************************************************
    *									*
    * If the matrix lu is passed (not NULL and different than a), the	*
    * matrix a is copied to lu which is used in the factorization and	*
    * will contain the results, preserving the contents of a.		*
    * Otherwise a will contain the results destroying its original	*
    * contents.								*
    *									*
    ********************************************************************/
    if ((lu != NULL) && (lu != a))
    {
	for (i = 0; i < (*n); i++)
	    for (j = 0; j < (*n); j++)
		lu[i*(*n)+j] = a[i*(*lda)+j];
	DGEFA(lu, lda, n, ipvt, &info);
    }	
    else
	DGEFA(a, lda, n, ipvt, &info);
	

    /********************************************************************
    *									*
    * A nonzero value for info indicates that u(info,info) = 0.0.  This *
    * is not necessarily an error condition, however it does indicate	*
    * that using the l*u factorization to solve (sgesl) or to invert	*
    * (sgedi) will divide by zero.					*
    *									*
    ********************************************************************/
    if (info != 0)   
    {
      (*status) = FAIL;
      fprintf(stderr,"ELECTRO-F-LUFACT Error in LU factorization of matrix, NSWC code %ld\n",status);
      return;
    }

  (*status) = SUCCESS;
  return;
}

/* ***********************************************************************
 * ROUTINE NAME lu_solve_linear
 *
 *
 * ABSTRACT  Solves the real system A*X=B using the factors computed
 *	     from lu_factor.  
 *           
 * ENVIRONMENT  lu_solve_linear(n, a, x, b, lda, ipvt, status)
 * 
 * INPUTS  
 *    int *n;               the order of matrix a
 *    float *a;             lu factored matrix output from lu_factor
 *    float *b;             right hand side vector (a*x=b)
 *    int *lda;             leading dimension of matrix
 *    int *ipvt;	    integer vector of pivot indices from lu_factor
 *          
 * OUTPUTS  
 *    float *x;		    the solution vector 
 *                          if b is not needed, pass b or NULL in for x
 *    int     *status;      SUCCESS
 *          
 * FUNCTIONS CALLED 
 *    sgesl (NSWC originally from LINPACK)
 *                                    
 * AUTHOR          Jeff Prentice
 *                                                                 
 * ***********************************************************************
 */


void lu_solve_linear(int *n, float *a, float *x, float *b, int *lda, 
		     int *ipvt, int *status)
{
    int i;	/* loop indices */
    int job=0;  /* indicates to solve a*x=b */

    /********************************************************************
    *									*
    * If the matrix x is passed (not NULL and different than b), the	*
    * matrix b is copied to x which will contain the results,		*
    * preserving the contents of b.  Otherwise b will contain the	*
    * results destroying its original contents.				*
    *									*
    ********************************************************************/
    if ((x != NULL) && (x != b))
    {
	for (i = 0; i < (*n); i++)
	    x[i] = b[i];
	SGESL(a, lda, n, ipvt, x, &job);
    }	
    else
	SGESL(a, lda, n, ipvt, b, &job);
	
  (*status) = SUCCESS;
  return;
}

/* ***********************************************************************
 * ROUTINE NAME dlu_solve_linear
 *
 *
 * ABSTRACT  Solves the real system A*X=B using the factors computed
 *	     from dlu_factor.  
 *           
 * ENVIRONMENT  dlu_solve_linear(n, a, x, b, lda, ipvt, status)
 * 
 * INPUTS  
 *    int *n;               the order of matrix a
 *    double *a;             lu factored matrix output from dlu_factor
 *    double *b;             right hand side vector (a*x=b)
 *    int *lda;             leading dimension of matrix
 *    int *ipvt;	    integer vector of pivot indices from dlu_factor
 *          
 * OUTPUTS  
 *    double *x;		    the solution vector 
 *                          if b is not needed, pass b or NULL in for x
 *    int     *status;      SUCCESS
 *          
 * FUNCTIONS CALLED 
 *    dgesl (NSWC originally from LINPACK)
 *                                    
 * AUTHOR          Jeff Prentice
 *                                                                 
 * ***********************************************************************
 */


void dlu_solve_linear(int *n, double *a, double *x, double *b, int *lda, 
		     int *ipvt, int *status)
{
    int i;	/* loop indices */
    int job=0;  /* indicates to solve a*x=b */

    /********************************************************************
    *									*
    * If the matrix x is passed (not NULL and different than b), the	*
    * matrix b is copied to x which will contain the results,		*
    * preserving the contents of b.  Otherwise b will contain the	*
    * results destroying its original contents.				*
    *									*
    ********************************************************************/
    if ((x != NULL) && (x != b))
    {
	for (i = 0; i < (*n); i++)
	    x[i] = b[i];
	DGESL(a, lda, n, ipvt, x, &job);
    }	
    else
	DGESL(a, lda, n, ipvt, b, &job);
	
  (*status) = SUCCESS;
  return;
}
