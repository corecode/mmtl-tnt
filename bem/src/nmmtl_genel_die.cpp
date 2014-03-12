
/*
  
  FACILITY:  NMMTL
  
  MODULE DESCRIPTION:
  
  Contains the function nmmtl_generated_elements_die()
  
  AUTHOR(S):
  
  Kevin J. Buchs
  
  CREATION DATE:  Fri Mar 20 08:43:53 1992
  
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

/* chain together the dielectric segments and node numbers used for the first
   and last elements generated from a particular segment */

typedef struct chain
{
  int nodestart,nodeend;
	double endx[2],endy[2];
  struct chain *next;
  
} CHAIN, *CHAIN_P;


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
  
  FUNCTION NAME:  nmmtl_generate_elements
  
  
  FUNCTIONAL DESCRIPTION:
  
  Generate elements from the segments along dielectric-to-dielectric
  boundaries.  It includes a  couple of tough operations, like finding 
  intersections among the segments.  Basically we check the list of
  completed segments when we process a new one.
  
  There is code here to check if the element sizes are within
  half_minimum_dimension.  This code is now disabled with this preprocessor
  constant to be defined to re-enable it:
  
  DIE_MIN_ELEMENT_SIZE
  
  FORMAL PARAMETERS:
  
  DIELECTRIC_SEGMENTS_P *ds,        - list of dielectric segments
  unsigned int *node_point_counter, - counter of node points
  int *number_elements              - total number of elements
  EXTENT_DATA_P extent_data         - given and desired extents

  RETURN VALUE:
  
  None
  
  CALLING SEQUENCE:
  
  nmmtl_generate_elements_die(die_elements,&node_point_counter,
                              &number_elements,extent_data);
  
  */


DELEMENTS_P nmmtl_generate_elements_die(DIELECTRIC_SEGMENTS_P ds,
					unsigned int *node_point_counter,
					int *number_elements,
					EXTENT_DATA_P extent_data)
{
  DIELECTRIC_SEGMENTS_P die_seg;
  CHAIN_P first_link = NULL,last_link,scan_chain;
  unsigned char intersection[2];
  unsigned int intersection_node[2];
  DELEMENTS_P start = NULL,element;
	double endx[2],endy[2];
  unsigned int cntr;
  unsigned int divisions;
  register unsigned int npcntr;
	double xincr,xhalfincr,x,yincr,yhalfincr,y;
  int first_element;
	double normalx,normaly;
  
  
  npcntr = *node_point_counter;
  
  die_seg = ds;
  while(die_seg != NULL)
  {
    
    /* come up with real x,y pairs for endpoints of segment */
    
    if(die_seg->orientation == VERTICAL_ORIENTATION)
    {
      
      endx[0] = endx[1] = die_seg->at;
      endy[0] = die_seg->start;
      endy[1] = die_seg->end;
      if(endy[0] < endy[1])
	      normalx = -1.0;           /* segment points upward - normal to left */
      else
	      normalx = 1.0;
      normaly = 0.0;
    }
    else  /* Horizontal orientation */
    {
      endy[0] = endy[1] = die_seg->at;
      endx[0] = die_seg->start;
      endx[1] = die_seg->end;
      if(endx[0] < endx[1])
	      normaly = 1.0;            /* segment points to right - normal is up */
      else
	      normaly = -1.0;
      normalx = 0.0;
    }
    
    /* Find any intersections with ends of segments already processed */
    
    intersection[0] = FALSE;
    intersection[1] = FALSE;
    scan_chain = first_link;
    
    while(scan_chain != NULL && !(intersection[0] && intersection[1]))
    {
      for(cntr = 0; cntr < 2; cntr++)
      {
	      if(scan_chain->endx[0] == endx[cntr] && 
	         scan_chain->endy[0] == endy[cntr])
	      {
	        intersection[cntr] = TRUE;
	        intersection_node[cntr] = scan_chain->nodestart;
	      }
	      else if(scan_chain->endx[1] == endx[cntr] && 
		            scan_chain->endy[1] == endy[cntr])
	      {
	        intersection[cntr] = TRUE;
	        intersection_node[cntr] = scan_chain->nodeend;
	      }
      }
      scan_chain = scan_chain->next;
    }
    
    /* Now create the elements from this segment */
    
    divisions = die_seg->divisions;
    
    /* disable the check for the minimum element size for dielectrics */
    
#ifdef DIE_MIN_ELEMENT_SIZE    
    /* will the natural sized elements be small enough ? */
    if(die_seg->length/divisions > half_minimum_dimension)
    {
      /* no, make them smaller */
      divisions = die_seg->length/half_minimum_dimension + 0.99;
    }
#endif
    
    xincr = (endx[1] - endx[0])/divisions;
    xhalfincr = xincr/2;
    yincr = (endy[1] - endy[0])/divisions;
    yhalfincr = yincr/2;
    
    x = endx[0];
    y = endy[0];
    
    /* Set up chain structure to store away data about this element for
       intersection detection later */
    
    if(first_link == NULL)
    {
      first_link = (CHAIN_P)malloc(sizeof(CHAIN));
      last_link = first_link;
    }
    else
    {
      last_link->next = (CHAIN_P)malloc(sizeof(CHAIN));
      last_link = last_link->next;
    }
    last_link->next = NULL;
    last_link->endx[0] = endx[0];
    last_link->endy[0] = endy[0];
    last_link->endx[1] = endx[1];
    last_link->endy[1] = endy[1];
    
    first_element = TRUE;
    
    while(divisions > 0)
    {
      (*number_elements)++;
      
      if(start == NULL)  /* first time through */
      {
	      /* create the head of the list */
	      element = (DELEMENTS_P)malloc(sizeof(DELEMENTS));
	      start = element;
      }
      else /* subsequent times through */
      {
	      /* link up to the last element created */
	      element->next = (DELEMENTS_P)malloc(sizeof(DELEMENTS));
	      element = element->next;
      }
      
      /* terminate the list for now */
      element->next = NULL;
      
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
      
      /* set epsilon values */
      element->epsilonplus = die_seg->epsilonplus;
      element->epsilonminus = die_seg->epsilonminus;
      
      /* set normal vectors */
      element->normalx = normalx;
      element->normaly = normaly;
      
      /* Now set up array indicies into the BIG arrays, this is a bit 
	       complex because of the need to take in to account the intersections
	       which are tested for above. */
      
      /* first node */
      
      if(first_element)  /* is this the first element? */
      {
	      first_element = FALSE;
	      if(intersection[0])
	        element->node[0] = last_link->nodestart = intersection_node[0];
	      else	
	        element->node[0] = last_link->nodestart = npcntr++;
      }
      else element->node[0] = npcntr++;
      
      /* middle node */
      element->node[1] = npcntr++;
      
      /* last node
	       if this isn't the last element for this segment, set up for 
	       overlap of node points, by not advancing npcntr.  Conversely,
	       if this is the last element, we advance npcntr.  If this last
	       element is an intersection with another segment - set the 
	       node based on the element of the other segment, but npcntr was
	       already advanced for the middle node and not used for this last
	       node so the same effect is achieved. */
      
      if(divisions == 1) /* is this the last element ? */
      {
	      if(intersection[1])
	        element->node[2] = last_link->nodeend = intersection_node[1];
	      else
      	  element->node[2] = last_link->nodeend = npcntr++;  
      }      
      else element->node[2] = npcntr;  
      
      /* advance to the next element */
      divisions--;
      
    } /* while looping through divisions of this segment */

    /* Check to see if there should be expansion of the element generation
       from this segment with non-linear nodes to a wider dimension.
       First, does segment go left to right?  In other words, is the starting
       x coordinate less than the ending x coordinate?  If the normal points
       up ( > 0.0 ) then we know the element goes left to right 
       */

    if(normaly > 0.0)
    {
      /* Do we need expansion on left end?  Check this by seeing if the flag was
         set to expand on left side (expand_left) and to see that we have approached
	 the left edge of the edge of the drawn cross section (left_cs_extent = end
	 of dielectric segment (end[0]).
	 */

      if(extent_data->expand_left && (extent_data->left_cs_extent == endx[0]))
        nmmtl_nl_expand(endx[0],extent_data->desired_left,-1*xincr,die_seg->epsilonplus,
                        die_seg->epsilonminus,normaly,y,&npcntr,&element,
                        number_elements,last_link->nodestart);

      /* need expansion on right end? */

      if(extent_data->expand_right && (extent_data->right_cs_extent == endx[1]))
        nmmtl_nl_expand(endx[1],extent_data->desired_right,xincr,die_seg->epsilonplus,
                        die_seg->epsilonminus,normaly,y,&npcntr,&element,
                        number_elements,last_link->nodeend);
    }

    /* Or does this segment goes right to left */
    else if(normaly < 0.0)
    {
      /* need expansion on left end? */

      if(extent_data->expand_left && (extent_data->left_cs_extent == endx[1]))
        nmmtl_nl_expand(endx[1],extent_data->desired_left,xincr,die_seg->epsilonplus,
                        die_seg->epsilonminus,normaly,y,&npcntr,&element,
                        number_elements,last_link->nodeend);

      /* need expansion on right end? */

      if(extent_data->expand_right && (extent_data->right_cs_extent == endx[0]))
        nmmtl_nl_expand(endx[0],extent_data->desired_right,-1*xincr,die_seg->epsilonplus,
                        die_seg->epsilonminus,normaly,y,&npcntr,&element,
                        number_elements,last_link->nodestart);
    }

    
    die_seg = die_seg->next;
  }
  
  *node_point_counter = npcntr;
  
  return(start);
  
}

