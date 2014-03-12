
/*
  
  FACILITY:  NMMTL
  
  MODULE DESCRIPTION:
  
  Contains the function nmmtl_sort_gnd_die_list()
  
  AUTHOR(S):
  
  Kevin J. Buchs
  
  CREATION DATE:  Wed Mar 18 11:50:01 1992
  
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
  
  FUNCTION NAME:  nmmtl_sort_gnd_die_list
  
  
  FUNCTIONAL DESCRIPTION:
  
  Produce a sorted list from the list of ground dielectric intersections
  
  FORMAL PARAMETERS:
  
  GND_DIE_LIST_P lower_gdl_head,
  - unsorted list of intersections
  SORTED_GND_DIE_LIST_P *lower_sorted_gdl,
  - lists of ground plane-dielectric intersections created by this function
  GND_DIE_LIST_P upper_gdl_head,
  - unsorted list of intersections
  SORTED_GND_DIE_LIST_P *upper_sorted_gdl,
  - lists of ground plane-dielectric intersections created by this function
  double left, double right
  - boundaries of ground planes
  
  RETURN VALUE:
  
  None
  
  CALLING SEQUENCE:
  
  nmmtl_sort_gnd_die_list(lower_gdl_head,lower_sorted_gdl,
                          upper_gdl_head,upper_sorted_gdl,
			  left_of_gnd_planes,
			  right_of_gnd_planes);
  
  */
void nmmtl_sort_gnd_die_list(GND_DIE_LIST_P lower_gdl_head,
			     SORTED_GND_DIE_LIST_P *lower_sorted_gdl,
			     GND_DIE_LIST_P upper_gdl_head,
			     SORTED_GND_DIE_LIST_P *upper_sorted_gdl,
			     double left,double right)
{
  double minstart,maxend;
  float *data;
  GND_DIE_LIST_P discard;
  
  *lower_sorted_gdl = NULL;
  
  if(lower_gdl_head != NULL)
  {
    minstart = lower_gdl_head->start;
    maxend = lower_gdl_head->end;
  }
  else
  {
    /* no dielectric touches lower ground plane?  Try to fake air */
    minstart = right;
    maxend = right;
  }
  
  while(lower_gdl_head != NULL)
  {
    /* check if dielectric exceeds the ground plane - if so silently trim it */
    if(left > lower_gdl_head->start) lower_gdl_head->start = left;
    if(right < lower_gdl_head->end) lower_gdl_head->end = right;
    
    /* keep track of the full extent */
    if(minstart > lower_gdl_head->start) minstart = lower_gdl_head->start;
    if(maxend   < lower_gdl_head->end)   maxend = lower_gdl_head->end;
    
    data = (float *)malloc(sizeof(float));
    *data = lower_gdl_head->epsilon;
    /* sort by end point */
    nmmtl_add_to_sorted_list(lower_sorted_gdl,lower_gdl_head->end,
			     (char *)data);
    discard = lower_gdl_head; 
    lower_gdl_head = lower_gdl_head->next;
    free(discard);
  }
  
  /* add in any air sections needed */
  
  /* piece missing on left - it will end at the lowest start */
  if(minstart > left)
  {
    data = (float *)malloc(sizeof(float));
    *data = AIR_CONSTANT;
    /* sort by end point */
    nmmtl_add_to_sorted_list(lower_sorted_gdl,minstart,
			     (char *)data);
  }
  
  /* piece missing on right - it will end at the right end */
  if(maxend < right)
  {
    data = (float *)malloc(sizeof(float));
    *data = AIR_CONSTANT;
    /* sort by end point */
    nmmtl_add_to_sorted_list(lower_sorted_gdl,right,
			     (char *)data);
  }
  
  /* upper ground plane if exists */
  if(upper_gdl_head != NULL)
  {
    *upper_sorted_gdl = NULL;
    
    minstart = upper_gdl_head->start;
    maxend = upper_gdl_head->end;
    
    while(upper_gdl_head != NULL)
    {
      /* check if dielectric exceeds the ground plane -
	 if so silently trim it */
      if(left > upper_gdl_head->start) upper_gdl_head->start = left;
      if(right < upper_gdl_head->end) upper_gdl_head->end = right;
      
      /* keep track of the full extent */
      if(minstart > upper_gdl_head->start) minstart = upper_gdl_head->start;
      if(maxend   < upper_gdl_head->end)   maxend = upper_gdl_head->end;
      
      data = (float *)malloc(sizeof(float));
      *data = upper_gdl_head->epsilon;
      /* sort by end point */
      nmmtl_add_to_sorted_list(upper_sorted_gdl,upper_gdl_head->end,
			       (char *)data);
      discard = upper_gdl_head; 
      upper_gdl_head = upper_gdl_head->next;
      free(discard);
    }
    
    /* add in any air sections needed */
    
    /* piece missing on left - it will end at the lowest start */
    if(minstart > left)
    {
      data = (float *)malloc(sizeof(float));
      *data = AIR_CONSTANT;
      /* sort by end point */
      nmmtl_add_to_sorted_list(upper_sorted_gdl,minstart,
			       (char *)data);
    }
    
    /* piece missing on right - it will end at the right end */
    if(maxend < right)
    {
      data = (float *)malloc(sizeof(float));
      *data = AIR_CONSTANT;
      /* sort by end point */
      nmmtl_add_to_sorted_list(upper_sorted_gdl,right,
			       (char *)data);
    }
  }
}
