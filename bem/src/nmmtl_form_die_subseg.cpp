
/*
  
  FACILITY:  NMMTL
  
  MODULE DESCRIPTION:
  
  Contains the function nmmtl_form_die_subseg
  
  AUTHOR(S):
  
  Kevin J. Buchs
  
  CREATION DATE:   26-JUL-1991 15:57:11
  
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
  
  FUNCTION NAME:  nmmtl_form_die_subseg
  
  FUNCTIONAL DESCRIPTION:
  
  Breaks  up dielectric rectangles into subsegments, one per side of the 
  rectangle and puts each on a separate list.
  
  FORMAL PARAMETERS:
  
  int plane_segments,
  - number of segments to break a plane into
  
  struct dielectric *dielectrics,
  - list of dielectric contours
  
  double top_of_bottom_plane,
  double bottom_of_top_plane,
  double left_of_gnd_planes,
  double right_of_gnd_planes,
  - range of ground plane(s) 
  - helps to determine if a dielectric is touching a ground plane
  
  struct dielectric_sub_segments **top_seg,
  struct dielectric_sub_segments **bottom_seg,
  struct dielectric_sub_segments **left_seg,
  struct dielectric_sub_segments **right_seg)
  - four lists of dielectric subsegments created
  
  SORTED_GND_DIE_LIST_P *lower_sorted_gdl,
  SORTED_GND_DIE_LIST_P *upper_sorted_gdl
  - lists of ground plane-dielectric intersections created by this function
  
  RETURN VALUE:
  
  SUCCESS
  
  CALLING SEQUENCE:
  
  status = nmmtl_form_die_subseg(plane_segments,dielectrics,
                                 top_of_bottom_plane,bottom_of_top_plane,
				 left_of_gnd_planes,right_of_gnd_planes,
				 &top_seg,&bottom_seg,&left_seg,&right_seg,
				 &lower_sorted_gdl,&upper_sorted_gdl);
  
  */

int
    nmmtl_form_die_subseg(int plane_segments,
			  struct dielectric *dielectrics,
			  double top_of_bottom_plane,
			  double bottom_of_top_plane,
			  double left_of_gnd_planes,
			  double right_of_gnd_planes,
			  struct dielectric_sub_segments **top_seg,
			  struct dielectric_sub_segments **bottom_seg,
			  struct dielectric_sub_segments **left_seg,
			  struct dielectric_sub_segments **right_seg,
			  SORTED_GND_DIE_LIST_P *lower_sorted_gdl,
			  SORTED_GND_DIE_LIST_P *upper_sorted_gdl)
{
  struct dielectric_sub_segments *new_seg;
  GND_DIE_LIST_P lower_gdl,lower_gdl_head = NULL;
  GND_DIE_LIST_P upper_gdl,upper_gdl_head = NULL;
  
  for( ;dielectrics != NULL;dielectrics = dielectrics->next)
  {
    /* new top segment */
    /* does dielectric top touch upper ground plane? */
    if(dielectrics->y1 == bottom_of_top_plane)
    {
      upper_gdl = nmmtl_build_gnd_die_list(&upper_gdl_head,upper_gdl,
					   dielectrics);
    }
    else
    {
      new_seg = (struct dielectric_sub_segments *)
	malloc(sizeof(struct dielectric_sub_segments));
      new_seg->next = *top_seg;
      *top_seg = new_seg;
      new_seg->at = dielectrics->y1;
      new_seg->start = dielectrics->x0;
      new_seg->end = dielectrics->x1;
      new_seg->divisions = plane_segments;
      new_seg->epsilon = dielectrics->constant;
#ifdef DIAG_SUBSEG
      printf("new top subsegment at %g over (%g,%g)\n",new_seg->at,
	      new_seg->start,new_seg->end);
#endif      
    }
    
    /* new bottom segment */
    /* does dielectric bottom sit on lower ground plane? */
    if(dielectrics->y0 == top_of_bottom_plane)
    {
      lower_gdl = nmmtl_build_gnd_die_list(&lower_gdl_head,lower_gdl,
					   dielectrics);
    }
    else  /* just regular lower die subsegment */
    {
      new_seg = (struct dielectric_sub_segments *)
	malloc(sizeof(struct dielectric_sub_segments));
      new_seg->next = *bottom_seg;
      *bottom_seg = new_seg;
      new_seg->at = dielectrics->y0;
      new_seg->start = dielectrics->x0;
      new_seg->end = dielectrics->x1;
      new_seg->divisions = plane_segments;
      new_seg->epsilon = dielectrics->constant;
#ifdef DIAG_SUBSEG
      printf("new bottom subsegment at %g over (%g,%g)\n",new_seg->at,
	      new_seg->start,new_seg->end);
#endif      
    }
    
    /* new right segment */
    if(dielectrics->x1 < right_of_gnd_planes)
    {
      new_seg = (struct dielectric_sub_segments *)
	malloc(sizeof(struct dielectric_sub_segments));
      new_seg->next = *right_seg;
      *right_seg = new_seg;
      new_seg->at = dielectrics->x1;
      new_seg->start = dielectrics->y0;
      new_seg->end = dielectrics->y1;
      new_seg->divisions = plane_segments;
      new_seg->epsilon = dielectrics->constant;
#ifdef DIAG_SUBSEG
      printf("new right subsegment at %g over (%g,%g)\n",new_seg->at,
	      new_seg->start,new_seg->end);
#endif      
    }
    
    
    /* new left segment */
    if(dielectrics->x0 > left_of_gnd_planes)
    {
      new_seg = (struct dielectric_sub_segments *)
	malloc(sizeof(struct dielectric_sub_segments));
      new_seg->next = *left_seg;
      *left_seg = new_seg;
      new_seg->at = dielectrics->x0;
      new_seg->start = dielectrics->y0;
      new_seg->end = dielectrics->y1;
      new_seg->divisions = plane_segments;
      new_seg->epsilon = dielectrics->constant;
#ifdef DIAG_SUBSEG
      printf("new left subsegment at %g over (%g,%g)\n",new_seg->at,
	      new_seg->start,new_seg->end);
#endif      
    } /* end new left segment */
    
  } /* end for dielectrics */
  
  
  /* clean up and sort the list of dielectric-ground plane intersections */
  
  nmmtl_sort_gnd_die_list(lower_gdl_head,lower_sorted_gdl,
			  upper_gdl_head,upper_sorted_gdl,
			  left_of_gnd_planes,
			  right_of_gnd_planes);
  
  
  return(SUCCESS);
}
