
/*
  
  FACILITY:  nmmtl
  
  MODULE DESCRIPTION:
  
  contains nmmtl_generate_elements_cls()
  
  AUTHOR(S):
  
  Kevin J. Buchs
  
  CREATION DATE:  Fri Mar 13 10:41:06 1992
  
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
  
  FUNCTION NAME:  nmmtl_generate_elements_cls
  
  
  FUNCTIONAL DESCRIPTION:
  
  Generate the elements from conductor line segments - called from 
  nmmtl_generate_elements.  Organize the elements in a linked list - 
  called a sub-list.  This sub-list may become part of a bigger list
  or it may be used by itself.  Use output parameters head and tail
  to return a pointer to the first and last elements on the sub-list.
  
  FORMAL PARAMETERS:
  
  LINE_SEGMENTS_P *clsp,           pointer to whole list of all conductor line
  segments - advanced in this routine, only
  until the next conductor is encountered.
  
  CELEMENTS_P *head,             return head of sub-list created here
  CELEMENTS_P *tail,             return tail of sub-list created here
  
  unsigned int *node_point_counter,  counting nodes allocated in elements
                                     allocated.
  
  RETURN VALUE:
  
  SUCCESS, FAIL
  
  CALLING SEQUENCE:
  
  nmmtl_generate_elements_cls(&cls,&head,&tail,&node_point_counter);
  
  */
int nmmtl_generate_elements_cls(LINE_SEGMENTS_P *clsp,
				CELEMENTS_P *head,
				CELEMENTS_P *tail,
				unsigned int *node_point_counter)
{
  int conductor;
  LINE_SEGMENTS_P cls;
  CELEMENTS_P element,start = NULL;
  unsigned int divisions;
  register unsigned int npcntr;
  unsigned int first_node;
	double xincr,xhalfincr,x,yincr,yhalfincr,y;
  unsigned char firstelement;
  
  cls = *clsp;
  conductor = cls->conductor;
  npcntr = *node_point_counter;
  first_node = npcntr;
  
  while(cls != NULL && cls->conductor == conductor)
  {
    divisions = cls->divisions;
    
#ifndef NO_HALF_MIN_CHECKING
    
    /* will the natural sized elements be small enough ? */
    if(cls->length/divisions > half_minimum_dimension)
    {
      /* no, make them smaller */
      divisions = cls->length/half_minimum_dimension + 0.99;
    }
    
#endif
    
    xincr = (cls->endx - cls->startx)/divisions;
    xhalfincr = xincr/2;
    yincr = (cls->endy - cls->starty)/divisions;
    yhalfincr = yincr/2;
    
    x=cls->startx;
    y=cls->starty;
    
    firstelement = TRUE;
    
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
      
      
      /* set up edge effects stuff, if either side is an edge 
         This is only activated on the first and last elements generated
	 from a given segment. */
      
      if(firstelement)
      {
	firstelement = FALSE;
	if(cls->theta2[0] != 0.0)
	{
	  element->edge[0] = (EDGEDATA_P)malloc(sizeof(EDGEDATA));
	  element->edge[0]->nu = cls->nu[0];
	  element->edge[0]->free_space_nu = cls->free_space_nu[0];
	}
	else element->edge[0] = NULL;
      }
      else element->edge[0] = NULL;
      
      if(divisions == 1) /* last element */
      {
	if(cls->theta2[1] != 0.0)
	{
	  element->edge[1] = (EDGEDATA_P)malloc(sizeof(EDGEDATA));
	  element->edge[1]->nu = cls->nu[1];
	  element->edge[1]->free_space_nu = cls->free_space_nu[1];
	}
	else element->edge[1] = NULL;
      }
      else element->edge[1] = NULL;
      
      /* the global coordinates at the various nodes */
      element->xpts[0] = x;
      element->ypts[0] = y;
      
      /* advance to the midpoint */
      x += xhalfincr;
      y += yhalfincr;
      element->xpts[1] = x;
      element->ypts[1] = y;
      
      /* advance to the endpoint */
      x += xhalfincr;
      y += yhalfincr;
      element->xpts[2] = x;
      element->ypts[2] = y;
      
      /* at this point cls->epsilon[0] should match cls->epsilon[1] */
      element->epsilon = cls->epsilon[0];
      
      /* array indicies into the BIG arrays */
      element->node[0] = npcntr++;
      element->node[1] = npcntr++;
      element->node[2] = npcntr;     /* set up for overlap of node points */
      
      /* advance to the next element */
      divisions--;
      
    } /* while looping through divisions of this segment */
    
    /* advance to the next segment */
    cls = cls->next;
    
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
  *clsp = cls;
  
  return(SUCCESS);
  
}
