
/* 
  
  FACILITY:  NMMTL
  
  MODULE DESCRIPTION:
  
  contains the function nmmtl_qsp_kernel()
  
  AUTHOR(S):
  
  Kevin J. Buchs
  
  CREATION DATE:  Fri Mar 20 16:02:12 1992
  
  COPYRIGHT:   Copyright (C) 1992 by Mayo Foundation. All rights reserved.
  
  */


/* 
 *******************************************************************
 **  INCLUDE FILES
 *******************************************************************
 */

#include "nmmtl.h"

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

#define no_condition_number yes

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

FILE *plotFile=NULL;

/* What should we set the slope to for a vertical line? */
const float INFINITE_SLOPE = FLT_MAX;

/* 
 *******************************************************************
 **  FUNCTION DECLARATIONS
 *******************************************************************
 */

void nmmtl_write_plot_data(
			   CONTOURS_P signal,
			   int conductor_counter,
			   DELEMENTS_P die_elements,
			   CONDUCTOR_DATA_P conductor_data,
			   float *sigma_vector,
			   FILE *outputFile
			 );

#ifdef IMSL_LU_ROUTE
int linrg();
int lfsrg();
#endif
#ifdef IMSL_INV_ROUTE
int lftrg();
#endif



/* 
 *******************************************************************
 **  FUNCTION DEFINITIONS
 *******************************************************************
 */


/* 
  
  FUNCTION NAME:  nmmtl_qsp_kernel
  
  
  FUNCTIONAL DESCRIPTION:
  
  The kernel of the quasi-static parameter calculation
  
  FORMAL PARAMETERS:
  
  int conductor_counter,               - number of conductors
  CONDUCTOR_DATA_P conductor_data,     - conductor elements organized by cond.
  DELEMENTS_P die_elements,            - all the die elements
  unsigned int node_point_counter,     - total number of node points
  unsigned int highest_conductor_node, - highest node number for conductors
  double length_scale,                 - a scale factor based on element length
  float **electrostatic_induction,     - out: results (almost capacitance)
  float **inductance,                  - out: results
  float *characteristic_impedance,     - out: results
  float *propagation_velocity,         - out: results
  float *equivalent_dielectric,        - out: results
  FILE *output_file1, *output_file2);  - file pointers to print results to.
  CONTOURS_P signals                   - list of signal data including names
  
  RETURN VALUE:
  
    SUCCESS, FAIL, or other Failure statuses
  
  CALLING SEQUENCE:
  
  nmmtl_qsp_kernel(conductor_counter,conductor_data,die_elements,
  node_point_counter,highest_conductor_node,
  length_scale,electrostatic_induction,
  inductance,characteristic_impedance,
  propagation_velocity,equivalent_dielectric,
  output_file1,output_file2,
  signals);
  
  */

int nmmtl_qsp_kernel(int conductor_counter,
				 CONDUCTOR_DATA_P conductor_data,
				 DELEMENTS_P die_elements,
				 unsigned int node_point_counter,
				 unsigned int highest_conductor_node,
				 double length_scale,
				 float **electrostatic_induction,
				 float **inductance,
				 float *characteristic_impedance,
				 float *propagation_velocity,
				 float *equivalent_dielectric,
				 FILE *output_file1,
				 FILE *output_file2,
				 CONTOURS_P signals)
{

  int ic, jc;
  int *ipvt;
  float **assemble_matrix;
  float *sigma_vector;
  float *potential_vector;
  float rcond;
  int status;
  int int_status;
  int matrix_order;
  int i,j;
  float **electrostatic_induction_free_space;
  char msg[256];
  char asmsg1[512],asmsg2[512]; /* strings for asymmetry messages */
  float error,error_sum,error_max;
  unsigned int error_count;
  CONTOURS_P activeLine;
  
  /* - - - - - - - -  Allocate the matricies and vectors  - - - - - - - - - */
  
  /* allocate and zero space for free space electrostatic induction */
  electrostatic_induction_free_space = (float **) dim2(conductor_counter,
						       conductor_counter,
						       sizeof(float));
  
  /* allocate and zero assemble matrix */
  assemble_matrix = (float **) dim2(node_point_counter,node_point_counter,sizeof(float));
  
  /* allocate and zero sigma vector */
  sigma_vector = (float *)calloc(node_point_counter,sizeof(float));
  
  /* allocate and zero potential vector */
  potential_vector = (float *)calloc(node_point_counter,sizeof(float));
  
  
  /* - - - - - - - -  Free Space Solution  - - - - - - - - - */
  
  /* First solve matrix equation for free space - no dielectrics
     Solve:  Ax = B for x, 
     Where:  A = assemble_matrix
     x = sigma_vector
     B = potential vector
     
     Start by calculating assemble matrix for free space:
     Amn, LHS of matrix equation
     
     */
 
  printf ("Calculate LHS (assemble) matrix in free space\n");
  
  nmmtl_assemble_free_space(conductor_counter,conductor_data,assemble_matrix);
  
#ifdef TRANSPOSE_ASSEMBLE
  for(i = 0; i<conductor_counter; i++)
  {
    for(j = i+1; j<conductor_counter; j++)
    {
      static float temp;
      temp = assemble_matrix[i][j];
      assemble_matrix[i][j] = assemble_matrix[j][i];
      assemble_matrix[j][i] = temp;
    }
  }
#endif
  
  
  /* We only need to solve for the conductor portion of the assemble matrix.
     So, we store away a smaller number for the order of the matrix */
  
  matrix_order = highest_conductor_node + 1; /* offset for 0th node */
  
#ifdef IMSL_LU_ROUTE
  
  /* allocate vector for pivoting info to keep */
  ipvt = calloc(matrix_order,sizeof(int));
  
  /*
    call IMSL routine to compute LU factorization of assemble_matrix matrix 
    
    matrix_order  = order of matrix 
    assemble_matrix     = node_point_counter x node_point_counter
    matrix to be factored 
    node_point_counter  = leading dimension of assemble_matrix 
    assemble_matrix     = factored matrix output 
    node_point_counter  = leading dimension of assemble_matrix 
    ipvt                = vector of length matrix_order containing
    pivoting infomation
    */
  
  lftrg(&matrix_order,assemble_matrix[0],&node_point_counter,
	assemble_matrix[0],&node_point_counter,&ipvt);
  
#elif NSWC_LU_ROUTE
  
  /* allocate vector for pivoting info to keep */
  ipvt = (int *)calloc(matrix_order,sizeof(int));
  
  /* call NSWC routine (via wrapper) to compute LU factorization of 
     assemble_matrix matrix:
     
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
     *    float *rcond;     condition number
     *    int *status;      SUCCESS or LUFACTCN
     *          
     */
  
#ifdef no_condition_number
  lu_factor(&matrix_order,assemble_matrix[0], assemble_matrix[0],
		 (int *) &node_point_counter,ipvt,&int_status);
  // int_status will always be returned as SUCCESS, but check in case
  // someone changes this.
  if(int_status != SUCCESS) return(FAIL);  /* translate to int */
#else
  lu_factor_cond(&matrix_order,assemble_matrix[0], assemble_matrix[0],
		 (int *) &node_point_counter,ipvt,&rcond,
                 &status);

  /* check condition number if a environmental is set */
  /* but don't follow exit... */
  if(test_logical("NMMTL_CONDITION_NUMBER"))
    {
      float t;
      t = 1.0 + rcond;
      if( t == 1.0 )
	printf ("Assemble(free space) Matrix Condition Number: Warning %g\n",rcond);
      else 
	printf ("Assemble(free space) Matrix Condition Number: OK %g\n",rcond);
    }

  if(status == ELECTRO_LUFACTCN)
    {
      printf ("\nAn ill-conditioned matrix prevents further computation.\n");
      printf ("This is often caused by mesh elements which are very small.\n");
      printf ("It is recommended that you adjust the CSEG parameter lower if\n");
      printf ("you are using a value much greater than the default.  Otherwise, simply\n");
      printf ("adjusting it up or down a small amount may affect the meshing.\n");

      /* don't return this status if the logical is set, so we can continue 
	 executing */
      if( ! test_logical("NMMTL_CONDITION_NUMBER")) return(status);

    }

#endif  /* #else no_condition_number */
  
#endif  /* #elif NSWC_LU_ROUTE */
  
  
  
  /* do for each conductor being charged */
  
  for (ic = 1; ic <= conductor_counter; ++ic)
  {
    
    /* Calculate RHS of matrix equation */
    
    sprintf(msg,"calculate RHS (load) matrix for conductor %d\n",ic);

    printf ("%s", msg);
    
    nmmtl_load_free_space(potential_vector,ic,conductor_data);

    printf ("Solve system of equations\n");
    
#ifdef IMSL_LU_ROUTE
    
    /*
      IMSL routine to solve linear system of equations once lftrg is
      called 
      
      matrix_order       = number of equations 
      assemble_matrix    = node_point_counter x node_point_counter
      matrix LU factored 
      node_point_counter = leading dimension of assemble_matrix 
      ipvt               = pivoting info from call to lftrg 
      potential_vector   = vector of RHS 
      1                  = means solve Ax = B 
      sigma_vector       = vector of length matrix_order which
      contains the solution.
      */
    
    lfsrg(&matrix_order, assemble_matrix[0], &node_point_counter,
	  ipvt, potential_vector, 1, sigma_vector);
    
#elif NSWC_LU_ROUTE
    
    
    /*
      NSWC routine to solve linear system of equations once lu_factor is
      called 
      
      * ENVIRONMENT  lu_solve_linear(n, a, x, b, lda, ipvt, status)
      * 
      * INPUTS  
      *    int *n;          the order of matrix a
      *    float *a;        lu factored matrix output from lu_factor
      *    float *b;        right hand side vector (a*x=b)
      *    int *lda;        leading dimension of matrix
      *    int *ipvt;	    integer vector of pivot indices from lu_factor
      *          
      * OUTPUTS  
      *    float *x;	    the solution vector 
      *                          if b is not needed, pass b or NULL in for x
      *    int *status;     SUCCESS
      *          
      */
    
    lu_solve_linear(&matrix_order,assemble_matrix[0],sigma_vector,
		    potential_vector,(int *) &node_point_counter,ipvt,
		    &int_status);
    // int_status will always be returned as SUCCESS, but check in case
    // someone changes this.
    if(int_status != SUCCESS) return(FAIL);  /* translate to int */
    
#endif  /* #elif NSWC_LU_ROUTE */
    
    /* integrate charge density to get total charge */
    /* charge is same as capacitance - since V=1 volt to output file. */
    
    printf ("Integrate charge density\n");
    
    nmmtl_charge_free_space(sigma_vector,conductor_counter,
			    conductor_data,
			    electrostatic_induction_free_space[ic-1]);
    
    /* zero out RHS vector for ic-th conductor */
    nmmtl_unload(potential_vector,ic,conductor_data);
    
  }     /* end loop for each conductor */
  
  
  /* calculate the inductance (inductance) matrix from
     the free space capacitance matrix. */
  
#ifdef IMSL_INV_ROUTE
  
  /*
    Calculate the inverse of the capacitance matrix 
    
    conductor_counter                  = order of matrix 
    electrostatic_induction_free_space = matrix to invert 
    conductor_counter                  = leading dimension of cap 
    inductance                         = inverse of matrix cap (output) 
    conductor_counter                  = leading dimension of inductance
    
    */
  
  linrg(&conductor_counter, electrostatic_induction_free_space[0],
	&conductor_counter, inductance,
	&conductor_counter);
  
#elif NSWC_INV_ROUTE
  
  /*
    Calculate the inverse of the capacitance matrix 
    
    conductor_counter                  = order of matrix 
    electrostatic_induction_free_space = matrix to invert 
    conductor_counter                  = leading dimension of cap 
    inductance                         = inverse of matrix cap (output) 
    conductor_counter                  = leading dimension of inductance
    
    */
  /*
    void invert_matrix_cond(int *n,float *a,float *b,
    int *lda, int *ldb, int *status)
    * INPUTS  
    *     n                   the size of the a and b is n*n 
    *     a                   the input matrix
    *     lda                 leading dimension of a
    *     ldb                 leading dimension of b 
    *          
    * OUTPUTS  
    *     b              the inverted matrix 
    *     status         SUCCESS or ELECTRO_INVRSINT
    
    */
  

#ifdef no_condition_number
  invert_matrix(&conductor_counter,electrostatic_induction_free_space[0],
		inductance[0],&conductor_counter,&conductor_counter,
		&int_status);
  // int_status will always be returned as SUCCESS, but check in case
  // someone changes this.
  if(int_status != SUCCESS) return(FAIL); /* translate to int */
#else
  invert_matrix_cond(&conductor_counter,electrostatic_induction_free_space[0],
		     inductance[0],&conductor_counter,&conductor_counter,
		     &rcond,&status);
  // check condition number if a environmental variable is set
  /* but don't follow exit... */
  if(test_logical("NMMTL_CONDITION_NUMBER"))
    {
      float t;
      t = 1.0 + rcond;
      if ( t == 1.0 ) 
	printf ("Capacitance Matrix Condition Number: Warning %g\n",rcond);
      else 
	printf ("Capacitance Matrix Condition Number: OK %g\n",rcond);
    }

  if(status == ELECTRO_INVRSINT)  
    {
      printf ("\nAn ill-conditioned matrix prevents further computation.\n");
      printf ("This is probably caused by conductors which have almost\n");
      printf ("No interaction with the other conductors, because of\n");
      printf ("distant spacing.  It is probably good to remove such\n");
      printf ("conductors from the simulation, and assume values are zero.\n");
      /* don't return this status if the logical is set, so we can continue 
	 executing */
      if( ! test_logical("NMMTL_CONDITION_NUMBER")) return(status);

    }
  
#endif /* #else no_condition_number */
  
#endif /* #elif NSWC_INV_ROUTE */
  
  /* multiply by mu naught and epsilon naught to scale inductance matrix' */
  
  for (ic = 0; ic < conductor_counter; ++ic)
    for (jc = 0; jc < conductor_counter; ++jc)
      inductance[jc][ic] *= C_SQUARED_INVERTED;
  
  /* Now compute the maximum and average relative error */
  
  error_max = 0.0;
  error_sum = 0.0;
  error_count = 0;
  
  for (ic = 0; ic < conductor_counter; ++ic)
  {
    for (jc = ic+1; jc < conductor_counter; ++jc)
    {
      error = fabs((inductance[jc][ic] - inductance[ic][jc]) /
		   inductance[ic][jc]);
      if(error > error_max) error_max = error;
      error_sum += error;
      error_count++;
    }
  }
  
  if(error_count > 0)
  {
    if(error_max > 0.01)
    {
      sprintf(asmsg1,
	      "**********\n\
  Asymmetry ratio for inductance matrix:\n\
     %f%% (max), %f%% (average).\n\
  (Note values greater than 1%% are a probable indication of too few elements.\n\
  Try adjusting CSEG and DSEG attributes.)\n\
**********",
	      error_max*100,error_sum*100/error_count);
      printf ("%s", asmsg1);
    }
    else
    {
      sprintf(asmsg1,
	      "  Asymmetry ratio for inductance matrix:\n\
     %f%% (max), %f%% (average)\n",
	      error_max*100,error_sum*100/error_count);
      printf ("%s", asmsg1);
    }
  }
  
  
  
  /* - - - - - - - -  Dielectric Solution  - - - - - - - - - */
  
  /* Next solve matrix equation for real dielectrics
     Solve:  Ax = B for x, 
     Where:  A = assemble_matrix
     x = sigma_vector
     B = potential vector
     
     Start by calculating assemble matrix for real dielectrics:
     Amn, LHS of matrix equation
     */
  
  /* zero out portion of the matrix that was used above */
  
  for(i=0;i <= highest_conductor_node;i++)
    for(j=0;j <= highest_conductor_node;j++)
      assemble_matrix[i][j] = 0.0;
  
  printf ("Calculate LHS (assemble) matrix in dielectric\n");
  
  nmmtl_assemble(conductor_counter,conductor_data,die_elements,
		 length_scale,assemble_matrix);
  
#ifdef TRANSPOSE_ASSEMBLE
  for(i = 0; i < node_point_counter; i++)
  {
    for(j = i+1; j < node_point_counter; j++)
    {
      static float temp;
      temp = assemble_matrix[i][j];
      assemble_matrix[i][j] = assemble_matrix[j][i];
      assemble_matrix[j][i] = temp;
    }
  }
#endif
  
  /* already offset for zeroth node */
  matrix_order = node_point_counter; /*use all nodes when dielectrics are in*/ 
  
#ifdef IMSL_LU_ROUTE
  
  /*
    call IMSL routine to compute LU factorization of assemble_matrix matrix 
    
    matrix_order        = order of matrix 
    assemble_matrix     = node_point_counter x node_point_counter
    matrix to be factored 
    node_point_counter  = leading dimension of assemble_matrix 
    assemble_matrix     = factored matrix output 
    node_point_counter  = leading dimension of assemble_matrix 
    ipvt                = vector of length node_point_counter containing
    pivoting infomation
    */
  
  lftrg(&matrix_order, assemble_matrix[0], &node_point_counter,
	assemble_matrix[0], &node_point_counter, &ipvt);
  
  
#elif NSWC_LU_ROUTE

  /* allocate vector for pivoting info to keep */
  ipvt = (int *)calloc(node_point_counter,sizeof(int));
  
  /* call NSWC routine (via wrapper) to compute LU factorization of 
     assemble_matrix matrix:
     
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
     *    float *rcond;     condition number
     *    int *status;      SUCCESS or ELECTRO_LUFACTCN  
     *          
     */
  

#ifdef no_condition_number
  lu_factor(&matrix_order,assemble_matrix[0], assemble_matrix[0],
		 (int *) &node_point_counter,ipvt,&int_status);
  // int_status will always be returned as SUCCESS, but check in case
  // someone changes this.
  if(int_status != SUCCESS) return(FAIL);  /* translate to int */
#else
  lu_factor_cond(&matrix_order,assemble_matrix[0],
	    assemble_matrix[0],(int *)&node_point_counter,ipvt,&rcond,
	    &status);

  /* check condition number if a environmental is set */
  /* but don't follow exit... */
  if(test_logical("NMMTL_CONDITION_NUMBER"))
    {
      float t;
      t = 1.0 + rcond;
      if( t == 1.0 )
	printf ("Assemble Matrix Condition Number: Warning %g\n",rcond);
      else 
	printf ("Assemble Matrix Condition Number: OK %g\n",rcond);
    }

  if(status == ELECTRO_LUFACTCN)
    {
      printf ("\nAn ill-conditioned matrix prevents further computation.\n");
      printf ("This is often caused by mesh elements which are very small.\n");
      printf ("It is recommended that you adjust the DSEG parameter lower if\n");
      printf ("you are using a value much greater than the default.  Otherwise, simply\n");
      printf ("adjusting it up or down a small amount may affect the meshing.\n");

      /* don't return this status if the logical is set, so we can continue 
	 executing */
      if( ! test_logical("NMMTL_CONDITION_NUMBER")) return(status);

    }

#endif /* #else no_condition_number */  
  
#endif /* #elif NSWC_LU_ROUTE */
  
  /* do for each conductor being charged */
  
  for (activeLine = signals,ic = 1; 
       ic <= conductor_counter; 
       activeLine = activeLine->next,++ic)
  {
    
    /* Calculate RHS of matrix equation */
    
    sprintf(msg,"calculate RHS (load) matrix for conductor %d\n",ic);

    printf ("%s", msg);
    
    nmmtl_load(potential_vector,ic,conductor_data);
    
    printf ("Solve system of equations\n");
    
#ifdef IMSL_LU_ROUTE
    
    /*
      IMSL routine to solve linear system of equations once lftrg is
      called 
      
      matrix_order = number of equations 
      assemble_matrix    = node_point_counter x node_point_counter
      matrix LU factored 
      node_point_counter = leading dimension of assemble_matrix 
      ipvt               = pivoting info from call to lftrg 
      potential_vector   = vector of RHS 
      1                  = means solve Ax = B 
      sigma_vector       = vector of length matrix_order which
      contains the solution.
      */
    
    lfsrg(&matrix_order, assemble_matrix[0], (int *) &node_point_counter, &ipvt,
	  &potential_vector, 1, sigma_vector);
    
#elif NSWC_LU_ROUTE
    
    
    /*
      NSWC routine to solve linear system of equations once lu_factor is
      called 
      
      * ENVIRONMENT  lu_solve_linear(n, a, x, b, lda, ipvt, status)
      * 
      * INPUTS  
      *    int *n;          the order of matrix a
      *    float *a;        lu factored matrix output from lu_factor
      *    float *b;        right hand side vector (a*x=b)
      *    int *lda;        leading dimension of matrix
      *    int *ipvt;	    integer vector of pivot indices from lu_factor
      *          
      * OUTPUTS  
      *    float *x;	    the solution vector 
      *                          if b is not needed, pass b or NULL in for x
      *    int *status;     SUCCESS
      *          
      */
    
    lu_solve_linear(&matrix_order,assemble_matrix[0],sigma_vector,
		    potential_vector,(int *)&node_point_counter,ipvt,
		    &int_status);
    
    // int_status will always be returned as SUCCESS, but check in case
    // someone changes this.
    if(int_status != SUCCESS) return(FAIL);  /* translate to int */
    
#endif /* #elif NSWC_LU_ROUTE */
    
    /* integrate charge density to get total charge */
    /* write charge - same as capacitance - since V=1 volt to output file. */
    
    printf ("Integrate charge density\n");
    
    nmmtl_charge(sigma_vector,conductor_counter,
		 conductor_data,electrostatic_induction[ic-1]);
    
    /* if the main opened the plotFile, then write out the plot data */
    if (plotFile != NULL)
      {
	nmmtl_write_plot_data(
			      activeLine,
			      conductor_counter,
			      die_elements,
			      conductor_data,
			      sigma_vector,
			      plotFile
			    );
      }
			  
    /* zero out RHS vector for ic-th conductor */
    nmmtl_unload(potential_vector,ic,conductor_data);
    
  }     /* end loop for each conductor */
  
  
  /* Now compute the maximum and average relative error */
  
  error_max = 0.0;
  error_sum = 0.0;
  error_count = 0;
  
  for (ic = 0; ic < conductor_counter; ++ic)
  {
    for (jc = ic+1; jc < conductor_counter; ++jc)
    {
      error = fabs((electrostatic_induction[jc][ic] -
		    electrostatic_induction[ic][jc]) /
		   electrostatic_induction[ic][jc]);
      if(error > error_max) error_max = error;
      error_sum += error;
      error_count++;
    }
  }
  
  if(error_count > 0)
  {
    if(error_max > 0.01)
    {
      
      sprintf(asmsg2,
	      "**********\n\
  Asymmetry ratio for electrostatic induction matrix:\n\
     %f%% (max), %f%% (average).\n\
  (Note values greater than 1%% are a probable indication of too few elements.\n\
  Try adjusting CSEG and DSEG attributes.)\n\
**********",
	      error_max*100,error_sum*100/error_count);
      printf ("%s", asmsg2);
      
    }
    else
    {
      sprintf(asmsg2,
	      "  Asymmetry ratio for electrostatic induction matrix:\n\
     %f%% (max), %f%% (average).\n",
	      error_max*100,error_sum*100/error_count);
      printf ("%s", asmsg2);
    }
  }
  
  
  /* - - - - - - - -  Output the matricies 'n stuff - - - - - - - - - */
  
  if(output_file1 != NULL)
  {
    nmmtl_output_matrices(output_file1,
			  electrostatic_induction,
			  inductance,
			  signals);
    if(error_count > 0)
    {
      fputs("\nAsymmetry Ratios:\n",output_file1);
      putc('\n',output_file1);
      fputs(asmsg1,output_file1);
      putc('\n',output_file1);
      fputs(asmsg2,output_file1);
      putc('\n',output_file1);
    }
  }
  
  if(output_file2 != NULL)
  {
    nmmtl_output_matrices(output_file2,
			  electrostatic_induction,
			  inductance,
			  signals);
    if(error_count > 0)
    {
      fputs("\nAsymmetry Ratios:\n",output_file2);
      putc('\n',output_file2);
      fputs(asmsg1,output_file2);
      putc('\n',output_file2);
      fputs(asmsg2,output_file2);
      putc('\n',output_file2);
    }
  }
  
  
  /* NOW: calculate the characteristic impedance and the propagation
     velocity */
  
  status = nmmtl_charimp_propvel_calculate(conductor_counter,
					   signals,
					   electrostatic_induction,
					   inductance,
					   electrostatic_induction_free_space,
					   characteristic_impedance,
					   propagation_velocity,
					   equivalent_dielectric,
					   output_file1,
					   output_file2);
  
  if(status != SUCCESS) return(status);
  
  
  /* Don't need to save this - since it is not returned */
  
  free2((void **)electrostatic_induction_free_space);
  
  return(SUCCESS);
  
}

