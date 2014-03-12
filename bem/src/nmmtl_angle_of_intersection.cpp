
/*
  
  FACILITY:  NMMTL
  
  MODULE DESCRIPTION:
  
  contains the nmmtl_angle_of_intersection function.
  
  AUTHOR(S):
  
  Kevin J. Buchs
  
  CREATION DATE:  27-NOV-1991 09:54:06
  
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
  
  FUNCTION NAME:  nmmtl_angle_of_intersection
  
  FUNCTIONAL DESCRIPTION:
  
  Find the angle of intersection of two line segments given by the
  vectors (displacements).  This angle is the angle needed to rotate
  from the first vector to the second vector.  Counterclockwise is
  defined as the positive direction.  First we do the dot product
  between the two vectors.  This gives us the angle, with no sign
  orientation.  Next the vector (cross) product is computed to give the
  sign.
  
  
  FORMAL PARAMETERS:
  
  float x1, y1   x and y displacement of first line segment
  float x2, y2   x and y displacement of second line segment
  
  RETURN VALUE:
  
  the angle of intersection
  
  CALLING SEQUENCE:
  
  intersection_angle = nmmtl_angle_of_intersection( deltax1,deltay1,
  deltax2,deltay2);
  
  */


float nmmtl_angle_of_intersection(float x1, float y1, float x2, float y2)
{
  float angle;
  float length1,length2;
  
  length1 = sqrt(x1*x1 + y1*y1);
  length2 = sqrt(x2*x2 + y2*y2);
  
  /* determine angle by doing dot product */
  
  angle = acos( (x1*x2 + y1*y2)/(length1*length2) );
  
  /* determine sign by doing cross product */
  
  if((x1*y2 - x2*y1) < 0) angle *= -1.0;
  
  return(angle);
}
