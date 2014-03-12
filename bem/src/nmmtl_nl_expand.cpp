/*
  
  FACILITY:  NMMTL
  
  MODULE DESCRIPTION:
  
  Contains the function nmmtl_nl_expand()
  
  AUTHOR(S):
  
  Kevin J. Buchs
  
  CREATION DATE:  Sept. 27, 1993
  
  COPYRIGHT:   Copyright (C) 1993 by Mayo Foundation. All rights reserved.
  
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
  
  FUNCTION NAME:  nmmtl_nl_expand
  
  
  FUNCTIONAL DESCRIPTION:

  Allows the generation of expansion elements for the dielectric
  interfaces.  This is a companion to nmmtl_genel_die.cxx, where
  plain elements for dielectric interfaces are created.  If the
  situation warrants it, the dielectric interfaces can be expanded
  with non-linearly sized elements which grow in size as one moves
  away from the conductor region.  This function will generate those
  non-linear elements if it is called from genel_die.cxx.

  We make this assumption -
    no expansion elements will have any points in common with other
    elements except for the starting point.

  FORMAL PARAMETERS:
  
	double xstart,                    - starting x value for expansion section
	double xend,                      - ending x value ""
	double incr_start,                - starting increment for an element (size of)
  float epsilonplus,               - epsilon to positive direction
  float epsilonminus,              - epsilon to negative direction
  float normaly,                   - y element of normal vector (normalx is zero)
	double y,                         - y value for elements to be generated
  unsigned int *node_point_counter - counting all node points 
  DELEMENTS_P *element_p           - last pointer in linked list of all elements
  int *number_elements             - global counter of number of elements generated
  unsigned int common_node         - this node point is in common with the linear part

  RETURN VALUE:
  
    None
  
  */

void nmmtl_nl_expand(double xstart,double xend,double incr_start,float epsilonplus,
										 float epsilonminus,float normaly,double y,
                     unsigned int *node_point_counter,
                     DELEMENTS_P *element_p, int *number_elements,
                     unsigned int common_node)
{

  DELEMENTS_P element;
  register unsigned int npcntr;
	double xincr,xhalfincr,x;
  int first_element;
	extern double NON_LINEARITY_FACTOR;
  npcntr = *node_point_counter;
  element = *element_p;

  xincr = incr_start;
  x = xstart;
  first_element = TRUE;

  /* This odd while loop expression needs some explanation.  When we approach
     the end of the expansion, it will most likely occur that the last element
     will be shorter than we wish.  It is the last little bit left at the end.
     If this little bit gets too small, we know this will cause the algorithm
     problems.  To avoid this we look two elements ahead (2*xincr) to see if
     we have reached the end.  If we are near, then we take the last full
     element and a part of one (the last little bit) and make it a bigger
     last element.  To check if we have reached the endpoint condition, we
     need two different checks depending upon which direction we are extending
     - more positive or more negative.  This is what is expressed by the (||)
     in the following expression.
     */

  while( (xincr > 0 && x+2*xincr < xend) || (xincr < 0 && x+2*xincr > xend) )
  {
    xhalfincr = xincr/2;
    (*number_elements)++;

    element->next = (DELEMENTS_P)malloc(sizeof(DELEMENTS));
    element = element->next;

    /* terminate the list for now */
    element->next = NULL;
      
    /* the global coordinates at the various nodes */
    element->xpts[0] = x;
    element->ypts[0] = y;
      
    /* advance to the midpoint */
    x += xhalfincr;
    element->xpts[1] = x;
    element->ypts[1] = y;
      
    /* advance to the endpoint */
    x += xhalfincr;
    element->xpts[2] = x;
    element->ypts[2] = y;
      
    /* set epsilon values */
    element->epsilonplus = epsilonplus;
    element->epsilonminus = epsilonminus;
      
    /* set normal vectors */
    element->normalx = 0.0;
    element->normaly = normaly;
      
    /* Now set up array indicies into the BIG arrays */
      
    /* first node */      
    if(first_element)  /* is this the first element? */
    {
      first_element = FALSE;
      element->node[0] = common_node;
    }
    else element->node[0] = npcntr++;
      
    /* middle node */
    element->node[1] = npcntr++;
     
    /* last node - don't advance, since this point is in common with
       the next element generated */
    element->node[2] = npcntr;  

    xincr *= NON_LINEARITY_FACTOR;
          

  } /* while looping through elements generated */

  /* last element gets handled specially */
  (*number_elements)++;

	element->next = (DELEMENTS_P)malloc(sizeof(DELEMENTS));
	element = element->next;

  /* terminate the list */
  element->next = NULL;
      
  /* the global coordinates at the various nodes */
  element->xpts[0] = x;
  element->ypts[0] = y;
      
  /* advance to the midpoint */
  element->xpts[1] = (xend + x)/2;
  element->ypts[1] = y;
      
  /* advance to the endpoint */
  element->xpts[2] = xend;
  element->ypts[2] = y;
      
  /* set epsilon values */
  element->epsilonplus = epsilonplus;
  element->epsilonminus = epsilonminus;
      
  /* set normal vectors */
  element->normalx = 0.0;
  element->normaly = normaly;
      
  /* Now set up array indicies into the BIG arrays */
      
  /* first node */      
  if(first_element)  /* is this the first element?  It could certainly be.  */
  {
    first_element = FALSE;
    element->node[0] = common_node;
  }
  else element->node[0] = npcntr++;
      
  /* middle node */
  element->node[1] = npcntr++;
     
  /* this is the last element, we advance npcntr since we don't want the
     next element to share a node with this one */
  element->node[2] = npcntr++;

  /* return the value of the node point counter after it has been used */
  *node_point_counter = npcntr;

  /* return the value of the element pointer after it has been used */
  *element_p = element;

}
