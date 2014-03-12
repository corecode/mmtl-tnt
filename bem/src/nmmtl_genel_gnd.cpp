
/*
  
  FACILITY:  NMMTL
  
  MODULE DESCRIPTION:
  
  Contains the function nmmtl_generate_elements_gnd()
  
  AUTHOR(S):
  
  Kevin J. Buchs
  
  CREATION DATE:  Tue Mar 17 14:05:08 1992
  
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
  
  FUNCTION NAME:  nmmtl_generate_elements_gnd
  
  
  FUNCTIONAL DESCRIPTION:
  
  Generate the elements from the ground plane(s).  Follow the projections 
  on to the upper plane, if any.
  
  There is extra code here to:
  1) apply minimum element size to ground plane
  2) project conductors on to upper ground plane.
  
  This code is not being used now.  It is #ifdef'd out with these
  two preprocessor constants:
  
  GND_PLANE_COND_PROJECTION
  GND_PLANE_MIN_ELEMENT_SIZE
  
  
  FORMAL PARAMETERS:
  
  CELEMENTS_P *gnd_plane_list_head,   Head of list - output
  unsigned int *node_point_counter,  counting nodes allocated in elements
  allocated.
  int gnd_planes,                    number of ground planes
  int pln_seg,                     number of segments to break planes into
  double bottom_of_top_plane,         coordinates of ground planes
  double left_of_gnd_planes,          ""
  double right_of_gnd_planes          ""
  -- ifdef GND_PLANE_COND_PROJECTION --
  COND_PROJ_LIST_P cond_projections,        projections of conductors on upper
  -- -- 
  ground plane
  SORTED_GND_DIE_LIST_P upper_sorted_gdl   list of where upper ground plane
                                            die intersections are
  
  RETURN VALUE:
  
  SUCCESS, FAIL
  
  CALLING SEQUENCE:
  
  nmmtl_generate_elements_gnd(&gnd_plane_list_head,
  &node_point_counter
  gnd_planes,
  pln_seg,
  bottom_of_top_plane,
  left_of_gnd_planes,
  right_of_gnd_planes,
  upper_sorted_gdl);
  
  */
int nmmtl_generate_elements_gnd(CELEMENTS_P *gnd_plane_list_head,
				unsigned int *node_point_counter,
				int gnd_planes,
				int pln_seg,
				double bottom_of_top_plane,
				double left_of_gnd_planes,
				double right_of_gnd_planes,
#ifdef GND_PLANE_COND_PROJECTION
				COND_PROJ_LIST_P cond_projections,
#endif
				SORTED_GND_DIE_LIST_P upper_sorted_gdl)
{
  CELEMENTS_P element,start = NULL;
  register unsigned int npcntr;
  double xhalfincr,x;
  double width,close_enough;
  
  npcntr = *node_point_counter;
  width = right_of_gnd_planes - left_of_gnd_planes;
  xhalfincr = width/(pln_seg*2);
  
  /* do not apply this constraint to ground planes */
#ifdef GND_PLANE_MIN_ELEMENT_SIZE
  if(xhalfincr * 2 > half_minimum_dimension)
    xhalfincr = half_minimum_dimension / 2;
#endif
  
  /* Why is the lower ground plane here - that is not needed */
  
  /* Upper ground plane */
  
  if(gnd_planes != 2) return(SUCCESS);
  
  
  /* use the conductor projections list in conjunction with the pln_seg
     and half_minimum_dimension, width and xhalfincr are determined
     above.
     */
  
  x = left_of_gnd_planes;
  close_enough = 1e-10*xhalfincr;
  
  /* add in checking against "close_enough" 
     to avoid floating point comparison problems */

  while(x < right_of_gnd_planes && 
	fabs(x - right_of_gnd_planes) > close_enough)
  {

    /*
printf("x=%23.20g, right=%23.20g, x-right=%23.20g\n", x,right_of_gnd_planes,x-right_of_gnd_planes);
*/

    
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
    
    /* no edge effects stuff */
    element->edge[0] = NULL;
    element->edge[1] = NULL;
    
    /* set the dielectric coeficient that the ground plane sees */
		element->epsilon = *((float *)upper_sorted_gdl->data);
    
    /* Set the global coordinates at the various nodes */
    
    /* start node */
    element->xpts[0] = x;
    element->ypts[0] = bottom_of_top_plane;
    element->ypts[1] = bottom_of_top_plane;
    element->ypts[2] = bottom_of_top_plane;
    
    /* is there a dielectric break sooner than the next normal length-based
       break? */

    if(x+2*xhalfincr > upper_sorted_gdl->key)
    {
      
      /* also - do not project the conductors - for now */
#ifdef GND_PLANE_COND_PROJECTION
      /* is there a conductor projection break sooner than that? */
      if(cond_projections != NULL && 
	 upper_sorted_gdl->key > cond_projections->key)
      {
	
	/* advance to the midpoint */
	x = (x+cond_projections->key)/2;
	element->xpts[1] = x;
	
	/* advance to the endpoint */
	x = cond_projections->key;
	element->xpts[2] = x;
	
	/* advance the conductor projections list */
	cond_projections = cond_projections->next;
	
      }
      else /* just use dielectric break */
#endif
      {
	
	/* advance to the midpoint */
	x = (x+upper_sorted_gdl->key)/2;
	element->xpts[1] = x;
	
	/* advance to the endpoint */
	x = upper_sorted_gdl->key;
	element->xpts[2] = x;
	
	/* advance the dielectric intersections list */
	upper_sorted_gdl = upper_sorted_gdl->next;
      }
      
    }
    
#ifdef GND_PLANE_COND_PROJECTION
    /* is there a conductor projection break sooner than the next */
    /* normal length-based break? */
    else if(cond_projections != NULL && x+2*xhalfincr > cond_projections->key)
    {
      
      /* advance to the midpoint */
      x = (x+cond_projections->key)/2;
      element->xpts[1] = x;
      
      /* advance to the endpoint */
      x = cond_projections->key;
      element->xpts[2] = x;
      
      /* advance the conductor projections list */
      cond_projections = cond_projections->next;
    }
#endif
    
    /* else - just take the normal length-based break */
    else
    {
      /* advance to the midpoint */
      x += xhalfincr;
      element->xpts[1] = x;
      
      /* advance to the endpoint */
      x += xhalfincr;
      element->xpts[2] = x;
    }    
    
    /* array indicies into the BIG arrays */
    element->node[0] = npcntr++;
    element->node[1] = npcntr++;
    element->node[2] = npcntr;     /* set up for overlap of node points */
    
  }   /* while traversing the upper ground plane */
  
  
  /* The next time this variable gets used, it will be a different conductor
     or dielectric and thus we should advance the counter to a new node.
     In other words, there is no physical link between the last node on
     the last element of this upper ground plane, generated from this
     function and the first node of the first element generated from the
     next call to another element generation function
     */
  
  *node_point_counter = ++npcntr;
  
  /* return head of list */
  *gnd_plane_list_head = start;
  
  return(SUCCESS);
  
}



