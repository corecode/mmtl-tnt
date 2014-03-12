
/*

  FACILITY:  nmmtl

  MODULE DESCRIPTION:


Find various types of intersections between line segments, and line
segments and arcs, and higher structures containing these.

Functions contained herein:

            nmmtl_cd_intersect
            nmmtl_rect_seg_inter
            nmmtl_poly_seg_inter
            nmmtl_circle_seg_inter
            nmmtl_arc_seg_inter
	    nmmtl_in_arc_range
	    nmmtl_cirseg_seg_inter
	    nmmtl_in_cirseg_range
	    nmmtl_seg_seg_inter
	    nmmtl_in_seg_range

  AUTHORS:

      Kevin J. Buchs

  CREATION DATE:  Tue Nov 26 16:25:53 1991

  COPYRIGHT:   Copyright (C) 1991 by Mayo Foundation. All rights reserved.

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

#ifdef USE_UNNEEDED_FUNCTIONS


/*

  FUNCTION NAME: nmmtl_cd_intersect

  FUNCTIONAL DESCRIPTION:

  Find the intersection between a contour and a dielectric segment.
  Just find the first one and return.

  FORMAL PARAMETERS:

  CONTOURS_P contour             the contour
  DIELECTRIC_SEGMENTS_P dieseg   the dielectric segment

  RETURN VALUE:

  The first intersection point found is returned as the structure
  POINT in static memory.  If you need to save it, copy it.

*/

POINT_P nmmtl_cd_intersect(CONTOURS_P contour,
			   DIELECTRIC_SEGMENTS_P dieseg)
{
  static POINT intersection;
  LINESEG segment;
  int an_intersection;
  
  if(dieseg->orientation == VERTICAL_ORIENTATION)
  {
    segment.x[0] = dieseg->at;
    segment.x[1] = dieseg->at;
    segment.y[0] = dieseg->start;
    segment.y[1] = dieseg->end;
  }
  else
  {
    segment.y[0] = dieseg->at;
    segment.y[1] = dieseg->at;
    segment.x[0] = dieseg->start;
    segment.x[1] = dieseg->end;
  }
  
  switch(contour->primitive)
  {
  case RECTANGLE :
    an_intersection = nmmtl_rect_seg_inter(contour,segment,&intersection);
    break;
  case POLYGON :
    an_intersection = nmmtl_poly_seg_inter(contour,segment,&intersection);
    break;
  case CIRCLE :
    an_intersection = nmmtl_circle_seg_inter(contour,segment,&intersection);
    break;
  }
  
  if(an_intersection == FALSE)
  {
    return(NULL);
  }
  else
  {
    return(&intersection);
  }
}



/*

  FUNCTION NAME: nmmtl_rect_seg_inter


  FUNCTIONAL DESCRIPTION:

  Find the intersection of a line segment with a rectangular contour.
  Return true if there is an intersection.  Basically, checks all four
  sides of the rectangle.

  FORMAL PARAMETERS:

  CONTOURS_P contour    rectangle contour
  LINESEG segment       line segment
  POINT_P intersection   resultant intersection (returned)
  
  RETURN VALUE:

  TRUE or FALSE 

  CALLING SEQUENCE:
  
  status = nmmtl_rect_seg_inter(contour,segment,&intersection);

   
*/
  
int nmmtl_rect_seg_inter(CONTOURS_P contour,
			 LINESEG segment,
			 POINT_P intersection)
{
  LINESEG rectsegment;
  int status;
  int colinear;
  POINT_P unused_intersection;
  

  /* bottom segment */

  rectsegment.x[0] = contour->x0;
  rectsegment.x[1] = contour->x1;
  rectsegment.y[0] = contour->y0;
  rectsegment.y[1] = contour->y0;

  status = nmmtl_seg_seg_inter(&rectsegment,&segment,&colinear,intersection,
			       unused_intersection);
  if(status = FALSE)
  {
    /* right segment */

    rectsegment.x[0] = contour->x1;
    rectsegment.y[1] = contour->y1;

    status = nmmtl_seg_seg_inter(&rectsegment,&segment,&colinear,intersection,
				 unused_intersection);
    if(status = FALSE)
    {
      /* top segment */
      
      rectsegment.x[0] = contour->x0;
      rectsegment.y[0] = contour->y1;

      status = nmmtl_seg_seg_inter(&rectsegment,&segment,&colinear,intersection,
				   unused_intersection);
      if(status = FALSE)
      {
	/* left segment */
	
	rectsegment.x[1] = contour->x0;
	rectsegment.y[0] = contour->y0;
      }
    }
  }
  return(status);
}



/*

  FUNCTION NAME: nmmtl_poly_seg_inter


  FUNCTIONAL DESCRIPTION:

  Find the intersection of a line segment with a polygonal contour.
Return TRUE if there is an intersection.  Search each segment of
polygon and if any one gives an intersection, then return.  Otherwise,
keep on checking.


  FORMAL PARAMETERS:

  CONTOURS_P contour    polygonal contour
  LINESEG segment       line segment
  POINT_P intersection   resultant intersection (returned)
  
  RETURN VALUE:

  TRUE or FALSE

  CALLING SEQUENCE:
  
  status = nmmtl_poly_seg_inter(contour,segment,&intersection);

   
*/

int nmmtl_poly_seg_inter(CONTOURS_P contour,
			 LINESEG segment,
			 POINT_P intersection)
{
  LINESEG polysegment;
  int status;
  POLYPOINTS_P pp;
  int colinear;
  POINT_P unused_intersection;

  /* set up for first interation of loop */
  pp = contour->points;
  
  /* set up to pretend first point is end of previous segment */
  polysegment.x[1] = pp->x;
  polysegment.y[1] = pp->y;
  
  pp = pp->next;
  
  for(; pp != NULL; pp = pp->next)
  {

    /* start of new segment is end of previous segment */
    polysegment.x[0] = polysegment.x[1];
    polysegment.y[0] = polysegment.y[1];

    /* end of new segment is new point */
    polysegment.x[1] = contour->x1;
    polysegment.y[1] = contour->y0;

    /* check for intersection */
    status = nmmtl_seg_seg_inter(&polysegment,&segment,&colinear,intersection,
				 unused_intersection);
    if(status = TRUE) return(TRUE);
  }
  
  return(FALSE);
}



/*

  FUNCTION NAME: nmmtl_circle_seg_inter


  FUNCTIONAL DESCRIPTION:

  Find the intersection of a line segment with a circular contour.


  FORMAL PARAMETERS:

  CONTOURS_P contour    circle contour
  LINESEG segment       line segment
  POINT_P intersection   resultant intersection
  
  RETURN VALUE:

  None

  CALLING SEQUENCE:
  
  nmmtl_circle_seg_inter(contour,segment,&intersection);

   
*/
  
int nmmtl_circle_seg_inter(CONTOURS_P contour,
			  LINESEG segment,
			  POINT_P intersection)
{
  POINT intersection2;
  int tangent; 
  ARC circle_arc;
  
  circle_arc.center_x = contour->x0;
  circle_arc.center_y = contour->y0;
  circle_arc.radius = contour->x1;
  /* full arc */
  circle_arc.start_angle = 0;
  circle_arc.end_angle = 2 * PI;
  
  return(nmmtl_arc_seg_inter(circle_arc,segment,intersection,&intersection2,
			     &tangent));
  
}

#endif /* #ifdef USE_UNNEEDED_FUNCTIONS */



/*

  FUNCTION NAME:  nmmtl_arc_seg_inter


  FUNCTIONAL DESCRIPTION:

Find out if there is an intersection between an arbitrary arc and a
line segment.  Do this by solving the equations for the intersection
and then trying to numerically solve the resulting quadratic equation.
You get zero, one, or two roots.  One root indicates the line is a
tangent.  Once this is solved, then try the root or roots to see if
they fit into the line segment's range and the circle segment's range
of angles.  The case where the line segment is vertical must be
handled differently.  In this case, substitute the constant value of x
from the line into the circle equation and solve for y.


  FORMAL PARAMETERS:

  ARC arc                  - arc input
  LINESEG seg              - line segment input
  POINT_P intersection1    - first intersection point or only
  POINT_P intersection2    - second intersection point if two
  int *tangent             - set for only one root to quadratic, indicating
                             that we have a tangent line.
  
  RETURN VALUE:

  0, 1, or 2 intersections found

  CALLING SEQUENCE:
   
  num_intersections = nmmtl_arc_seg_inter(circle_arc,segment,
                                          &intersection1,&intersection2,
                                          &tangent);
*/

int nmmtl_arc_seg_inter(ARC arc,
			LINESEG seg,
			POINT_P intersection1,
			POINT_P intersection2,
			int *tangent)
{
	double slope;   /* for slope of segment equation */
	double intercept;   /* y intercept of segment equation */
	double A,B,C;  /* coeficients of quadratic equation */
	double s;  /* under square root term */
	double root_x,root_y;
  int num_intersections = 0; 
  

  *tangent = FALSE;

  /* check for vertical */

  if(seg.x[1] == seg.x[0])
  {
      /* check for tangential intersection on left or right */
      if( (seg.x[0] == (arc.center_x - arc.radius)) ||
	  (seg.x[0] == (arc.center_x + arc.radius)) )
      {
	root_y = arc.center_y;
	/* does this fit into segment's range */
	if((seg.y[1] > seg.y[0] && root_y <= seg.y[1] && root_y >= seg.y[0]) ||
	   (seg.y[1] < seg.y[0] && root_y >= seg.y[1] && root_y <= seg.y[0]))
	{
	  root_x = seg.x[0];
	  /* does this fit into arc's range */
	  if(nmmtl_in_arc_range(arc,root_x,root_y))
	  {
	    intersection1->y = root_y;
	    intersection1->x = root_x;
            *tangent = TRUE;
	    return(1);
	  }
	}
      }
      
      /* else, vertical, but not tangential */
      /* check for chordal intersections - check generally if segment
	 falls within left and right horizontal extents of circle. */
      else if( (seg.x[0] > (arc.center_x - arc.radius)) &&
	      (seg.x[0] < (arc.center_x + arc.radius)) )
      {
	/* find the roots by solving a reduced intersection equation */
	s = arc.radius * arc.radius - 
	  (seg.x[0] - arc.center_x) * (seg.x[0] - arc.center_x);
	
	/* the following two should not occur, ruled out by the enclosing
	   if statement.  Also, the single solution should be covered by
	   the previous tangential intersection section */
	/* imaginary solution */
	if(s < 0) return(0);
	/* single solution - tangential */
	if(s == 0) return(0);
	
	root_x = seg.x[0];
	
	/* check (-) root first */
	root_y = arc.center_y - sqrt(s);
	/* does this fit into segment's range */
	if((seg.y[1] > seg.y[0] && root_y <= seg.y[1] && root_y >= seg.y[0]) ||
	   (seg.y[1] < seg.y[0] && root_y >= seg.y[1] && root_y <= seg.y[0]))
	{
	  /* does this fit into arc's range */
	  if(nmmtl_in_arc_range(arc,root_x,root_y))
	  {
	    intersection1->y = root_y;
	    intersection1->x = root_x;
	    num_intersections = 1; 
	  }
	}
	
	/* check (+) root second */
	root_y = arc.center_y + sqrt(s);
	/* does this fit into segment's range */
	if((seg.y[1] > seg.y[0] && root_y <= seg.y[1] && root_y >= seg.y[0]) ||
	   (seg.y[1] < seg.y[0] && root_y >= seg.y[1] && root_y <= seg.y[0]))
	{
	  /* does this fit into arc's range */
	  if(nmmtl_in_arc_range(arc,root_x,root_y))
	  {
	    /* see if a previous intersection was found - then set second */
	    if(num_intersections == 1)
	    {
	      intersection2->y = root_y;
	      intersection2->x = root_x;
	      num_intersections = 2;
	    }
            /* otherwise, this is the first intersection */
	    else
	    {
	      intersection1->y = root_y;
	      intersection1->x = root_x;
	      num_intersections = 1; 
	    }
	  }
	} /* if second root in line segment range */
	
	return(num_intersections); /* number of intersection found */
	
      } /* else, vertical, non-tangential */
  } /* if, line segment is vertical */

  /* check for horizontal */
  else if(seg.y[1] == seg.y[0])
  {
      /* check for tangential intersection on bottom or top */
      if( (seg.y[0] == (arc.center_y - arc.radius)) ||
	  (seg.y[0] == (arc.center_y + arc.radius)) )
      {
	root_x = arc.center_x;
	/* does this fit into segment's range */
	if((seg.x[1] > seg.x[0] && root_x <= seg.x[1] && root_x >= seg.x[0]) ||
	   (seg.x[1] < seg.x[0] && root_x >= seg.x[1] && root_x <= seg.x[0]))
	{
	  root_y = seg.y[0];
	  /* does this fit into arc's range */
	  if(nmmtl_in_arc_range(arc,root_x,root_y))
	  {
	    intersection1->y = root_y;
	    intersection1->x = root_x;
	    *tangent = TRUE;
	    return(1);
	  }
	}
      }
      
      /* else, horizontal, but not tangential */
      /* check for chordal intersections - check generally if segment
	 falls within top and bottom vertical extents of circle. */
      else if((seg.y[0] > (arc.center_y - arc.radius)) &&
	      (seg.y[0] < (arc.center_y + arc.radius)) )
      {
	/* find the roots by solving a reduced intersection equation */
	s = arc.radius * arc.radius - 
	  (seg.y[0] - arc.center_y) * (seg.y[0] - arc.center_y);
	
	/* the following two should not occur, ruled out by the enclosing
	   if statement.  Also, the single solution should be covered by
	   the previous tangential intersection section */
	/* imaginary solution */
	if(s < 0) return(0);
	/* single solution - tangential */
	if(s == 0) return(0);
	
	root_y = seg.y[0];
	
	/* check (-) root first */
	root_x = arc.center_x - sqrt(s);
	/* does this fit into segment's range */
	if((seg.x[1] > seg.x[0] && root_x <= seg.x[1] && root_x >= seg.x[0]) ||
	   (seg.x[1] < seg.x[0] && root_x >= seg.x[1] && root_x <= seg.x[0]))
	{
	  /* does this fit into arc's range */
	  if(nmmtl_in_arc_range(arc,root_x,root_y))
	  {
	    intersection1->y = root_y;
	    intersection1->x = root_x;
	    num_intersections = 1; 
	  }
	}
	
	/* check (+) root second */
	root_x = arc.center_x + sqrt(s);
	/* does this fit into segment's range */
	if((seg.x[1] > seg.x[0] && root_x <= seg.x[1] && root_x >= seg.x[0]) ||
	   (seg.x[1] < seg.x[0] && root_x >= seg.x[1] && root_x <= seg.x[0]))
	{
	  /* does this fit into arc's range */
	  if(nmmtl_in_arc_range(arc,root_x,root_y))
	  {
	    /* see if a previous intersection was found - then set second */
	    if(num_intersections == 1)
	    {
	      intersection2->y = root_y;
	      intersection2->x = root_x;
	      num_intersections = 2;
	    }
	    else
	    {
	      intersection1->y = root_y;
	      intersection1->x = root_x;
	      num_intersections = 1; 
	    }
	  }
	} /* if second root in line segment range */
	
	return(num_intersections); /* number of intersection found */
	
      } /* in range for chordal intersection */
  }
  else /* non-vertical, non-horizontal segment */
  {
      /* determine segment equation */
      slope = (seg.y[1] - seg.y[0]) / (seg.x[1] - seg.x[0]);
      intercept = seg.x[0] - seg.y[0]/slope;
      
      /* terms of quadratic equation to solve */
      A = slope*slope + 1;
      B = 2*slope*(intercept-arc.center_y) - 2*arc.center_x;
      C = (intercept - arc.center_y) * (intercept - arc.center_y) + 
	arc.center_x * arc.center_x - 
	  arc.radius * arc.radius;
      
      /* square root term of quadratic equation solution determines all */
      s = B*B - 4*A*C;
      
      /* imaginary solutions */
      if(s < 0) return(0);
      
      /* single solution - check to see if in the segment range */
      if(s == 0)
      {
	*tangent = TRUE;
	root_x = -B/(2*A);
	/* does this fit into segment's range */
	if((seg.x[1] > seg.x[0] && root_x <= seg.x[1] && root_x >= seg.x[0]) ||
	   (seg.x[1] < seg.x[0] && root_x >= seg.x[1] && root_x <= seg.x[0]))
	{
	  root_y = slope * root_x + intercept;
	  /* does this fit into arc's range */
	  if(nmmtl_in_arc_range(arc,root_x,root_y))
	  {
	    intersection1->y = root_y;
	    intersection1->x = root_x;
	    return(1);
	  }
	}
      }
      
      /* two solutions - check to see if in the segment range */
      else
      {
	/* try (+) root first */
	root_x = (-B + sqrt(s))/2*A;
	/* does this fit into segment's range */
	if((seg.x[1] > seg.x[0] && root_x <= seg.x[1] && root_x >= seg.x[0]) ||
	   (seg.x[1] < seg.x[0] && root_x >= seg.x[1] && root_x <= seg.x[0]))
	{
	  root_y = slope * root_x + intercept;
	  /* does this fit into arc's range */
	  if(nmmtl_in_arc_range(arc,root_x,root_y))
	  {
	    intersection1->y = root_y;
	    intersection1->x = root_x;
	    num_intersections = 1; 
	  }
	}
	/* now try (-) root */
	root_x = (-B - sqrt(s))/2*A;
	/* does this fit into segment's range */
	if((seg.x[1] > seg.x[0] && root_x <= seg.x[1] && root_x >= seg.x[0]) ||
	   (seg.x[1] < seg.x[0] && root_x >= seg.x[1] && root_x <= seg.x[0]))
	{
	  root_y = slope * root_x + intercept;
	  /* does this fit into arc's range */
	  if(nmmtl_in_arc_range(arc,root_x,root_y))
	  {
	    if(num_intersections == 1)
	    {
	      intersection2->y = root_y;
	      intersection2->x = root_x;
	      num_intersections = 2;
	    }
	    else
	    {
	      intersection1->y = root_y;
	      intersection1->x = root_x;
	      num_intersections = 1; 
	    }
	  }
	} /* if second root in line segment range */
	
	return(num_intersections); /* number of intersection found */
	
      } /* end else two roots, try both */
    } /* end else, non-horizontal, non-vertical segment */

    return(0);

}

  

/*

  FUNCTION NAME:  nmmtl_in_arc_range


  FUNCTIONAL DESCRIPTION:

An x and y coordinate is assumed to lie on the circumference of a
circle.  This function determines if the point lies within a certain
angle range determined by the start_angle and end_angle of the arc
data structure.  Basically convert the x and y to an angle and check
the range.

  FORMAL PARAMETERS:

  ARC arc       arc to check against
  double root_x  x coordinate of point
  double root_y  y ""

  RETURN VALUE:

  TRUE or FALSE

  CALLING SEQUENCE:
   
  status = nmmtl_in_arc_range(arc,root_x,root_y);
*/

int nmmtl_in_arc_range(ARC arc,double root_x, double root_y)
{                                             
  double theta;
  
  /* yields a theta between pi/2 and -pi/2 */
  if(root_x == arc.center_x)
  {
    if(root_y > arc.center_y) theta = PI/2;
    else theta = - PI/2;
  }
  else
    theta = atan((root_y - arc.center_y)/(root_x - arc.center_x));

  /* determine if 2nd or 3rd quadrant by sign of root_x - arc.center_x */
  if(root_x - arc.center_x < 0) theta += PI;

  /* fourth quadrant angle gets offset */
  else if(theta < 0) theta += 2*PI;
  
  /* do the angle inclusion test */
  if(theta >= arc.start_angle && theta <= arc.end_angle)
    return(TRUE);
  
  return(FALSE);
}



/*

  FUNCTION NAME:  nmmtl_cirseg_seg_inter


  FUNCTIONAL DESCRIPTION:

Just like nmmtl_arc_seg_inter but takes a circle segment as input to
avoid copying data around.

Find out if there is an intersection between an arbitrary circle
segment and a line segment.  Do this by solving the equations for the
intersection and then trying to numerically solve the resulting
quadratic equation.  You get zero, one, or two roots.  One root
indicates the line is a tangent.  Once this is solved, then try the
root or roots to see if they fit into the line segment's range and the
circle segment's range of angles.  The case where the line segment is
vertical must be handled differently.  In this case, substitute the
constant value of x from the line into the circle equation and solve
for y.


  FORMAL PARAMETERS:

  CIRCLE_SEGMENTS_P cirseg    - circle segment input
  LINESEG_P seg              - line segment input
  POINT_P intersection1    - first intersection point or only
  POINT_P intersection2    - second intersection point if two
  int *tangent             - set for only one root to quadratic, indicating
                             that we have a tangent line.
  
  RETURN VALUE:

  0, 1, or 2 intersections found

  CALLING SEQUENCE:
   
  num_intersections = nmmtl_cirseg_seg_inter(cirseg,segment,
                                          &intersection1,&intersection2,
                                          &tangent);
*/

int nmmtl_cirseg_seg_inter(CIRCLE_SEGMENTS_P cirseg,
			   LINESEG_P seg,
			   POINT_P intersection1,
			   POINT_P intersection2,
			   int *tangent)
{
	double slope;   /* for slope of segment equation */
	double intercept;   /* y intercept of segment equation */
	double A,B,C;  /* coeficients of quadratic equation */
	double s;  /* under square root term */
	double root_x,root_y;
	double q;
	int num_intersections = 0; 
  

  *tangent = FALSE;

  /* check for vertical */

  if(seg->x[1] == seg->x[0])
  {
      /* check for tangential intersection on left or right */
      if( (seg->x[0] == (cirseg->centerx - cirseg->radius)) ||
	  (seg->x[0] == (cirseg->centerx + cirseg->radius)) )
      {
	root_y = cirseg->centery;
	/* does this fit into segment's range */
	if((seg->y[1] > seg->y[0] && root_y <= seg->y[1] && root_y >= seg->y[0]) ||
	   (seg->y[1] < seg->y[0] && root_y >= seg->y[1] && root_y <= seg->y[0]))
	{
	  root_x = seg->x[0];
	  /* does this fit into cirseg's range */
	  if(nmmtl_in_cirseg_range(cirseg,root_x,root_y))
	  {
	    intersection1->y = root_y;
	    intersection1->x = root_x;
            *tangent = TRUE;
	    return(1);
	  }
	}
      }
      
      /* else, vertical, but not tangential */
      /* check for chordal intersections - check generally if segment
	 falls within left and right horizontal extents of circle. */
      else if( (seg->x[0] > (cirseg->centerx - cirseg->radius)) &&
	      (seg->x[0] < (cirseg->centerx + cirseg->radius)) )
      {
	/* find the roots by solving a reduced intersection equation */
	s = cirseg->radius * cirseg->radius -
	  (seg->x[0] - cirseg->centerx) * (seg->x[0] - cirseg->centerx);
	
	/* the following two should not occur, ruled out by the enclosing
	   if statement.  Also, the single solution should be covered by
	   the previous tangential intersection section */
	/* imaginary solution */
	if(s < 0) return(0);
	/* single solution - tangential */
	if(s == 0) return(0);
	
	root_x = seg->x[0];
	
	/* check (-) root first */
	root_y = cirseg->centery - sqrt(s);
	/* does this fit into segment's range */
	if((seg->y[1] > seg->y[0] && root_y <= seg->y[1] && root_y >= seg->y[0]) ||
	   (seg->y[1] < seg->y[0] && root_y >= seg->y[1] && root_y <= seg->y[0]))
	{
	  /* does this fit into cirseg's range */
	  if(nmmtl_in_cirseg_range(cirseg,root_x,root_y))
	  {
	    intersection1->y = root_y;
	    intersection1->x = root_x;
	    num_intersections = 1; 
	  }
	}
	
	/* check (+) root second */
	root_y = cirseg->centery + sqrt(s);
	/* does this fit into segment's range */
	if((seg->y[1] > seg->y[0] && root_y <= seg->y[1] && root_y >= seg->y[0]) ||
	   (seg->y[1] < seg->y[0] && root_y >= seg->y[1] && root_y <= seg->y[0]))
	{
	  /* does this fit into cirseg's range */
	  if(nmmtl_in_cirseg_range(cirseg,root_x,root_y))
	  {
	    /* see if a previous intersection was found - then set second */
	    if(num_intersections == 1)
	    {
	      intersection2->y = root_y;
	      intersection2->x = root_x;
	      num_intersections = 2;
	    }
            /* otherwise, this is the first intersection */
	    else
	    {
	      intersection1->y = root_y;
	      intersection1->x = root_x;
	      num_intersections = 1; 
	    }
	  }
	} /* if second root in line segment range */
	
	return(num_intersections); /* number of intersection found */
      } /* else, vertical, non-tangential */
  } /* if, line segment is vertical */

  /* check for horizontal */
  else if(seg->y[1] == seg->y[0])
  {
      /* check for tangential intersection on bottom or top */
      if( (seg->y[0] == (cirseg->centery - cirseg->radius)) ||
	  (seg->y[0] == (cirseg->centery + cirseg->radius)) )
      {
	root_x = cirseg->centerx;
	/* does this fit into segment's range */
	if((seg->x[1] > seg->x[0] && root_x <= seg->x[1] && root_x >= seg->x[0]) ||
	   (seg->x[1] < seg->x[0] && root_x >= seg->x[1] && root_x <= seg->x[0]))
	{
	  root_y = seg->y[0];
	  /* does this fit into cirseg's range */
	  if(nmmtl_in_cirseg_range(cirseg,root_x,root_y))
	  {
	    intersection1->y = root_y;
	    intersection1->x = root_x;
	    *tangent = TRUE;
	    return(1);
	  }
	}
      }
      
      /* else, horizontal, but not tangential */
      /* check for chordal intersections - check generally if segment
	 falls within top and bottom vertical extents of circle. */
      else if((seg->y[0] > (cirseg->centery - cirseg->radius)) &&
	      (seg->y[0] < (cirseg->centery + cirseg->radius)) )
      {
	/* find the roots by solving a reduced intersection equation */
	s = cirseg->radius * cirseg->radius -
	  (seg->y[0] - cirseg->centery) * (seg->y[0] - cirseg->centery);
	
	/* the following two should not occur, ruled out by the enclosing
	   if statement.  Also, the single solution should be covered by
	   the previous tangential intersection section */
	/* imaginary solution */
	if(s < 0) return(0);
	/* single solution - tangential */
	if(s == 0) return(0);
	
	root_y = seg->y[0];
	
	/* check (-) root first */
	root_x = cirseg->centerx - sqrt(s);
	/* does this fit into segment's range */
	if((seg->x[1] > seg->x[0] && root_x <= seg->x[1] && root_x >= seg->x[0]) ||
	   (seg->x[1] < seg->x[0] && root_x >= seg->x[1] && root_x <= seg->x[0]))
	{
	  /* does this fit into cirseg's range */
	  if(nmmtl_in_cirseg_range(cirseg,root_x,root_y))
	  {
	    intersection1->y = root_y;
	    intersection1->x = root_x;
	    num_intersections = 1; 
	  }
	}
	
	/* check (+) root second */
	root_x = cirseg->centerx + sqrt(s);
	/* does this fit into segment's range */
	if((seg->x[1] > seg->x[0] && root_x <= seg->x[1] && root_x >= seg->x[0]) ||
	   (seg->x[1] < seg->x[0] && root_x >= seg->x[1] && root_x <= seg->x[0]))
	{
	  /* does this fit into cirseg's range */
	  if(nmmtl_in_cirseg_range(cirseg,root_x,root_y))
	  {
	    /* see if a previous intersection was found - then set second */
	    if(num_intersections == 1)
	    {
	      intersection2->y = root_y;
	      intersection2->x = root_x;
	      num_intersections = 2;
	    }
	    else
	    {
	      intersection1->y = root_y;
	      intersection1->x = root_x;
	      num_intersections = 1; 
	    }
	  }
	} /* if second root in line segment range */
	
	return(num_intersections); /* number of intersection found */
	
      } /* in range for chordal intersection */
  }
  else /* non-vertical, non-horizontal segment */
  {
    /* determine segment equation */
    slope = (seg->y[1] - seg->y[0]) / (seg->x[1] - seg->x[0]);
    if(slope == 0.0) intercept = seg->y[0];
    else intercept = seg->x[0] - seg->y[0]/slope;
    
    /* terms of quadratic equation to solve */    
    A = slope*slope + 1;
    B = 2*slope*(intercept-cirseg->centery) - 2*cirseg->centerx;
    C = (intercept - cirseg->centery) * (intercept - cirseg->centery) + 
      cirseg->centerx * cirseg->centerx - 
	cirseg->radius * cirseg->radius;
    
    /* square root term of quadratic equation solution determines all */
    s = B*B - 4*A*C;
    
    /* imaginary solutions */
    if(s < 0) return(0);
    
    /* single solution - check to see if in the segment range */
    if(s == 0)
    {
      *tangent = TRUE;
      root_x = -B/(2*A);
      /* does this fit into segment's range */
      if((seg->x[1] > seg->x[0] && root_x <= seg->x[1] && root_x >= seg->x[0])
	 ||
	 (seg->x[1] < seg->x[0] && root_x >= seg->x[1] && root_x <= seg->x[0]))
      {
	root_y = slope * root_x + intercept;
	/* does this fit into cirseg's range */
	if(nmmtl_in_cirseg_range(cirseg,root_x,root_y))
	{
	  intersection1->y = root_y;
	  intersection1->x = root_x;
	  return(1);
	}
      }
    }
    
    /* two solutions - check to see if in the segment range */
    else
    {
      /* try one root first */
      // Implemented a new technique here...
      //    ...the old technique
      //    root_x = (-B + sqrt(s))/2*A;
      //    ...the new technique is from Numerical Recipes in C, p. 156
      q = -0.5 * ( B + (B/fabs(B))*sqrt(B*B - 4*A*C));
      root_x = q/A;
      
      /* does this fit into segment's range */
      if((seg->x[1] > seg->x[0] && root_x <= seg->x[1] && root_x >= seg->x[0])
	 ||
	 (seg->x[1] < seg->x[0] && root_x >= seg->x[1] && root_x <= seg->x[0]))
      {
	root_y = slope * root_x + intercept;
	/* does this fit into cirseg's range */
	if(nmmtl_in_cirseg_range(cirseg,root_x,root_y))
	{
	  intersection1->y = root_y;
	  intersection1->x = root_x;
	  num_intersections = 1; 
	}
      }
      /* now try other root */
      // ...old technique
      //    root_x = (-B - sqrt(s))/2*A;
      root_x = C/q;
      
      /* does this fit into segment's range */
      if((seg->x[1] > seg->x[0] && root_x <= seg->x[1] && root_x >= seg->x[0])
	 ||
	 (seg->x[1] < seg->x[0] && root_x >= seg->x[1] && root_x <= seg->x[0]))
      {
	root_y = slope * root_x + intercept;
	/* does this fit into cirseg's range */
	if(nmmtl_in_cirseg_range(cirseg,root_x,root_y))
	{
	  if(num_intersections == 1)
	  {
	    intersection2->y = root_y;
	    intersection2->x = root_x;
	    num_intersections = 2;
	  }
	  else
	  {
	    intersection1->y = root_y;
	    intersection1->x = root_x;
	    num_intersections = 1; 
	  }
	}
      } /* if second root in line segment range */

      return(num_intersections); /* number of intersection found */

    } /* else two roots, try both */
  } /* else line segment is not horizontal or vertical */

  return(0);

}

  

/*

  FUNCTION NAME:  nmmtl_in_cirseg_range


  FUNCTIONAL DESCRIPTION:

Similar to nmmtl_in_arc_range, but takes an circle segment instead of an
arc as input.

An x and y coordinate is assumed to lie on the circumference of a
circle.  This function determines if the point lies within a certain
angle range determined by the start_angle and end_angle of the cirseg
data structure.  Basically convert the x and y to an angle and check
the range.

  FORMAL PARAMETERS:

  CIRCLE_SEGMENTS_P cirseg       circle segment to check against
  double root_x  x coordinate of point
  double root_y  y ""

  RETURN VALUE:

  TRUE or FALSE

  CALLING SEQUENCE:
   
  status = nmmtl_in_cirseg_range(cirseg,root_x,root_y);
*/

int nmmtl_in_cirseg_range(CIRCLE_SEGMENTS_P cirseg,double root_x,double root_y)
{
  double theta;
  
  /* yields a theta between pi/2 and -pi/2 */
  if(root_x == cirseg->centerx)
  {
    if(root_y > cirseg->centery) theta = PI/2;
    else theta = - PI/2;
  }
  else
    theta = atan((root_y - cirseg->centery)/(root_x - cirseg->centerx));

  /* determine if 2nd or 3rd quadrant by sign of root_x - cirseg->centerx */
  if(root_x - cirseg->centerx < 0) theta += PI;

  /* fourth quadrant angle gets offset */
  else if(theta < 0) theta += 2*PI;
  
  /* do the angle inclusion test */
  if(theta >= cirseg->startangle && theta <= cirseg->endangle)
    return(TRUE);
  
  return(FALSE);
}



/*

  FUNCTION NAME: nmmtl_seg_seg_inter


  FUNCTIONAL DESCRIPTION:

  Find out if there is an intersection between two line segments.
Return TRUE if there is, FALSE if not.  If an intersection, it could
be a point or a line in the case of parallel, colinear segments.  The
flag, colinear is set appropriately.  Also, intersection2 is returned
to indicate the other endpoint of the line segment formed by
intersecting colinear segments.

This function brings up the issue of how close the floating point
numbers should be.  Let us here, assume that they need to be exact.
Basically the same operations are performed on both sets of numbers,
so one would hope they are exact

It is assumed the the X coordinates of segment1 and segment2 are in
increasing order, i.e. segment1->x[1] >= segment1->x[0].

With colinear intersection, there are two intersection points.  They
are organized such that the x value of intersection is less than the
x value of intersection2.  The Y values may vary depending upon the
slope.

  FORMAL PARAMETERS:

  LINESEG_P segment1       
  LINESEG_P segment2
  int *colinear FALSE, unless there is a region of common overlap.
  POINT_P intersection  pointer to intersection returned.  If colinear,
                        this is one side of the resultant segment formed.
  POINT_P intersection2  If colinear, this is other side of resultant
                         segment.  Otherwise, not used.
  
  RETURN VALUE:

  TRUE or FALSE

  CALLING SEQUENCE:

  status = nmmtl_seg_seg_inter(segment1,segment2,&colinear,intersection,
                               intersection2);
   
*/

int nmmtl_seg_seg_inter(LINESEG_P segment1,
			LINESEG_P segment2,
			int *colinear,
			POINT_P intersection,
			POINT_P intersection2)
{
  double intercept1,intercept2;  /* y intercepts */
  double slope1,slope2;  /* slopes */
  double root_x,root_y;
  

  *colinear = FALSE;
  
  /* determine the slope and intercept of segment 1 */

  if(segment1->x[1] == segment1->x[0])
  {
    slope1 = INFINITE_SLOPE;
    intercept1 = segment1->x[0];
  }
  else
  {
    slope1 =
      (segment1->y[1] - segment1->y[0]) / (segment1->x[1] - segment1->x[0]);
    if(slope1 == 0.0)
    {
      intercept1 = segment1->y[0];
    }
    else
    {
      intercept1 = segment1->x[0] - segment1->y[0]/slope1;
    }
  }
  
  /* determine the slope and intercept of segment 2 */

  if(segment2->x[1] == segment2->x[0])
  {
    slope2 = INFINITE_SLOPE;
    intercept2 = segment2->x[0];
  }
  else
  {
    slope2 =
      (segment2->y[1] - segment2->y[0]) / (segment2->x[1] - segment2->x[0]);
    if(slope2 == 0.0)
    {
      intercept2 = segment2->y[0];
    }
    else
    {
      intercept2 = segment2->x[0] - segment2->y[0]/slope2;
    }
  }
  
  /*
    Are these segments parallel ?  This brings up the issue of how close
		the floating point numbers should be.  Let us here, assume that they
    need to be exact.  Basically the same operations are performed on both
    sets of numbers, so one would hope they are exact.
    */

  if(slope1 == slope2)
  {

    /* they are parallel */
    
    if(intercept1 == intercept2)
    {
      /* they are colinear, find region of overlap if there is one */
      /* by looking for the nesting of one segment inside of another */
      
      /* the special case of vertical lines is handled differently */
      /* than this next section, which just looks at the x coordinates */
      /* to determine if nesting has occurred */
      
      if(slope1 != INFINITE_SLOPE)
      {
	/* try first endpoint of segment1 */
	
	if(nmmtl_in_seg_range(segment2,segment1->x[0],segment1->y[0]))
	{
	  /* first endpoint of seg1 is within the bounds of seg2 */

	  /* first match - set both intersections to the endpoint */
	  intersection->x = segment1->x[0];
	  intersection->y = segment1->y[0];
	  intersection2->x = segment1->x[0];
	  intersection2->y = segment1->y[0];
	  *colinear = TRUE;
	}
	
	/* try second endpoint of segment1 */
	
	if(nmmtl_in_seg_range(segment2,segment1->x[1],segment1->y[1]))
	{
	  /* second endpoint of seg1 is within the bounds of seg2 */

	  if(*colinear == TRUE)
	  {
	    /* subsequent match, see if it moves either endpoint */
	    if(segment1->x[1] < intersection->x)
	    {
	      intersection->x = segment1->x[1];
	      intersection->y = segment1->y[1];
	    }
	    else if(segment1->x[1] > intersection2->x)
	    {
	      intersection2->x = segment1->x[1];
	      intersection2->y = segment1->y[1];
	    }	 
	  }
	  else
	  {
	    /* first match - set both intersections to the endpoint */
	    intersection->x = segment1->x[1];
	    intersection->y = segment1->y[1];
	    intersection2->x = segment1->x[1];
	    intersection2->y = segment1->y[1];
	    *colinear = TRUE;
	  }
	}
	
	/* try first endpoint of segment2 */
	
	if(nmmtl_in_seg_range(segment1,segment2->x[0],segment2->y[0]))
	{
	  /* first endpoint of seg2 is within the bounds of seg1 */

	  if(*colinear == TRUE)
	  {
	    /* subsequent match, see if it moves either endpoint */
	    if(segment2->x[0] < intersection->x)
	    {
	      intersection->x = segment2->x[0];
	      intersection->y = segment2->y[0];
	    }
	    else if(segment2->x[0] > intersection2->x)
	    {
	      intersection2->x = segment2->x[0];
	      intersection2->y = segment2->y[0];
	    }	 
	  }
	  else
	  {
	    /* first match - set both intersections to the endpoint */
	    intersection->x = segment2->x[0];
	    intersection->y = segment2->y[0];
	    intersection2->x = segment2->x[0];
	    intersection2->y = segment2->y[0];
	    *colinear = TRUE;
	  }
	}
	
	/* try second endpoint of segment2 */
	
	if(nmmtl_in_seg_range(segment1,segment2->x[1],segment2->y[1]))
	{
	  /* second endpoint of seg2 is within the bounds of seg1 */

	  if(*colinear == TRUE)
	  {
	    /* subsequent match, see if it moves either endpoint */
	    if(segment2->x[1] < intersection->x)
	    {
	      intersection->x = segment2->x[1];
	      intersection->y = segment2->y[1];
	    }
	    else if(segment2->x[1] > intersection2->x)
	    {
	      intersection2->x = segment2->x[1];
	      intersection2->y = segment2->y[1];
	    }	 
	  }
	  else
	  {
	    /* first match - set both intersections to the endpoint */
	    intersection->x = segment2->x[1];
	    intersection->y = segment2->y[1];
	    intersection2->x = segment2->x[1];
	    intersection2->y = segment2->y[1];
	    *colinear = TRUE;
	  }
	}
      }
      else
      {
	/* special case of vertical lines */
	
	/* try first endpoint of segment1 */
	
	if(segment1->y[0] >= segment2->y[0] &&
	   segment1->y[0] <= segment2->y[1])
	{
	  /* first endpoint of seg1 is within the bounds of seg2 */
	  
	  /* first match - set both intersections to the endpoint */
	  intersection->x = segment1->x[0];
	  intersection->y = segment1->y[0];
	  intersection2->x = segment1->x[0];
	  intersection2->y = segment1->y[0];
	  *colinear = TRUE;
	}
	
	/* try second endpoint of segment1 */
	
	if(segment1->y[1] >= segment2->y[0] &&
	   segment1->y[1] <= segment2->y[1])
	{
	  /* second endpoint of seg1 is within the bounds of seg2 */
	  
	  if(*colinear == TRUE)
	  {
	    
	    /* subsequent match, see if it moves either endpoint */
	    if(segment1->y[1] < intersection->y)
	    {
	      intersection->x = segment1->x[1];
	      intersection->y = segment1->y[1];
	    }
	    else if(segment1->y[1] > intersection2->y)
	    {
	      intersection2->x = segment1->x[1];
	      intersection2->y = segment1->y[1];
	    }	 
	  }
	  else
	  {
	    /* first match - set both intersections to the endpoint */
	    intersection->x = segment1->x[1];
	    intersection->y = segment1->y[1];
	    intersection2->x = segment1->x[1];
	    intersection2->y = segment1->y[1];
	    *colinear = TRUE;
	  }
	}
	
	/* try first endpoint of segment2 */
	
	if(segment2->y[0] >= segment1->y[0] &&
	   segment2->y[0] <= segment1->y[1] )
	{
	  /* first endpoint of seg1 is within the bounds of seg2 */
	  
	  if(*colinear == TRUE)
	  {
	    /* subsequent match, see if it moves either endpoint */
	    if(segment2->y[0] < intersection->y)
	    {
	      intersection->x = segment2->x[0];
	      intersection->y = segment2->y[0];
	    }
	    else if(segment2->y[0] > intersection2->y)
	    {
	      intersection2->x = segment2->x[0];
	      intersection2->y = segment2->y[0];
	    }	 
	  }
	  else
	  {
	    /* first match - set both intersections to the endpoint */
	    intersection->x = segment2->x[0];
	    intersection->y = segment2->y[0];
	    intersection2->x = segment2->x[0];
	    intersection2->y = segment2->y[0];
	    *colinear = TRUE;
	  }
	}
	
	/* try second endpoint of segment2 */
	
	if(segment2->y[1] >= segment1->y[0] &&
	   segment2->y[1] <= segment1->y[1])
	{
	  /* second endpoint of seg1 is within the bounds of seg2 */
	  
	  if(*colinear == TRUE)
	  {
	    /* subsequent match, see if it moves either endpoint */
	    if(segment2->y[1] < intersection->y)
	    {
	      intersection->x = segment2->x[1];
	      intersection->y = segment2->y[1];
	    }
	    else if(segment2->y[1] > intersection2->y)
	    {
	      intersection2->x = segment2->x[1];
	      intersection2->y = segment2->y[1];
	    }	 
	  }
	  else
	  {
	    /* first match - set both intersections to the endpoint */
	    intersection->x = segment2->x[1];
	    intersection->y = segment2->y[1];
	    intersection2->x = segment2->x[1];
	    intersection2->y = segment2->y[1];
	    *colinear = TRUE;
	  }
	}
      }

      /* if there is no region of overlap, return no-intersection */
      if(*colinear == FALSE)
	return(FALSE);
	
      /* check if a fluke really results in a single point of colinearity */
      if(intersection->x == intersection2->x &&
	 intersection->y == intersection2->y) 
      {
	*colinear = FALSE;
      }
      
      return(TRUE);
    }
    else 
    {
      /* segments are parallel, but intercepts do not match */
      return(FALSE);
    }
  }
  else
  {
    /* non co-linear lines - find the point of intersection, if one */
    
    if(slope1 == INFINITE_SLOPE)
    {
      root_x = segment1->x[0];
      root_y = root_x * slope2 + intercept2;
      /* is root in range ? */
      if(nmmtl_in_seg_range(segment1,root_x,root_y) &&
	 nmmtl_in_seg_range(segment2,root_x,root_y))
      {
	intersection->x = root_x;
	intersection->y = root_y;
	return(TRUE);
      }
    }
    else if(slope2 == INFINITE_SLOPE)
    {
      root_x = segment2->x[0];
      root_y = root_x * slope1 + intercept1;
      /* is root in range ? */
      if(nmmtl_in_seg_range(segment1,root_x,root_y) &&
	 nmmtl_in_seg_range(segment2,root_x,root_y))
      {
	intersection->x = root_x;
	intersection->y = root_y;
	return(TRUE);
      }
    }
    else
    {
      root_x = (intercept1 - intercept2) / (slope1 - slope2);
      root_y = root_x * slope1 + intercept1;

      /* is root in range ? */
      if(nmmtl_in_seg_range(segment1,root_x,root_y) &&
	 nmmtl_in_seg_range(segment2,root_x,root_y))
      {
	intersection->x = root_x;
	intersection->y = root_y;
	return(TRUE);
      }
    }
  }    

  /* failed to find an intersection */
  return(FALSE);
}



/*

  FUNCTION NAME:  nmmtl_in_seg_range


  FUNCTIONAL DESCRIPTION:

  Determine if a point given by an x and y coordinate is within a segment's 
  range, regardless of the order of the segment's points


  FORMAL PARAMETERS:

  LINESEG_P segment
  double x, y

  
  RETURN VALUE:

  TRUE or FALSE


  CALLING SEQUENCE:

  in_range = nmmtl_in_seg_range(segment,x,y);
   
*/

int nmmtl_in_seg_range(LINESEG_P segment,double x,double y)
{
  
  int diff_x_0,diff_x_1,diff_y_0,diff_y_1,diff_x,diff_y;
  double diff;
  

#define DIFF_POS  1
#define DIFF_NEG  2
#define DIFF_ZERO 4
#define DIFF_POS_OR_ZERO (DIFF_POS | DIFF_ZERO)
#define DIFF_NEG_OR_ZERO (DIFF_NEG | DIFF_ZERO)

  diff = x - segment->x[0];
  if(diff < 0) diff_x_0 = DIFF_NEG;
  else if(diff > 0) diff_x_0 = DIFF_POS;
  else diff_x_0 = DIFF_ZERO;
  diff = x - segment->x[1];
  if(diff < 0) diff_x_1 = DIFF_NEG;
  else if(diff > 0) diff_x_1 = DIFF_POS;
  else diff_x_1 = DIFF_ZERO;

  if(diff_x_0 & DIFF_NEG_OR_ZERO && diff_x_1 & DIFF_POS_OR_ZERO)
    diff_x = TRUE;
  else if(diff_x_0 & DIFF_POS_OR_ZERO && diff_x_1 & DIFF_NEG_OR_ZERO)
    diff_x = TRUE;
  else
    diff_x = FALSE;
  if(diff_x == FALSE) return(FALSE);

  diff = y - segment->y[0];
  if(diff < 0) diff_y_0 = DIFF_NEG;
  else if(diff > 0) diff_y_0 = DIFF_POS;
  else diff_y_0 = DIFF_ZERO;
  diff = y - segment->y[1];
  if(diff < 0) diff_y_1 = DIFF_NEG;
  else if(diff > 0) diff_y_1 = DIFF_POS;
  else diff_y_1 = DIFF_ZERO;

  if(diff_y_0 & DIFF_NEG_OR_ZERO && diff_y_1 & DIFF_POS_OR_ZERO)
    diff_y = TRUE;
  else if(diff_y_0 & DIFF_POS_OR_ZERO && diff_y_1 & DIFF_NEG_OR_ZERO)
    diff_y = TRUE;
  else
    diff_y = FALSE;
  if(diff_y == FALSE) return(FALSE);
  else return(TRUE);
    
}
