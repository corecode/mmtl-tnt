
/*
  
  FACILITY:  NMMTL
  
  MODULE DESCRIPTION:
  
  Contains the nmmtl_load(), and nmmtl_load_free_space() functions
  
  AUTHOR(S):
  
  Kevin J. Buchs
  
  CREATION DATE:  Thu Apr  2 11:57:24 1992
  
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
  
  FUNCTION NAME:  nmmtl_load
  
  
  FUNCTIONAL DESCRIPTION:
  
  Sets up the potential vector to set a particular signal line to one volt
  potential at one time.  See nmmtl_unload for un-doing this.
  
  FORMAL PARAMETERS:
  
  float *potential_vector            - vector of potential on all elements
  int conductor_number               - the number of the conductor to set to
  one volt potential
  CONDUCTOR_DATA_P conductor_data,   - array of data on conductors
  
  RETURN VALUE:
  
  none
  
  CALLING SEQUENCE:
  
  nmmtl_load(potential_vector,ic,conductor_data);
  
  
  */

void nmmtl_load(float *potential_vector,
		int conductor_number,
		CONDUCTOR_DATA_P conductor_data)
{
  unsigned int Legendre_counter;
  int i;
	double shape[INTERP_PTS];
	double Jacobian;
  CELEMENTS_P cel;
  float nu0,nu1;
  
#ifdef BEM3_VARIANT
	double coef;
  coef = EPSILON_NAUGHT/4E-12;
#endif
  
  /* assume the potential_vector is already zeroed */
  
  cel = conductor_data[conductor_number].elements;
  while(cel != NULL)
  {
    for(Legendre_counter = 0; Legendre_counter < Legendre_root_l_max;
	Legendre_counter++)
    {
      if(cel->edge[0] != NULL || cel->edge[1] != NULL)
      {
	/* if given edge is really an edge, set the true value of nu,
	   otherwise, don't really care */
	nu0 = cel->edge[0] ? cel->edge[0]->nu : 0;
	nu1 = cel->edge[1] ? cel->edge[1]->nu : 0;
	nmmtl_shape_c_edge(Legendre_root_l[Legendre_counter],shape,cel,
			   nu0,nu1);
      }
      else
	nmmtl_shape(Legendre_root_l[Legendre_counter],shape);
      
      nmmtl_jacobian_c(Legendre_root_l[Legendre_counter],cel,&Jacobian);
      
      for(i=0; i < INTERP_PTS; i++)
      {
#ifdef BEM3_VARIANT
	potential_vector[cel->node[i]] +=
	  coef * Legendre_weight_a[Legendre_counter] * shape[i] * Jacobian;
#else
	potential_vector[cel->node[i]] +=
	  EPSILON_NAUGHT * Legendre_weight_a[Legendre_counter] * shape[i] *
	    Jacobian;
#endif
      }
    }
    cel = cel->next;
  }
}




/*
  
  FUNCTION NAME:  nmmtl_load_free_space
  
  
  FUNCTIONAL DESCRIPTION:
  
  Sets up the potential vector to set a particular signal line to one volt
  potential at one time.  See nmmtl_unload for un-doing this.
  
  Just like nmmtl_load, but calls nmmtl_shape_c_edge with the free
  space values for nu.
  
  FORMAL PARAMETERS:
  
  float *potential_vector            - vector of potential on all elements
  int conductor_number               - the number of the conductor to set to
  one volt potential
  CONDUCTOR_DATA_P conductor_data,   - array of data on conductors
  
  RETURN VALUE:
  
  none 
  
  CALLING SEQUENCE:
  
  nmmtl_load_free_space(potential_vector,ic,conductor_data);
  
  
  */

void nmmtl_load_free_space(float *potential_vector,
			   int conductor_number,
			   CONDUCTOR_DATA_P conductor_data)
{
  unsigned int Legendre_counter;
  int i;
	double shape[INTERP_PTS];
	double Jacobian;
  CELEMENTS_P cel;
  float nu0,nu1;
  
#ifdef BEM3_VARIANT
	double coef;
  coef = EPSILON_NAUGHT/4E-12;
#endif
  
  /* assume the potential_vector is already zeroed */
  
  cel = conductor_data[conductor_number].elements;
  while(cel != NULL)
  {
    for(Legendre_counter = 0; Legendre_counter < Legendre_root_l_max;
	Legendre_counter++)
    {
      if(cel->edge[0] != NULL || cel->edge[1] != NULL)
      {
	/* if given edge is really an edge, set the true value of nu,
	   otherwise, don't really care */
	nu0 = cel->edge[0] ? cel->edge[0]->free_space_nu : 0;
	nu1 = cel->edge[1] ? cel->edge[1]->free_space_nu : 0;
	nmmtl_shape_c_edge(Legendre_root_l[Legendre_counter],shape,cel,
			   nu0,nu1);
      }
      else
	nmmtl_shape(Legendre_root_l[Legendre_counter],shape);
      
      nmmtl_jacobian_c(Legendre_root_l[Legendre_counter],cel,&Jacobian);
      
      for(i=0; i < INTERP_PTS; i++)
      {
#ifdef BEM3_VARIANT
	potential_vector[cel->node[i]] +=
	  coef * Legendre_weight_a[Legendre_counter] * shape[i] * Jacobian;
#else
	potential_vector[cel->node[i]] +=
	  EPSILON_NAUGHT * Legendre_weight_a[Legendre_counter] * shape[i] *
	    Jacobian;
#endif
      }
    }
    cel = cel->next;
  }
}

