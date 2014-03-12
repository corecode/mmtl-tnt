
/*
  
  FACILITY:  NMMTL
  
  MODULE DESCRIPTION:
  
  Contains nmmtl_assemble_free_space function.
  
  AUTHOR(S):
  
  Kevin J. Buchs
  
  CREATION DATE:  Mon Mar 30 12:47:58 1992
  
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
  
  FUNCTION NAME:  nmmtl_assemble_free_space
  
  FUNCTIONAL DESCRIPTION:
  
  Calculates the assemble matrix for the Boundary element 
  solution of Multilayer, Multiconductor Transmission Line
  Quasi-static Parameter calculations.  This is the free space
  version of nmmtl_assemble, and it assumes that no dielectric
  material exists, it is all free space.  It goes through the elements
  in the system only if they are dielectric-conductor
  elements, and computes their contribution to each node in the
  system.  Later, the assemble matrix is used to solve a matrix equation
  
  FORMAL PARAMETERS:
  
  int conductor_counter,             - how many conductors
  CONDUCTOR_DATA_P conductor_data,   - array of data on conductors
  float **assemble_matrix            - out: resultant assemble matrix
  
  RETURN VALUE:
  
  None
  
  CALLING SEQUENCE:
  
  nmmtl_assemble_free_space(conductor_counter,conductor_data,
                            assemble_matrix);
  
  */

void nmmtl_assemble_free_space(int conductor_counter,
			       CONDUCTOR_DATA_P conductor_data,
			       float **assemble_matrix)
{
  
  int i,j,cond_num,inner_cond_num;
  CELEMENTS_P cel,inner_cel;
  int Legendre_counter;
  double x,y;  /* interpolated coordinates */
  double shape[INTERP_PTS];
  double value[INTERP_PTS];
  double Jacobian;
  double nu0,nu1;
  
  
  /* matrix should be zeroed */
  
  /* create outer loop on the the conductor elements - by looping on
     both conductors and then each element of each conductor */
  
  for(cond_num = 0; cond_num <= conductor_counter; cond_num++)
  {
    cel=conductor_data[cond_num].elements;
    while(cel != NULL)
    {
      for(Legendre_counter = 0; Legendre_counter < Legendre_root_a_max;
	  Legendre_counter++)
      {
	nmmtl_shape(Legendre_root_a[Legendre_counter],shape);
	
	/* interpolate x,y coordinate using no_edge shape function */
	x = 0.0;
	y = 0.0;
	for(i=0;i < INTERP_PTS;i++)
	{
	  x += shape[i]*cel->xpts[i];
	  y += shape[i]*cel->ypts[i];
	}
	
	/* if an edge element - recalculate shape using edge effects */
	
	if(cel->edge[0] != NULL || cel->edge[1] != NULL)
	{
	  /* if given edge is really an edge, set the true value of nu,
	     otherwise, don't really care */
	  nu0 = cel->edge[0] ? cel->edge[0]->free_space_nu : 0;
	  nu1 = cel->edge[1] ? cel->edge[1]->free_space_nu : 0;
	  nmmtl_shape_c_edge(Legendre_root_a[Legendre_counter],shape,cel,
			     nu0,nu1);
	}
	
	nmmtl_jacobian_c(Legendre_root_a[Legendre_counter],cel,&Jacobian);
	
	/* Now an inner loop over all the elements, performing an
	   integration in each call to nmmtl_interval_*** */
	
	/* inner loop on conductor elements - broken into 3 parts - want
	   to act differently for the self element - inner_cel == cel and
	   this will only need to be checked while inner_cond_num == cond_num 
	   */
	
	/* PART 1 */
	
	for(inner_cond_num = 0; inner_cond_num < cond_num;
	    inner_cond_num++)
	{
	  inner_cel=conductor_data[inner_cond_num].elements;
	  while(inner_cel != NULL)
	  {
	    nmmtl_interval_c_fs(x,y,inner_cel,value);
	    
	    /* now add in the contributions to the the basis points */
	    for(i=0;i < INTERP_PTS;i++)
	    {
	      for(j=0;j < INTERP_PTS;j++)
	      {
		assemble_matrix[inner_cel->node[j]][cel->node[i]] +=
		  ASSEMBLE_CONST_1 * Legendre_weight_a[Legendre_counter] *
		    shape[i] * value[j] * Jacobian;
	      }
	    }

	    inner_cel = inner_cel->next;
	  } /* while inner looping on elements of a particular conductor */
	} /* for inner looping on the conductors */
	
	/* PART 2 */
	
	inner_cel=conductor_data[inner_cond_num].elements;
	while(inner_cel != NULL)
	{
	  /* Are we at the self element ? */
	  if(cel == inner_cel)
	    nmmtl_interval_self_c_fs(x,y,inner_cel,value,
				     Legendre_root_a[Legendre_counter]);
	  else
	    nmmtl_interval_c_fs(x,y,inner_cel,value);
	  
	  /* now add in the contributions to the the basis points */
	  for(i=0;i < INTERP_PTS;i++)
	  {
	    for(j=0;j < INTERP_PTS;j++)
	    {
	      assemble_matrix[inner_cel->node[j]][cel->node[i]] +=
		ASSEMBLE_CONST_1 * Legendre_weight_a[Legendre_counter] *
		  shape[i] * value[j] * Jacobian;
	    }
	  }
	  inner_cel = inner_cel->next;
	} /* while inner looping on elements of a particular conductor */
	
	
	/* PART 3 */
	
	for(inner_cond_num++; inner_cond_num <= conductor_counter;
	    inner_cond_num++)
	{
	  inner_cel=conductor_data[inner_cond_num].elements;
	  while(inner_cel != NULL)
	  {
	    nmmtl_interval_c_fs(x,y,inner_cel,value);
	    
	    /* now add in the contributions to the the basis points */
	    for(i=0;i < INTERP_PTS;i++)
	    {
	      for(j=0;j < INTERP_PTS;j++)
	      {
		assemble_matrix[inner_cel->node[j]][cel->node[i]] +=
		  ASSEMBLE_CONST_1 * Legendre_weight_a[Legendre_counter] *
		    shape[i] * value[j] * Jacobian;
	      }
	    }
	    inner_cel = inner_cel->next;
	  } /* while inner looping on elements of a particular conductor */
	} /* for inner looping on the conductors */
	
      } /* while stepping through Guass-Legendre roots */
      
      cel = cel->next;
      
    } /* while outer looping on elments of a conductor */
  } /* while outer looping on conductors */
  
}


