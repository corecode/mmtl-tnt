/*
  
  FACILITY:   NMMTL
  
  MODULE DESCRIPTION:
  
  Contains the function nmmtl_combine_die
  
  AUTHOR(S):
  
  Kevin J. Buchs
  
  CREATION DATE:   26-JUL-1991 15:56:27
  
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
  
  FUNCTION NAME:  nmmtl_combine_die
  
  
  FUNCTIONAL DESCRIPTION:
  
  Pair up tops and bottoms, and lefts and rights of dielectric
  rectangles to form dielectric interfaces.  Calls nmmtl_form_die_subseg
  to form top, bottom, left and right subsegments from the pieces of the
  dielectric rectangles.  Then, calls nmmtl_merge_die_subseg twice, once
  for horizontal and once for vertical subsegments.
  
  FORMAL PARAMETERS:
  
  struct dielectric *dielectrics
  
  Raw input dielectric geometric structures
  
  int plane_segments
  
  How many segments to divide each plane into.
  
  int gnd_planes
  
  How many ground planes there are
  
  double top_of_bottom_plane
  
  Where the bottom ground plane meets a dielectric
  
  double bottom_of_top_plane
  
  Where the top ground plane, if any, meets a dielectric
  
  double left_of_gnd_planes
  double right_of_gnd_planes
  
  Left and right extents of the ground planes.
  
  struct dielectric_segments **dielectric_segments
  
  Return list of dielectric interface segments.
  
  SORTED_GND_DIE_LIST_P *lower_sorted_gdl,
  SORTED_GND_DIE_LIST_P *upper_sorted_gdl
  - lists of ground plane-dielectric intersections created by this function
  
  
  RETURN VALUE:
  
  FAIL or SUCCESS
  
  CALLING SEQUENCE:
  
  status = nmmtl_combine_die(dielectrics,plane_segments,
  gnd_planes,top_of_bottom_plane,
  bottom_of_top_plane,left_of_gnd_planes,
  right_of_gnd_planes,
  &dielectric_segments,
  &lower_sorted_gdl,&upper_sorted_gdl)
  */


int nmmtl_combine_die(struct dielectric *dielectrics,
		      int plane_segments,
		      int gnd_planes,double top_of_bottom_plane,
		      double bottom_of_top_plane,double left_of_gnd_planes,
					double right_of_gnd_planes,
		      struct dielectric_segments **dielectric_segments,
		      SORTED_GND_DIE_LIST_P *lower_sorted_gdl,
		      SORTED_GND_DIE_LIST_P *upper_sorted_gdl)
{
  int status;
  int segment_number = 0;
  struct dielectric_sub_segments *top_seg;
  struct dielectric_sub_segments *bottom_seg;
  struct dielectric_sub_segments *left_seg;
  struct dielectric_sub_segments *right_seg;
  
  
  top_seg = NULL;
  bottom_seg = NULL;
  left_seg = NULL;
  right_seg = NULL;
  
  if(gnd_planes < 2)
  {
    /* Trick: */
    /* if only one ground plane, offset where the top of the dielectrics */
    /* should be to avoid including the top to the top dielectric in the */
    /* subsegments list */
    
    status = nmmtl_form_die_subseg(plane_segments,dielectrics,
				   top_of_bottom_plane,
				   1.0 + bottom_of_top_plane,
				   left_of_gnd_planes,right_of_gnd_planes,
				   &top_seg,&bottom_seg,
				   &left_seg,&right_seg,
				   lower_sorted_gdl,
				   upper_sorted_gdl);
  }
  else 
  {
    
    /* else, check real boundary and exclude those dielectric sub segments */
    /* which are touching the top ground plane. */
    
    status = nmmtl_form_die_subseg(plane_segments,dielectrics,
				   top_of_bottom_plane,bottom_of_top_plane,
				   left_of_gnd_planes,right_of_gnd_planes,
				   &top_seg,&bottom_seg,
				   &left_seg,&right_seg,
				   lower_sorted_gdl,
				   upper_sorted_gdl);
  }
  if(status != SUCCESS) return(status);
  
  
  /* normal is UP, so pass in top first, since it will point from top */
  /* segments into bottom segments when the interface is put together. */
  
  status = nmmtl_merge_die_subseg(HORIZONTAL_ORIENTATION,&segment_number,
				  bottom_of_top_plane,&top_seg,&bottom_seg,
				  dielectric_segments);
  
  if(status != SUCCESS) return(status);
  
  
  /* normal is to left, so pass in left first, and to point left you */
  /* would go from the left boundary into the right boundary */
  
  status = nmmtl_merge_die_subseg(VERTICAL_ORIENTATION,&segment_number,
				  bottom_of_top_plane,&left_seg,&right_seg,
				  dielectric_segments);
  
  if(status != SUCCESS) return(status);
  
  return(SUCCESS);
  
}
