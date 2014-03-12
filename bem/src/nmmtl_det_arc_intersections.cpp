
/*
  
  FACILITY:  NMMTL
  
  MODULE DESCRIPTION:
  
  Contains nmmtl_determine_arc_intersectio() which finds where
  dielectric segments and circular conductor segments intersect.
  
  AUTHOR(S):
  
  Kevin J. Buchs
  
  CREATION DATE:  Fri Jan 10 14:35:35 1992
  
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

/* classifications of intersection points */

#define IP_I1D0 1   /* intersection 1 hit on initial point of dielectric seg */
#define IP_I1D1 2
#define IP_I2D0 4
#define IP_I2D1 8
#define IP_I1C0 16  /* intersection 1 hit on initial point of conductor arc */
#define IP_I1C1 32
#define IP_I2C0 64
#define IP_I2C1 128



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
  
  FUNCTION NAME:  nmmtl_determine_arc_intersectio
  
  
  FUNCTIONAL DESCRIPTION:
  
  Find the intersections between dielectric-dielectric interface
  segments and conductor circular segments.  Also see
  nmmtl_determine_intersections.c which does the same thing for
  linear conductor segments.  This determination of intersections will
  result in the following operations upon these segments: fracturing of
  both types of segments into pairs of smaller segments with different
  properties, reducing the size of segments, setting epsilon and
  theta1/nu properties for conductor segments..
  
  How many different kinds of circle-segment line-segment intersections
  can we think of?
  
  ============================================================================
  Legend
  ============================================================================
  
  c                                d
  c			      d
  c			      d
  c			      d
  c			      d
  c			      d
  c			      d
  c			      d
  c			      d
  c                  	      d
  
  This is a circular segment           This is a line segment of a
  of a conductor                       dielectric-dielectric interface
  
  
  c
  dddxddd
  c
  
  X is the intersection point.
  
  ============================================================================
  Types of intersections
  ============================================================================
  
  
  ==============================================================================
  
  d                      1) Endpoint of circle
  d
  d  	       	      Actions:
  x
  d    c	       	      set epsilon for conductor
  d       c	       	      dielectric is fractured
  d         c	       	      
  d           c
  c
  c
  c
  c
  c
  
  ==============================================================================
  
  d
  d
  d
  d                       2) Endpoints of both die and
  x                        conductor, die outside circle
  c
  c                   Actions: set epsilon for conductor
  c
  c
  c
  c
  c
  c
  c
  
  ==============================================================================
  
  x
  d   c
  d      c
  d        c                  3) Endpoints of both, die inside
  d          c
  c                 Actions: set end_in_conductor for die
  c
  c
  c
  c
  
  ==============================================================================
  
  c
  c
  c
  c                  4) Die endpoint on radius, die inside
  c
  ddddddddx                 Actions: set end_in_conductor for die
  c
  c
  c
  c
  
  ==============================================================================
  
  c
  c                     5) Die endpoint on radius, die outside
  c
  c                  Actions:
  c
  xdddddddddd       fracture circle segment, set epsilon
  c                  for each half.
  c
  c
  c
  
  ==============================================================================
  
  c
  c
  c                   6) Die passes through radius
  c
  c                 Actions:
  dddddddxdddddd
  c                  fracture die and set
  c                   end_in_conductor for die inside
  c                     fracture circle and set epsilons
  c
  
  ==============================================================================
  
  c    d
  c d
  x                   7) Chordal
  dc
  d c                 Actions:
  d c
  dc                  fracture conductor into 3 parts, set
  x                   epsilons.
  c d
  c    d                   middle part of die removed.  New piece
  created.
  ==============================================================================
  
  c    d
  c d
  x                   8) Chordal terminating
  dc
  d c                 Actions:
  d c
  dc                  fracture conductor into 2 parts, set
  x                   epsilons.
  c
  c                        interior part of die removed.
  
  ==============================================================================
  
  c
  c
  x                   9) Chordal bi-terminating
  dc
  d c                 Actions: die removed.
  d c
  dc
  x
  c
  c
  
  ==============================================================================
  
  c      d
  c   d
  c d
  cd                 10) Tangential plain
  x
  x                 Actions: Do nothing
  cd
  c d
  c   d
  c      d
  
  ==============================================================================
  
  c
  c
  c
  c
  c                 11) Tangential terminating
  x
  cd                 Actions: Do nothing
  c d
  c   d
  c      d
  d
  
  ==============================================================================
  
  c
  c
  c
  c                  12) Tangential terminating at endpoints,
  c                 die outside
  c
  c                  Actions: set epsilon on conductor
  c
  c
  xddddddddddddd
  
  ==============================================================================
  
  c
  c
  c
  c                  13) Tangential terminating at endpoint
  c                 die both sides
  c
  c                  Actions: set epsilon on conductor
  c
  c
  ddddddxdddddddd
  
  ==============================================================================
  
  c
  c
  c
  c                  14) Tangential terminating at endpoints,
  c                 die "inside"
  c
  c                  Actions: Nothing
  c
  c
  ddddddx
  
  ==============================================================================
  
  d
  d
  d
  x
  d  c
  d    c                   15) Chordal on conductor endpoints
  d     c
  d      c                 Actions:
  d      c
  d     c                  set epsilons.
  d    c
  d  c
  x                        middle part of die removed.  New piece
  d                        created.
  d
  d
  
  ==============================================================================
  
  
  
  
  x
  d  c
  d    c                   16) Chordal on conductor endpoints
  d     c                      and one die endpoint
  d      c                 Actions:
  d      c
  d     c                  set epsilons.
  d    c
  d  c
  x                        middle part of die removed.  Shrink
  d                        leftover.
  d
  d
  
  ==============================================================================
  
  
  x
  d  c
  d    c                   17) Chordal on conductor endpoints
  d     c                      and both die endpoints
  d      c
  d      c                 Actions: die removed
  d     c
  d    c
  d  c
  x
  
  
  ==============================================================================
  
  d
  d
  x
  d c
  d  c                   18) Chordal on one conductor endpoint
  d  c                      and no die endpoints
  d  c
  d c                 Actions:
  x
  c d                 die piece added, original shrunk to
  c    d                remove middle section
  c        d
  d              fracture circle segment, 
  epsilon values set
  ==============================================================================
  
  d
  d
  x
  d c
  d  c                   19) Chordal on one conductor endpoint
  d  c                      and one die endpoint
  d  c
  d c                 Actions: die removed by shrinking
  x                     the original 
  c 
  c                     Epsilon set.
  c        
  
  
  ==============================================================================
  
  x
  d c
  d  c                   20) Chordal on conductor endpoints
  d  c                      and both die endpoints
  d  c
  d c                 Actions: die removed                  
  x
  c
  c  
  c        
  
  
  ==============================================================================
  
  x
  d c                     21) Chordal intersection on one 
  d  c                       conductor endpoint and different
  d  c                      intersection on one die endpoint
  d  c		      Actions:                             
  d c                                                      
  x		      die removed by shrinking the original
  c d		                                           
  c    d		      fracture circle segment,
  c        d               epsilon values set
  
  ==============================================================================
  
  The dielectric and conductor segments have a 0th and 1st endpoints.  The
  are given by the order they are listed.
  
  We have these constants to define where an intersection occurred:
  
  I1D0       intersection1 is on dielectric endpoint 0
  I2D0       intersection2 is on dielectric endpoint 0
  I1D1       intersection1 is on dielectric endpoint 1
  I2D1       intersection2 is on dielectric endpoint 1
  I1C0       intersection1 is on conductor endpoint 0
  I2C0       intersection2 is on conductor endpoint 0
  I1C1       intersection1 is on conductor endpoint 1
  I2C1       intersection2 is on conductor endpoint 1
  
  The variable it is defined to be each of these in a unique bit position.
  
  Of the above types of intersections, these will be the possible combinations,
  where ? = (0,1) :
  
  1)   I?C?
  2)   I?C? and I?D?
  3)   I?C? and I?D?
  4)   I?D?
  5)   I?D?
  6)   none
  7)   none
  8)   I?D? (one)
  9)   I?D0 and I?D1 (two)
  10)   none
  11)  I?D?
  12)  I?C? and I?D?
  13)  I?C?
  14)  I?C? and I?D?
  15)  I?C0 and I?C1
  16)  I?C0, I?C1, and I?D?
  17)  I?C0, I?C1, I?D0, and I?D1
  18)  I?C?
  19)  I?C? and I?D?
  20)  I?C?, I?D0, and I?D1
  21)  (I1C? and I2D?) or (I2C? and I1D?)
  
  Distingushing features:
  
  One Intersection:
  
  1) and 13) :
  13) is tangential
  2), 3), 12), and 14) :
  2) piece of die has less than 90 degree angle to normal
  3) piece of die has greater than 90 degree angle to normal
  12) tangential - die outside
  14) tangential - die inside
  4), 5), and 11) :
  4) piece of die has greater than 90 degree angle to normal
  5) piece of die has less than 90 degree angle to normal
  11) tangential
  6), and 10) :
  6) single intersection
  10) tangential
  
  Two Intersections:
  
  7)  0  conductor and 0  die endpoint hits
  8)  0  conductor and 1 die endpoint hits
  9)  0  conductor and 2 die endpoint hits
  15) 2 conductor and 0  die endpoint hits
  16) 2 conductor and 1  die endpoint hits
  17) 2 conductor and 2 die endpoint hits
  18) 1 conductor and 0 die endpoint hits
  19) 1 conductor and 1 die endpoint hits - same intersections
  20) 1 conductor and 2 die endpoint hits
  21) 1 conductor and 1 die endpoint hits - opposite intersections
  
  Tagentiality is determined by the angle with the intersection routine, which
  returns a flag.  Angle with the normal is easy to get, the normal is the line
  segment from the center of the arc to the intesection point.
  
  For 12), and 14), we need to determine if the die is "inside" or
  "outside".  For 13) we want to know which piece is inside.  Really,
  all is outside, since it what we have is a tangent line.  However, if
  you drew a normal at the intersection point, "inside is the opposide
  direction from the direction the arc goes away from the normal.  The
  algorithm is:
  
  if at initial point of arc and left turn (+90 degrees) to the die from
  normal, then it is outside.
  
  if at terminal point of arc and right turn (-90) to the die from
  normal, then it is outside.
  
  
  FORMAL PARAMETERS:
  
  CIRCLE_SEGMENTS_P *circle_segments,     The list of circle segments
  DIELECTRIC_SEGMENTS_P *dielectric_segments  The list of dielectric segments
  
  RETURN VALUE:
  
  SUCCESS or FAIL
  
  CALLING SEQUENCE:
  
  status = nmmtl_determine_intersectio(&circle_segments,&dielectric_segments
  );
  
  */

int nmmtl_determine_arc_intersectio(CIRCLE_SEGMENTS_P *circle_segments,
				    DIELECTRIC_SEGMENTS_P *dielectric_segments)
{
  LINESEG dseg;
  POINT intersection1,intersection2;
  int number_of_intersections,tangent;
  CIRCLE_SEGMENTS_P segment, last_segment, new_cs, new_cs_2;
  DIELECTRIC_SEGMENTS_P dieseg, last_dieseg, new_ds = NULL;
  double intersection_angle,inter_angle[2];
  long int ip;  /* intersection points - recordkeeping flag for IP_* */
  int vert_die; /* flags indicating that these are vertical
		   segments, i.e. slope in infinite */
  int die_inc_dir; /* TRUE or FALSE - conductor or dielectric
		      is in increasing direction */
  int cirseg_endpoint; /* indicates if an intersection is on an endpoint of
			  the circle segment. */
  int cseg_index,dseg_index; /* initial or terminal points */
  double angle_to_normal;
  int intersection_type; /* just a serially numbered type of intersection -
			    see the above documentation */
  int cond_hits,die_hits; /* number of intersections that are on endpoints */
  
  int break_out_of_conductor_loop = FALSE;
  
  /* Now detemine the intersections of conductor circle segments with
     dielectric-dielectric segments.
     
     Since there is some copying of data for each dielectric segment,
     move ta loop to the outside to avoid repeating that work
     */
  
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
    segment = *circle_segments;
    break_out_of_conductor_loop = FALSE;
    while(segment != NULL && break_out_of_conductor_loop == FALSE)
    {
      /* determine if the dieseg was adjusted within the loop and hence,
	 dseg needs to be recomputed */
      if(new_ds != NULL)
      {
	new_ds = NULL;
	
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
      }
      
      number_of_intersections = nmmtl_cirseg_seg_inter(segment,&dseg,
						       &intersection1,
						       &intersection2,
						       &tangent);
      if(number_of_intersections > 0)
      {
	
	/* classify the intersection points */
	
	ip = 0;
	cond_hits = 0;
	die_hits = 0;
	
	if(intersection1.x == dseg.x[0] &&
	   intersection1.y == dseg.y[0])
	{
	  ip |= IP_I1D0;
	  die_hits++;
	}
	if(intersection1.x == dseg.x[1] &&
	   intersection1.y == dseg.y[1])
	{
	  ip |= IP_I1D1;
	  die_hits++;
	}
	cirseg_endpoint = nmmtl_cirseg_endpoint(segment,&intersection1);
	if(cirseg_endpoint == INITIAL_ENDPOINT)
	{
	  ip |= IP_I1C0;
	  cond_hits++;
	}
	if(cirseg_endpoint == TERMINAL_ENDPOINT)
	{
	  ip |= IP_I1C1;
	  cond_hits++;
	}
	if(number_of_intersections == 2)
	{
	  if(intersection2.x == dseg.x[0] &&
	     intersection2.y == dseg.y[0])
	  {
	    ip |= IP_I2D0;
	    die_hits++;
	  }
	  if(intersection2.x == dseg.x[1] &&
	     intersection2.y == dseg.y[1])
	  {
	    ip |= IP_I2D1;
	    die_hits++;
	  }
	  cirseg_endpoint = nmmtl_cirseg_endpoint(segment,&intersection2);
	  if(cirseg_endpoint == INITIAL_ENDPOINT)
	  {
	    ip |= IP_I2C0;
	    cond_hits++;
	  }
	  if(cirseg_endpoint == TERMINAL_ENDPOINT)
	  {
	    ip |= IP_I2C1;
	    cond_hits++;
	  }
	}
	
	/* Now divide things up further */
	
	if(number_of_intersections == 1)
	{
	  /* one intersection */
	  
	  if(ip & IP_I1D0) dseg_index = INITIAL_ENDPOINT;
	  else dseg_index = TERMINAL_ENDPOINT;
	  if(ip & IP_I1C0) cseg_index = INITIAL_ENDPOINT;
	  else cseg_index = TERMINAL_ENDPOINT;
	  
	  if(ip & (IP_I1C0 | IP_I1C1))
	  {
	    /* hit one conductor endpoint */
	    
	    if(ip & (IP_I1D0 | IP_I1D1))
	    {
	      /* hit one dielectric endpoint */
	      angle_to_normal =
		nmmtl_cirseg_angle_to_normal(segment,&intersection1,
					     &dseg,dseg_index);
	      if(tangent)
	      {
		if(angle_to_normal > 0.0 && cseg_index == INITIAL_ENDPOINT ||
		   angle_to_normal < 0.0 && cseg_index == TERMINAL_ENDPOINT)
		  intersection_type = 14;
		else
		  intersection_type = 12;
	      }
	      else
	      {
		if(fabs(angle_to_normal) < PI/2)
		  intersection_type = 2;
		else
		  intersection_type = 3;
	      }
	    } /* one die hit */
	    else
	    {
	      /* no die hit, one cond hit */
	      if(tangent) intersection_type = 13;
	      else intersection_type = 1;
	    }                      /* no die hit, one cond hit */
	  }                        /* one cond hit */
	  else
	  {
	    /* no cond hit */
	    if(ip & (IP_I1D0 | IP_I1D1))
	    {
	      /* die hit */
	      angle_to_normal =
		nmmtl_cirseg_angle_to_normal(segment,&intersection1,
					     &dseg,dseg_index);
	      if(tangent)
	      {
		intersection_type = 11;
	      }
	      else
	      {
		if(fabs(angle_to_normal) < PI/2)
		  intersection_type = 5;
		else
		  intersection_type = 4;
	      }
	    } /* one die hit */
	    else
	    {
	      /* no die hit, no cond hit */
	      if(tangent) intersection_type = 10;
	      else intersection_type = 6;
	      
	    }                      /* no die hit, one cond hit */
	  }                        /* no cond hit */
	}                          /* one intersection */
	else
	{
	  /* two intersections */
	  switch(cond_hits)
	  {
	  case 0:
	    switch(die_hits)
	    {
	    case 0: intersection_type = 7; break; 
	    case 1: intersection_type = 8; break; 
	    case 2: intersection_type = 9; break; 
	    }
	    break;
	  case 1:
	    switch(die_hits)
	    {
	    case 0: intersection_type = 18; break; 
	    case 1: 
	      /* are conductor and die endpoint intersections different or
		 the same intersection ? */
	      if(ip & (IP_I1C0 | IP_I1C1) && ip & (IP_I2D0 | IP_I2D1) ||
		 ip & (IP_I2C0 | IP_I2C1) && ip & (IP_I1D0 | IP_I1D1))
		intersection_type = 19;     /* different endpoint */
	      else intersection_type = 21;  /* same endpoint */
	      break; 
	    case 2: intersection_type = 20; break; 
	    }
	    break;
	  case 2:
	    switch(die_hits)
	    {
	    case 0: intersection_type = 15; break; 
	    case 1: intersection_type = 16; break; 
	    case 2: intersection_type = 17; break; 
	    }
	    break;
	  }
	}                          /* two intersections */
	
	/* Now that we have fully determined the intersection type, take
	   the appropriate action based on that type. */
	
	switch(intersection_type)
	{
	case 1 :
	  /*
	    Endpoint of circle                 
	    
	    Actions:                              
	    
	    set epsilon for conductor
	    dielectric is fractured
	    
	    */
	  
	  
	  /* Create new dielectric segment by splitting the old one. */
	  new_ds = 
	    (DIELECTRIC_SEGMENTS_P)malloc(sizeof(DIELECTRIC_SEGMENTS));
	  new_ds->next = dieseg->next;
	  dieseg->next = new_ds;
	  new_ds->at = dieseg->at;
	  new_ds->end = dieseg->end;
	  new_ds->epsilonplus = dieseg->epsilonplus;
	  new_ds->epsilonminus = dieseg->epsilonminus;
	  new_ds->segment_number = dieseg->segment_number;
	  new_ds->end_in_conductor = dieseg->end_in_conductor;
	  new_ds->orientation = dieseg->orientation;
	  
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
	  
	  new_ds->length = new_ds->end - new_ds->start;
	  new_ds->divisions = (int)(dieseg->divisions * 
	    (new_ds->length/dieseg->length) + 1.0);
	  dieseg->length -= new_ds->length;
	  dieseg->divisions -= (new_ds->divisions - 1);
	  
	  /* which end of die is inside ? */
	  angle_to_normal =
	    nmmtl_cirseg_angle_to_normal(segment,&intersection1,&dseg,0);
	  if(fabs(angle_to_normal) < PI/2)
	  {
	    /* intersection to initial endpoint is inside */
	    dieseg->end_in_conductor |= 0X02;  /* set bit 2 */
	    new_ds->end_in_conductor &= 0XFFFFFFFE;  /* clear bit 1 */
	    
	    if( ip & IP_I1C0 )
	      segment->epsilon[0] = dieseg->epsilonplus;
	    else
	      segment->epsilon[1] = dieseg->epsilonminus;
	  }
	  else
	  {
	    /* intersection to initial endpoint is outside */
	    dieseg->end_in_conductor &= 0XFFFFFFFD;  /* clear bit 2 */
	    new_ds->end_in_conductor |= 0X01;  /* set bit 1 */
	    
	    if( ip & IP_I1C0 )
	      segment->epsilon[0] = dieseg->epsilonminus;
	    else
	      segment->epsilon[1] = dieseg->epsilonplus;
	  }
	  
	  break;
	  
	case 2 :
	  /*
	    Endpoints of both die and          
	    conductor, die outside circle         
	    
	    Actions: set epsilon for conductor    
	    */
	  /* find the appropriate epsilon */
	  if(ip & IP_I1D0)
	  {
	    /* intersection is with initial endpoint of die */
	    if( ip & IP_I1C0 )
	    {
	      segment->epsilon[0] = dieseg->epsilonplus;
	    }
	    else
	    {
	      segment->epsilon[1] = dieseg->epsilonminus;
	    }
	  }
	  else
	  {
	    /* intersection is with terminal endpoint of die */
	    if( ip & IP_I1C0 )
	    {
	      segment->epsilon[0] = dieseg->epsilonminus;
	    }
	    else
	    {
	      segment->epsilon[1] = dieseg->epsilonplus;
	    }
	  }
	  
	  
	  break;
	  
	case 3 :
	  /*
	    Endpoints of both, die inside      
	    
	    Actions: set end_in_conductor for die                   
	    */
	  
	  /* which end of die is inside ? */
	  if(ip & IP_I1D0)
	  {
	    /* initial endpoint is on conductor */
	    dieseg->end_in_conductor |= 0X01;  /* set bit 1 */
	  }
	  else
	  {
	    /* terminal endpoint is on conductor */
	    dieseg->end_in_conductor |= 0X02;  /* set bit 2 */
	  }
	  
	  break;
	  
	case 4 :
	  
	  /*
	    Die endpoint on radius, die inside 
	    
	    Actions: set end_in_conductor for die
	    */
	  
	  /* which end of die is inside ? */
	  if(ip & IP_I1D0)
	  {
	    /* initial endpoint is on conductor */
	    dieseg->end_in_conductor |= 0X01;  /* set bit 1 */
	  }
	  else
	  {
	    /* terminal endpoint is on conductor */
	    dieseg->end_in_conductor |= 0X02;  /* set bit 2 */
	  }
	  
	  break;
	  
	case 5 :
	  /*
	    Die endpoint on radius, die outside
	    
	    Actions:                              
	    
	    fracture circle segment, set epsilon  
	    for each half.                        
	    */
	  
	  /* create new conductor circle segment */
	  new_cs = (CIRCLE_SEGMENTS_P) malloc(sizeof(CIRCLE_SEGMENTS));
	  new_cs->centerx = segment->centerx;
	  new_cs->centery = segment->centery;
	  new_cs->radius = segment->radius;
	  new_cs->conductor = segment->conductor;
	  new_cs->endangle = segment->endangle;
	  new_cs->epsilon[1] = segment->epsilon[1];
	  
	  /* find the angle of the intersection */
	  intersection_angle =
	    nmmtl_cirseg_point_angle(segment,intersection1.x,intersection1.y);
	  /* set up the coordinates */
	  segment->endangle = intersection_angle;
	  new_cs->startangle = intersection_angle;
	  new_cs->radians = new_cs->endangle - new_cs->startangle;
	  new_cs->divisions = (int)(segment->divisions *
	    (new_cs->radians / segment->radians) + 1.0);
	  segment->divisions += 1 - new_cs->divisions;
	  segment->radians -= new_cs->radians;
	  /* hook into the list */
	  new_cs->next = segment->next;
	  segment->next = new_cs;
	  
	  if(ip & IP_I1D0)
	  {
	    /* intersection at initial point of die */
	    new_cs->epsilon[0] = dieseg->epsilonplus;
	    segment->epsilon[1] = dieseg->epsilonminus;
	  }
	  else
	  {
	    /* intersection at terminal point of die */
	    new_cs->epsilon[0] = dieseg->epsilonminus;
	    segment->epsilon[1] = dieseg->epsilonplus;
	  }
	  
	  break;
	  
	case 6 :
	  /*
	    Die passes through radius          
	    
	    Actions:                              
	    
	    fracture die and set
	    end_in_conductor for die inside
	    
	    fracture circle and set epsilons      
	    */
	  
	  /* Create new dielectric segment by splitting the old one. */
	  new_ds = 
	    (DIELECTRIC_SEGMENTS_P)malloc(sizeof(DIELECTRIC_SEGMENTS));
	  new_ds->next = dieseg->next;
	  dieseg->next = new_ds;
	  new_ds->at = dieseg->at;
	  new_ds->end = dieseg->end;
	  new_ds->epsilonplus = dieseg->epsilonplus;
	  new_ds->epsilonminus = dieseg->epsilonminus;
	  new_ds->segment_number = dieseg->segment_number;
	  new_ds->end_in_conductor = dieseg->end_in_conductor;
	  new_ds->orientation = dieseg->orientation;
	  
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
	  
	  new_ds->length = new_ds->end - new_ds->start;
	  new_ds->divisions = (int)(dieseg->divisions * 
	    (new_ds->length/dieseg->length) + 1.0);
	  dieseg->length -= new_ds->length;
	  dieseg->divisions -= (new_ds->divisions - 1);
	  
	  /* create new conductor circle segment */
	  new_cs = (CIRCLE_SEGMENTS_P) malloc(sizeof(CIRCLE_SEGMENTS));
	  new_cs->centerx = segment->centerx;
	  new_cs->centery = segment->centery;
	  new_cs->radius = segment->radius;
	  new_cs->conductor = segment->conductor;
	  new_cs->endangle = segment->endangle;
	  new_cs->epsilon[1] = segment->epsilon[1];
	  
	  /* find the angle of the intersection */
	  intersection_angle =
	    nmmtl_cirseg_point_angle(segment,intersection1.x,intersection1.y);
	  /* set up the coordinates */
	  segment->endangle = intersection_angle;
	  new_cs->startangle = intersection_angle;
	  new_cs->radians = new_cs->endangle - new_cs->startangle;
	  new_cs->divisions = (int)(segment->divisions *
	    (new_cs->radians / segment->radians) + 1.0);
	  segment->divisions += 1 - new_cs->divisions;
	  segment->radians -= new_cs->radians;
	  /* hook into the list */
	  new_cs->next = segment->next;
	  segment->next = new_cs;
	  
	  /* which end of die is inside ? */
	  angle_to_normal =
	    nmmtl_cirseg_angle_to_normal(segment,&intersection1,&dseg,0);
	  if(fabs(angle_to_normal) < PI/2)
	  {
	    /* intersection to initial endpoint is inside */
	    /* set appropriate epsilon values */
	    new_cs->epsilon[0] = dieseg->epsilonplus;
	    segment->epsilon[1] = dieseg->epsilonminus;
	    
	    dieseg->end_in_conductor |= 0X02;  /* set bit 2 */
	    new_ds->end_in_conductor &= 0XFFFFFFFE;  /* clear bit 1 */
	  }
	  else
	  {
	    /* intersection to initial endpoint is outside */
	    /* set appropriate epsilon values */
	    new_cs->epsilon[0] = dieseg->epsilonminus;
	    segment->epsilon[1] = dieseg->epsilonplus;
	    
	    dieseg->end_in_conductor &= 0XFFFFFFFD;  /* clear bit 2 */
	    new_ds->end_in_conductor |= 0X01;  /* set bit 1 */
	  }
	  
	  break;
	  
	case 7 :
	  /*
	    Chordal                            
	    
	    Actions:                              
	    
	    fracture conductor into 3 parts, set  
	    epsilons.                             
	    
	    middle part of die removed.  New piece
	    created.                              
	    */
	  
	  /* create two new conductor circle segments */
	  new_cs = (CIRCLE_SEGMENTS_P) malloc(sizeof(CIRCLE_SEGMENTS));
	  new_cs->centerx = segment->centerx;
	  new_cs->centery = segment->centery;
	  new_cs->radius = segment->radius;
	  new_cs->conductor = segment->conductor;
	  new_cs_2 = (CIRCLE_SEGMENTS_P) malloc(sizeof(CIRCLE_SEGMENTS));
	  new_cs_2->centerx = segment->centerx;
	  new_cs_2->centery = segment->centery;
	  new_cs_2->radius = segment->radius;
	  new_cs_2->conductor = segment->conductor;
	  /* hook into the list */
	  new_cs_2->next = segment->next;
	  new_cs->next = new_cs_2;
	  segment->next = new_cs;
	  new_cs_2->endangle = segment->endangle;
	  new_cs_2->epsilon[1] = segment->epsilon[1];
	  
	  /* Create new dielectric segment by splitting the old one. */
	  new_ds = 
	    (DIELECTRIC_SEGMENTS_P)malloc(sizeof(DIELECTRIC_SEGMENTS));
	  new_ds->next = dieseg->next;
	  dieseg->next = new_ds;
	  new_ds->at = dieseg->at;
	  new_ds->end = dieseg->end;
	  
	  new_ds->epsilonplus = dieseg->epsilonplus;
	  new_ds->epsilonminus = dieseg->epsilonminus;
	  new_ds->segment_number = dieseg->segment_number;
	  new_ds->end_in_conductor = dieseg->end_in_conductor;
	  new_ds->orientation = dieseg->orientation;
	  /* neither ends in a conductor - so clear the bits */
	  dieseg->end_in_conductor &= 0XFFFD;  /* clear bit 2 */
	  new_ds->end_in_conductor &= 0XFFFE;  /* clear bit 1 */
	  
	  /* find which intersection comes first on circle segment */
	  
	  /* find the angles of the intersections */
	  inter_angle[0] = 
	    nmmtl_cirseg_point_angle(segment,intersection1.x,intersection1.y);
	  inter_angle[1] =
	    nmmtl_cirseg_point_angle(segment,intersection2.x,intersection2.y);
	  
	  if(inter_angle[0] < inter_angle[1])
	  {
	    /* intersection1 is first */
	    
	    /* set up the coordinates */
	    segment->endangle = inter_angle[0];
	    new_cs->startangle = inter_angle[0];
	    new_cs->endangle = inter_angle[1];
	    new_cs->radians = new_cs->endangle - new_cs->startangle;
	    new_cs->divisions = (int)(segment->divisions *
	      (new_cs->radians / segment->radians) + 1.0);
	    new_cs_2->startangle = inter_angle[1];
	    new_cs_2->radians = new_cs_2->endangle - new_cs_2->startangle;
	    new_cs_2->divisions = (int)(segment->divisions *
	      (new_cs_2->radians / segment->radians) + 1.0);
	    segment->divisions = (int)(1.0 + segment->divisions *
	      (1 - (new_cs->radians + new_cs_2->radians)/segment->radians));
	    segment->radians -= new_cs->radians + new_cs_2->radians;
	    
	    /* which end of die goes inside ? */
	    angle_to_normal =
	      nmmtl_cirseg_angle_to_normal(segment,&intersection1,&dseg,0);
	    if(fabs(angle_to_normal) < PI/2)
	    {
	      /* from intersection1, initial end of die is inside conductor */
	      segment->epsilon[1] = dieseg->epsilonminus;
	      new_cs_2->epsilon[0] = dieseg->epsilonminus;
	      new_cs->epsilon[0] = dieseg->epsilonplus;
	      new_cs->epsilon[1] = dieseg->epsilonplus;
	      if(dieseg->orientation == VERTICAL_ORIENTATION)
	      {
		new_ds->start = intersection1.y;
		dieseg->end = intersection2.y;
	      }
	      else
	      {
		new_ds->start = intersection1.x;
		dieseg->end = intersection2.x;
	      }
	      
	    }
	    else
	    {
	      /* from intersection1, initial end of die is outside conductor */
	      segment->epsilon[1] = dieseg->epsilonplus;
	      new_cs_2->epsilon[0] = dieseg->epsilonplus;
	      new_cs->epsilon[0] = dieseg->epsilonminus;
	      new_cs->epsilon[1] = dieseg->epsilonminus;
	      if(dieseg->orientation == VERTICAL_ORIENTATION)
	      {
		new_ds->start = intersection2.y;
		dieseg->end = intersection1.y;
	      }
	      else
	      {
		new_ds->start = intersection2.x;
		dieseg->end = intersection1.x;
	      }
	    }
	  }
	  else
	  {
	    /* intersection2 is first */
	    
	    /* set up the coordinates */
	    segment->endangle = inter_angle[1];
	    new_cs->startangle = inter_angle[1];
	    new_cs->endangle = inter_angle[0];
	    new_cs->radians = new_cs->endangle - new_cs->startangle;
	    new_cs->divisions = (int)(segment->divisions *
	      (new_cs->radians / segment->radians) + 1.0);
	    new_cs_2->startangle = inter_angle[0];
	    new_cs_2->radians = new_cs_2->endangle - new_cs_2->startangle;
	    new_cs_2->divisions = (int)(segment->divisions *
	      (new_cs_2->radians / segment->radians) + 1.0);
	    segment->divisions = (int)(1.0 + segment->divisions *
	      (1 - (new_cs->radians + new_cs_2->radians)/segment->radians));
	    segment->radians -= new_cs->radians + new_cs_2->radians;
	    
	    /* which end of die goes inside ? */
	    angle_to_normal =
	      nmmtl_cirseg_angle_to_normal(segment,&intersection2,&dseg,0);
	    if(fabs(angle_to_normal) < PI/2)
	    {
	      /* from intersection2, initial end of die is inside conductor */
	      segment->epsilon[1] = dieseg->epsilonminus;
	      new_cs_2->epsilon[0] = dieseg->epsilonminus;
	      new_cs->epsilon[0] = dieseg->epsilonplus;
	      new_cs->epsilon[1] = dieseg->epsilonplus;
	      if(dieseg->orientation == VERTICAL_ORIENTATION)
	      {
		new_ds->start = intersection2.y;
		dieseg->end = intersection1.y;
	      }
	      else
	      {
		new_ds->start = intersection2.x;
		dieseg->end = intersection1.x;
	      }
	      
	    }
	    else
	    {
	      /* from intersection1, initial end of die is outside conductor */
	      segment->epsilon[1] = dieseg->epsilonplus;
	      new_cs_2->epsilon[0] = dieseg->epsilonplus;
	      new_cs->epsilon[0] = dieseg->epsilonminus;
	      new_cs->epsilon[1] = dieseg->epsilonminus;
	      if(dieseg->orientation == VERTICAL_ORIENTATION)
	      {
		new_ds->start = intersection1.y;
		dieseg->end = intersection2.y;
	      }
	      else
	      {
		new_ds->start = intersection1.x;
		dieseg->end = intersection2.x;
	      }
	    }
	  }
	  
	  
	  /* finally, compute the redistribution of divisions based on
	     length */
	  new_ds->length = new_ds->end - new_ds->start;
	  new_ds->divisions = (int)(dieseg->divisions * 
	    (new_ds->length/dieseg->length) + 1.0);
	  dieseg->divisions = (int)(dieseg->divisions *
            (dieseg->end - dieseg->start)/dieseg->length + 1.00);
	  dieseg->length = dieseg->end - dieseg->start;
	  
	  break;
	  
	case 8 :
	  /*
	    Chordal terminating                
	    
	    Actions:                              
	    
	    fracture conductor into 2 parts, set  
	    epsilons.                             
	    
	    interior part of die removed.         
	    */
	  /* This is a simplified version of #7 above */
	  
	  
	  /* create two new conductor circle segments */
          new_cs = (CIRCLE_SEGMENTS_P) malloc(sizeof(CIRCLE_SEGMENTS));
	  new_cs->centerx = segment->centerx;
	  new_cs->centery = segment->centery;
	  new_cs->radius = segment->radius;
	  new_cs->conductor = segment->conductor;
	  /* hook into the list */
	  new_cs->next = segment->next;
	  segment->next = new_cs;
	  new_cs->endangle = segment->endangle;
	  new_cs->epsilon[1] = segment->epsilon[1];
	  
	  /* find which intersection comes first on circle segment */
	  
	  /* find the angles of the intersections */
	  inter_angle[0] = 
	    nmmtl_cirseg_point_angle(segment,intersection1.x,intersection1.y);
          inter_angle[1] =
	    nmmtl_cirseg_point_angle(segment,intersection2.x,intersection2.y);
	  
          if(ip & (IP_I2D0 | IP_I2D1))
          {
	    /* intersection2 is where die endpoint hit, intersection1 
	       splits up the conductor segments */
	    
	    /* set up the coordinates */
	    segment->endangle = inter_angle[0];
	    new_cs->startangle = inter_angle[0];
	    
	    if(ip & IP_I2D1)
	    {
	      /* from intersection1, initial end of die is outside conductor */
	      segment->epsilon[1] = dieseg->epsilonplus;
	      new_cs->epsilon[0] = dieseg->epsilonminus;
	      if(dieseg->orientation == VERTICAL_ORIENTATION)
	      {
		dieseg->end = intersection1.y;
	      }
	      else
	      {
		dieseg->end = intersection1.x;
	      }
	    }
	    else /* ip & IP_I2D0 */
	    {
	      /* from intersection1, terminal end of die is outside
		 conductor */
	      segment->epsilon[1] = dieseg->epsilonminus;
	      new_cs->epsilon[0] = dieseg->epsilonplus;
	      if(dieseg->orientation == VERTICAL_ORIENTATION)
	      {
		dieseg->start = intersection1.y;
	      }
	      else
	      {
		dieseg->start = intersection1.x;
	      }
	    }
	  }
	  else  /* ip & (IP_I1D0 | IP_I1D1) */
          {
	    /* intersection1 is where die endpoint hit, intersection2 
	       splits up the conductor segments */
	    
	    /* set up the coordinates */
	    segment->endangle = inter_angle[1];
	    new_cs->startangle = inter_angle[1];
	    
	    if(ip & IP_I1D1)
	    {
	      /* from intersection2, initial end of die is outside conductor */
	      segment->epsilon[1] = dieseg->epsilonplus;
	      new_cs->epsilon[0] = dieseg->epsilonminus;
	      if(dieseg->orientation == VERTICAL_ORIENTATION)
	      {
		dieseg->end = intersection2.y;
	      }
	      else
	      {
		dieseg->end = intersection2.x;
	      }
	    }
	    else /* ip & IP_I1D0 */
	    {
	      /* from intersection2, terminal end of die is outside
		 conductor */
	      segment->epsilon[1] = dieseg->epsilonminus;
	      new_cs->epsilon[0] = dieseg->epsilonplus;
	      if(dieseg->orientation == VERTICAL_ORIENTATION)
	      {
		dieseg->start = intersection2.y;
	      }
	      else
	      {
		dieseg->start = intersection2.x;
	      }
	    }
          }
	  
	  
	  /* finally, compute the redistribution of divisions based on
	     length */
          new_cs->radians = new_cs->endangle - new_cs->startangle;
	  new_cs->divisions = (int)(segment->divisions *
	    (new_cs->radians / segment->radians) + 1.0);
	  segment->divisions += 1 - new_cs->divisions;
	  segment->radians -= new_cs->radians;
	  
	  dieseg->divisions = (int)(dieseg->divisions * 
	    ((dieseg->end - dieseg->start)/dieseg->length) + 1.0);
	  dieseg->length = dieseg->end - dieseg->start;
	  /* flag that we changed the dieseg and dseg needs to be 
	     recomputed */
	  new_ds = (DIELECTRIC_SEGMENTS_P)1;
	  
	  break;
	  
	case 9 :
	  /*
	    Chordal bi-terminating             
	    
	    Actions: die removed.                 
	    */
	  
	  /* remove die */
	  if(last_dieseg != NULL)
	  {
	    last_dieseg->next = dieseg->next; /* bypass on list */
	    free(dieseg);
	    dieseg = last_dieseg->next;
	  }
	  else
	  {
	    /* removing first element on the list */
	    *dielectric_segments = (*dielectric_segments)->next;
	    free(dieseg);
	    dieseg = *dielectric_segments; 
	  }
	  break_out_of_conductor_loop = TRUE;
	  break;
	  
	case 10 :
	  /*
	    Tangential plain                  
	    
	    Actions: Do nothing                   
	    */
	  
	  break;
	  
	case 11 :
	  /*
	    Tangential terminating            
	    
	    Actions: Do nothing                   
	    */
	  
	  break;
	  
	case 12 :
	  /*
	    Tangential terminating at endpoint
	    die outside                           
	    
	    Actions: set epsilon on conductor     
	    */
	  
	  /* which end of conductor segment and then which end of die 
	     determine epsilon value to use */
	  if(ip & IP_I1C0)
	  {
	    if(ip & IP_I1D0) segment->epsilon[0] = dieseg->epsilonplus;
	    segment->epsilon[0] = dieseg->epsilonminus;
	  }
	  else  /* ip & IP_I1C1 */
	  {
	    if(ip & IP_I1D1) segment->epsilon[1] = dieseg->epsilonplus;
	    segment->epsilon[1] = dieseg->epsilonminus;
	  }
	  break;
	  
	case 13 :
	  /*
	    Tangential terminating at endpoint
	    die both sides                        
	    
	    Actions: set epsilon on conductor     
	    */
	  /* determine which way die segment is going by testing angle 
	     from normal to segment (initial to terminal) */
	  angle_to_normal =
	    nmmtl_cirseg_angle_to_normal(segment,&intersection1,&dseg,0);
	  if(angle_to_normal < 0.0)
	  {
	    /* initial endpoint of die is left turn from normal */
	    if(ip & IP_I1C0) segment->epsilon[0] = dieseg->epsilonminus; 
	    else segment->epsilon[1] = dieseg->epsilonplus; 
	  }
	  else
	  {
	    /* initial endpoint of die is right turn from normal */
	    if(ip & IP_I1C0) segment->epsilon[0] = dieseg->epsilonplus; 
	    else segment->epsilon[1] = dieseg->epsilonminus; 
	  }
	  
	  
	  break;
	  
	case 14 :
	  /*
	    Tangential terminating at endpoint
	    die "inside"                          
	    
	    Actions: Nothing                      
	    */
	  
	  break;
	  
	case 15 :
	  /*
	    Chordal on conductor endpoints    
	    
	    Actions:                              
	    
	    set epsilons.                         
	    
	    
	    middle part of die removed.  New piece
	    created.                              
	    */
	  
	  /* Create new dielectric segment by splitting the old one. */
	  new_ds = 
	    (DIELECTRIC_SEGMENTS_P)malloc(sizeof(DIELECTRIC_SEGMENTS));
	  new_ds->next = dieseg->next;
	  dieseg->next = new_ds;
	  new_ds->at = dieseg->at;
	  new_ds->end = dieseg->end;
	  new_ds->epsilonplus = dieseg->epsilonplus;
	  new_ds->epsilonminus = dieseg->epsilonminus;
	  new_ds->segment_number = dieseg->segment_number;
	  new_ds->end_in_conductor = dieseg->end_in_conductor;
	  new_ds->orientation = dieseg->orientation;
	  /* neither ends in a conductor - so clear the bits */
	  dieseg->end_in_conductor &= 0XFFFD;  /* clear bit 2 */
	  new_ds->end_in_conductor &= 0XFFFE;  /* clear bit 1 */
	  
	  if(ip & IP_I1C0)
	  {
	    
	    /* intersection1 is first */
	    
	    /* which end of die goes inside ? */
	    angle_to_normal =
	      nmmtl_cirseg_angle_to_normal(segment,&intersection1,&dseg,0);
	    if(fabs(angle_to_normal) < PI/2)
	    {
	      /* terminal end of die is closest to intersection1 */
	      segment->epsilon[0] = dieseg->epsilonplus;
	      segment->epsilon[1] = dieseg->epsilonplus;
	      if(dieseg->orientation == VERTICAL_ORIENTATION)
	      {
		new_ds->start = intersection1.y;
		dieseg->end = intersection2.y;
	      }
	      else
	      {
		new_ds->start = intersection1.x;
		dieseg->end = intersection2.x;
	      }
	      
	    }
	    else
	    {
	      /* initial end of die is closest to intersection1 */
	      segment->epsilon[0] = dieseg->epsilonminus;
	      segment->epsilon[1] = dieseg->epsilonminus;
	      if(dieseg->orientation == VERTICAL_ORIENTATION)
	      {
		new_ds->start = intersection2.y;
		dieseg->end = intersection1.y;
	      }
	      else
	      {
		new_ds->start = intersection2.x;
		dieseg->end = intersection1.x;
	      }
	    }
	  }
	  else /* ip & IP_I2C0 */
	  {
	    /* intersection2 is first */
	    
	    /* which end of die goes inside ? */
	    angle_to_normal =
	      nmmtl_cirseg_angle_to_normal(segment,&intersection2,&dseg,0);
	    if(fabs(angle_to_normal) < PI/2)
	    {
	      /* from intersection2, initial end of die is inside conductor */
	      segment->epsilon[0] = dieseg->epsilonplus;
	      segment->epsilon[1] = dieseg->epsilonplus;
	      if(dieseg->orientation == VERTICAL_ORIENTATION)
	      {
		new_ds->start = intersection2.y;
		dieseg->end = intersection1.y;
	      }
	      else
	      {
		new_ds->start = intersection2.x;
		dieseg->end = intersection1.x;
	      }
	      
	    }
	    else
	    {
	      /* from intersection1, initial end of die is outside conductor */
	      segment->epsilon[0] = dieseg->epsilonminus;
	      segment->epsilon[1] = dieseg->epsilonminus;
	      if(dieseg->orientation == VERTICAL_ORIENTATION)
	      {
		new_ds->start = intersection1.y;
		dieseg->end = intersection2.y;
	      }
	      else
	      {
		new_ds->start = intersection1.x;
		dieseg->end = intersection2.x;
	      }
	    }
	  }
	  
	  
	  /* finally, compute the redistribution of divisions based on
	     length */
	  new_ds->length = new_ds->end - new_ds->start;
	  new_ds->divisions = (int)(dieseg->divisions * 
	    (new_ds->length/dieseg->length) + 1.0);
	  dieseg->length -= new_ds->length;
	  dieseg->divisions -= (new_ds->divisions - 1);
	  
	  
	  break;
	  
	case 16 :
	  /*
	    Chordal on conductor endpoints    
	    and one die endpoint              
	    Actions:                              
	    
	    set epsilons.                         
	    
	    
	    middle part of die removed.  Shrink   
	    leftover.                             
	    */
	  
	  if(ip & IP_I1D0)
	  {
	    /* terminal end of die is overhanging beyond intersection2 */
	    if(dieseg->orientation == VERTICAL_ORIENTATION)
	      dieseg->start = intersection2.y;
	    else
	      dieseg->start = intersection2.x;
	    
	    /* which side is conductor ? */
	    
	    if(ip & IP_I1C0)
	    {
	      segment->epsilon[0] = dieseg->epsilonminus;
	      segment->epsilon[1] = dieseg->epsilonminus;
	    }
	    else /* ip & IP_I1C1 */
	    {
	      segment->epsilon[0] = dieseg->epsilonplus;
	      segment->epsilon[1] = dieseg->epsilonplus;
	    }
	  }
	  else if(ip & IP_I2D0)
	  {
	    /* terminal end of die is overhanging beyond intersection1 */
	    if(dieseg->orientation == VERTICAL_ORIENTATION)
	      dieseg->start = intersection1.y;
	    else
	      dieseg->start = intersection1.x;
	    
	    /* which side is conductor ? */
	    
	    if(ip & IP_I2C0)
	    {
	      segment->epsilon[0] = dieseg->epsilonminus;
	      segment->epsilon[1] = dieseg->epsilonminus;
	    }
	    else /* ip & IP_I2C1 */
	    {
	      segment->epsilon[0] = dieseg->epsilonplus;
	      segment->epsilon[1] = dieseg->epsilonplus;
	    }
	  }
	  else if(ip & IP_I1D1)
	  {
	    /* initial end of die is overhanging beyond intersection2 */
	    if(dieseg->orientation == VERTICAL_ORIENTATION)
	      dieseg->end = intersection2.y;
	    else
	      dieseg->end = intersection2.x;
	    
	    /* which side is conductor ? */
	    
	    if(ip & IP_I1C0)
	    {
	      segment->epsilon[0] = dieseg->epsilonplus;
	      segment->epsilon[1] = dieseg->epsilonplus;
	    }
	    else /* ip & IP_I1C1 */
	    {
	      segment->epsilon[0] = dieseg->epsilonminus;
	      segment->epsilon[1] = dieseg->epsilonminus;
	    }
	  }
	  else /* ip & IP_I2D1 */
	  {
	    /* initial end of die is overhanging beyond intersection1 */
	    if(dieseg->orientation == VERTICAL_ORIENTATION)
	      dieseg->end = intersection1.y;
	    else
	      dieseg->end = intersection1.x;
	    
	    /* which side is conductor ? */
	    
	    if(ip & IP_I2C0)
	    {
	      segment->epsilon[0] = dieseg->epsilonplus;
	      segment->epsilon[1] = dieseg->epsilonplus;
	    }
	    else /* ip & IP_I2C1 */
	    {
	      segment->epsilon[0] = dieseg->epsilonminus;
	      segment->epsilon[1] = dieseg->epsilonminus;
	    }
	  }
	  
	  /* finally, compute the redistribution of divisions based on
	     ratio of new length to old length */
	  
	  dieseg->divisions = (int)(dieseg->divisions * 
	    ((dieseg->end - dieseg->start)/dieseg->length) + 1.0);
	  dieseg->length = dieseg->end - dieseg->start;
	  
	  /* flag that we changed the dieseg and dseg needs to be 
	     recomputed */
	  new_ds = (DIELECTRIC_SEGMENTS_P)1;
	  
	  break;
	  
	case 17 :
	  /*
	    Chordal on conductor endpoints    
	    and both die endpoints            
	    
	    Actions: die removed                  
	    */
	  
	  /* remove die */
	  if(last_dieseg != NULL)
	  {
	    last_dieseg->next = dieseg->next; /* bypass on list */
	    free(dieseg);
	    dieseg = last_dieseg->next;
	  }
	  else
	  {
	    /* removing first element on the list */
	    *dielectric_segments = (*dielectric_segments)->next;
	    free(dieseg);
	    dieseg = *dielectric_segments; 
	  }
	  break_out_of_conductor_loop = TRUE;
	  break;
	  
	case 18 :
	  /*
	    Chordal on one conductor endpoint 
	    and no die endpoints              
	    
	    Actions:                              
	    
	    die piece added, original shrunk to   
	    remove middle section                 
	    
	    fracture circle segment, epsilon values set                    
	    */
	  /* create new conductor circle segment */
	  new_cs = (CIRCLE_SEGMENTS_P) malloc(sizeof(CIRCLE_SEGMENTS));
	  new_cs->centerx = segment->centerx;
	  new_cs->centery = segment->centery;
	  new_cs->radius = segment->radius;
	  new_cs->conductor = segment->conductor;
	  new_cs->endangle = segment->endangle;
	  new_cs->epsilon[1] = segment->epsilon[1];
	  /* hook into the list */
	  new_cs->next = segment->next;
	  segment->next = new_cs;
	  
	  /* Create new dielectric segment by splitting the old one. */
	  new_ds = 
	    (DIELECTRIC_SEGMENTS_P)malloc(sizeof(DIELECTRIC_SEGMENTS));
	  new_ds->next = dieseg->next;
	  dieseg->next = new_ds;
	  new_ds->at = dieseg->at;
	  new_ds->end = dieseg->end;
	  
	  new_ds->epsilonplus = dieseg->epsilonplus;
	  new_ds->epsilonminus = dieseg->epsilonminus;
	  new_ds->segment_number = dieseg->segment_number;
	  new_ds->end_in_conductor = dieseg->end_in_conductor;
	  new_ds->orientation = dieseg->orientation;
	  /* neither ends in a conductor - so clear the bits */
	  dieseg->end_in_conductor &= 0XFFFD;  /* clear bit 2 */
	  new_ds->end_in_conductor &= 0XFFFE;  /* clear bit 1 */
	  
	  /* find the angle of the intersections */
	  inter_angle[0] =
	    nmmtl_cirseg_point_angle(segment,intersection1.x,intersection1.y);
	  inter_angle[1] =
	    nmmtl_cirseg_point_angle(segment,intersection2.x,intersection2.y);
	  
	  /* which intersection was the one at the conductor endpoint? */
	  if(ip & (IP_I1C0 | IP_I1C1))
	  {
	    /* intersection1 is at conductor endpoint, intersection2 
	       splits the circle segment */
	    
	    /* set up the coordinates */
	    segment->endangle = inter_angle[1];
	    new_cs->startangle = inter_angle[1];
	    
	    /* which end of die goes inside from intersection2 ? */
	    angle_to_normal =
	      nmmtl_cirseg_angle_to_normal(segment,&intersection2,&dseg,0);
	    if(fabs(angle_to_normal) < PI/2)
	    {
	      /* from intersection2, initial end of die is inside conductor */
	      
	      if(dieseg->orientation == VERTICAL_ORIENTATION)
	      {
		new_ds->start = intersection2.y;
		dieseg->end = intersection1.y;
	      }
	      else
	      {
		new_ds->start = intersection2.x;
		dieseg->end = intersection1.x;
	      }
	      
	      segment->epsilon[1] = dieseg->epsilonminus;
	      new_cs->epsilon[0] = dieseg->epsilonplus;
	      
	      /* which side of conductor circle segment is hit? */
	      if(ip & IP_I1C0)
	      {
		/* initial side of conductor is at intersection */
		segment->epsilon[0] = dieseg->epsilonminus;
	      }
	      else
	      {
		/* terminal side of conductor is at intersection */
		new_cs->epsilon[1] = dieseg->epsilonplus;
	      }
	    }
	    else
	    {
	      /* from intersection2, initial end of die is outside conductor */
	      
	      if(dieseg->orientation == VERTICAL_ORIENTATION)
	      {
		new_ds->start = intersection1.y;
		dieseg->end = intersection2.y;
	      }
	      else
	      {
		new_ds->start = intersection1.x;
		dieseg->end = intersection2.x;
	      }
	      
	      segment->epsilon[1] = dieseg->epsilonplus;
	      new_cs->epsilon[0] = dieseg->epsilonminus;
	      
	      /* which side of conductor circle segment is hit? */
	      if(ip & IP_I1C0)
	      {
		/* initial side of conductor is at intersection */
		segment->epsilon[0] = dieseg->epsilonplus;
	      }
	      else
	      {
		/* terminal side of conductor is at intersection */
		new_cs->epsilon[1] = dieseg->epsilonminus;
	      }
	    }
	  }
	  else /* ip & (IP_I2C0 | IP_I2C1) */
	  {
	    /* intersection2 is at conductor endpoint, intersection1 
	       splits the circle segment */
	    
	    /* set up the coordinates */
	    segment->endangle = inter_angle[0];
	    new_cs->startangle = inter_angle[0];
	    
	    /* which end of die goes inside from intersection1 ? */
	    angle_to_normal =
	      nmmtl_cirseg_angle_to_normal(segment,&intersection1,&dseg,0);
	    if(fabs(angle_to_normal) < PI/2)
	    {
	      /* from intersection1, initial end of die is inside conductor */
	      
	      if(dieseg->orientation == VERTICAL_ORIENTATION)
	      {
		new_ds->start = intersection1.y;
		dieseg->end = intersection2.y;
	      }
	      else
	      {
		new_ds->start = intersection1.x;
		dieseg->end = intersection2.x;
	      }
	      
	      segment->epsilon[1] = dieseg->epsilonminus;
	      new_cs->epsilon[0] = dieseg->epsilonplus;
	      
	      /* which side of conductor circle segment is hit? */
	      if(ip & IP_I2C0)
	      {
		/* initial side of conductor is at intersection */
		segment->epsilon[0] = dieseg->epsilonminus;
	      }
	      else
	      {
		/* terminal side of conductor is at intersection */
		new_cs->epsilon[1] = dieseg->epsilonplus;
	      }
	    }
	    else
	    {
	      /* from intersection1, initial end of die is outside conductor */
	      
	      if(dieseg->orientation == VERTICAL_ORIENTATION)
	      {
		new_ds->start = intersection2.y;
		dieseg->end = intersection1.y;
	      }
	      else
	      {
		new_ds->start = intersection2.x;
		dieseg->end = intersection1.x;
	      }
	      
	      segment->epsilon[1] = dieseg->epsilonplus;
	      new_cs->epsilon[0] = dieseg->epsilonminus;
	      
	      /* which side of conductor circle segment is hit? */
	      if(ip & IP_I1C0)
	      {
		/* initial side of conductor is at intersection */
		segment->epsilon[0] = dieseg->epsilonplus;
	      }
	      else
	      {
		/* terminal side of conductor is at intersection */
		new_cs->epsilon[1] = dieseg->epsilonminus;
	      }
	    }
	  }
	  
	  /* finally, compute the redistribution of divisions based on
	     length and angle */
	  new_ds->length = new_ds->end - new_ds->start;
	  new_ds->divisions = (int)(dieseg->divisions * 
	    (new_ds->length/dieseg->length) + 1.0);
	  dieseg->divisions -= (new_ds->divisions - 1);
	  dieseg->length -= new_ds->length;
	  
	  new_cs->radians = new_cs->endangle - new_cs->startangle;
	  new_cs->divisions = (int)(segment->divisions *
	    (new_cs->radians / segment->radians) + 1.0);
	  segment->divisions += 1 - new_cs->divisions;
	  segment->radians -= new_cs->radians;
	  
	  break;
	  
	case 19 :
	  /*
	    Chordal on one conductor endpoint 
	    and one die endpoint              
	    
	    Actions: die removed by shrinking     
	    the original                       
	    
	    Epsilon set.                          
	    */
	  if(ip & IP_I1D0)
	  {
	    /* reduce die segment size by moving ending point to 
	       intersection2 */
	    if(vert_die) dieseg->end = intersection2.y;
	    else dieseg->end = intersection2.x;
	    if(ip & IP_I2C0)
	      segment->epsilon[0] = dieseg->epsilonplus;
	    else /* ip & IP_I2C1 */
	      segment->epsilon[1] = dieseg->epsilonminus;
	  }
	  else if(ip & IP_I1D1)
	  {
	    /* reduce die segment size by moving starting point to 
	       intersection2 */
	    if(vert_die) dieseg->start = intersection2.y;
	    else dieseg->start = intersection2.x;
	    if(ip & IP_I2C0)
	      segment->epsilon[1] = dieseg->epsilonminus;
	    else /* ip & IP_I2C1 */
	      segment->epsilon[0] = dieseg->epsilonplus;
	  }
	  else if(ip & IP_I2D0)
	  {
	    /* reduce die segment size by moving ending point to 
	       intersection1 */
	    if(vert_die) dieseg->end = intersection1.y;
	    else dieseg->end = intersection1.x;
	    if(ip & IP_I1C1)
	      segment->epsilon[1] = dieseg->epsilonminus;
	    else /* ip & IP_I1C0 */
	      segment->epsilon[0] = dieseg->epsilonplus;
	  }
	  else /* if(ip & IP_I2D1) */
	  {
	    /* reduce die segment size by moving starting point to 
	       intersection1 */
	    if(vert_die) dieseg->start = intersection1.y;
	    else dieseg->start = intersection1.x;
	    if(ip & IP_I1C1)
	      segment->epsilon[1] = dieseg->epsilonplus;
	    else /* ip & IP_I1C0 */
	      segment->epsilon[0] = dieseg->epsilonminus;
	  }
	  
	  /* adjust divisions and set new length */
	  dieseg->divisions = (int)(dieseg->divisions *
	    (dieseg->end - dieseg->start)/dieseg->length + 1.0);
	  dieseg->length = dieseg->end - dieseg->start;
	  
	  /* flag that we changed the dieseg and dseg needs to be 
	     recomputed */
	  new_ds = (DIELECTRIC_SEGMENTS_P)1;
	  
	  break;
	  
	case 20 :
	  /*
	    Chordal on conductor endpoints    
	    and both die endpoints            
	    
	    Actions: die removed                  
	    */
	  
	  /* remove die */
	  
	  if(last_dieseg != NULL)
	  {
	    last_dieseg->next = dieseg->next; /* bypass on list */
	    free(dieseg);
	    dieseg = last_dieseg->next;
	  }
	  else
	  {
	    /* removing first element on the list */
	    *dielectric_segments = (*dielectric_segments)->next;
	    free(dieseg);
	    dieseg = *dielectric_segments; 
	  }
	  break_out_of_conductor_loop = TRUE;
	  break;
	  
	case 21 :
	  /*
	    Chordal intersection on one conductor endpoint 
	    and different intersection on one die endpoint              
	    
	    Actions: die removed by shrinking     
	    the original                       
	    
	    fracture circle segment, epsilon values set                    
	    
	    */
	  
	  /* create new conductor circle segment */
	  new_cs = (CIRCLE_SEGMENTS_P) malloc(sizeof(CIRCLE_SEGMENTS));
	  new_cs->centerx = segment->centerx;
	  new_cs->centery = segment->centery;
	  new_cs->radius = segment->radius;
	  new_cs->conductor = segment->conductor;
	  new_cs->endangle = segment->endangle;
	  new_cs->epsilon[1] = segment->epsilon[1];
	  /* hook into the list */
	  new_cs->next = segment->next;
	  segment->next = new_cs;
	  
	  if( ip & (IP_I1D0 | IP_I1D1) )
	  {
	    /* set up the coordinates */
	    intersection_angle =
	      nmmtl_cirseg_point_angle(segment,intersection2.x,
				       intersection2.y);
	    if(ip & IP_I1D0)
	    {
	      /* reduce die segment size by moving starting point to 
		 intersection2 */
	      if(vert_die) dieseg->start = intersection2.y;
	      else dieseg->start = intersection2.x;
	      segment->epsilon[1] = dieseg->epsilonminus;
	      new_cs->epsilon[0] = dieseg->epsilonplus;
	    }
	    else
	    {
	      /* reduce die segment size by moving ending point to 
		 intersection2 */
	      if(vert_die) dieseg->end = intersection2.y;
	      else dieseg->end = intersection2.x;
	      segment->epsilon[1] = dieseg->epsilonplus;
	      new_cs->epsilon[0] = dieseg->epsilonminus;
	    }
	    
	  }
	  else /* ip & (IP_I2D0 | IP_I2D1) */
	  {
	    /* set up the coordinates */
	    intersection_angle =
	      nmmtl_cirseg_point_angle(segment,intersection1.x,
				       intersection1.y);
	    if(ip & IP_I2D0)
	    {
	      /* reduce die segment size by moving starting point to 
		 intersection1 */
	      if(vert_die) dieseg->start = intersection1.y;
	      else dieseg->start = intersection1.x;
	      segment->epsilon[1] = dieseg->epsilonminus;
	      new_cs->epsilon[0] = dieseg->epsilonplus;
	    }
	    else
	    {
	      /* reduce die segment size by moving ending point to 
		 intersection1 */
	      if(vert_die) dieseg->end = intersection1.y;
	      else dieseg->end = intersection1.x;
	      segment->epsilon[1] = dieseg->epsilonplus;
	      new_cs->epsilon[0] = dieseg->epsilonminus;
	    }
	  }
	  
	  /* adjust divisions and set new length */
	  dieseg->divisions = (int)(dieseg->divisions *
	    (dieseg->end - dieseg->start)/dieseg->length + 1.0);
	  dieseg->length = dieseg->end - dieseg->start;
	  
	  segment->endangle = intersection_angle;
	  new_cs->startangle = intersection_angle;
	  new_cs->radians = new_cs->endangle - new_cs->startangle;
	  new_cs->divisions = (int)(segment->divisions *
	    (new_cs->radians / segment->radians) + 1.0);
	  segment->divisions += 1 - new_cs->divisions;
	  segment->radians -= new_cs->radians;
	  
	  /* flag that we changed the dieseg and dseg needs to be 
	     recomputed */
	  new_ds = (DIELECTRIC_SEGMENTS_P)1;
	  
	  break;
	  
	  default :
	  {
	    
	    fprintf(stderr,"ELECTRO-F-INTERNAL Internal error:  checking intersections between conductors and dielectrics; Choices for circle segment/die intersection \
types fell through\n");
	    return(FAIL);
	  }
	  
	}                          /* switch on intersection type */  
	
      }                            /* if there is an intersection */
      
      last_segment = segment;
      segment = segment->next;
    }                                /* while looping through the segments */
    
    if(break_out_of_conductor_loop == FALSE)
    {
      last_dieseg = dieseg;
      dieseg = dieseg->next;
    }
    
  }                              /* looping on the dielectric segs */
  return(SUCCESS);
  
}
