
/*
  
  FACILITY:  NMMTL
  
  MODULE DESCRIPTION:
  
  Contains the function nmmtl_new_die_seg
  
  AUTHOR(S):
  
  Kevin J. Buchs
  
  CREATION DATE:    1-AUG-1991 11:54:23
  
  COPYRIGHT:   Copyright (C) 1991,1992 by Mayo Foundation. All rights reserved.
  
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
  
  FUNCTION NAME:  nmmtl_new_die_seg
  
  
  FUNCTIONAL DESCRIPTION:
  
  Create a new dielectric interface segment based on geometric data on
  an overlap between two dielectric subsegments.
  
  FORMAL PARAMETERS:
  
  struct dielectric_segments **dielectric_segments,
  
  list to which the new segment is added
  
  struct dielectric_sub_segments *list1,
  struct dielectric_sub_segments *list2,
  
  pointers to two subsegments which have some overlap
  
  int orientation,
  
  horizontal or vertical subsegments
  
  int segment_number,
  
  unique number to assign to the new segment
  
	double overlap_left,
	double overlap_right,
  
  the extents of the region of overlap
  
  RETURN VALUE:
  
  SUCCESS
  
  CALLING SEQUENCE:
  
  status = nmmtl_new_die_seg(&dielectric_segments,
  list1,list2,
  orientation,segment_number,
  overlap_left,overlap_right);
  
  */


/* add new dielectric segment based on overlap */

int nmmtl_new_die_seg(struct dielectric_segments **dielectric_segments,
		      struct dielectric_sub_segments *list1,
		      struct dielectric_sub_segments *list2,
		      int orientation,int segment_number,
		      double overlap_left, double overlap_right)
{
  
  struct dielectric_segments *new_segment;
  int divisions1, divisions2;
  
  /* Avoid creating a segment where there is no difference in dielectric
     constant */
  
  if(list1->epsilon == list2->epsilon) return(SUCCESS);

  /* Avoid creating a zero length segment */

  if(overlap_left == overlap_right) return(SUCCESS);
  
  new_segment = (struct dielectric_segments *)
    malloc(sizeof(struct dielectric_segments));
  new_segment->next = *dielectric_segments;
  *dielectric_segments = new_segment;
  new_segment->at = list1->at;
  new_segment->start = overlap_left;
  new_segment->end = overlap_right;
  new_segment->length = overlap_right - overlap_left;
  new_segment->epsilonminus = list1->epsilon;
  new_segment->epsilonplus = list2->epsilon;
  
  /* find the largest number of proportional number of divisions in the */
  /* segment based on its membership to list1 and list2 */
  
  divisions1 = (int)(0.99 + list1->divisions * 
    (overlap_right - overlap_left)/(list1->end - list1->start));
  
  divisions2 = (int)(0.99 + list2->divisions * 
    (overlap_right - overlap_left)/(list2->end - list2->start));
  
  new_segment->divisions = divisions1 > divisions2 ? divisions1 :
    divisions2;
  
  new_segment->segment_number = segment_number;
  new_segment->end_in_conductor = 0;
  new_segment->orientation = orientation;
  
#ifdef DIAG_DIE_SEG
  nmmtl_dump_dielectric_segment(new_segment);
#endif
  
  return(SUCCESS);
}
