
/*
  
  FACILITY:  NMMTL
  
  MODULE DESCRIPTION: 
  
  Contains nmmtl_determine_intersections() which finds where
  dielectric segments and conductor line segments intersect.
  
  AUTHOR(S):
  
  Kevin J. Buchs
  
  CREATION DATE:  Tue Jan  7 08:34:47 1992
  
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

/* types of colinear intersections */
#define CL_EXACT   1
#define CL_D0      2
#define CL_D1      3
#define CL_D2      4
#define CL_C0      5
#define CL_C1      6
#define CL_C2      7
#define CL_SD0C0   8
#define CL_SD0C1   9
#define CL_SD1C0  10
#define CL_SD1C1  11

/* types of intersection points for colinear intersections */
#define IP_C0I1 1
#define IP_C0I2 2
#define IP_C1I1 4
#define IP_C1I2 8
#define IP_D0I1 16
#define IP_D0I2 32
#define IP_D1I1 64
#define IP_D1I2 128

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

float nmmtl_find_nu(float epsilon1,float epsilon2,float theta1,float theta2);

/*
 *******************************************************************
 **  FUNCTION DEFINITIONS
 *******************************************************************
 */


/*
  
  FUNCTION NAME:  nmmtl_determine_intersections
  
  
  FUNCTIONAL DESCRIPTION:
  
  Find the intersections between dielectric-dielectric interface
  segments and conductor line segments.  Also see
  nmmtl_determine_arc_intersections.c which does the same thing for
  circular conductor segments.  This determination of intersections will
  result in the following operations upon these segments: fracturing of
  both types of segments into pairs of smaller segments with different
  properties, reducing the side of segments, setting epsilon and
  theta1/nu properties for conductor segments..
  
  You can have one of two types of intersections: point and colinear.
  In latter there is a region over which the two line segments join.
  The point intersection is at a single point.
  
  With the point intersection, there are two special cases: when the
  intersection occurs at the endpoint of either the conductor segment
  or the dielectric segment.
  
  In the case of colinear intersection, there are six special cases.
  The following drawings will illustrate these.  In them, the conductor
  and dielectric segments are taken to be overlapping.
  
  Legend:
  {
  
  cccccccccc  = conductor segment.
  dddddddddd  = dielectric segment.
  
  ^         = point of intersection
  
  
  Description                      Action taken
  ----------------------      ----------------------------------------
  1. exact overlap             conductor segment: no fragmentation, set
  epsilon appropriately.
  ccccccccccc                dielectric segment: remove whole segment
  ddddddddddd
  ^         ^
  
  2. die overhang - 1 side     cond: no fragmentation, set epsilon
  die:  adjust size to overhang only
  ccccccccc
  ddddddddddd
  ^       ^
  
  3. die overhang - 2 sides    cond: no fragmentation, set epsilon
  die:  adjust size to overhang on one side, 
  ccccc                         create new seg for other overhang
  dddddddddddd
  ^   ^
  
  4. cond overhang - 1 side    cond: fragment into 2 pieces, set epsilon on one
  die:  remove whole segment
  cccccccccccc
  dddddddd
  ^      ^
  
  5. cond overhang - 2 sides   cond: fragment into 3 pieces, set epsilon on one.
  die:  remove whole segment
  ccccccccccccccc
  dddddddd
  ^      ^
  
  6. skewed                    cond: fragment into 2 pieces, set epsilon on one.
  die:  adjust size to overhang only
  cccccccccccc
  ddddddddddddd
  ^       ^
  
  There are variations on 2, 4 and 6 for left and right.  There are
  variations on all for different directions of conductor segment and
  dielectric segment.  The following symbolic constants are used to
  represent each situation:
  
  1. CL_EXACT
  2. CL_D0,    (overhang on terminal side of dielectric)
  CL_D1     (overhang on initial side of dielectric)
  3. CL_D2  
  4. CL_C0,    (overhang on terminal side of conductor)
  CL_C1     (overhang on initial side of conductor)
  5. CL_C2
  6. CL_SD0C0, (overhang of terminal side of die and cond)
  CL_SD0C1, (overhang of term die and init cond)
  CL_SD1C0, (overhang of init die and term cond)
  CL_SD1C1  (overhang on initial side of die and cond)
  
  FORMAL PARAMETERS:
  
  LINE_SEGMENTS_P *line_segments          The list of line segments to be
  checked
  DIELECTRIC_SEGMENTS_P *dielectric_segments  The list of dielectric segments
  
  RETURN VALUE:
  
  SUCCESS or FAIL
  
  CALLING SEQUENCE:
  
  */

int nmmtl_determine_intersections(LINE_SEGMENTS_P *line_segments,
				  DIELECTRIC_SEGMENTS_P *dielectric_segments)
{
  LINESEG dseg,cseg;
  int colinear;
  POINT intersection1,intersection2;
  LINE_SEGMENTS_P segment, last_segment, new_ls = NULL, new_ls_2;
  DIELECTRIC_SEGMENTS_P dieseg, last_dieseg, new_ds;
  float theta1,turn_angle;
  float deltax,deltay;
  long int it;  /* intersection type - recordkeeping flag for IP_* */
  long int cl;  /* colinear type - recordkeeping flag for CL_* */
  int vert_cond, vert_die; /* flags indicating that these are vertical 
			      segments, i.e. slope in infinite */
  int cond_inc_dir,die_inc_dir; /* TRUE or FALSE - conductor or dielectric
				   is in increasing direction */
  int inter_inc_dir; /* TRUE or FALSE - intersections (if 2) are in order of
			increasing from intersection1 to intersection2 */
  
  
  /* now detemine the intersections of conductor line segments with */
  /* dielectric-dielectric segments */
  
  segment = *line_segments;
  while(segment != NULL)
  {
    cseg.x[0] = segment->startx;
    cseg.y[0] = segment->starty;
    cseg.x[1] = segment->endx;
    cseg.y[1] = segment->endy;
    if(cseg.x[0] == cseg.x[1])
    {
      vert_cond = TRUE;
      /* is direction increasing or decreasing */
      if(cseg.y[1] > cseg.y[0]) cond_inc_dir = TRUE;
      else cond_inc_dir = FALSE;
    }
    else
    {
      vert_cond = FALSE;
      if(cseg.x[1] > cseg.x[0]) cond_inc_dir = TRUE;
      else cond_inc_dir = FALSE;
    }
    
    dieseg = *dielectric_segments;
    last_dieseg = NULL;
    
    while(dieseg != NULL)
    {
      if(dieseg->orientation == VERTICAL_ORIENTATION)
      {
	dseg.x[0] = dieseg->at;
	dseg.x[1] = dieseg->at;
	dseg.y[0] = dieseg->start;
	dseg.y[1] = dieseg->end;
	vert_die = TRUE;
	if(dseg.y[1] > dseg.y[0]) die_inc_dir = TRUE;
	else die_inc_dir = FALSE;
      }
      else
      {
	dseg.y[0] = dieseg->at;
	dseg.y[1] = dieseg->at;
	dseg.x[0] = dieseg->start;
	dseg.x[1] = dieseg->end;
	vert_die = FALSE; 
	if(dseg.x[1] > dseg.x[0]) die_inc_dir = TRUE;
	else die_inc_dir = FALSE;
      }
      
      if(new_ls != NULL)
      {
	/* segment was changed within dieseg loop - so set up cseg again */
	cseg.x[0] = segment->startx;
	cseg.y[0] = segment->starty;
	cseg.x[1] = segment->endx;
	cseg.y[1] = segment->endy;
	new_ls = NULL;
      }
      
      
      if(nmmtl_seg_seg_inter(&cseg,&dseg,&colinear,
			     &intersection1,&intersection2))
      {
	
	/* CONDUCTOR SEGMENT FIRST */
	
	if(colinear == FALSE)
	{
	  /* single point intersection is what we have
	     
	     does it intersect on one end of the conductor line segment?
	     
	     The dielectric segment is dealt with later.
	     */
	  
	  if(intersection1.x == cseg.x[0] &&
	     intersection1.y == cseg.y[0] )
	  {
	    /* find the proper value of epsilon by seeing if a turn from
	       the dielectric segment into the conductor segment is a
	       positive turn - that would mean use epsilonplus.  */
	    
	    turn_angle = nmmtl_angle_of_intersection(dseg.x[1] - dseg.x[0],
						     dseg.y[1] - dseg.y[0],
						     cseg.x[1] - cseg.x[0],
						     cseg.y[1] - cseg.y[0]);

	    if(turn_angle > 0)
	    {
	      /* if the turn angle is PI, then we have an end-to-end
		 straight line intersection - we can say nothing about
		 the epsilon value */
	      if(turn_angle < PI)
		segment->epsilon[0] = dieseg->epsilonplus;
	      
	      /* now find out if this is the end of a conductor - come to
		 an edge where we need to compute the theta1 angle */
	      if(segment->theta2[0] != 0.0F)
	      {
		if(segment->interior < 0)
		{
		  theta1 = PI - turn_angle;
		}
		else
		{
		  theta1 = turn_angle;
		}
		
		/* compute NU */
		segment->nu[0] = nmmtl_find_nu(dieseg->epsilonplus,
					       dieseg->epsilonminus,
					       theta1,segment->theta2[0]);
		segment->edge_pair[0]->nu[1] = segment->nu[0];
		segment->free_space_nu[0] = PI/segment->theta2[0];
		segment->edge_pair[0]->free_space_nu[1] =
		  segment->free_space_nu[0];
	      }
	    }
	    else /* turn_angle < 0 */
	    {
	      /* if the turn angle is PI, then we have an end-to-end
		 straight line intersection - we can say nothing about
		 the epsilon value */
	      if(turn_angle > -1*PI)
		segment->epsilon[0] = dieseg->epsilonminus;
	      
	      /* now find out if this is the end of a conductor - come to
		 an edge where we need to compute the theta1 angle */
	      if(segment->theta2[0] != 0.0F)
	      {
		if(segment->interior < 0)
		{
		  theta1 = 0 - turn_angle;
		}
		else
		{
		  theta1 = PI + turn_angle;
		}
		
		/* compute NU */
		segment->nu[0] = nmmtl_find_nu(dieseg->epsilonminus,
					       dieseg->epsilonplus,
					       theta1,segment->theta2[0]);
		segment->edge_pair[0]->nu[1] = segment->nu[0];
		segment->free_space_nu[0] = PI/segment->theta2[0];
		segment->edge_pair[0]->free_space_nu[1] =
		  segment->free_space_nu[0];
	      }
	    }	      /* else turn_angle < 0 */
	  }           /* if intersection = cseg 0 */
	  
	  else if(intersection1.x == cseg.x[1] &&
		  intersection1.y == cseg.y[1] )
	  {
	    /* find the proper value of epsilon by seeing if a turn from
	       the dielectric segment into the conductor segment is a
	       positive turn - that would mean use epsilonminus.  */
	    
	    turn_angle = nmmtl_angle_of_intersection(dseg.x[1] - dseg.x[0],
						     dseg.y[1] - dseg.y[0],
						     cseg.x[1] - cseg.x[0],
						     cseg.y[1] - cseg.y[0]);
	    if(turn_angle > 0)
	    {
	      /* if the turn angle is PI, then we have an end-to-end
		 straight line intersection - we can say nothing about
		 the epsilon value */
	      if(turn_angle < PI)
		segment->epsilon[1] = dieseg->epsilonminus;
	      
	      /* now find out if this is the end of a conductor - come to
		 an edge where we need to compute the theta1 angle */
	      if(segment->theta2[1] != 0.0F)
	      {
		if(segment->interior < 0)
		{
		  theta1 = turn_angle;
		}
		else
		{
		  theta1 = PI - turn_angle;
		}
		
		/* compute NU */
		segment->nu[1] = nmmtl_find_nu(dieseg->epsilonminus,
					       dieseg->epsilonplus,
					       theta1,segment->theta2[1]);
		segment->edge_pair[1]->nu[0] = segment->nu[1];
		segment->free_space_nu[1] = PI/segment->theta2[1];
		segment->edge_pair[1]->free_space_nu[0] =
		  segment->free_space_nu[1];
	      }
	    }
	    else /* turn_angle < 0 */
	    {
	      /* if the turn angle is PI, then we have an end-to-end
		 straight line intersection - we can say nothing about
		 the epsilon value */
	      if(turn_angle > -1*PI)
		segment->epsilon[1] = dieseg->epsilonplus;
	      
	      /* now find out if this is the end of a conductor - come to
		 an edge where we need to compute the theta1 angle */
	      if(segment->theta2[1] != 0.0F)
	      {
		if(segment->interior < 0)
		{
		  theta1 = PI + turn_angle;
		}
		else
		{
		  theta1 = 0 - turn_angle;
		}
		
		/* compute NU */
		segment->nu[1] = nmmtl_find_nu(dieseg->epsilonplus,
					       dieseg->epsilonminus,
					       theta1,segment->theta2[1]);
		segment->edge_pair[1]->nu[0] = segment->nu[1];
		segment->free_space_nu[1] = PI/segment->theta2[1];
		segment->edge_pair[1]->free_space_nu[0] =
		  segment->free_space_nu[1];
	      }
	    }	      /* else turn_angle < 0 */
	  }           /* else if insersection at cseg [1] */
	  else 
	  {
	    /* What we have is an intersection somewhere in the middle
	       of the conductor segment.  Proceed normally and fracture
	       the conductor segment.  */
	    
	    /* create new line segment */
	    new_ls = (LINE_SEGMENTS_P) malloc(sizeof(LINE_SEGMENTS));
	    new_ls->interior = segment->interior;
	    new_ls->conductor = segment->conductor;
	    new_ls->epsilon[1] = segment->epsilon[1];
	    
	    /* set up the coordinates and geometry */
	    new_ls->endx = segment->endx;
	    new_ls->endy = segment->endy;
	    new_ls->startx = intersection1.x;
	    new_ls->starty = intersection1.y;
	    segment->endx = intersection1.x;
	    segment->endy = intersection1.y;
	    
	    deltax = new_ls->endx - new_ls->startx;
	    deltay = new_ls->endy - new_ls->starty;
	    new_ls->length = sqrt(deltax*deltax + deltay*deltay);
	    new_ls->divisions = (int)(segment->divisions *
	      (new_ls->length / segment->length) + 1.0);
	    segment->divisions += 1 - new_ls->divisions;
	    segment->length -= new_ls->length;
	    
	    
	    /* hook into the list */
	    new_ls->next = segment->next;
	    segment->next = new_ls;
	    
	    /* interior (i.e. those at the new segment ends created where
	       the dielectric intersected) nu, theta2 and edge pairs are not
	       meaninful so null out. */
	    
	    new_ls->nu[0] = -1.0;
	    new_ls->nu[1] = segment->nu[1];
	    segment->nu[1] = -1.0;
	    
	    new_ls->free_space_nu[0] = -1.0;
	    new_ls->free_space_nu[1] = segment->free_space_nu[1];
	    segment->free_space_nu[1] = -1.0;
	    
	    new_ls->theta2[1] = segment->theta2[1];
	    new_ls->theta2[0] = 0.0;
	    segment->theta2[1] = 0.0;
	    
	    new_ls->edge_pair[1] = segment->edge_pair[1];
	    new_ls->edge_pair[0] = NULL;
	    segment->edge_pair[1] = NULL;
	    
	    /* find the proper value of epsilon by seeing if a turn from
	       the dielectric segment into the conductor segment is a
	       positive turn - that would mean use epsilonplus for the
	       ending half of the conductor segment and epsilonminus for
	       the starting half of the conductor segment.  */
	    
	    if(nmmtl_angle_of_intersection(dseg.x[1] - dseg.x[0],
					   dseg.y[1] - dseg.y[0],
					   cseg.x[1] - cseg.x[0],
					   cseg.y[1] - cseg.y[0]) > 0)
	    {
	      new_ls->epsilon[0] = dieseg->epsilonplus;
	      segment->epsilon[1] = dieseg->epsilonminus;
	    }
	    else
	    {
	      new_ls->epsilon[0] = dieseg->epsilonminus;
	      segment->epsilon[1] = dieseg->epsilonplus;
	    }
	  }                        /* else intersection1 in middle of */
	  /* conductor segment */
	  
	  /*
	    Now we need to work on the dielectric segment that is intersected
	    at a single point by a conductor segment.
	    
	    First, skip this if the intersection is at either end of the
	    dielectric segment.  Check this in two ways: first if not a
	    horizontal segment, then see if the x value of the intersection1
	    matches either of the endpoint x values of the dielectric segment.
	    Otherwise, if it is a vertical segment, then check the y values
	    at the endpoints.
	    */
	  
	  if(
	     (dseg.x[0] != dseg.x[1] &&
	      intersection1.x != dseg.x[0] &&
	      intersection1.x != dseg.x[1])            ||
	     (intersection1.y != dseg.y[0] &&
	      intersection1.y != dseg.y[1]))
	  {
	    /* Create new dielectric segment by splitting the old one. */
	    new_ds = 
	      (DIELECTRIC_SEGMENTS_P)malloc(sizeof(DIELECTRIC_SEGMENTS));
	    new_ds->next = dieseg->next;
	    dieseg->next = new_ds;
	    new_ds->at = dieseg->at;
	    new_ds->end = dieseg->end;
	    if(dieseg->orientation == VERTICAL_ORIENTATION)
	    {
	      new_ds->start = intersection1.y;
	      dieseg->end = intersection1.y;
	    }
	    else
	    {
	      new_ds->start = intersection1.x;
	      dieseg->end = intersection1.x;
	    }
	    
	    new_ds->epsilonplus = dieseg->epsilonplus;
	    new_ds->epsilonminus = dieseg->epsilonminus;
	    new_ds->segment_number = dieseg->segment_number;
	    new_ds->end_in_conductor = dieseg->end_in_conductor;
	    new_ds->orientation = dieseg->orientation;
	    new_ds->length = new_ds->end - new_ds->start;
	    new_ds->divisions = (int)(dieseg->divisions * 
	      (new_ds->length/dieseg->length) + 1.0);
	    dieseg->length -= new_ds->length;
	    dieseg->divisions -= (new_ds->divisions - 1);
	    
	    
	    /*
	      Determine which half of the dielectric segment is inside
	      the conductor by turning the angle from the conductor
	      segment into the dielectric segment.  If the turn is the same
	      direction as the turn towards the interior of the conductor,
	      then the initial portion of the dielectric segment is outside
	      of the conductor.  Otherwise, vise versa.
	      */
	    turn_angle = nmmtl_angle_of_intersection(cseg.x[1] - cseg.x[0],
						     cseg.y[1] - cseg.y[0],
						     dseg.x[1] - dseg.x[0],
						     dseg.y[1] - dseg.y[0]);
	    if((turn_angle > 0 && segment->interior > 0) ||
	       (turn_angle < 0 && segment->interior < 0))
	    {
	      /* initial portion of dielectric segment is outside of the
		 conductor */
	      new_ds->end_in_conductor |= 1;       /* set bit 1 */
	      dieseg->end_in_conductor &= 0XFFFD;  /* clear bit 2 */
	    }
	    else 
	    {
	      /* terminal portion of dielectric segment is outside of the
		 conductor */
	      new_ds->end_in_conductor &= 0XFFFE;  /* clear bit 1 */
	      dieseg->end_in_conductor |= 2;       /* set bit 2 */
	    }
	  }   /* if intersection in middle of dielectric segment */
	  
	}                          /* if the intersection is a point */
	
	else 
	{
	  
	  /* Intersection is a colinear region of overlap.  This gets to be
	     a pain.  First determine what type of overlap situation we have.
	     This is done differently if the segment is vertical.
	     */
	  
	  it = 0;
	  
	  if(vert_cond)
	  {
	    if(intersection1.y == cseg.y[0]) it |= IP_C0I1;
	    if(intersection2.y == cseg.y[0]) it |= IP_C0I2;
	    if(intersection1.y == cseg.y[1]) it |= IP_C1I1;
	    if(intersection2.y == cseg.y[1]) it |= IP_C1I2;
	  }
	  else
	  {
	    if(intersection1.x == cseg.x[0]) it |= IP_C0I1;
	    if(intersection2.x == cseg.x[0]) it |= IP_C0I2;
	    if(intersection1.x == cseg.x[1]) it |= IP_C1I1;
	    if(intersection2.x == cseg.x[1]) it |= IP_C1I2;
	  }
	  if(vert_die)
	  {
	    if(intersection2.y > intersection1.y) inter_inc_dir = TRUE;
	    else inter_inc_dir = FALSE;
	    
	    if(intersection1.y == dseg.y[0]) it |= IP_D0I1;
	    if(intersection2.y == dseg.y[0]) it |= IP_D0I2;
	    if(intersection1.y == dseg.y[1]) it |= IP_D1I1;
	    if(intersection2.y == dseg.y[1]) it |= IP_D1I2;
	  }
	  else
	  {
	    if(intersection2.x > intersection1.x) inter_inc_dir = TRUE;
	    else inter_inc_dir = FALSE;
	    
	    if(intersection1.x == dseg.x[0]) it |= IP_D0I1;
	    if(intersection2.x == dseg.x[0]) it |= IP_D0I2;
	    if(intersection1.x == dseg.x[1]) it |= IP_D1I1;
	    if(intersection2.x == dseg.x[1]) it |= IP_D1I2;
	  }
	  
	  /*
	    Caution: don't be fooled into thinking you can combine the
	    logical tests below.  You cannot combine & and && operators.
	    Don't do it!
	    */
	  
	  if((it & (IP_C0I1 | IP_C0I2)) &&
	     (it & (IP_C1I1 | IP_C1I2)) &&
	     (it & (IP_D0I1 | IP_D0I2)) &&
	     (it & (IP_D1I1 | IP_D1I2))     ) cl = CL_EXACT;
	  
	  else if((it & (IP_C0I1 | IP_C0I2)) &&
		  (it & (IP_C1I1 | IP_C1I2))    )
	  {
	    /* both conductor ends at intersections, which die ends are? */
	    if(it & (IP_D0I1 | IP_D0I2)) cl = CL_D0;
	    else if(it & (IP_D1I1 | IP_D1I2)) cl = CL_D1;
	    else cl = CL_D2;
	  }
	  
	  else if((it & (IP_D0I1 | IP_D0I2)) &&
		  (it & (IP_D1I1 | IP_D1I2))    )
	  {
	    /* both die ends at intersections, which conductor ends are? */
	    if(it & (IP_C0I1 | IP_C0I2)) cl = CL_C0;
	    else if(it & (IP_C1I1 | IP_C1I2)) cl = CL_C1;
	    else cl = CL_C2;
	  }
	  
	  /* skewed - which end of conductor hit and which end of die hit?
	     The other end is overlaping */
	  else if((it & (IP_D0I1 | IP_D0I2)) &&
		  (it & (IP_C0I1 | IP_C0I2))     ) cl = CL_SD0C0;
	  
	  else if((it & (IP_D0I1 | IP_D0I2)) &&
		  (it & (IP_C1I1 | IP_C1I2))     ) cl = CL_SD0C1;
	  
	  else if((it & (IP_D1I1 | IP_D1I2)) &&
		  (it & (IP_C0I1 | IP_C0I2))     ) cl = CL_SD1C0;
	  
	  else if((it & (IP_D1I1 | IP_D1I2)) &&
		  (it & (IP_C1I1 | IP_C1I2))     ) cl = CL_SD1C1;
	  else
	  {
	    fprintf(stderr,"ELECTRO-F-INTERNAL Internal error:  checking intersections between conductors and dielectrics; Choices for colinear intersection types fell through\n");
	    return(FAIL);
	  }
	  
	  
	  /*
	    Now we handle the different type of overlap situations we
	    could encounter.  If you have a better way, let me know.
	    */
	  
	  switch(cl)
	  {
	    
	  case CL_EXACT:
	    
	    /* conductor segment action: no fragmentation,
	       set epsilon appropriately
	       
	       Find the proper value of epsilon by seeing if
	       the dielectric segment and the conductor segment go in
	       the same direction.  If they do, and the interior of the
	       conductor is in the positive direction, we want epsilon on
	       the other side, which is epsilonminus.  If segments are in
	       opposite direction then it reverses.
	       */
	    if(cond_inc_dir == die_inc_dir)
	    {
	      /* same direction */
	      if(segment->interior > 0)
	      {
		/* inside of conductor is a positive turn - same side that
		   epsilonplus is on, so use epsilonminus */
		segment->epsilon[0] = dieseg->epsilonminus;
		segment->epsilon[1] = dieseg->epsilonminus;
	      }
	      else
	      {
		segment->epsilon[0] = dieseg->epsilonplus;
		segment->epsilon[1] = dieseg->epsilonplus;
	      }
	    }
	    else
	    {
	      /* opposite direction */
	      if(segment->interior > 0)
	      {
		/* inside of conductor is a positive turn - same side that
		   epsilonminus is on (since they are in opposite directions,
		   so use epsilonplus */
		segment->epsilon[0] = dieseg->epsilonplus;
		segment->epsilon[1] = dieseg->epsilonplus;
	      }
	      else
	      {
		segment->epsilon[0] = dieseg->epsilonminus;
		segment->epsilon[1] = dieseg->epsilonminus;
	      }
	    }
	    /* dielectric segment action: remove whole segment
	       
	       To do this, we are slightly tricky.  The end of the
	       dielectric loop has these statements:
	       
	       last_dieseg = dieseg;
	       dieseg = dieseg->next;
	       
	       To play into this correctly, we just set the value of
	       dieseg back to last_dieseg.  This will allow last_dieseg to
	       remain unchanged, but dieseg will advance to the next one,
	       which is the one after the one we removed.
	       
	       */
	    /* is this not the first element on the list ? */
	    if(last_dieseg != NULL)
	    {
	      last_dieseg->next = dieseg->next; /* bypass on list */
	      free(dieseg);
	      dieseg = last_dieseg;
	    }
	    else /* first element on the list - do special operations */
	    {
	      last_dieseg = dieseg->next; /* bypass on list */
	      free(dieseg);
	      dieseg = NULL;
	    }
	    
	    break;
	    
	  case CL_D0:
	    
	    /* conductor segment action: no fragmentation, set epsilon
	       
	       Find the proper value of epsilon by seeing if
	       the dielectric segment and the conductor segment go in
	       the same direction.  If they do, and the interior of the
	       conductor is in the positive direction, we want epsilon on
	       the other side, which is epsilonminus.  If segments are in
	       opposite direction then it reverses.
	       */
	    if(cond_inc_dir == die_inc_dir)
	    {
	      /* same direction */
	      if(segment->interior > 0)
	      {
		/* inside of conductor is a positive turn - same side that
		   epsilonplus is on, so use epsilonminus */
		segment->epsilon[0] = dieseg->epsilonminus;
		segment->epsilon[1] = dieseg->epsilonminus;
	      }
	      else
	      {
		segment->epsilon[0] = dieseg->epsilonplus;
		segment->epsilon[1] = dieseg->epsilonplus;
	      }
	    }
	    else
	    {
	      /* opposite direction */
	      if(segment->interior > 0)
	      {
		/* inside of conductor is a positive turn - same side that
		   epsilonminus is on (since they are in opposite directions,
		   so use epsilonplus */
		segment->epsilon[0] = dieseg->epsilonplus;
		segment->epsilon[1] = dieseg->epsilonplus;
	      }
	      else
	      {
		segment->epsilon[0] = dieseg->epsilonminus;
		segment->epsilon[1] = dieseg->epsilonminus;
	      }
	    }
	    
	    /* dielectric segment action: adjust size to overhang only
	       
	       Overhang is on terminal side of dielectric - so we leave 
	       the end alone and adjust the start to the intersection.
	       
	       First, determine which intersection point hit on initial side
	       of die seg.  Take the other intersection point as the start of
	       the overhang.
	       */
	    if(it & IP_D0I1)
	    {
	      /* intersection2 determines the start of the overhang */
	      if(vert_die) dieseg->start = intersection2.y;
	      else dieseg->start = intersection2.x;
	      /* adjust divisions and set new length */
	      dieseg->divisions = (int)(dieseg->divisions *
		(dieseg->end - dieseg->start)/dieseg->length + 1.0);
	      dieseg->length = dieseg->end - dieseg->start;
	    }
	    else /* it & IP_D0I2 should be true */
	    {
	      /* intersection1 determines the start of the overhang */
	      if(vert_die) dieseg->start = intersection1.y;
	      else dieseg->start = intersection1.x;
	      /* adjust divisions and set new length */
	      dieseg->divisions = (int)(dieseg->divisions *
		(dieseg->end - dieseg->start)/dieseg->length + 1.0);
	      dieseg->length = dieseg->end - dieseg->start;
	    }
	    
	    break;
	    
	  case CL_D1:
	    
	    /* conductor segment action: no fragmentation, set epsilon
	       Find the proper value of epsilon by seeing if
	       the dielectric segment and the conductor segment go in
	       the same direction.  If they do, and the interior of the
	       conductor is in the positive direction, we want epsilon on
	       the other side, which is epsilonminus.  If segments are in
	       opposite direction then it reverses.
	       */
	    if(cond_inc_dir == die_inc_dir)
	    {
	      /* same direction */
	      if(segment->interior > 0)
	      {
		/* inside of conductor is a positive turn - same side that
		   epsilonplus is on, so use epsilonminus */
		segment->epsilon[0] = dieseg->epsilonminus;
		segment->epsilon[1] = dieseg->epsilonminus;
	      }
	      else
	      {
		segment->epsilon[0] = dieseg->epsilonplus;
		segment->epsilon[1] = dieseg->epsilonplus;
	      }
	    }
	    else
	    {
	      /* opposite direction */
	      if(segment->interior > 0)
	      {
		/* inside of conductor is a positive turn - same side that
		   epsilonminus is on (since they are in opposite directions,
		   so use epsilonplus */
		segment->epsilon[0] = dieseg->epsilonplus;
		segment->epsilon[1] = dieseg->epsilonplus;
	      }
	      else
	      {
		segment->epsilon[0] = dieseg->epsilonminus;
		segment->epsilon[1] = dieseg->epsilonminus;
	      }
	    }
	    
	    /* dielectric segment action: adjust size to overhang only
	       
	       Overhang is on initial side of dielectric - so we leave 
	       the start alone and adjust the end to the intersection.
	       
	       First, determine which intersection point hit on terminal side
	       of die seg.  Take the other intersection point as the end of
	       the overhang.
	       */
	    if(it & IP_D1I1)
	    {
	      /* intersection2 determines the start of the overhang */
	      if(vert_die) dieseg->end = intersection2.y;
	      else dieseg->end = intersection2.x;
	      /* adjust divisions and set new length */
	      dieseg->divisions = (int)(dieseg->divisions *
		(dieseg->end - dieseg->start)/dieseg->length + 1.0);
	      dieseg->length = dieseg->end - dieseg->start;
	    }
	    else /* it & IP_D1I2 should be true */
	    {
	      /* intersection1 determines the start of the overhang */
	      if(vert_die) dieseg->end = intersection1.y;
	      else dieseg->end = intersection1.x;
	      /* adjust divisions and set new length */
	      dieseg->divisions = (int)(dieseg->divisions *
		(dieseg->end - dieseg->start)/dieseg->length + 1.0);
	      dieseg->length = dieseg->end - dieseg->start;
	    }
	    
	    break;
	    
	  case CL_D2:
	    
	    /* conductor segment action: no fragmentation, set epsilon
	       Find the proper value of epsilon by seeing if
	       the dielectric segment and the conductor segment go in
	       the same direction.  If they do, and the interior of the
	       conductor is in the positive direction, we want epsilon on
	       the other side, which is epsilonminus.  If segments are in
	       opposite direction then it reverses.
	       */
	    if(cond_inc_dir == die_inc_dir)
	    {
	      /* same direction */
	      if(segment->interior > 0)
	      {
		/* inside of conductor is a positive turn - same side that
		   epsilonplus is on, so use epsilonminus */
		segment->epsilon[0] = dieseg->epsilonminus;
		segment->epsilon[1] = dieseg->epsilonminus;
	      }
	      else
	      {
		segment->epsilon[0] = dieseg->epsilonplus;
		segment->epsilon[1] = dieseg->epsilonplus;
	      }
	    }
	    else
	    {
	      /* opposite direction */
	      if(segment->interior > 0)
	      {
		/* inside of conductor is a positive turn - same side that
		   epsilonminus is on (since they are in opposite directions,
		   so use epsilonplus */
		segment->epsilon[0] = dieseg->epsilonplus;
		segment->epsilon[1] = dieseg->epsilonplus;
	      }
	      else
	      {
		segment->epsilon[0] = dieseg->epsilonminus;
		segment->epsilon[1] = dieseg->epsilonminus;
	      }
	    }
	    
	    /* dielectric segment action: adjust size to overhang on one side, 
	       create new seg for other overhang.
	       
	       First create the new segment and copy relevant parts
	       */
	    
	    new_ds = (DIELECTRIC_SEGMENTS_P)
	      malloc(sizeof(DIELECTRIC_SEGMENTS));
	    new_ds->next = dieseg->next;
	    dieseg->next = new_ds;
	    new_ds->at = dieseg->at;
	    new_ds->end = dieseg->end;
	    new_ds->epsilonplus = dieseg->epsilonplus;
	    new_ds->epsilonminus = dieseg->epsilonminus;
	    new_ds->segment_number = dieseg->segment_number;
	    new_ds->orientation = dieseg->orientation;
	    /* clear initial end_in_conductor bit - bit 0 */
	    new_ds->end_in_conductor = dieseg->end_in_conductor & 2;
	    /* clear terminal end_in_conductor bit - bit 1 */
	    dieseg->end_in_conductor &= 1;
	    
	    /*
	      Now the harsh reality hits: we must determine which intersection
	      creates which part of the overhang.  The variable die_inc_dir
	      will tell us if the die seg is in the increasing direction.
	      The variable inter_inc_dir tell us if the intersections are
	      increasing from intersection1 to intersection2.  If both of these
	      match, then intersection1 determines the end of the initial
	      overhang and intersection2 determines the end of the terminal
	      overhang.
	      
	      */
	    
	    if(die_inc_dir == inter_inc_dir)
	    {
	      if(vert_die)
	      {
		dieseg->end = intersection1.y;
		new_ds->start = intersection2.y;
	      }
	      else
	      {
		dieseg->end = intersection1.x;
		new_ds->start = intersection2.x;
	      }	      
	    }
	    else
	    {
	      if(vert_die)
	      {
		dieseg->end = intersection2.y;
		new_ds->start = intersection1.y;
	      }
	      else
	      {
		dieseg->end = intersection2.x;
		new_ds->start = intersection1.x;
	      }
	    }
	    
	    new_ds->length = new_ds->end - new_ds->start;
	    new_ds->divisions = (int)(dieseg->divisions *
	      (new_ds->length/dieseg->length) + 1.0);
	    dieseg->divisions = (int)(dieseg->divisions *
	      ((dieseg->end - dieseg->start)/dieseg->length) + 1.0);
	    dieseg->length = dieseg->end - dieseg->start;
	    
	    break;
	    
	  case CL_C0:	
	    
	    /* conductor segment action: fragment into 2 pieces,
	       set epsilon on one
	       
	       Create new line segment.
	       */
	    new_ls = (LINE_SEGMENTS_P) malloc(sizeof(LINE_SEGMENTS));
	    new_ls->interior = segment->interior;
	    new_ls->conductor = segment->conductor;
	    
	    /* set up the coordinates and geometry */
	    new_ls->endx = segment->endx;
	    new_ls->endy = segment->endy;
	    
	    /*
	      Now, determine which intersection point hit on initial side
	      of cond seg.  Take the other intersection point as the start of
	      the overhang.
	      */
	    if(it & IP_C0I2)
	    {
	      /* intersection1 determines the start of the overhang */
	      new_ls->startx = intersection1.x;
	      new_ls->starty = intersection1.y;
	      segment->endx = intersection1.x;
	      segment->endy = intersection1.y;
	    }
	    else /* it & IP_COI1 should be true */
	    {
	      /* intersection2 determines the start of the overhang */
	      new_ls->startx = intersection2.x;
	      new_ls->starty = intersection2.y;
	      segment->endx = intersection2.x;
	      segment->endy = intersection2.y;
	    }
	    
	    
	    deltax = new_ls->endx - new_ls->startx;
	    deltay = new_ls->endy - new_ls->starty;
	    new_ls->length = sqrt(deltax*deltax + deltay*deltay);
	    new_ls->divisions = (int)(segment->divisions *
	      (new_ls->length / segment->length) + 1.0);
	    segment->divisions += 1 - new_ls->divisions;
	    segment->length -= new_ls->length;
	    
	    
	    /* hook into the list */
	    new_ls->next = segment->next;
	    segment->next = new_ls;
	    
	    /* interior (i.e. those at the new segment ends created where
	       the dielectric intersected) nu, theta2 and edge pairs are not
	       meaninful so null out. */
	    
	    new_ls->nu[0] = -1.0;
	    new_ls->nu[1] = segment->nu[1];
	    segment->nu[1] = -1.0;
	    
	    new_ls->free_space_nu[0] = -1.0;
	    new_ls->free_space_nu[1] = segment->free_space_nu[1];
	    segment->free_space_nu[1] = -1.0;
	    
	    new_ls->theta2[1] = segment->theta2[1];
	    new_ls->theta2[0] = 0.0;
	    segment->theta2[1] = 0.0;
	    
	    new_ls->edge_pair[1] = segment->edge_pair[1];
	    new_ls->edge_pair[0] = NULL;
	    segment->edge_pair[1] = NULL;
	    
	    /* Epsilon values for the overhang piece are not further
	       determined. */
	    
	    new_ls->epsilon[1] = segment->epsilon[1];
	    new_ls->epsilon[0] = 0.0;
	    
	    /*
	      Find the proper value of epsilon on overlap piece by seeing if
	      the dielectric segment and the conductor segment go in
	      the same direction.  If they do, and the interior of the
	      conductor is in the positive direction, we want epsilon on
	      the other side, which is epsilonminus.  If segments are in
	      opposite direction then it reverses.
	      */
	    
	    if(cond_inc_dir == die_inc_dir)
	    {
	      /* same direction */
	      if(segment->interior > 0)
	      {
		/* inside of conductor is a positive turn - same side that
		   epsilonplus is on, so use epsilonminus */
		segment->epsilon[0] = dieseg->epsilonminus;
		segment->epsilon[1] = dieseg->epsilonminus;
	      }
	      else
	      {
		segment->epsilon[0] = dieseg->epsilonplus;
		segment->epsilon[1] = dieseg->epsilonplus;
	      }
	    }
	    else
	    {
	      /* opposite direction */
	      if(segment->interior > 0)
	      {
		/* inside of conductor is a positive turn - same side that
		   epsilonminus is on (since they are in opposite directions,
		   so use epsilonplus */
		segment->epsilon[0] = dieseg->epsilonplus;
		segment->epsilon[1] = dieseg->epsilonplus;
	      }
	      else
	      {
		segment->epsilon[0] = dieseg->epsilonminus;
		segment->epsilon[1] = dieseg->epsilonminus;
	      }
	    }
	    
	    /* dielectric segment action: remove whole segment
	       
	       To do this, we are slightly tricky.  The end of the
	       dielectric loop has these statements:
	       
	       last_dieseg = dieseg;
	       dieseg = dieseg->next;
	       
	       To play into this correctly, we just set the value of
	       dieseg back to last_dieseg.  This will allow last_dieseg to
	       remain unchanged, but dieseg will advance to the next one,
	       which is the one after the one we removed.
	       
	       */
	    /* is this not the first element on the list ? */
	    if(last_dieseg != NULL)
	    {
	      last_dieseg->next = dieseg->next; /* bypass on list */
	      free(dieseg);
	      dieseg = last_dieseg;
	    }
	    else /* first element on the list - do special operations */
	    {
	      last_dieseg = dieseg->next; /* bypass on list */
	      free(dieseg);
	      dieseg = NULL;
	    }
	    break;
	    
	  case CL_C1:	
	    
	    /* conductor segment action: fragment into 2 pieces,
	       set epsilon on one
	       
	       Create new line segment.
	       */
	    new_ls = (LINE_SEGMENTS_P) malloc(sizeof(LINE_SEGMENTS));
	    new_ls->interior = segment->interior;
	    new_ls->conductor = segment->conductor;
	    
	    /* set up the coordinates and geometry */
	    new_ls->endx = segment->endx;
	    new_ls->endy = segment->endy;
	    
	    /*
	      Now, determine which intersection point hit on initial side
	      of cond seg.  Take the other intersection point as the start of
	      the overhang.
	      */
	    if(it & IP_C1I2)
	    {
	      /* intersection1 determines the start of the overhang */
	      new_ls->startx = intersection1.x;
	      new_ls->starty = intersection1.y;
	      segment->endx = intersection1.x;
	      segment->endy = intersection1.y;
	    }
	    else /* it & IP_C1I1 should be true */
	    {
	      /* intersection2 determines the start of the overhang */
	      new_ls->startx = intersection2.x;
	      new_ls->starty = intersection2.y;
	      segment->endx = intersection2.x;
	      segment->endy = intersection2.y;
	    }
	    
	    
	    deltax = new_ls->endx - new_ls->startx;
	    deltay = new_ls->endy - new_ls->starty;
	    new_ls->length = sqrt(deltax*deltax + deltay*deltay);
 	    new_ls->divisions = (int)(segment->divisions *
 	      (new_ls->length / segment->length) + 1.0);
	    segment->divisions += 1 - new_ls->divisions;
	    segment->length -= new_ls->length;
	    
	    
	    /* hook into the list */
	    new_ls->next = segment->next;
	    segment->next = new_ls;
	    
	    /* interior (i.e. those at the new segment ends created where
	       the dielectric intersected) nu, theta2 and edge pairs are not
	       meaninful so null out. */
	    
	    new_ls->nu[0] = -1.0;
	    new_ls->nu[1] = segment->nu[1];
	    segment->nu[1] = -1.0;
	    
	    new_ls->free_space_nu[0] = -1.0;
	    new_ls->free_space_nu[1] = segment->free_space_nu[1];
	    segment->free_space_nu[1] = -1.0;
	    
	    new_ls->theta2[1] = segment->theta2[1];
	    new_ls->theta2[0] = 0.0;
	    segment->theta2[1] = 0.0;
	    
	    new_ls->edge_pair[1] = segment->edge_pair[1];
	    new_ls->edge_pair[0] = NULL;
	    segment->edge_pair[1] = NULL;
	    
	    /* Epsilon values for the overhang piece are not further
	       determined. */
	    
	    segment->epsilon[1] = 0.0;
	    
	    /*
	      Find the proper value of epsilon on overlap piece by seeing if
	      the dielectric segment and the conductor segment go in
	      the same direction.  If they do, and the interior of the
	      conductor is in the positive direction, we want epsilon on
	      the other side, which is epsilonminus.  If segments are in
	      opposite direction then it reverses.
	      */
	    
	    if(cond_inc_dir == die_inc_dir)
	    {
	      /* same direction */
	      if(new_ls->interior > 0)
	      {
		/* inside of conductor is a positive turn - same side that
		   epsilonplus is on, so use epsilonminus */
		new_ls->epsilon[0] = dieseg->epsilonminus;
		new_ls->epsilon[1] = dieseg->epsilonminus;
	      }
	      else
	      {
		new_ls->epsilon[0] = dieseg->epsilonplus;
		new_ls->epsilon[1] = dieseg->epsilonplus;
	      }
	    }
	    else
	    {
	      /* opposite direction */
	      if(new_ls->interior > 0)
	      {
		/* inside of conductor is a positive turn - same side that
		   epsilonminus is on (since they are in opposite directions,
		   so use epsilonplus */
		new_ls->epsilon[0] = dieseg->epsilonplus;
		new_ls->epsilon[1] = dieseg->epsilonplus;
	      }
	      else
	      {
		new_ls->epsilon[0] = dieseg->epsilonminus;
		new_ls->epsilon[1] = dieseg->epsilonminus;
	      }
	    }
	    
	    
	    /* dielectric segment action: remove whole segment
	       
	       To do this, we are slightly tricky.  The end of the
	       dielectric loop has these statements:
	       
	       last_dieseg = dieseg;
	       dieseg = dieseg->next;
	       
	       To play into this correctly, we just set the value of
	       dieseg back to last_dieseg.  This will allow last_dieseg to
	       remain unchanged, but dieseg will advance to the next one,
	       which is the one after the one we removed.
	       
	       */
	    /* is this not the first element on the list ? */
	    if(last_dieseg != NULL)
	    {
	      last_dieseg->next = dieseg->next; /* bypass on list */
	      free(dieseg);
	      dieseg = last_dieseg;
	    }
	    else /* first element on the list - do special operations */
	    {
	      last_dieseg = dieseg->next; /* bypass on list */
	      free(dieseg);
	      dieseg = NULL;
	    }
	    break;
	    
	  case CL_C2:
	    
	    /* conductor segment action: fragment into 3 pieces,
	       set epsilon on one.
	       
	       We need to create two new line segments.  We use new_ls for
	       the middle one, and, new_ls_2 for the terminal one.
	       */
	    
	    new_ls = (LINE_SEGMENTS_P) malloc(sizeof(LINE_SEGMENTS));
	    new_ls_2 = (LINE_SEGMENTS_P) malloc(sizeof(LINE_SEGMENTS));
	    new_ls->interior = segment->interior;
	    new_ls_2->interior = segment->interior;
	    new_ls->conductor = segment->conductor;
	    new_ls_2->conductor = segment->conductor;
	    
	    /* set up the coordinates and geometry */
	    new_ls_2->endx = segment->endx;
	    new_ls_2->endy = segment->endy;
	    
	    /*
	      Now, determine which intersection point hit on initial side
	      of cond seg.  Take the other intersection1 point as the end of
	      the first piece of overhang.  Intersection2 will be the start
	      of the second piece of overhang.
	      */
	    if(cond_inc_dir == inter_inc_dir)
	    {
	      /* intersection1 determines the end of segment, start of new_ls,
		 the end of the first overhang */
	      segment->endx = intersection1.x;
	      segment->endy = intersection1.y;
	      new_ls->startx = intersection1.x;
	      new_ls->starty = intersection1.y;
	      new_ls->endx = intersection2.x;
	      new_ls->endy = intersection2.y;
	      new_ls_2->startx = intersection2.x;
	      new_ls_2->starty = intersection2.y;
	    }
	    else
	    {
	      /* intersection2 determines the end of segment, start of new_ls,
		 the end of the first overhang */
	      segment->endx = intersection2.x;
	      segment->endy = intersection2.y;
	      new_ls->startx = intersection2.x;
	      new_ls->starty = intersection2.y;
	      new_ls->endx = intersection1.x;
	      new_ls->endy = intersection1.y;
	      new_ls_2->startx = intersection1.x;
	      new_ls_2->starty = intersection1.y;
	    }
	    
	    
            deltax = new_ls->endx - new_ls->startx;
	    deltay = new_ls->endy - new_ls->starty;
	    new_ls->length = sqrt(deltax*deltax + deltay*deltay);
	    new_ls->divisions = (int)(segment->divisions *
	      (new_ls->length / segment->length) + 1.0);
	    deltax = new_ls->endx - new_ls_2->startx;
	    deltay = new_ls->endy - new_ls_2->starty;
	    new_ls_2->length = sqrt(deltax*deltax + deltay*deltay);
	    new_ls_2->divisions = (int)(segment->divisions *
	      (new_ls_2->length / segment->length) + 1.0);
	    
	    segment->divisions = (int)(segment->divisions *
	      (sqrt(deltax*deltax + deltay*deltay) / segment->length) + 1.0);
	    segment->length = sqrt(deltax*deltax + deltay*deltay);
	    
	    
	    /* hook into the list */
	    new_ls_2->next = segment->next;
	    new_ls->next = new_ls_2;
	    segment->next = new_ls;
	    
	    /* interior (i.e. those at the new segment ends created where
	       the dielectric intersected) nu, theta2 and edge pairs are not
	       meaninful so null out. */
	    
	    new_ls->nu[0] = -1.0;
	    new_ls->nu[1] = -1.0;
	    new_ls_2->nu[0] = -1.0;
	    new_ls_2->nu[1] = segment->nu[1];
	    segment->nu[1] = -1.0;
	    
	    new_ls->free_space_nu[0] = -1.0;
	    new_ls->free_space_nu[1] = -1.0;
	    new_ls_2->free_space_nu[0] = -1.0;
	    new_ls_2->free_space_nu[1] = segment->free_space_nu[1];
	    segment->free_space_nu[1] = -1.0;
	    
	    new_ls->theta2[1] = 0.0;
	    new_ls->theta2[0] = 0.0;
	    new_ls_2->theta2[1] = segment->theta2[1];
	    new_ls_2->theta2[0] = 0.0;
	    segment->theta2[1] = 0.0;
	    
	    new_ls->edge_pair[1] = NULL;
	    new_ls->edge_pair[0] = NULL;
	    new_ls_2->edge_pair[1] = segment->edge_pair[1];
	    new_ls_2->edge_pair[0] = NULL;
	    segment->edge_pair[1] = NULL;
	    
	    /* Epsilon values for the overhang piece are not further
	       determined. */
	    
	    new_ls_2->epsilon[0] = 0.0;
	    new_ls_2->epsilon[1] = segment->epsilon[1];
	    segment->epsilon[1] = 0.0;
	    
	    /*
	      Find the proper value of epsilon on overlap piece by seeing if
	      the dielectric segment and the conductor segment go in
	      the same direction.  If they do, and the interior of the
	      conductor is in the positive direction, we want epsilon on
	      the other side, which is epsilonminus.  If segments are in
	      opposite direction then it reverses.
	      */
	    
	    if(cond_inc_dir == die_inc_dir)
	    {
	      /* same direction */
	      if(new_ls->interior > 0)
	      {
		/* inside of conductor is a positive turn - same side that
		   epsilonplus is on, so use epsilonminus */
		new_ls->epsilon[0] = dieseg->epsilonminus;
		new_ls->epsilon[1] = dieseg->epsilonminus;
	      }
	      else
	      {
		new_ls->epsilon[0] = dieseg->epsilonplus;
		new_ls->epsilon[1] = dieseg->epsilonplus;
	      }
	    }
	    else
	    {
	      /* opposite direction */
	      if(new_ls->interior > 0)
	      {
		/* inside of conductor is a positive turn - same side that
		   epsilonminus is on (since they are in opposite directions,
		   so use epsilonplus */
		new_ls->epsilon[0] = dieseg->epsilonplus;
		new_ls->epsilon[1] = dieseg->epsilonplus;
	      }
	      else
	      {
		new_ls->epsilon[0] = dieseg->epsilonminus;
		new_ls->epsilon[1] = dieseg->epsilonminus;
	      }
	    }
	    
	    
	    /* dielectric segment action: remove whole segment
	       
	       To do this, we are slightly tricky.  The end of the
	       dielectric loop has these statements:
	       
	       last_dieseg = dieseg;
	       dieseg = dieseg->next;
	       
	       To play into this correctly, we just set the value of
	       dieseg back to last_dieseg.  This will allow last_dieseg to
	       remain unchanged, but dieseg will advance to the next one,
	       which is the one after the one we removed.
	       
	       */
	    /* is this not the first element on the list ? */
	    if(last_dieseg != NULL)
	    {
	      last_dieseg->next = dieseg->next; /* bypass on list */
	      free(dieseg);
	      dieseg = last_dieseg;
	    }
	    else /* first element on the list - do special operations */
	    {
	      last_dieseg = dieseg->next; /* bypass on list */
	      free(dieseg);
	      dieseg = NULL;
	    }
	    break;
	    
	  case CL_SD0C0:
	    
	    /* conductor segment action: fragment into 2 pieces,
	       set epsilon on one.
	       
	       
	       Create new line segment.
	       */
	    new_ls = (LINE_SEGMENTS_P) malloc(sizeof(LINE_SEGMENTS));
	    new_ls->interior = segment->interior;
	    new_ls->conductor = segment->conductor;
	    
	    /* set up the coordinates and geometry */
	    new_ls->endx = segment->endx;
	    new_ls->endy = segment->endy;
	    
	    /*
	      Now, determine which intersection point hit on initial side
	      of cond seg.  Take the other intersection point as the start of
	      the overhang.
	      */
	    if(it & IP_C0I2)
	    {
	      /* intersection1 determines the start of the overhang */
	      new_ls->startx = intersection1.x;
	      new_ls->starty = intersection1.y;
	      segment->endx = intersection1.x;
	      segment->endy = intersection1.y;
	    }
	    else /* it & IP_C0I1 should be true */
	    {
	      /* intersection2 determines the start of the overhang */
	      new_ls->startx = intersection2.x;
	      new_ls->starty = intersection2.y;
	      segment->endx = intersection2.x;
	      segment->endy = intersection2.y;
	    }
	    
	    
	    deltax = new_ls->endx - new_ls->startx;
	    deltay = new_ls->endy - new_ls->starty;
	    new_ls->length = sqrt(deltax*deltax + deltay*deltay);
	    new_ls->divisions = (int)(segment->divisions *
	      (new_ls->length / segment->length) + 1.0);
	    segment->divisions += 1 - new_ls->divisions;
	    segment->length -= new_ls->length;
	    
	    
	    /* hook into the list */
	    new_ls->next = segment->next;
	    segment->next = new_ls;
	    
	    /* interior (i.e. those at the new segment ends created where
	       the dielectric intersected) nu, theta2 and edge pairs are not
	       meaninful so null out. */
	    
	    new_ls->nu[0] = -1.0;
	    new_ls->nu[1] = segment->nu[1];
	    segment->nu[1] = -1.0;
	    
	    new_ls->free_space_nu[0] = -1.0;
	    new_ls->free_space_nu[1] = segment->free_space_nu[1];
	    segment->free_space_nu[1] = -1.0;
	    
	    new_ls->theta2[1] = segment->theta2[1];
	    new_ls->theta2[0] = 0.0;
	    segment->theta2[1] = 0.0;
	    
	    new_ls->edge_pair[1] = segment->edge_pair[1];
	    new_ls->edge_pair[0] = NULL;
	    segment->edge_pair[1] = NULL;
	    
	    /* Epsilon values for the overhang piece are not further
	       determined. */
	    
	    new_ls->epsilon[0] = 0.0;
	    new_ls->epsilon[1] = segment->epsilon[1];
	    
	    /*
	      Find the proper value of epsilon on overlap piece by seeing if
	      the dielectric segment and the conductor segment go in
	      the same direction.  If they do, and the interior of the
	      conductor is in the positive direction, we want epsilon on
	      the other side, which is epsilonminus.  If segments are in
	      opposite direction then it reverses.
	      */
	    
	    if(cond_inc_dir == die_inc_dir)
	    {
	      /* same direction */
	      if(segment->interior > 0)
	      {
		/* inside of conductor is a positive turn - same side that
		   epsilonplus is on, so use epsilonminus */
		segment->epsilon[0] = dieseg->epsilonminus;
		segment->epsilon[1] = dieseg->epsilonminus;
	      }
	      else
	      {
		segment->epsilon[0] = dieseg->epsilonplus;
		segment->epsilon[1] = dieseg->epsilonplus;
	      }
	    }
	    else
	    {
	      /* opposite direction */
	      if(segment->interior > 0)
	      {
		/* inside of conductor is a positive turn - same side that
		   epsilonminus is on (since they are in opposite directions,
		   so use epsilonplus */
		segment->epsilon[0] = dieseg->epsilonplus;
		segment->epsilon[1] = dieseg->epsilonplus;
	      }
	      else
	      {
		segment->epsilon[0] = dieseg->epsilonminus;
		segment->epsilon[1] = dieseg->epsilonminus;
	      }
	    }
	    
	    /* dielectric segment action: adjust size to overhang only
	       
	       Overhang is on terminal side of dielectric - so we leave 
	       the end alone and adjust the start to the intersection.
	       
	       First, determine which intersection point hit on initial side
	       of die seg.  Take the other intersection point as the start of
	       the overhang.
	       */
	    if(it & IP_D0I1)
	    {
	      /* intersection2 determines the start of the overhang */
	      if(vert_die) dieseg->start = intersection2.y;
	      else dieseg->start = intersection2.x;
	      /* adjust divisions and set new length */
	      dieseg->divisions = (int)(dieseg->divisions *
		(dieseg->end - dieseg->start)/dieseg->length + 1.0);
	      dieseg->length = dieseg->end - dieseg->start;
	    }
	    else /* it & IP_D0I2 should be true */
	    {
	      /* intersection1 determines the start of the overhang */
	      if(vert_die) dieseg->start = intersection1.y;
	      else dieseg->start = intersection1.x;
	      /* adjust divisions and set new length */
	      dieseg->divisions = (int)(dieseg->divisions *
		(dieseg->end - dieseg->start)/dieseg->length + 1.0);
	      dieseg->length = dieseg->end - dieseg->start;
	    }
	    
	    break;
	    
	  case CL_SD0C1:
	    
	    /* conductor segment action: fragment into 2 pieces,
	       set epsilon on one.
	       
	       
	       Create new line segment.
	       */
	    new_ls = (LINE_SEGMENTS_P) malloc(sizeof(LINE_SEGMENTS));
	    new_ls->interior = segment->interior;
	    new_ls->conductor = segment->conductor;
	    
	    /* set up the coordinates and geometry */
	    new_ls->endx = segment->endx;
	    new_ls->endy = segment->endy;
	    
	    /*
	      Now, determine which intersection point hit on terminal side
	      of cond seg.  Take the other intersection point as the end of
	      the overhang.
	      */
	    if(it & IP_C1I2)
	    {
	      /* intersection1 determines the end of the overhang */
	      new_ls->startx = intersection1.x;
	      new_ls->starty = intersection1.y;
	      segment->endx = intersection1.x;
	      segment->endy = intersection1.y;
	    }
	    else /* it & IP_C1I1 should be true */
	    {
	      /* intersection2 determines the end of the overhang */
	      new_ls->startx = intersection2.x;
	      new_ls->starty = intersection2.y;
	      segment->endx = intersection2.x;
	      segment->endy = intersection2.y;
	    }	    
	    
	    deltax = new_ls->endx - new_ls->startx;
	    deltay = new_ls->endy - new_ls->starty;
	    new_ls->length = sqrt(deltax*deltax + deltay*deltay);
	    new_ls->divisions = (int)(segment->divisions *
	      (new_ls->length / segment->length) + 1.0);
	    segment->divisions += 1 - new_ls->divisions;
	    segment->length -= new_ls->length;
	    
	    /* hook into the list */
	    new_ls->next = segment->next;
	    segment->next = new_ls;
	    
	    /* interior (i.e. those at the new segment ends created where
	       the dielectric intersected) nu, theta2 and edge pairs are not
	       meaninful so null out. */
	    
	    new_ls->nu[0] = -1.0;
	    new_ls->nu[1] = segment->nu[1];
	    segment->nu[1] = -1.0;
	    
	    new_ls->free_space_nu[0] = -1.0;
	    new_ls->free_space_nu[1] = segment->free_space_nu[1];
	    segment->free_space_nu[1] = -1.0;
	    
	    new_ls->theta2[1] = segment->theta2[1];
	    new_ls->theta2[0] = 0.0;
	    segment->theta2[1] = 0.0;
	    
	    new_ls->edge_pair[1] = segment->edge_pair[1];
	    new_ls->edge_pair[0] = NULL;
	    segment->edge_pair[1] = NULL;
	    
	    /* Epsilon values for the overhang piece are not further
	       determined. */
	    
	    segment->epsilon[1] = 0.0;
	    
	    /*
	      Find the proper value of epsilon on overlap piece by seeing if
	      the dielectric segment and the conductor segment go in
	      the same direction.  If they do, and the interior of the
	      conductor is in the positive direction, we want epsilon on
	      the other side, which is epsilonminus.  If segments are in
	      opposite direction then it reverses.
	      */
	    
	    if(cond_inc_dir == die_inc_dir)
	    {
	      /* same direction */
	      if(new_ls->interior > 0)
	      {
		/* inside of conductor is a positive turn - same side that
		   epsilonplus is on, so use epsilonminus */
		new_ls->epsilon[0] = dieseg->epsilonminus;
		new_ls->epsilon[1] = dieseg->epsilonminus;
	      }
	      else
	      {
		new_ls->epsilon[0] = dieseg->epsilonplus;
		new_ls->epsilon[1] = dieseg->epsilonplus;
	      }
	    }
	    else
	    {
	      /* opposite direction */
	      if(new_ls->interior > 0)
	      {
		/* inside of conductor is a positive turn - same side that
		   epsilonminus is on (since they are in opposite directions,
		   so use epsilonplus */
		new_ls->epsilon[0] = dieseg->epsilonplus;
		new_ls->epsilon[1] = dieseg->epsilonplus;
	      }
	      else
	      {
		new_ls->epsilon[0] = dieseg->epsilonminus;
		new_ls->epsilon[1] = dieseg->epsilonminus;
	      }
	    }
	    
	    /* dielectric segment action: adjust size to overhang only
	       
	       Overhang is on terminal side of dielectric - so we leave 
	       the end alone and adjust the start to the intersection.
	       
	       First, determine which intersection point hit on initial side
	       of die seg.  Take the other intersection point as the start of
	       the overhang.
	       */
	    if(it & IP_D0I1)
	    {
	      /* intersection2 determines the start of the overhang */
	      if(vert_die) dieseg->start = intersection2.y;
	      else dieseg->start = intersection2.x;
	      /* adjust divisions and set new length */
	      dieseg->divisions = (int)(dieseg->divisions *
		(dieseg->end - dieseg->start)/dieseg->length + 1.0);
	      dieseg->length = dieseg->end - dieseg->start;
	    }
	    else /* it & IP_D0I2 should be true */
	    {
	      /* intersection1 determines the start of the overhang */
	      if(vert_die) dieseg->start = intersection1.y;
	      else dieseg->start = intersection1.x;
	      /* adjust divisions and set new length */
	      dieseg->divisions = (int)(dieseg->divisions *
		(dieseg->end - dieseg->start)/dieseg->length + 1.0);
	      dieseg->length = dieseg->end - dieseg->start;
	    }
	    break;
	    
	  case CL_SD1C0:
	    
	    /* conductor segment action: fragment into 2 pieces,
	       set epsilon on one.
	       
	       Create new line segment.
	       */
	    new_ls = (LINE_SEGMENTS_P) malloc(sizeof(LINE_SEGMENTS));
	    new_ls->interior = segment->interior;
	    new_ls->conductor = segment->conductor;
	    
	    /* set up the coordinates and geometry */
	    new_ls->endx = segment->endx;
	    new_ls->endy = segment->endy;
	    
	    /*
	      Now, determine which intersection point hit on initial side
	      of cond seg.  Take the other intersection point as the start of
	      the overhang.
	      */
	    if(it & IP_C0I2)
	    {
	      /* intersection1 determines the start of the overhang */
	      new_ls->startx = intersection1.x;
	      new_ls->starty = intersection1.y;
	      segment->endx = intersection1.x;
	      segment->endy = intersection1.y;
	    }
	    else /* it & IP_C0I1 should be true */
	    {
	      /* intersection2 determines the start of the overhang */
	      new_ls->startx = intersection2.x;
	      new_ls->starty = intersection2.y;
	      segment->endx = intersection2.x;
	      segment->endy = intersection2.y;
	    }
	    
	    deltax = new_ls->endx - new_ls->startx;
	    deltay = new_ls->endy - new_ls->starty;
	    new_ls->length = sqrt(deltax*deltax + deltay*deltay);
	    new_ls->divisions = (int)(segment->divisions *
	      (new_ls->length / segment->length) + 1.0);
	    segment->divisions += 1 - new_ls->divisions;
	    segment->length -= new_ls->length;
	    
	    /* hook into the list */
	    new_ls->next = segment->next;
	    segment->next = new_ls;
	    
	    /* interior (i.e. those at the new segment ends created where
	       the dielectric intersected) nu, theta2 and edge pairs are not
	       meaninful so null out. */
	    
	    new_ls->nu[0] = -1.0;
	    new_ls->nu[1] = segment->nu[1];
	    segment->nu[1] = -1.0;
	    
	    new_ls->free_space_nu[0] = -1.0;
	    new_ls->free_space_nu[1] = segment->free_space_nu[1];
	    segment->free_space_nu[1] = -1.0;
	    
	    new_ls->theta2[1] = segment->theta2[1];
	    new_ls->theta2[0] = 0.0;
	    segment->theta2[1] = 0.0;
	    
	    new_ls->edge_pair[1] = segment->edge_pair[1];
	    new_ls->edge_pair[0] = NULL;
	    segment->edge_pair[1] = NULL;
	    
	    /* Epsilon values for the overhang piece are not further
	       determined. */
	    
	    new_ls->epsilon[0] = 0.0;
	    new_ls->epsilon[1] = 0.0;
	    
	    /*
	      Find the proper value of epsilon on overlap piece by seeing if
	      the dielectric segment and the conductor segment go in
	      the same direction.  If they do, and the interior of the
	      conductor is in the positive direction, we want epsilon on
	      the other side, which is epsilonminus.  If segments are in
	      opposite direction then it reverses.
	      */
	    
	    if(cond_inc_dir == die_inc_dir)
	    {
	      /* same direction */
	      if(segment->interior > 0)
	      {
		/* inside of conductor is a positive turn - same side that
		   epsilonplus is on, so use epsilonminus */
		segment->epsilon[0] = dieseg->epsilonminus;
		segment->epsilon[1] = dieseg->epsilonminus;
	      }
	      else
	      {
		segment->epsilon[0] = dieseg->epsilonplus;
		segment->epsilon[1] = dieseg->epsilonplus;
	      }
	    }
	    else
	    {
	      /* opposite direction */
	      if(segment->interior > 0)
	      {
		/* inside of conductor is a positive turn - same side that
		   epsilonminus is on (since they are in opposite directions,
		   so use epsilonplus */
		segment->epsilon[0] = dieseg->epsilonplus;
		segment->epsilon[1] = dieseg->epsilonplus;
	      }
	      else
	      {
		segment->epsilon[0] = dieseg->epsilonminus;
		segment->epsilon[1] = dieseg->epsilonminus;
	      }
	    }
	    
	    /* dielectric segment action: adjust size to overhang only
	       
	       Overhang is on initial side of dielectric - so we leave 
	       the start alone and adjust the end to the intersection.
	       
	       First, determine which intersection point hit on terminal side
	       of die seg.  Take the other intersection point as the end of
	       the overhang.
	       */
	    if(it & IP_D1I1)
	    {
	      /* intersection2 determines the end of the overhang */
	      if(vert_die) dieseg->end = intersection2.y;
	      else dieseg->end = intersection2.x;
	      /* adjust divisions and set new length */
	      dieseg->divisions = (int)(dieseg->divisions *
		(dieseg->end - dieseg->start)/dieseg->length + 1.0);
	      dieseg->length = dieseg->end - dieseg->start;
	    }
	    else /* it & IP_D1I2 should be true */
	    {
	      /* intersection1 determines the end of the overhang */
	      if(vert_die) dieseg->end = intersection1.y;
	      else dieseg->end = intersection1.x;
	      /* adjust divisions and set new length */
	      dieseg->divisions = (int)(dieseg->divisions *
		(dieseg->end - dieseg->start)/dieseg->length + 1.0);
	      dieseg->length = dieseg->end - dieseg->start;
	    }
	    break;
	    
	  case CL_SD1C1:
	    
	    /* conductor segment action: fragment into 2 pieces,
	       set epsilon on one.
	       
	       Create new line segment.
	       */
	    new_ls = (LINE_SEGMENTS_P) malloc(sizeof(LINE_SEGMENTS));
	    new_ls->interior = segment->interior;
	    new_ls->conductor = segment->conductor;
	    
	    /* set up the coordinates and geometry */
	    new_ls->endx = segment->endx;
	    new_ls->endy = segment->endy;
	    
	    /*
	      Now, determine which intersection point hit on initial side
	      of cond seg.  Take the other intersection point as the start of
	      the overhang.
	      */
	    if(it & IP_C1I2)
	    {
	      /* intersection1 determines the end of the overhang */
	      new_ls->startx = intersection1.x;
	      new_ls->starty = intersection1.y;
	      segment->endx = intersection1.x;
	      segment->endy = intersection1.y;
	    }
	    else /* it & IP_C1I1 should be true */
	    {
	      /* intersection2 determines the end of the overhang */
	      new_ls->startx = intersection2.x;
	      new_ls->starty = intersection2.y;
	      segment->endx = intersection2.x;
	      segment->endy = intersection2.y;
	    }
	    
	    deltax = new_ls->endx - new_ls->startx;
	    deltay = new_ls->endy - new_ls->starty;
	    new_ls->length = sqrt(deltax*deltax + deltay*deltay);
	    new_ls->divisions = (int)(segment->divisions *
	      (new_ls->length / segment->length) + 1.0);
	    segment->divisions += 1 - new_ls->divisions;
	    segment->length -= new_ls->length;
	    
	    /* hook into the list */
	    new_ls->next = segment->next;
	    segment->next = new_ls;
	    
	    /* interior (i.e. those at the new segment ends created where
	       the dielectric intersected) nu, theta2 and edge pairs are not
	       meaninful so null out. */
	    
	    new_ls->nu[0] = -1.0;
	    new_ls->nu[1] = segment->nu[1];
	    segment->nu[1] = -1.0;
	    
	    new_ls->free_space_nu[0] = -1.0;
	    new_ls->free_space_nu[1] = segment->free_space_nu[1];
	    segment->free_space_nu[1] = -1.0;
	    
	    new_ls->theta2[1] = segment->theta2[1];
	    new_ls->theta2[0] = 0.0;
	    segment->theta2[1] = 0.0;
	    
	    new_ls->edge_pair[1] = segment->edge_pair[1];
	    new_ls->edge_pair[0] = NULL;
	    segment->edge_pair[1] = NULL;
	    
	    /* Epsilon values for the overhang piece are not further
	       determined. */
	    
	    segment->epsilon[1] = 0.0;
	    
	    /*
	      Find the proper value of epsilon on overlap piece by seeing if
	      the dielectric segment and the conductor segment go in
	      the same direction.  If they do, and the interior of the
	      conductor is in the positive direction, we want epsilon on
	      the other side, which is epsilonminus.  If segments are in
	      opposite direction then it reverses.
	      */
	    
	    if(cond_inc_dir == die_inc_dir)
	    {
	      /* same direction */
	      if(new_ls->interior > 0)
	      {
		/* inside of conductor is a positive turn - same side that
		   epsilonplus is on, so use epsilonminus */
		new_ls->epsilon[0] = dieseg->epsilonminus;
		new_ls->epsilon[1] = dieseg->epsilonminus;
	      }
	      else
	      {
		new_ls->epsilon[0] = dieseg->epsilonplus;
		new_ls->epsilon[1] = dieseg->epsilonplus;
	      }
	    }
	    else
	    {
	      /* opposite direction */
	      if(new_ls->interior > 0)
	      {
		/* inside of conductor is a positive turn - same side that
		   epsilonminus is on (since they are in opposite directions,
		   so use epsilonplus */
		new_ls->epsilon[0] = dieseg->epsilonplus;
		new_ls->epsilon[1] = dieseg->epsilonplus;
	      }
	      else
	      {
		new_ls->epsilon[0] = dieseg->epsilonminus;
		new_ls->epsilon[1] = dieseg->epsilonminus;
	      }
	    }
	    
	    /* dielectric segment action: adjust size to overhang only
	       
	       Overhang is on initial side of dielectric - so we leave 
	       the start alone and adjust the end to the intersection.
	       
	       First, determine which intersection point hit on terminal side
	       of die seg.  Take the other intersection point as the end of
	       the overhang.
	       */
	    if(it & IP_D1I1)
	    {
	      /* intersection2 determines the end of the overhang */
	      if(vert_die) dieseg->end = intersection2.y;
	      else dieseg->end = intersection2.x;
	      /* adjust divisions and set new length */
	      dieseg->divisions = (int)(dieseg->divisions *
		(dieseg->end - dieseg->start)/dieseg->length + 1.0);
	      dieseg->length = dieseg->end - dieseg->start;
	    }
	    else /* it & IP_D1I2 should be true */
	    {
	      /* intersection1 determines the end of the overhang */
	      if(vert_die) dieseg->end = intersection1.y;
	      else dieseg->end = intersection1.x;
	      /* adjust divisions and set new length */
	      dieseg->divisions = (int)(dieseg->divisions *
		(dieseg->end - dieseg->start)/dieseg->length + 1.0);
	      dieseg->length = dieseg->end - dieseg->start;
	    }
	    
	    break;
	  }                        /* switch on type of colinear overlap */
	  
	}                          /* else the intersection is overlap */
	
      }                            /* if there is an intersection */
      
      /* check if first element from list was removed */
      if(dieseg == NULL)
      {
	dieseg = last_dieseg;
	*dielectric_segments = dieseg;
	last_dieseg = NULL;
      }
      else
      {
	last_dieseg = dieseg;
	dieseg = dieseg->next;
      }
      
      
    }                              /* for loop on the dielectric segs */
    last_segment = segment;
    segment = segment->next;
  }                                /* while looping through the segments */
  return(SUCCESS);
}

