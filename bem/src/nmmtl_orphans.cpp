
/*
  
  FACILITY:  NMMTL
  
  MODULE DESCRIPTION:
  
  Contains:
  
  nmmtl_orphans
  
  AUTHOR(S):
  
  Kevin J. Buchs
  
  CREATION DATE:  Wed Feb  5 10:44:48 1992
  
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
  
  FUNCTION NAME:  nmmtl_orphans
  
  
  FUNCTIONAL DESCRIPTION:
  
  Find the orphaned conductor and dielectric segments and 
  process them accordingly.  The conductor segments which are completely
  contained within a dielectric rectangle will not get their epsilon values
  assigned any other way.  There may be dielectric segments which have both
  ends in a conductor - or even one end in a conductor and thus are really
  non-existant - an artifact.
  
  
  FORMAL PARAMETERS:
  
  CIRCLE_SEGMENTS_P *conductor_cs,     circle segments
  LINE_SEGMENTS_P *conductor_ls,       line segments
  DIELECTRICS_P dielectrics,           original die rectangles
  double air_starts,                   Y coordinate of highest dielectric
                                         - where air starts
  DIELECTRIC_SEGMENTS_P *dielectric_segments  die segments
  
  RETURN VALUE:   SUCCESS OR FAIL
  
  CALLING SEQUENCE:
  
  status = nmmtl_orphans(conductor_cs,conductor_ls,dielectrics,
                         dielectric_segments);
  
  */

int nmmtl_orphans(CIRCLE_SEGMENTS_P *conductor_cs,
			      LINE_SEGMENTS_P *conductor_ls,
			      DIELECTRICS_P dielectrics,
			      double air_starts,
			      DIELECTRIC_SEGMENTS_P *dielectric_segments)
{
  
  register CIRCLE_SEGMENTS_P cs;
  register LINE_SEGMENTS_P ls;
  POINT center;                    /* this can't be register as per DEC C */
  register double radius;
  LINESEG LS;                      /* this can't be register as per DEC C */
  register DIELECTRICS_P die;
  DIELECTRIC_SEGMENTS_P ds, ds_temp;
  char msg[256];
  
  
  
  
  /* first look for orphaned conductor line segments - both epsilon
     values are zero */
  
  ls = *conductor_ls;
  while(ls != NULL)
  {
    if(ls->epsilon[0] == 0.0F && ls->epsilon[1] == 0.0F)
    {
      /* is there no top ground plane and is the line segment
	 above the dielectric layers, sitting in air? */
      /* broaden this Fri Oct 18 19:41:48 1996 -- Kevin J. Buchs to include
         segments which have one end on the air start y value, like one 
	 end of a rectangle or trapezoid.  We won't expect the case where
	 both ends are at that y value, since that segment would have been
	 pulled out previously as matching the die boundary, but even if we 
	 do get it here, there will be no problem.
	 */
      if(air_starts != 0.0 && ls->endy >= air_starts && ls->starty >= air_starts)
      {
	ls->epsilon[0] = AIR_CONSTANT;
	ls->epsilon[1] = AIR_CONSTANT;
      }
      else
      {
	
	/* this ls is a candidate for complete containment in a die
	   rectangle */
	
	LS.x[0] = ls->startx;
	LS.y[0] = ls->starty;
	LS.x[1] = ls->endx;
	LS.y[1] = ls->endy;
	
	die = dielectrics;
	
	/* check against all the die rectangles */
	while(die != NULL)
	{
	  if(nmmtl_seg_in_die_rect(die,&LS))
	  {
	    /* found a die rectangle that encloses this line segment -
	       set epsilon and break loop */
	    ls->epsilon[0] = die->constant;
	    ls->epsilon[1] = die->constant;
	    break;
	  }
	  die = die->next;
	}
	
	/* we will have broken to here, unless we did not find the enclosing
	   die region - then, die == NULL and this is an error */
	if(die == NULL)
	{
	  printf("ELECTRO-W-ORPHAN_CS Cannot find dielectric constant for conductor line segment over (%f,%f) to (%f,%f).  Setting to AIR.\n",
		  ls->startx,ls->starty,ls->endx,ls->endy);
	  ls->epsilon[0] = AIR_CONSTANT;
	  ls->epsilon[1] = AIR_CONSTANT;
	  
	}       /* if didn't find an enclosing die */
	
      }         /* if conductor segment not above die stack */
      
    }           /* if conductor has both epsilons set to zero */
    
    
    /* fill in any missing blanks such that everything is consistent */
    /* at this point, both epsilons will no longer be zero, set which
       ever one is still zero, if either */
    
    if(ls->epsilon[0] == 0.0F) ls->epsilon[0] = ls->epsilon[1];
    if(ls->epsilon[1] == 0.0F) ls->epsilon[1] = ls->epsilon[0];
    
    ls = ls->next;
    
  }             /* loop through conductor segments */
  
  /* next look for orphaned conductor circle segments - both epsilon
     values are zero and the full circle is intact */
  
  cs = *conductor_cs;
  while(cs != NULL)
  {
    if(cs->epsilon[0] == 0.0F && cs->epsilon[1] == 0.0F)
    {
      /* is there no top ground plane and is the circle
	 above the dielectric layers, sitting in air? */
      if(air_starts != 0.0 && cs->centery - cs->radius > air_starts)
      {
	cs->epsilon[0] = AIR_CONSTANT;
	cs->epsilon[1] = AIR_CONSTANT;
      }
      else
      {
	
	/* this cs is a candidate for complete containment in a die
	   rectangle */
	
	center.x = cs->centerx;
	center.y = cs->centery;
	radius = cs->radius;
	
	die = dielectrics;
	
	/* check against all the die rectangles */
	while(die != NULL)
	{
	  if(nmmtl_circle_in_die_rect(die,&center,radius))
	  {
	    /* found a die rectangle that encloses this line segment -
	       set epsilon and break loop */
	    cs->epsilon[0] = die->constant;
	    cs->epsilon[1] = die->constant;
	    break;
	  }
	  die = die->next;
	}
	
	/* we will have broken to here, unless we did not find the enclosing
	   die region - then, die == NULL and this is an error */
	if(die == NULL)
	{
	  printf("ELECTRO-W-ORPHAN_CS Cannot find dielectric constant for conductor circle segment centered at (%f,%f).  Setting to AIR.\n",cs->centerx,cs->centery);
	  cs->epsilon[0] = AIR_CONSTANT;
	  cs->epsilon[1] = AIR_CONSTANT;
	  
	}       /* if didn't find an enclosing die */
	
      }         /* if conductor circle not above die stack */
      
    }           /* if conductor has both epsilons set to zero */
    

    /* fill in any missing blanks such that everything is consistent */
    /* at this point, both epsilons will no longer be zero, set which
       ever one is still zero, if either */

    if(cs->epsilon[0] == 0.0F) cs->epsilon[0] = cs->epsilon[1];
    if(cs->epsilon[1] == 0.0F) cs->epsilon[1] = cs->epsilon[0];

    cs = cs->next;
    
  }             /* loop through conductor circles */
  
  
  /* Last, look through the dielectric segments to see if we can find
     any orphans there.  The orphan here would be when the segment has
     a non-zero end_in_conductor field, indicating that one or more ends
     were intersections with a conductor boundary and it was inside the
     conductor.
     */
  
  /* first deal with it at the beginning of the list */
  while(*dielectric_segments && (*dielectric_segments)->end_in_conductor)
  {
    /* remove first item on the list */
    ds_temp = *dielectric_segments;
    *dielectric_segments = ds_temp->next;
    free(ds_temp);
  }
  
  /* now deal with anywhere else on the list */
  ds = *dielectric_segments;
  if(ds != NULL)
  {
    while(ds->next)
    {
      if(ds->next->end_in_conductor)
      {
	ds_temp = ds->next;
	ds->next = ds->next->next;
	free(ds_temp);
      }
      else
      {
	ds = ds->next;
      }
    }
  }
  return(SUCCESS);
  
}
