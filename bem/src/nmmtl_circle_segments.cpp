
/*
  
  FACILITY:  NMMTL
  
  MODULE DESCRIPTION:
  
  Routines dealing with circle segments.
  
  nmmtl_cirseg_endpoint
  nmmtl_cirset_angle_to_normal
  nmmtl_cirseg_point_angle
  nmmtl_cirseg_angle_point
  
  AUTHOR(S):
  
  Kevin J. Buchs
  
  CREATION DATE:  Mon Jan 13 14:48:00 1992
  
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
  
  FUNCTION NAME:  nmmtl_cirseg_endpoint
  
  
  FUNCTIONAL DESCRIPTION:
  
  Determine if a given point hits either endpoint of the circle segment.
  
  Assume the point is on the circumference, so just find the angle to
  the point and see if it matches one of the angles of the circle 
  segment.
  
  
  FORMAL PARAMETERS:
  
  
  CIRCLE_SEGMENTS_P cirseg
  
  POINT_P point
  
  
  RETURN VALUE:
  
  None
  
  CALLING SEQUENCE:
  
  cirseg_endpoint = nmmtl_cirseg_endpoint(cirseg,intersection1);   
  */

int nmmtl_cirseg_endpoint(CIRCLE_SEGMENTS_P cirseg,POINT_P point)
{
  
	double theta;
  
  /* yields a theta between pi/2 and -pi/2 */
  if(point->x == cirseg->centerx)
  {
    if(point->y > cirseg->centery) theta = PI/2;
    else theta = - PI/2;
  }
  else
    theta = atan((point->y - cirseg->centery)/(point->x - cirseg->centerx));
  
  /* determine if 2nd or 3rd quadrant by sign of point->y - cirseg->centery */
  if(point->x - cirseg->centerx < 0) theta += PI;
  
  /* handle the case of exactly PI radians */
  if(theta == 0.0F && point->y == cirseg->centery &&
     point->x < cirseg->centerx)
    theta = PI;
  
  /* fourth quadrant angle gets offset */
  if(theta < 0) theta += 2*PI;
  
  /* check angle against endpoints */
  
  if(theta == cirseg->startangle) return(INITIAL_ENDPOINT);
  
  else if(theta == cirseg->endangle) return(TERMINAL_ENDPOINT);
  
  else return(NO_ENDPOINT);
}  


/*
  
  FUNCTION NAME:  nmmtl_cirset_angle_to_normal
  
  
  FUNCTIONAL DESCRIPTION:
  
  Compute the angle between a normal vector for given circle segment to 
  the point given and then turning to an endpoint of the given segment.
  
  FORMAL PARAMETERS:
  
  CIRCLE_SEGMENTS_P cirseg
  POINT_P point
  LINESEG_P seg
  int seg_index
  
  RETURN VALUE:
  
  the angle
  
  CALLING SEQUENCE:
  
  nmmtl_cirseg_angle_to_normal(segment,&intersection1,
  &dseg,dseg_index)   
  */
double nmmtl_cirseg_angle_to_normal(CIRCLE_SEGMENTS_P cirseg,
				   POINT_P point,
				   LINESEG_P seg,
				   int seg_index)
{
	double x1,y1,x2,y2;
  
  x1 = point->x - cirseg->centerx;
  y1 = point->y - cirseg->centery;
  /*
    
    Modified from this :
    
    x2 = seg->x[seg_index] - point->x;
    y2 = seg->y[seg_index] - point->y;
    
    To what is below :
    
    to avoid potentially, the point being on an endpoint of the segment.  It has
    the same effect.  We just need to determine which direction the caller wanted
    to go.  If he wanted to go from point to dseg [0], i.e. seg_index=0, then 
    we will go from dseg[1] to dseg[0].  Since the line segment is a line, this
    will have the correct intended result.
    
    */
  if(seg_index == 0)
  {
    x2 = seg->x[1] - seg->x[0];
    y2 = seg->y[1] - seg->y[0];
  }
  else
  {
    x2 = seg->x[0] - seg->x[1];
    y2 = seg->y[0] - seg->y[1];
  }
  
  return(nmmtl_angle_of_intersection(x1,y1,x2,y2));
  
  
}


/*
  
  FUNCTION NAME:  nmmtl_cirseg_point_angle
  
  
  FUNCTIONAL DESCRIPTION:
  
  Find the angle to the given point within the circle segment.
  
  FORMAL PARAMETERS:
  
  CIRCLE_SEGMENTS_P cirseg,    - pointer to the circle segment
	double point_x,               - point of interest x coordinate
	double point_y                - y coordinate
  
  RETURN VALUE:
  
	the angle in double
  
  CALLING SEQUENCE:
  
  angle = nmmtl_cirseg_point_angle(segment,x,y);
  
  */


double nmmtl_cirseg_point_angle(CIRCLE_SEGMENTS_P cirseg,double point_x,
						 double point_y)
{
	double theta;
  
  /* yields a theta between pi/2 and -pi/2 */
  if(point_x == cirseg->centerx)
  {
    if(point_y > cirseg->centery) theta = PI/2;
    else theta = - PI/2;
  }
  else  
    theta = atan((point_y - cirseg->centery)/(point_x - cirseg->centerx));
  
  /* determine if 2nd or 3rd quadrant by sign of point_y - cirseg->centery */
  if(point_x - cirseg->centerx < 0) theta += PI;
  
  /* handle the case of exactly PI radians */
  if(theta == 0.0F && point_y == cirseg->centery &&
     point_x < cirseg->centerx)
    theta = PI;
  
  /* fourth quadrant angle gets offset */
  if(theta < 0) theta += 2*PI;
  
  return(theta);
}



/*
  
  FUNCTION NAME:  nmmtl_cirseg_angle_point
  
  
  FUNCTIONAL DESCRIPTION:
  
  Find the x and y coordinates of a point on the circumference at the
  given angle.
  
  FORMAL PARAMETERS:
  
  CIRCLE_SEGMENTS_P cirseg - the circle segment
	double angle              - the angle to place point at
	double *point_x,*point_y              - returned coordinants
  
  
  RETURN VALUE:
  
  None
  
  CALLING SEQUENCE:
  
  */

void nmmtl_cirseg_angle_point(CIRCLE_SEGMENTS_P cirseg,double angle,
						double *point_x,double *point_y)
{
  
  *point_x = cirseg->radius * cos(angle) + cirseg->centerx;
  *point_y = cirseg->radius * sin(angle) + cirseg->centery;
  
}
