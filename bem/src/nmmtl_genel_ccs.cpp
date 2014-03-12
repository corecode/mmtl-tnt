
/*

  Facility:  nmmtl
  
  MODULE DESCRIPTION:
  
  contains nmmtl_generate_elements_ccs()
  
  AUTHOR(S):
  
  Kevin J. Buchs
  
  CREATION DATE:  Mon Mar 16 14:32:14 1992
  
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
  
  FUNCTION NAME:  nmmtl_generate_elements_ccs
  
  
  FUNCTIONAL DESCRIPTION:
  
  Generate the elements from conductor circle segments - called from 
  nmmtl_generate_elements.  Organize the elements in a linked list - 
  called a sub-list.  This sub-list may become part of a bigger list
  or it may be used by itself.  Use output parameters head and tail
  to return a pointer to the first and last elements on the sub-list.
  
  
  FORMAL PARAMETERS:
  
  CIRCLE_SEGMENTS_P *ccsp,         pointer to whole list of all conductor
  circle
  segments - advanced in this routine, only
  until the next conductor is encountered.
  
  CELEMENTS_P *head,             return head of sub-list created here
  CELEMENTS_P *tail,             return tail of sub-list created here
  
  unsigned int *node_point_counter,  counting nodes allocated in elements
                                     allocated.
  
  RETURN VALUE:
  
  SUCCESS, FAIL
  
  CALLING SEQUENCE:
  
  status = nmmtl_generate_elements_ccs(&ccs,&head,&tail,&node_point_counter);
  
  */

int nmmtl_generate_elements_ccs(CIRCLE_SEGMENTS_P *ccsp,
				CELEMENTS_P *head,
				CELEMENTS_P *tail,
				unsigned int *node_point_counter)
{
  int conductor;
  CIRCLE_SEGMENTS_P ccs;
  CELEMENTS_P element,start = NULL;
  unsigned int divisions;
  register unsigned int npcntr;
  unsigned int first_node;
	double anglehalfincr,angleincr,angle;
	double x,y;
	double length;
  
  ccs = *ccsp;
  conductor = ccs->conductor;
  npcntr = *node_point_counter;
  first_node = npcntr;
  
  while(ccs != NULL && ccs->conductor == conductor)
  {
    divisions = ccs->divisions;
    length = ccs->radius*(ccs->endangle - ccs->startangle);
    
#ifndef NO_HALF_MIN_CHECKING
    
    /* will the natural sized elements be small enough ? */
    if(length/divisions > half_minimum_dimension)
    {
      /* no, make them smaller */
      divisions = length/half_minimum_dimension + 0.99;
    }
    
#endif
    
    angleincr = (ccs->endangle - ccs->startangle) / divisions;
    anglehalfincr = angleincr/2;
    /* calculate the starting coordinates */
    angle = ccs->startangle;
    nmmtl_cirseg_angle_point(ccs,angle,&x,&y);
    
    /* loop based on a count of the number of divisions */
    
    while( divisions > 0 )
    {
      if(start == NULL)  /* first time through */
      {
	/* create the head of the list */
	element = (CELEMENTS_P)malloc(sizeof(CELEMENTS));
	start = element;
      }
      else /* subsequent times through */
      {
	/* link up to the last element created */
	element->next = (CELEMENTS_P)malloc(sizeof(CELEMENTS));
	element = element->next;
      }
      
      /* terminate the list for now */
      element->next = NULL;
      
      /* no edge effects stuff for circular elements */
      element->edge[0] = NULL;
      element->edge[1] = NULL;
      
      /* the global coordinates at the various nodes */
      /* starting point - already calculated */
			element->xpts[0] = x;
      element->ypts[0] = y;
      
      /* midpoint */
      angle += anglehalfincr;
      nmmtl_cirseg_angle_point(ccs,angle,&x,&y);
      element->xpts[1] = x;
      element->ypts[1] = y;
      
      /* end point - leave angle set for next element */
      angle += anglehalfincr;
      nmmtl_cirseg_angle_point(ccs,angle,&x,&y);
      element->xpts[2] = x;
      element->ypts[2] = y;
      
      /* at this point ccs->epsilon[0] should match ccs->epsilon[1] */
      element->epsilon = ccs->epsilon[0];
      
      /* array indicies into the BIG arrays */
      element->node[0] = npcntr++;
      element->node[1] = npcntr++;
      element->node[2] = npcntr;     /* set up for overlap of node points */
      
      /* advance to the next element */
      divisions--;
      
    } /* while looping through divisions of this segment */
    
    /* advance to the next segment */
    ccs = ccs->next;
    
  }   /* while looping through segments of the same conductor */
  
  /* Now, the last node of the last element must really be the same as the
     first node of the first element - the conductor is continuous */
  
  element->node[2] = first_node;
  
  /* The next time this gets called, it will be a different conductor
     or dielectric and thus we should advance the counter to a new node.
     In other words, there is no physical link between the last node on
     the last element generated from this call to this function and the
     first node of the first element generated from the next call to this
     function
     
     However:  We don't need to advance because the last node was not really
     used, since it was first assigned to npcntr but then reassigned to 
     first_node.  Thus, the value of npcntr can be used, since it has not
     already been used.
     
     */
  
  *node_point_counter = npcntr;
  
  /* return head and tail of list */
  *head = start;
  *tail = element;
  
  /* adjust pointer over segments processed */
  *ccsp = ccs;
  
  return(SUCCESS);
  
}
