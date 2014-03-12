
/*
  
  FACILITY:  NMMTL
  
  MODULE DESCRIPTION:
  
  Contains function nmmtl_merge_die_subseg
  
  AUTHOR(S):
  
  Kevin J. Buchs
  
  CREATION DATE:  26-JUL-1991 15:57:64
  
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
  
  FUNCTION NAME:  nmmtl_merge_die_subseg
  
  
  FUNCTIONAL DESCRIPTION:
  
  Pair up dielectric subsegments to form dielectric interface
  segments.  This function only works on vertical or horizontal at one
  time.  Two lists of subsegments are passed in.  Subsegments from the
  seg1 list are taken to be the normal side and set the epsilonplus
  value, whereas seg2 subsegments are the anti-normal side and set
  epsilonminus.  A loop of traversing the seg1 list with a nested loop
  of traversing the seg2 list is the basic structure.
  
  
  FORMAL PARAMETERS:
  
  int orientation
  - horizontal or vertical
  
  int *segment_number,
  - the unique segment number to use and increment
  
  double top_stack,
  - the top dielectric boundary with air or upper groundplane
  
  struct dielectric_sub_segments **seg1,
  - subsegment list, taken to be normal side, to be matched with subsegments
  from seg2
  
  struct dielectric_sub_segments **seg2,
  - subsegment list, taken to be anti-normal side, to be matched with
  subsegments from seg2
  
  struct dielectric_segments **dielectric_segments)
  - new dielectric interface elements are added to this list
  
  RETURN VALUE:
  
  SUCCESS or FAIL
  
  CALLING SEQUENCE:
  
  status = nmmtl_merge_die_subseg(orientation,&segment_number,top_stack,
  &seg1,&seg2,&dielectric_segments);
  
  */

int nmmtl_merge_die_subseg(int orientation,int *segment_number,
                           double top_stack,
			   struct dielectric_sub_segments **seg1,
			   struct dielectric_sub_segments **seg2,
			   struct dielectric_segments **dielectric_segments)
{
  struct dielectric_sub_segments *list1, *list2;
  struct dielectric_sub_segments *prev1, *prev2;
  struct dielectric_sub_segments *new_sub_segment;
	double overlap_left, overlap_right;
  int left_overhang, right_overhang;
  int divisions;
  int status;
  int olp_status;
  unsigned char restart_2; /* a flag to indicate if list 2 should checked */
  /* from the beginning again */
  
  list1 = *seg1;
  prev1 = NULL;
  
  while(list1 != NULL)
  {
    
#ifdef DIAG_MERGE_DIE
    printf("looping on list1:");
    printf("  %s ",
	    orientation == VERTICAL_ORIENTATION ?
	    "vertical" : "horizontal");
    printf(" at: %1.1g, start: %1.1g, end: %1.1g\n",list1->at,
	    list1->start,list1->end);
#endif	
    
    list2 = *seg2;
    prev2 = NULL;
    while(list1 != NULL && list2 != NULL)
    {
      
      restart_2 = 0;
      
#ifdef DIAG_MERGE_DIE
      printf("restarting list2:");
      printf("  %s ",
	      orientation == VERTICAL_ORIENTATION ?
	      "vertical" : "horizontal");
      printf(" at: %1.1g, start: %1.1g, end: %1.1g\n",list2->at,
	      list2->start,list2->end);
#endif	
      
      olp_status = nmmtl_overlap_parallel_seg(list1,list2,&overlap_left,
					      &overlap_right,&left_overhang,
					      &right_overhang);
      if(olp_status == 2) return(FAIL);
      if(olp_status == 1)
      {
	
#ifdef DIAG_MERGE_DIE
	printf("segment overlap");
	printf("  orientation = %s\n",
		orientation == VERTICAL_ORIENTATION ?
		"vertical" : "horizontal");
	printf("  at: %1.1g, left: %1.1g, right: %1.1g\n",list1->at,
		overlap_left,overlap_right);
	switch(left_overhang)
	{
	case 1:
	  printf("  left_overhang 1: (%1.1g to %1.1g)\n",list1->start,
		  overlap_left);
	  break;
	case 2:
	  printf("  left_overhang 2: (%1.1g to %1.1g)\n",list2->start,
		  overlap_left);
	  break;
	}
 	switch(right_overhang)
	{
	case 1:
	  printf("  right_overhang 1: (%1.1g to %1.1g)\n",
                  overlap_right,list1->end);
	  break;
	case 2:
	  printf("  right_overhang 2: (%1.1g to %1.1g)\n",
                  overlap_right,list2->end);
	  break;
	}
#endif 	
	
	
	/* add new dielectric segment based on overlap */
	
	status = nmmtl_new_die_seg(dielectric_segments,list1,list2,orientation,
				   *segment_number,overlap_left,overlap_right);
	if(status != SUCCESS) return(status);
	*segment_number += 1;
	
	
	/* check for pieces left from list 1 */
	
	/* if list1 overlaps on both sides */
	/* - split and make new sub segment */
	if(left_overhang == 1 && right_overhang == 1)
	{
	  /* make new right sub segment */
	  new_sub_segment = (struct dielectric_sub_segments *)
	    malloc(sizeof(struct dielectric_sub_segments));
	  new_sub_segment->next = list1->next;
	  new_sub_segment->at = list1->at;
	  new_sub_segment->start = overlap_right; 
	  new_sub_segment->end = list1->end;
	  new_sub_segment->epsilon = list1->epsilon;
	  new_sub_segment->divisions = (int)(0.99 + list1->divisions *
	    (list1->end - overlap_right)/(list1->end - list1->start));
	  /* modify old one to be new left sub segment */
	  list1->next = new_sub_segment;
	  divisions = (int)(0.99 + list1->divisions *
	    (overlap_left - list1->start)/(list1->end - list1->start));
	  list1->divisions = divisions;
	  list1->end = overlap_left;
	}
	
	/* if list1 just overlaps on left side - make element on list */
	/* smaller by moving end in */
	else if(left_overhang == 1)
	{
	  divisions = (int)(list1->divisions * 
	    (overlap_left - list1->start)/(list1->end - list1->start) + 0.99);
	  list1->divisions = divisions;
	  list1->end = overlap_left;
	}
	
	/* if list1 just overlaps on left side - make element on list */
	/* smaller by moving start in */
	else if(right_overhang == 1)
	{
	  divisions = (int)(list1->divisions * 
	    (list1->end - overlap_right)/(list1->end - list1->start) + 0.99);
	  list1->divisions = divisions;
	  list1->start = overlap_right;
	}
	
	/* no pieces left from list1 subsegment - complete overlap */
	/* remove old subsegment. */
	else 
	{
	  if(prev1 != NULL)
	  {
	    prev1->next = list1->next;
	    free(list1);
	    list1 = prev1->next;
	  }
	  else
	  {
	    prev1 = list1;
	    list1 = list1->next;
	    *seg1 = list1;
	    free(prev1);
	    prev1 = NULL;
	  }
	  /* flag that we should */
	  /* restart other list, since scanning with new piece from list1 */
	  /* one now */
	  
	  restart_2 = 1;
	  
	}
	
	/* check for pieces left from list 2 */
	
	/* if list2 overlaps on both sides */
	/* - split and make new sub segment */
	if(left_overhang == 2 && right_overhang == 2)
	{
	  /* make new right sub segment */
	  new_sub_segment = (struct dielectric_sub_segments *)
	    malloc(sizeof(struct dielectric_sub_segments));
	  new_sub_segment->next = list2->next;
	  new_sub_segment->at = list2->at;
	  new_sub_segment->start = overlap_right; 
	  new_sub_segment->end = list2->end;
	  new_sub_segment->epsilon = list2->epsilon;
	  new_sub_segment->divisions = (int)(0.99 + list2->divisions *
	    (list2->end - overlap_right)/(list2->end - list2->start));
	  /* modify old one to be new left sub segment */
	  list2->next = new_sub_segment;
	  divisions = (int)(0.99 + list2->divisions *
	    (overlap_left - list2->start)/(list2->end - list2->start));
	  list2->divisions = divisions;
	  list2->end = overlap_left;
	}
	
	/* if list2 just overlaps on left side - make element on list */
	/* smaller by moving end in */
	else if(left_overhang == 2)
	{
	  divisions = (int)(list2->divisions * 
	    (overlap_left - list2->start)/(list2->end - list2->start) + 0.99);
	  list2->divisions = divisions;
	  list2->end = overlap_left;
	}
	
	/* if list2 just overlaps on left side - make element on list */
	/* smaller by moving start in */
	else if(right_overhang == 2)
	{
	  divisions = (int)(list2->divisions * 
	    (list2->end - overlap_right)/(list2->end - list2->start) + 0.99);
	  list2->divisions = divisions;
	  list2->start = overlap_right;
	}
	
	/* no pieces left from list2 subsegment - complete overlap */
	/* remove old subsegment. */
	else 
	{
	  if(prev2 != NULL)
	  {
	    prev2->next = list2->next;
	    free(list2);
	    list2 = prev2->next;
	  }
	  else
	  {
	    prev2 = list2;
	    list2 = list2->next;
	    *seg2 = list2;
	    free(prev2);
	    prev2 = NULL;
	  }
	}
      }
      
      /* no overlap - a miss - advance list 2 */
      
      else 
      {
	prev2 = list2;
	list2 = list2->next;
      }
      
      if(restart_2 == 1)
      {
#ifdef DIAG_MERGE_DIE
	printf("restarting list2\n");	  
#endif	
	list2 = *seg2;
	prev2 = NULL;
      }
      
      
    }
    
    /* reached the end of list 2, advance list 1 and rescan list 2 */
    
#ifdef DIAG_MERGE_DIE
    printf("advance list 1\n");
#endif	
    prev1 = list1;
    if(list1 != NULL) list1 = list1->next;
  }
  
  status = nmmtl_fill_die_gaps(orientation,segment_number,top_stack,seg1,seg2,
			       dielectric_segments);
  
  return(SUCCESS);
}
