
/*
  
  FACILITY:  NMMTL
  
  MODULE DESCRIPTION:
  
  Contains:
  
  nmmtl_seg_in_die_rect
  nmmtl_circle_in_die_rect
  
  AUTHOR(S):
  
  Kevin J. Buchs
  
  CREATION DATE:  Wed Feb  5 08:19:24 1992
  
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
  
  FUNCTION NAME:  nmmtl_seg_in_die_rect
  
  
  FUNCTIONAL DESCRIPTION:
  
  Determine if a line segment is contained within a dielectric rectangle.
  All we need to do is check the x and y ranges for inclusion.
  
  FORMAL PARAMETERS:
  
  DIELECTRICS_P die_rect   the dielectric rectangle
  LINESEG_P line           the line segment
  
  RETURN VALUE:
  
  TRUE OR FALSE
  
  CALLING SEQUENCE:
  
  intersection = nmmtl_seg_in_die_rect(dielectric,line_seg);
  
  */

int nmmtl_seg_in_die_rect(DIELECTRICS_P die_rect,LINESEG_P line)
{
  if(line->x[0] >= die_rect->x0 && line->x[0] <= die_rect->x1 &&
     line->x[1] >= die_rect->x0 && line->x[1] <= die_rect->x1 &&
     line->y[0] >= die_rect->y0 && line->y[0] <= die_rect->y1 &&
     line->y[1] >= die_rect->y0 && line->y[1] <= die_rect->y1)
  {
    return(TRUE);
  }
  else
  {
    return(FALSE);
  }
}



/*
  
  FUNCTION NAME:  nmmtl_circle_in_die_rect
  
  
  FUNCTIONAL DESCRIPTION:
  
  Determine if a circle is contained within a dielectric rectangle.
  All we need to do is check the x and y ranges for inclusion.
  
  FORMAL PARAMETERS:
  
  DIELECTRICS_P die_rect      the dielectric rectangle
  POINT_P center         center of the circle
  double radius           radius of the circle
  
  RETURN VALUE:
  
  TRUE OR FALSE
  
  CALLING SEQUENCE:
  
  if(nmmtl_circle_in_die_rect(die,&center,radius))
  
  */

int nmmtl_circle_in_die_rect(DIELECTRICS_P die_rect,POINT_P center,
			     double radius)
{
  double top,bot,left,right;
  
  top = center->y + radius;
  bot = center->y - radius;
  left = center->x - radius;
  right = center->x + radius;
  
  if(bot >= die_rect->y0 && top <= die_rect->y1 &&
     left >= die_rect->x0 && right <= die_rect->x1)
  {
    return(TRUE);
  }
  else
  {
    return(FALSE);
  }
  
}
