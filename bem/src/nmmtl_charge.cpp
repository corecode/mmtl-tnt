
/*
  
  FACILITY:  NMMTL
  
  MODULE DESCRIPTION:
  
  Contains the functions: nmmtl_charge() and nmmtl_charge_free_space()
  
  AUTHOR(S):
  
  Kevin J. Buchs
  
  CREATION DATE:  Thu Apr  2 14:55:00 1992
  
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
/*
 *******************************************************************
 **  FUNCTION DECLARATIONS
 *******************************************************************
 */
/*
 *******************************************************************
 **  FUNCTION DEFINITIONS
 *******************************************************************
 */


/*
  
  FUNCTION NAME:   nmmtl_charge()
  
  
  FUNCTIONAL DESCRIPTION:
  
  Integrates the sigma vector over each conductor to arrive at a value for
  electrostatic induction.
  
  FORMAL PARAMETERS:
  
  float *sigma_vector                - vector of charge distributions over 
  conductors
  int conductor_counter,             - how many conductors
  CONDUCTOR_DATA_P conductor_data,   - array of data on conductors
  float *electrostatic_induction,    - out: results (almost capacitance)
  single row of matrix
  
  
  RETURN VALUE:
  
  SUCCESS,FAIL
  
  CALLING SEQUENCE:
  
  nmmtl_charge(sigma_vector,conductor_counter,
  conductor_data,electrostatic_induction);
  
  */

void nmmtl_charge(float *sigma_vector,
		  int conductor_counter,
		  CONDUCTOR_DATA_P conductor_data,
		  float *electrostatic_induction)
{
  int cond_num;
  CELEMENTS_P cel;
  int Legendre_counter;
  double Jacobian;
  int i;
  double shape[INTERP_PTS];
  float nu0,nu1;
  
  for(cond_num = 1;cond_num <= conductor_counter; cond_num++)
  {
    /* zero it out */
    electrostatic_induction[cond_num-1] = 0.0;
    
    cel=conductor_data[cond_num].elements;
    while(cel != NULL)
    {
      for(Legendre_counter = 0; Legendre_counter < Legendre_root_c_max;
	  Legendre_counter++)
      {
	if(cel->edge[0] != NULL || cel->edge[1] != NULL)
	{
	  /* if given edge is really an edge, set the true value of nu,
	     otherwise, don't really care */
	  nu0 = cel->edge[0] ? cel->edge[0]->nu : 0;
	  nu1 = cel->edge[1] ? cel->edge[1]->nu : 0;
	  nmmtl_shape_c_edge(Legendre_root_c[Legendre_counter],shape,cel,
			     nu0,nu1);
	}
	else
	  nmmtl_shape(Legendre_root_c[Legendre_counter],shape);
	
	nmmtl_jacobian_c(Legendre_root_c[Legendre_counter],cel,&Jacobian);
	
	/* now add in the contributions to the the basis points */
	for(i=0;i < INTERP_PTS;i++)
	  electrostatic_induction[cond_num-1] +=
	    cel->epsilon * Legendre_weight_c[Legendre_counter] *
	      shape[i] * sigma_vector[cel->node[i]] * Jacobian;
	
      } /* for looping on the Legendre points */
      
      cel = cel->next;
      
    } /* while looping on elements of particular conductor */
  } /* for all conductors */
}



/*
  
  FUNCTION NAME:   nmmtl_charge_free_space()
  
  
  FUNCTIONAL DESCRIPTION:
  
  Integrates the sigma vector over each conductor to arrive at a value for
  electrostatic induction.  Similar to nmmtl_charge, except for:
  
  (1) calls nmmtl_shape_c_edge with the free space values of nu.
  (2) uses AIR_CONSTANT instead of real epsilon value for conductor
  
  FORMAL PARAMETERS:
  
  float *sigma_vector                - vector of charge distributions over 
  conductors
  int conductor_counter,             - how many conductors
  CONDUCTOR_DATA_P conductor_data,   - array of data on conductors
  float *electrostatic_induction,    - out: results (almost capacitance)
  single row of matrix
  
  
  RETURN VALUE:
  
  SUCCESS,FAIL
  
  CALLING SEQUENCE:
  
  nmmtl_charge_free_space(sigma_vector,conductor_counter,
  conductor_data,electrostatic_induction);
  
  */

void nmmtl_charge_free_space(float *sigma_vector,
			     int conductor_counter,
			     CONDUCTOR_DATA_P conductor_data,
			     float *electrostatic_induction)
{
  int cond_num;
  CELEMENTS_P cel;
  int Legendre_counter;
  double Jacobian;
  int i;
	double shape[INTERP_PTS];
  float nu0,nu1;
  
  for(cond_num = 1;cond_num <= conductor_counter; cond_num++)
  {
    /* zero it out */
    electrostatic_induction[cond_num-1] = 0.0;
    
    cel=conductor_data[cond_num].elements;
    while(cel != NULL)
    {
      for(Legendre_counter = 0; Legendre_counter < Legendre_root_c_max;
	  Legendre_counter++)
      {
	if(cel->edge[0] != NULL || cel->edge[1] != NULL)
	{
	  /* if given edge is really an edge, set the true value of nu,
	     otherwise, don't really care */
	  nu0 = cel->edge[0] ? cel->edge[0]->free_space_nu : 0;
	  nu1 = cel->edge[1] ? cel->edge[1]->free_space_nu : 0;
	  nmmtl_shape_c_edge(Legendre_root_c[Legendre_counter],shape,cel,
			     nu0,nu1);
	}
	else
	  nmmtl_shape(Legendre_root_c[Legendre_counter],shape);
	
	nmmtl_jacobian_c(Legendre_root_c[Legendre_counter],cel,&Jacobian);
	
	/* now add in the contributions to the the basis points */
	for(i=0;i < INTERP_PTS;i++)
	  electrostatic_induction[cond_num-1] +=
	    AIR_CONSTANT * Legendre_weight_c[Legendre_counter] *
	      shape[i] * sigma_vector[cel->node[i]] * Jacobian;
	
      } /* for looping on the Legendre points */
      
      cel = cel->next;
      
    } /* while looping on elements of particular conductor */
  } /* for all conductors */
}

