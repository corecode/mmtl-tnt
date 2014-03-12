#ifndef math_library_h
#define math_library_h

/* header file for routines using the math library */
/* one letter prefixes in routine names have the following meanings

      --  no prefix indicates single precision real
    c --  complex data being used 
    d --  double sized data being used
    d_c -- double complex data is being used

*/

#ifndef complex_numbers_h
#include "complex_numbers.h"
#endif

#ifndef math_h
#include <math.h>
#endif

/* Create aliases for NSWC FORTRAN routines so C programs can call them */
/* on Alpha/OSF and Sun Sparc hosts */

//--------------------------------------
#ifdef FORTRAN_UNDERBARS


#define FFT fft_
#define CEIGV ceigv_
#define CMSLV1 cmslv1_
#define CMTMS cmtms_
#define DCMSLV dcmslv_
#define MSLV mslv_
#define DMSLV dmslv_
#define MTMS mtms_
//#define HBRD hbrd_
//#define HYBRD hybrd_
//#define LMDIFF lmdiff_
//#define LMDIF lmdif_
#define SGEFA sgefa_
#define SGECO sgeco_
#define SGESL sgesl_     
#define DGEFA dgefa_
#define DGESL dgesl_     

/* And create aliases so C routines in math_library.c can be called from
   FORTRAN on Alpha and Sparc */

#define c_fft c_fft_
#define c_fft_inv c_fft_inv_
#define c_set_calc_eigenvalues c_set_calc_eigenvalues_
#define c_init_calc_eigenvalues c_init_calc_eigenvalues_
#define c_calc_eigenvalues c_calc_eigenvalues_
#define c_mult_matricies c_mult_matricies_
#define mult_matricies mult_matricies_
#define c_set_invert_matrix c_set_invert_matrix_
#define c_init_invert_matrix c_init_invert_matrix_
#define c_invert_matrix c_invert_matrix_
#define set_invert_matrix set_invert_matrix_
#define init_invert_matrix init_invert_matrix_
#define invert_matrix invert_matrix_
#define invert_matrix_cond invert_matrix_cond_
#define d_set_invert_matrix d_set_invert_matrix_
#define d_init_invert_matrix d_init_invert_matrix_
#define d_invert_matrix d_invert_matrix_
#define c_set_solve_linear c_set_solve_linear_
#define c_init_solve_linear c_init_solve_linear_
#define c_solve_linear c_solve_linear_
#define d_c_set_solve_linear d_c_set_solve_linear_
#define d_c_init_solve_linear d_c_init_solve_linear_
#define d_c_solve_linear d_c_solve_linear_
//#define set_solve_nonlinear set_solve_nonlinear_
//#define init_solve_nonlinear init_solve_nonlinear_
//#define solve_nonlinear solve_nonlinear_
#define lu_factor lu_factor_
#define lu_factor_cond lu_factor_cond_
#define lu_solve_linear lu_solve_linear_

//  For Gnu gcc and g77, we need double-underbars, before and after
// the name.
#else
#ifdef __GCC__

#define FFT _fft_
#define CEIGV _ceigv_
#define CMSLV1 _cmslv1_
#define CMTMS _cmtms_
#define DCMSLV _dcmslv_
#define MSLV _mslv_
#define DMSLV _dmslv_
#define MTMS _mtms_
//#define HBRD _hbrd_
//#define HYBRD _hybrd_
//#define LMDIFF _lmdiff_
//#define LMDIF _lmdif_
#define SGEFA _sgefa_
#define SGECO _sgeco_
#define SGESL _sgesl_     
#define DGEFA _dgefa_
#define DGESL _dgesl_     

// Other hosts (hp7, you need just case conversion to call FORTRAN from C
// Since FORTRAN uppercase is all converted to lowercase.
//--------------------------------------

#else


#define FFT fft
#define CEIGV ceigv
#define CMSLV1 cmslv1
#define CMTMS cmtms
#define DCMSLV dcmslv
#define MSLV mslv
#define DMSLV dmslv
#define MTMS mtms
//#define HBRD hbrd
//#define HYBRD hybrd
//#define LMDIFF lmdiff
//#define LMDIF lmdif
#define SGEFA sgefa
#define SGECO sgeco
#define SGESL sgesl     
#define DGEFA dgefa
#define DGESL dgesl     

// end of else for fortran underbars

#endif
#endif

// C function definitions


extern "C" void c_fft(int *n,COMPLEX *a,COMPLEX *c,int *status);

extern "C" void c_fft_inv(int *n,COMPLEX *a,COMPLEX *c,int *status);


extern "C" void c_set_calc_eigenvalues(int *n);

extern "C" void c_init_calc_eigenvalues(int * status);

extern "C" void c_calc_eigenvalues(COMPLEX *c,int *n,COMPLEX *eval,COMPLEX *evect,
			      int *ldc, int *ldevect, int *status); 


extern "C" void c_mult_matricies(int *ra,int *carb, int *cb,
			   COMPLEX *a, COMPLEX *b, COMPLEX *c,
			   int *lda, int *ldb, int *ldc,int *status);

extern "C" void mult_matricies(int *ra,int *carb, int *cb,
			   float *a, float *b, float *c,
			   int *lda, int *ldb, int *ldc,int *status);


extern "C" void c_set_invert_matrix(int *n);

extern "C" void c_init_invert_matrix(int *status);

extern "C" void c_invert_matrix(int *n,COMPLEX *a,COMPLEX *b,
			   int *lda, int *ldb, int *status);

extern "C" void set_invert_matrix(int *n);

extern "C" void init_invert_matrix(int *status);

extern "C" void invert_matrix(int *n,float *a,float *b,
			   int *lda, int *ldb, int *status);

extern "C" void invert_matrix_cond(int *n,float *a,float *b,
				   int *lda, int *ldb, float *rcond,
				   int *status);

extern "C" void d_set_invert_matrix(int *n);

extern "C" void d_init_invert_matrix(int *status);

extern "C" void d_invert_matrix(int *n,double *a,double *b,
			   int *lda, int *ldb, int *status);


extern "C" void c_set_solve_linear(int *n);

extern "C" void c_init_solve_linear(int *status);

extern "C" void c_solve_linear(int *n, COMPLEX *a, COMPLEX *b,
		      COMPLEX *c, int *lda, int *status);


extern "C" void d_c_set_solve_linear(int *n);

extern "C" void d_c_init_solve_linear(int * status);

extern "C" void d_c_solve_linear(int *n, DOUBLE_COMPLEX *a, DOUBLE_COMPLEX *b,
		      DOUBLE_COMPLEX *c, int *lda, int *status);


//extern "C" void set_solve_nonlinear(int *n);

//extern "C" void init_solve_nonlinear(int *status);

//extern "C" void solve_nonlinear(void (*f)(),void (*f2)(), int *n,float *eps,
//               float *tol, float *x, float *fvec, int *status);


extern "C" void lu_factor(int *n, float *a, float *lu, int *lda, 
		 int *ipvt, int *status);

extern "C" void lu_factor_cond(int *n, float *a, float *lu, int *lda, 
		 int *ipvt, float *rcond, int *status);

extern "C" void lu_solve_linear(int *n, float *a, float *x, float *b, int *lda,
		 int *ipvt, int *status);

extern "C" void dlu_factor(int *n, double *a, double *lu, int *lda, 
		 int *ipvt, int *status);

extern "C" void dlu_solve_linear(int *n, double *a, double *x, double *b,
		 int *lda, int *ipvt, int *status);

/* Declarations of NSWC routines */

extern "C" void FFT(COMPLEX *c, int *n, int *sign, int *ierr);

extern "C" void CEIGV(int *bal,
		 float *c_calc_eigenvalues_ar,float *c_calc_eigenvalues_ai,
		 int *n1, int *n2,
		 float *c_calc_eigenvalues_wr,float *c_calc_eigenvalues_wi,
		 float *c_calc_eigenvalues_zr,float *c_calc_eigenvalues_zi,
		 int *ierr,
		 float *c_calc_eigenvalues_temp);

extern "C"  void CMTMS(int *ra, int *carb, int *cb, COMPLEX *a, int *lda,
			   COMPLEX *b, int *ldb, COMPLEX *c, int *ldc);

extern "C"  void MTMS(int *ra,int *carb,int *cb,float *a,int *lda,
			  float *b,int *ldb,float *c,int *ldc);

extern "C"  void CMSLV1(int *calc_inv,int *n,int *zero_dim1,
			    COMPLEX *c_solve_linear_a, int *lda,
			    COMPLEX *c_solve_linear_b, 
			    int *n2,
			    int *ierr,
			    int *c_invert_matrix_ipvt,
			    COMPLEX *c_invert_matrix_wrk);

extern "C"  void MSLV(int *calc_inv,int *n,int *zero_dim1,
			  float *b,int *ldb,int *dum,int *zero_dim2,
			  float *t1, float *rcond,int *ierr,
			  int *invert_matrix_ipvt,float *invert_matrix_wrk);

extern "C"  void DMSLV(int *calc_inv,int *n,int *zero_dim1,
			  double *b,int *ldb,int *dum,int *zero_dim2,
			  double *t1, double *rcond,int *ierr,
			  int *invert_matrix_ipvt,double *invert_matrix_wrk);

extern "C"  void DCMSLV(int *calc_inv,
                   int *n1,
			    int *one_dim,
			    double *d_c_solve_linear_ar,
			    double *d_c_solve_linear_ai,
			    int *lda,
			    double *d_c_solve_linear_br,
			    double *d_c_solve_linear_bi,
			    int *n2,
			    int *ierr,
			    int *d_c_solve_linear_ipvt,
			    double *d_c_solve_linear_wrk);

extern "C" void SGEFA(float *lu, int *lda, int *n, int *ipvt, int *info);

extern "C" void SGECO(float *lu, int *lda, int *n, int *ipvt, float *rcond, 
                 int *info);

extern "C" void SGESL(float *a, int *lda, int *n, int *ipvt, float *x, int *job);

extern "C" void DGEFA(double *lu, int *lda, int *n, int *ipvt, int *info);

extern "C" void DGESL(double *a, int *lda, int *n, int *ipvt, double *x, int *job);


#endif
