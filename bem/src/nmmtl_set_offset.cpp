
/*
  
  FACILITY:  NMMTL
  
  MODULE DESCRIPTION:
  
  Contains the function nmmtl_set_offset
  
  AUTHOR(S):
  
  Kevin J. Buchs
  
  CREATION DATE:   19-JUL-1991 09:49:00
  
  COPYRIGHT:   Copyright (C) 1991-92 by Mayo Foundation. All rights reserved.
  
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
  
  FUNCTION NAME:  nmmtl_set_offset
  
  
  FUNCTIONAL DESCRIPTION:
  
  We want the geometry to end up such that the top of the lower ground
  plane is located at y=0.  If this is not already the case, it requires that
  all y coordinates be adjusted by some offset.  That is what this function 
  will do.  It goes through all the signals, groundwires, and dielectrics 
  and adjusts each Y coordinate by the given amount.
  
  FORMAL PARAMETERS:
  
  double offset,                   - offset to use
  struct dielectric *dielectrics, - list of dielectrics
  struct contour *signals,        - list of signal conductors
  struct contour *groundwires     - list of groundwire conductors
  
  RETURN VALUE:
  
  SUCCESS
  
  CALLING SEQUENCE:
  
  nmmtl_set_offset(offset,dielectrics,signals,groundwires);
  
  */


int nmmtl_set_offset(double offset,
				 struct dielectric *dielectrics,
				 struct contour *signals,
				 struct contour *groundwires)
{
  struct contour *contours;
  struct polypoints *pp;
  int flag = 0;
  
  while(flag < 2)
  {
    contours = flag == 0 ? signals : groundwires;
    flag++;
    
    for(;contours != NULL;contours = contours->next)
    {
      switch(contours->primitive)
      {
      case RECTANGLE :
	/* offset the corners */
	contours->y0 -= offset;
	contours->y1 -= offset;
	break;
      case POLYGON :
	/* offset the top and bottom extents */
	contours->y0 -= offset;
	contours->y1 -= offset;
	/* offset each point */
	for(pp=contours->points;pp != NULL; pp = pp->next)
	{
	  pp->y -= offset;
	}
	break;
      case CIRCLE :
	/* offset the center */
	contours->y0 -= offset;
	break;
      }
    }
  }
  
  for(;dielectrics != NULL;dielectrics = dielectrics->next)
  {
    /* offset the corners */
    dielectrics->y0 -= offset;
    dielectrics->y1 -= offset;
  }
  return(SUCCESS);
  
}

