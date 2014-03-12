/*
  
  FACILITY:  NMMTL
  
  MODULE DESCRIPTION:
  
  Contains the function nmmtl_fill_die_gaps
  
  AUTHOR(S):
  
  Kevin J. Buchs
  
  CREATION DATE:  1-AUG-1991 11:54:03
  
  COPYRIGHT:   Copyright (C) 1991,1992 by Mayo Foundation. All rights reserved.
  
  */


/*
 *******************************************************************
 **  INCLUDE FILES
 *******************************************************************
 */


#include "nmmtl.h"
#include <string.h>

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
  
  FUNCTION NAME:  nmmtl_fill_die_gaps
  
  FUNCTIONAL DESCRIPTION:
  
  Process the left-over dielectric subsegments, those which do not pair
  up with a corresponding segment to form a dielectric interface segment.
  Any which are vertical are a big problem.  If the horizontal ones match the top
  of the dielectric stack, then they are ok.  In any case, the dielectric
  segment is created with an interface to air.
  
  FORMAL PARAMETERS:
  
  int orientation, 
  - horizontal or vertical
  
  int *segment_number,
  - a unique number to assign to newly created segments
  
  double top_stack,
  - a y value for the top of the dielectric stack
  
  struct dielectric_sub_segments **seg1,  
  struct dielectric_sub_segments **seg2,
  - the two lists of subsegments which could not be paired with each other.
  
  struct dielectric_segments **dielectric_segments
  - the list of dielectric interface segments, which might be added to
  
  
  RETURN VALUE:
  
  None
  
  CALLING SEQUENCE:
  
  status = nmmtl_fill_die_gaps(orientation,&segment_number,top_stack,
  &seg1,&seg2,
  &dielectric_segments)
  
  */

int nmmtl_fill_die_gaps(int orientation,int *segment_number,
                        double top_stack,
			struct dielectric_sub_segments **seg1,
			struct dielectric_sub_segments **seg2,
			struct dielectric_segments **dielectric_segments)
{
  
  struct dielectric_segments *new_segment;
  struct dielectric_sub_segments *list, *old;
  char msg[256],*smsg;
  
  strcpy(msg,orientation == VERTICAL_ORIENTATION ? 
	 "vert bound at " : "horiz bound at ");
  smsg = msg + strlen(msg);
  
  /* process list one */
  
  list = *seg1;
  
  while(list != NULL)
  {
    
    /* Ignore the unusual case where the dielectric
       constant matches AIR */

    if(list->epsilon != AIR_CONSTANT)
    {

      /* don't print a warning message if this boundary with air is the */
      /* normal top of the dielectric stack */
    
      if(orientation != HORIZONTAL_ORIENTATION || list->at != top_stack)
      {
        /* print warning message */
        sprintf(smsg,"%g over %g to %g (meters)",
    	        list->at,list->start,list->end);

	printf("ELECTRO-W-DIEAIR Diel interf with AIR along %s\n",msg);
      }
    
      /* create a new segment */
    
      new_segment = (struct dielectric_segments *)
        malloc(sizeof(struct dielectric_segments));
      new_segment->next = *dielectric_segments;
      *dielectric_segments = new_segment;
      new_segment->at = list->at;
      new_segment->start = list->start;
      new_segment->end = list->end;
      new_segment->length = list->end - list->start;
    
      /* epsilonplus is air */
      new_segment->epsilonminus = list->epsilon;
      new_segment->epsilonplus = AIR_CONSTANT;
    
      new_segment->divisions = list->divisions;
      new_segment->segment_number = *segment_number++;
      new_segment->end_in_conductor = 0;
      new_segment->orientation = orientation;
    }
    
#ifdef DIAG_DIE_SEG
    nmmtl_dump_dielectric_segment(new_segment);
#endif
    
    old = list;
    list = list->next;
    free(old);
  }
  
  /* process list two */
  
  list = *seg2;
  
  while(list != NULL)
  {
    
    /* create a new segment, except in the unusual case where the dielectric
       constant matches AIR */
    
    if(list->epsilon != AIR_CONSTANT)
    {

      /* print warning message */
      sprintf(smsg,"%g over %g to %g (meters)",
  	      list->at,list->start,list->end);
      printf("ELECTRO-W-DIEAIR Diel interf with AIR along %s\n",msg);
    
      new_segment = (struct dielectric_segments *)
        malloc(sizeof(struct dielectric_segments));
      new_segment->next = *dielectric_segments;
      *dielectric_segments = new_segment;
      new_segment->at = list->at;
      new_segment->start = list->start;
      new_segment->end = list->end;
      new_segment->length = list->end - list->start;
    
      /* epsilonminus is air */
      new_segment->epsilonminus = AIR_CONSTANT;
      new_segment->epsilonplus = list->epsilon;
    
      new_segment->divisions = list->divisions;
      new_segment->segment_number = *segment_number++;
      new_segment->end_in_conductor = 0;
      new_segment->orientation = orientation;
    }
    
    old = list;
    list = list->next;
    free(old);
  }
  
  return(SUCCESS);
}

