/*
  
  FACILITY:  NMMTL
  
  MODULE DESCRIPTION:
  
  Dump (diagnostic output) of various geometry structures.
  
  AUTHORS:
  
  Kevin J. Buchs
  
  CREATION DATE:  Tue Nov 26 14:16:32 1991
  
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


extern FILE *dump_file;


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
  
  FUNCTION NAME: nmmtl_dump_geometry
  
  FUNCTIONAL DESCRIPTION:
  
  Dump the geometry after reading it from the graphic file.
  
  
  FORMAL PARAMETERS:
  
  
  int cntr_seg
  int pln_seg
  float coupling
  float risetime
  float conductivity
  float frequency
  float half_minimum_dimension
  int gnd_planes
  float top_ground_plane_thickness
  float bottom_ground_plane_thickness
  struct dielectric *dielectrics
  struct contour *signals
  struct contour *groundwires
  
  */



void nmmtl_dump_geometry(int cntr_seg,int pln_seg,
			 float coupling,float risetime,
			 float the_conductivity,float frequency,
			 float half_minimum_dimension,
			 int gnd_planes,
			 float top_ground_plane_thickness,
			 float bottom_ground_plane_thickness,
			 struct dielectric *dielectrics,
			 struct contour *signals,
			 struct contour *groundwires)
{
  struct contour *last;
  float conductivity = the_conductivity;

  fprintf(dump_file,"\n\n\t\tDUMP OF GEOMETRY\n\n");
  fprintf(dump_file,
	  "cntr_seg=%d, pln_seg=%d, coupling=%g, risetime=%g\n",
	  cntr_seg,pln_seg,coupling,risetime);
  fprintf(dump_file,"conductivity=%g, frequency=%g, half min dim=%g\n",
	  conductivity,frequency,half_minimum_dimension);
  fprintf(dump_file,
	  "%d ground planes, top thickness=%g, bottom thickness=%g\n\n",
	  gnd_planes,top_ground_plane_thickness,bottom_ground_plane_thickness);
  
  fprintf(dump_file,"SIGNAL CONDUCTORS:\n\n");
  for(;signals != NULL;signals = signals->next)
  {

      if ( signals->conductivity != 0.0 )
	conductivity = signals->conductivity;

     switch(signals->primitive)
    {
    case RECTANGLE :
      nmmtl_dump_rectangle(signals);
      break;
    case POLYGON :
      nmmtl_dump_polygon(signals);
      break;
    case CIRCLE :
      nmmtl_dump_circle(signals);
      break;
    }
    fprintf(dump_file,"\n");
  }
  fprintf(dump_file,"\n");
  
  fprintf(dump_file,"GROUND CONDUCTORS:\n\n");
  for(;groundwires != NULL;groundwires = groundwires->next)
  {
    switch(groundwires->primitive)
    {
    case RECTANGLE :
      nmmtl_dump_rectangle(groundwires);
      break;
    case POLYGON :
      nmmtl_dump_polygon(groundwires);
      break;
    case CIRCLE :
      nmmtl_dump_circle(groundwires);
      break;
    }
    fprintf(dump_file,"\n");
  }
  fprintf(dump_file,"\n");
  
  last = NULL;
  
  fprintf(dump_file,"DIELECTRICS:\n\n");
  for (struct dielectric *die = dielectrics; die != NULL; die = die->next)
  {
    fprintf(dump_file,"\tlow left = (%g,%g), up right = (%g,%g)\n",
	    die->x0,
	    die->y0,die->x1,die->y1);
    fprintf(dump_file,"\tconstant=%g, tangent=%g\n\n",die->constant,
	    die->tangent);
  }
  fprintf(dump_file,"\n");
  
}



/*
  
  FUNCTION NAME:  nmmtl_dump_polygon
  
  
  FUNCTIONAL DESCRIPTION:
  
  Dump out the characteristics of a polygonal contour.
  
  FORMAL PARAMETERS:
  
  CONTOURS_P contour
  
  RETURN VALUE:
  
  None
  
  CALLING SEQUENCE:
  
  nmmtl_dump_polygon(this_contour);
  
  */

void nmmtl_dump_polygon(CONTOURS_P contour)
{
  struct polypoints *pp;
  
  fprintf(dump_file,
	  "%c %s %s\n",contour->primitive,contour->name,"polygon");
  fprintf(dump_file,"\tlowest y = %g, highest y = %g, perimeter = %g\n",
	  contour->y0,contour->y1,contour->x0);
  fprintf(dump_file,"\tpoints:\n");
  for(pp=contour->points;pp != NULL; pp = pp->next)
  {
    fprintf(dump_file,"\t\t%g,%g\n",pp->x,pp->y);
  }
}



/*
  
  FUNCTION NAME:  nmmtl_dump_rectangle
  
  FUNCTIONAL DESCRIPTION:
  
  Dump out the characteristics of a rectangular contour
  
  FORMAL PARAMETERS:
  
  CONTOURS_P contour
  
  RETURN VALUE:
  
  None
  
  CALLING SEQUENCE:
  
  nmmtl_dump_rectangle(this_contour);
  
  */


void nmmtl_dump_rectangle(CONTOURS_P contour)
{
  
  fprintf(dump_file,"%c %s %s\n",contour->primitive,contour->name,
	  "rectangle");
  fprintf(dump_file,
	  "\tlow left = (%g,%g), up right = (%g,%g)\n",contour->x0,
	  contour->y0,contour->x1,contour->y1);
}



/*
  
  FUNCTION NAME:  nmmtl_dump_circle
  
  FUNCTIONAL DESCRIPTION:
  
  Dump out the characteristics of a rectangular contour
  
  FORMAL PARAMETERS:
  
  CONTOURS_P contour
  
  RETURN VALUE:
  
  None
  
  CALLING SEQUENCE:
  
  nmmtl_dump_circle(this_contour);
  
  */


void nmmtl_dump_circle(CONTOURS_P contour)
{
  
  fprintf(dump_file,"%c %s %s\n",contour->primitive,
	  contour->name,"circle");
  fprintf(dump_file,"\tcenter=(%g,%g), radius=%g\n",contour->x0,
	  contour->y0,contour->x1);
}

