
/*
  
  FACILITY:  NMMTL
  
  MODULE DESCRIPTION:
  
  Contains nmmtl_assemble function.
  
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
  
  FUNCTION NAME:  nmmtl_assemble
  
  FUNCTIONAL DESCRIPTION:
  
  Calculates the assemble matrix for the Boundary element 
  solution of Multilayer, Multiconductor Transmission Line
  Quasi-static Parameter calculations.  Goes through all elements
  in the system, whether dielectric-conductor or dielectric-dielectric
  elements, and computes their contribution to each node in the
  system.  Later, the assemble matrix is used to solve a matrix equation
  
  FORMAL PARAMETERS:
  
  int conductor_counter,             - how many conductors
  CONDUCTOR_DATA_P conductor_data,   - array of data on conductors
  DELEMENTS_P die_elements,          - all die element data
  double length_scale,                - a scale factor based on element length
  float **assemble_matrix            - out: resultant assemble matrix
  
  RETURN VALUE:
  
  None
  
  CALLING SEQUENCE:
  
  nmmtl_assemble(conductor_counter,conductor_data,die_elements,
                 assemble_matrix);
  
  */

void nmmtl_assemble(int conductor_counter,
		    CONDUCTOR_DATA_P conductor_data,
		    DELEMENTS_P die_elements,
		    float length_scale,
		    float **assemble_matrix)
{
  
  int i,j,cond_num,inner_cond_num;
  CELEMENTS_P cel,inner_cel;
  DELEMENTS_P del,inner_del;
  int Legendre_counter;
	double x,y;  /* interpolated coordinates */
	double shape[INTERP_PTS];
	double value[INTERP_PTS];
	double Jacobian;
	double coef1,coef2;
  float nu0,nu1;
  
  /* matrix should be zeroed */
  
  /* first create outer loop on the the conductor elements - by looping on
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
	  nu0 = cel->edge[0] ? cel->edge[0]->nu : 0;
	  nu1 = cel->edge[1] ? cel->edge[1]->nu : 0;
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
	    /* outer element is a conductor - TRUE,0,0 for last args */
	    nmmtl_interval_c(x,y,inner_cel,value,TRUE,0,0);
	    
	    /* now add in the contributions to the the basis points */
	    for(i=0;i < INTERP_PTS;i++)
	      for(j=0;j < INTERP_PTS;j++)
	      {
#ifdef BEM3_VARIANT
		double x,y;
		x = ASSEMBLE_CONST_1 / 4e-12;
		y = Legendre_weight_a[Legendre_counter];
		assemble_matrix[inner_cel->node[j]][cel->node[i]] +=
		  x * y * shape[i] * value[j] * Jacobian;
#else
		assemble_matrix[inner_cel->node[j]][cel->node[i]] +=
		  ASSEMBLE_CONST_1 * Legendre_weight_a[Legendre_counter] *
		    shape[i] * value[j] * Jacobian;
#endif	    
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
	    nmmtl_interval_self_c(x,y,inner_cel,value,
				  Legendre_root_a[Legendre_counter]);
	  else
	    /* outer element is a conductor - TRUE,0,0 for last args */
	    nmmtl_interval_c(x,y,inner_cel,value,TRUE,0,0);
	  
	  /* now add in the contributions to the the basis points */
	  for(i=0;i < INTERP_PTS;i++)
	    for(j=0;j < INTERP_PTS;j++)
	    {
#ifdef BEM3_VARIANT
				double x,y;
	      x = ASSEMBLE_CONST_1 / 4e-12;
	      y = Legendre_weight_a[Legendre_counter];
	      assemble_matrix[inner_cel->node[j]][cel->node[i]] +=
		x * y * shape[i] * value[j] * Jacobian;
#else
	      assemble_matrix[inner_cel->node[j]][cel->node[i]] +=
		ASSEMBLE_CONST_1 * Legendre_weight_a[Legendre_counter] *
		  shape[i] * value[j] * Jacobian;
#endif	  
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
	    /* outer element is a conductor - TRUE,0,0 for last args */
	    nmmtl_interval_c(x,y,inner_cel,value,TRUE,0,0);
	    
	    /* now add in the contributions to the the basis points */
	    for(i=0;i < INTERP_PTS;i++)
	      for(j=0;j < INTERP_PTS;j++)
	      {
#ifdef BEM3_VARIANT
		double x,y;
		x = ASSEMBLE_CONST_1 / 4e-12;
		y = Legendre_weight_a[Legendre_counter];
		assemble_matrix[inner_cel->node[j]][cel->node[i]] +=
		  x * y * shape[i] * value[j] * Jacobian;
#else
		assemble_matrix[inner_cel->node[j]][cel->node[i]] +=
		  ASSEMBLE_CONST_1 * Legendre_weight_a[Legendre_counter] *
		    shape[i] * value[j] * Jacobian;
#endif	    
	      }
	    inner_cel = inner_cel->next;
	  } /* while inner looping on elements of a particular conductor */
	} /* for inner looping on the conductors */
	
	
	/* inner loop on dielectric elements */
	
	inner_del = die_elements;
	while(inner_del != NULL)
	{
	  /* outer element is a conductor - TRUE,0,0 for last args */
	  nmmtl_interval_d(x,y,inner_del,value,TRUE,0,0);
	  
	  /* now add in the contributions to the the basis points */
	  for(i=0;i < INTERP_PTS;i++)
	    for(j=0;j < INTERP_PTS;j++)
	    {
#ifdef BEM3_VARIANT
	      double x,y;
	      x = ASSEMBLE_CONST_1 / 4e-12;
	      y = Legendre_weight_a[Legendre_counter];
	      assemble_matrix[inner_del->node[j]][cel->node[i]] +=
		x * y * shape[i] * value[j] * Jacobian;
#else
	      assemble_matrix[inner_del->node[j]][cel->node[i]] +=
		ASSEMBLE_CONST_1 * Legendre_weight_a[Legendre_counter] *
		  shape[i] * value[j] * Jacobian;
#endif	  
	    }
	  inner_del = inner_del->next;
	} /* while inner looping on dielectric elements */
	
      } /* while stepping through Guass-Legendre roots */
      
      cel = cel->next;
      
    } /* while outer looping on elments of a conductor */
  } /* while outer looping on conductors */
  
  
  /* now create outer loop on the the dielectric elements */
  
  del = die_elements;
  while(del != NULL)
  {
#ifdef BEM3_VARIANT
    coef2 = length_scale * (del->epsilonplus - del->epsilonminus) * 
      ASSEMBLE_CONST_1 * 1.0e+6;
    coef1 = length_scale * (del->epsilonplus + del->epsilonminus) /
      (2.0 * AIR_CONSTANT) * 1.0e+6;
#else
    coef2 = length_scale * (del->epsilonplus - del->epsilonminus) * 
      ASSEMBLE_CONST_1;
    coef1 = length_scale * (del->epsilonplus + del->epsilonminus) /
      (2.0 * AIR_CONSTANT);
#endif
    
    for(Legendre_counter = 0; Legendre_counter < Legendre_root_a_max;
	Legendre_counter++)
    {
      
      nmmtl_shape(Legendre_root_a[Legendre_counter],shape);
      
      /* interpolate x,y coordinate using no_edge shape function */
      x = 0.0;
      y = 0.0;
      for(i=0;i < INTERP_PTS;i++)
      {
	x += shape[i]*del->xpts[i];
	y += shape[i]*del->ypts[i];
      }
      
      nmmtl_jacobian_d(Legendre_root_a[Legendre_counter],del,&Jacobian);
      
      /* first one double integral */
      
      for(i=0;i < INTERP_PTS;i++)
	for(j=0;j < INTERP_PTS;j++)
	  assemble_matrix[del->node[j]][del->node[i]] +=
	    coef1 * Legendre_weight_a[Legendre_counter] *
	      shape[i] * shape[j] * Jacobian;
      
      
      /* Now an inner loop over all the elements, performing an
	 integration in each call to nmmtl_interval_*** */
      
      if(coef2 != 0.0)
      {
	
	/* inner loop on conductor elements */
	
	for(inner_cond_num = 0; inner_cond_num <= conductor_counter;
	    inner_cond_num++)
	{
	  inner_cel=conductor_data[inner_cond_num].elements;
	  while(inner_cel != NULL)
	  {
	    /* outer element is not a conductor - FALSE,normalx,normaly
	       for last args */
	    nmmtl_interval_c(x,y,inner_cel,value,FALSE,del->normalx,
			     del->normaly);
	    
	    /* now add in the contributions to the the basis points */
	    for(i=0;i < INTERP_PTS;i++)
	      for(j=0;j < INTERP_PTS;j++)
	      {
		assemble_matrix[inner_cel->node[j]][del->node[i]] +=
		  coef2 * Legendre_weight_a[Legendre_counter] *
		    shape[i] * value[j] * Jacobian;
	      }
	    inner_cel = inner_cel->next;
	  } /* while inner looping on elements of a particular conductor */
	} /* while inner looping on the conductors */
	
	/* inner loop on dielectric elements */
	
	inner_del = die_elements;
	while(inner_del != NULL)
	{
	  /* Are we at the self element ? */
	  if(del == inner_del)
	  {
	    nmmtl_interval_self_d(x,y,inner_del,value,
				  Legendre_root_a[Legendre_counter],
				  del->normalx,del->normaly);
	  }
	  else
	  {
	    /* outer element is not a conductor - FALSE,normalx,normaly
	       for last arg */
	    nmmtl_interval_d(x,y,inner_del,value,FALSE,del->normalx,
			     del->normaly);
	  }
	  
	  /* now add in the contributions to the the basis points */
	  for(i=0;i < INTERP_PTS;i++)
	    for(j=0;j < INTERP_PTS;j++)
	    {
	      assemble_matrix[inner_del->node[j]][del->node[i]] +=
		coef2 * Legendre_weight_a[Legendre_counter] *
		  shape[i] * value[j] * Jacobian;
	    }
	  inner_del = inner_del->next;
	} /* while inner looping on dielectric elements */
	
      } /* if coef2 != 0.0 */
      
    } /* while stepping through Guass-Legendre roots */
    del = del->next;
    
  } /* while outer looping on die elements */
}


