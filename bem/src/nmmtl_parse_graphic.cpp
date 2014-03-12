
/*

   FACILITY:  NMMTL

   MODULE DESCRIPTION:

   Contains the global function nmmtl_parse_graphic, and
   the static functions which it calls:

   input_pin_attrib,
   input_icon_attrib,
   extrc_pts,
   find_pin,
   fill_contour,
   fill_dielectric

   AUTHOR(S):

   David Endry

   CREATION DATE:   1-1-86

   COPYRIGHT:   Copyright (C) 1986-92 by Mayo Foundation. All rights reserved.

   */

/*
 *******************************************************************
 **  INCLUDE FILES
 *******************************************************************
 */

#include "nmmtl.h"
#include "electro.h"

#include <string.h>
#include <ctype.h>


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

/*  for brain dead math.h implementations, like windows */
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/* color specific error messages */

#define COLOR_ERR(c)   (c==RED ? "red"   : c==GREEN ? "green"   : \
			c==YELLOW ? "yellow" : c==BLUE ? "blue" : \
			c==MAGENTA ? "magenta" : c==CYAN ? "cyan" : \
                        c==WHITE ? "white" : "unknown color")


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

int output (const char *fmt,...);

void header_c(FILE *fptr,char * program, char * ext);

static int input_pin_attrib(struct pins **front,FILE *in,int units);
static int input_icon_attrib(FILE *in,
					 int *cntr_seg,
					 int *pln_seg,
					 float *coupling,
					 float *risetime,
					 float *conductivity,
					 float *frequency,
					 float *top_ground_plane_thickness,
					 float *bottom_ground_plane_thickness,
					 int *units);

static int extrct_pts(char *line,struct contour *polygon,
				  int *color,
				  struct pins **front,
				  float *minimum_dimension);

static struct polypoints *find_pin(struct pins **front,
				   double x0, double y0);

static int fill_contour(struct contour *node, char prim,
	 			         char *line, int color,
				         FILE *in,int units, 
				         float *conductivity);

static int fill_dielectric(struct dielectric *node,char *line,
				       FILE *in,int units);

/*
 *******************************************************************
 **  FUNCTION DEFINITIONS
 *******************************************************************
 */


/* Very simple convert routine to save redundant code.  The idea is
comparable to nmmtl_convert_units.cxx and the switch cases in this
code.  This conversion is called by all the cross-sectional parsing
routines which parse the old style attributes.  Since the new
attributes assume meters, this conversion should convert from
whatever units into meters.  The default for the input of units from
the graphic file if none are specified is mils.  This is that
which compares to 2dly in the past and the recently enforced mils
units within 2dlf.  */

static void convert(int unitsflag, float &value)
{
  switch(unitsflag)
  {
  case UNITS_MILS :
    value *= MILS_TO_METERS;
    break;
  case UNITS_MICRONS :
    value *= MICRONS_TO_METERS;
    break;
  case UNITS_INCHES :
    value *= INCHES_TO_METERS;
    break;
  case UNITS_METERS :
    // Internally the program uses meters.    
    // value *= 1.0;	      /* no conversion needed */
    break;
  case UNITS_NO_UNITS :
    value *= MILS_TO_METERS; /* assume mils are the unknown unit */
    break;
  }
}

// also a version which uses a double and doesn't mangle it into a float

static void convert(int unitsflag, double &value)
{
  switch(unitsflag)
  {
  case UNITS_MILS :
    value *= MILS_TO_METERS;
    break;
  case UNITS_MICRONS :
    value *= MICRONS_TO_METERS;
    break;
  case UNITS_INCHES :
    value *= INCHES_TO_METERS;
    break;
  case UNITS_METERS :
    // Internally the program uses meters.    
    // value *= 1.0;	      /* no conversion needed */
    break;
  case UNITS_NO_UNITS :
    value *= MILS_TO_METERS; /* assume mils are the unknown unit */
    break;
  }
}

/*
   
   FUNCTION NAME:  nmmtl_parse_graphic()
   
   
   FUNCTIONAL DESCRIPTION:
   
   generate a data structure of subsections and dielectric constants
   around signals, grounds and dielectric interfaces represented by a 2D
   GPGE graphic cross section diagram of an electromagnetic system. These
   structures are then used to build a mesh of boundary elements in order
   to allow calculation of static EM parameters.  Separate linked lists
   are used for signal and ground wires but both are called contours (use
   contour data struct). Signal and ground wires may be gpge rectangles,
   circles or polygon primitives. Signal wires are always yellow, ground
   wires dark blue, ground planes must use the icon ground_plane (this is
   a dark blue rectangle). Only the number of ground planes are
   important. If 2 ground planes exist, points are generated along the
   top's bottom (two consecutive points make a segment). Dielectric
   layers are represented by hollow (not filled) gpge white or green
   rectangles. A linked list of dielectric layers is created, points
   along dielectric interfaces (line where two adjacent dielectric layers
   touch) are output.  Primitive attributes are used to define
   coordinates of signals, ground wires and dielectric layers making the
   diagram NOT_SCALE.  The old MMTL program implemented two types of
   drawings, a to-scale and a not-to-scale.  This program only implements
   a not-to-scale.  The attributes must be there.
   
   An attribute on the drawing indicates the units used, either meters,
   microns, mils, or inches.  Mils is assumed if this attribute is
   absent.
   
   If the user desires to use a transmission line x-section in a network
   simulation, an icon must be created.  If the icon exists, this program
   will match the signal names with the icon pin names and generate
   netlist for the icon.
   
   Some features have been ommitted.  Some of the code remains, blocked
   by ifdefs.  These are CHECK_SCALE and SORT_DIELECTRICS.  Some of the
   comments from omitted features:
   
   In the dielectric list, the rightmost link is the bottom layer
   (d_bottom) the left most is the top layer.
   .. otherwise gpge coordinates are used for the
   TO_SCALE primitives. There may be no mixed scale diagrams.
   
   
   FORMAL PARAMETERS:
   
   INPUTS:
   
   input file: node.graphic
   
   OUTPUTS:
   
   cntr_seg : number of contour segments (defaults to 6)
   pln_seg  : number of plane segments (defaults to 15)
   coupling : coupling length of transmission line in meters
   risetime : risetime at which to do the analysis in seconds
   conductivity : conductivity in mhos/meter
   half_minimum_dimension : half the smallest side or radius
   gnd_planes : the number of ground planes : 1 or 2
   top_ground_plane_thickness : in meters
   bottom_ground_plane_thickness : in meters
   dielectrics : pointer to linked list of dielectric layers
   signals  : pointer to the head of the signal linked list
   groundwires  : pointer to the head of the ground wire linked list
   num_signals : number of signal conductors
   num_grounds : number of ground conductors (including upper plane)
   units : the user-specified or default units for measurement
   
   FUNCTIONS CALLED:
   header_c, mcms_access, mcms_deaccess,
   get_error, filescan, get_node, get_design_dir
   
   
   RETURN VALUE:
   
   SUCCESS, FAIL
   
   
   CALLING SEQUENCE:
   
   status = nmmtl_parse_graphic(&cntr_seg,&pln_seg,&coupling,
   &risetime,
   &conductivity,&frequency,
   &half_minimum_dimension,&gnd_planes,
   &top_ground_plane_thickness,
   &bottom_ground_plane_thickness,
   &dielectrics,&signals,&groundwires,
   &num_signals,&num_grounds,&units);
   
   
   
   */

int nmmtl_parse_graphic(char *filename,
			int *cntr_seg,int *pln_seg,
			float *coupling,float *risetime,
			float *conductivity, float *frequency,
			float *half_minimum_dimension,int *gnd_planes,
			float *top_ground_plane_thickness,
			float *bottom_ground_plane_thickness,
			struct dielectric **dielectrics,
			struct contour **signals,
			struct contour **groundwires,
			int *num_signals,
			int *num_grounds,
			int *units)
{
  char line[GPGE_MAX];		/* input GPGE graphic file lines */
  char *fgets_status;
  char msg[1280];			/* custom string error messages */
  double x0, y0, x1, y1, t0;	/* temporary coordinates */
#ifdef CHECK_SCALE
  double radius;		/* calculated radius of circles */
#endif
  struct pins *p_front = NULL;	/* pointer to front of pin list */
  struct polypoints *t_att;	/* ptr for pin attribute list */
  struct dielectric *d_temp;	/* temps for manipulating lists */
                                /* to create, update lists */
  struct contour *c_temp,*c_prev;
  int color;			/* determine the kind of primitive */
  
#ifdef CHECK_SCALE
  int global_scale = NOT_SET;	/* globally TO_SCALE or NOT_SCALE */
  int local_scale;		/* for checking per primitive */
#endif
  
  int t1;	        	/* check FORTRAN dimens (# pieces) */
  int status;	/* function return status */
  FILE *in  = NULL;		/* node.GRAPHIC input file */
  NETLIST_REC *netlist;
  int version;			/* node version number for netlist */
  int auto_gen = 0;		/* for signal name generation */
  double offset = 1.0e20;	/* find offset of top of bottom ground plane */
                                /* initialize to some big number */
  
  double highest_dielectric = -1.0e20; /* the level of the top of the highest */
                                      /* dielectric, initialized to some very*/
                                      /* unlikely number. */
  float minimum_dimension = FLT_MAX;
  
  int upper_ground_planes = 0;  /* keep count of drawn ground planes */
  int lower_ground_planes = 0;
  double ground_x_min = DBL_MAX, ground_x_max = DBL_MIN;
  float c_conductivity = 0.0;

  char olddefault[20],response[20];
  char fullfilespec[1024];

  /* initialize the parameters set by icon attributes */
  
  //  *cntr_seg = 0;
  //  *pln_seg = 0;
  *coupling = 0.0;
  *risetime = 0.0;
  *conductivity = 0.0;
  *frequency = 0.0;
  *gnd_planes = 0;
  *top_ground_plane_thickness = 0.0;
  *bottom_ground_plane_thickness = 0.0;
  *units = UNITS_NO_UNITS;
  
  
  /* - - - - - - - - OPEN FILES then PRINT HEADERS - - - - - - - - - - -  */
  
  version = 1;
  // now try to open the file
  sprintf (fullfilespec, "%s.graphic", filename);
  if ( !(in = fopen(fullfilespec, "r")))
  {
    printf ("**** Could not open graphic file %s\n", fullfilespec);
    return(FAIL);		/* Fatal error; could not open graphic file */
  }
  
  
  /* Read the icon section of the graphic file. If the icon does not
     exist, a flag is set and the program will continue. If the icon
     exists but is incorrect in some way, the program will terminate.	 */
  
  netlist = (NETLIST_REC *)calloc(1,sizeof(NETLIST_REC));
  if (electro_read_icon_section(in,netlist,NULL) != SUCCESS)
  {
    return(FAIL);
  }
  
  /* read up to the object section, to second "PINGRID" */
  
  fgets_status = fgets(line, GPGE_MAX, in);
  
  while(fgets_status != NULL && strncmp(line,"$OBJECT;",8) != 0)
    fgets(line, GPGE_MAX, in);
  
  
  fgets_status = fgets(line, GPGE_MAX, in);
  
  while(fgets_status != NULL && strncmp(line,"$PINGRID",8) != 0)
    fgets(line, GPGE_MAX, in);
  
  
  /* - - - RETRIEVE PERTINENT ICON ATTRIBUTES (if next line is "$IATT;") - - */
  /* get the next line */
  fgets_status = fgets(line, GPGE_MAX, in);
  
  
  /* look at contents of next line */
  if(strncmp(line,"\t\t$IATT;",8) == 0)
  {
    int tmp_cntr_seg = 0;
    int tmp_pln_seg  = 0;

    if(input_icon_attrib(in,&tmp_cntr_seg,&tmp_pln_seg,
			 coupling,risetime,
			 conductivity,
			 frequency,top_ground_plane_thickness,
			 bottom_ground_plane_thickness,units)
       != SUCCESS)
    {
      return(FAIL);
    }


    printf ("ICON conductivity: %g\n", *conductivity);
    // If no cntr_seg and pln_seg values were entered on the
    // command line or were set to zero, set to the new values.
    if ( *cntr_seg < 1 ) 
      {
	*cntr_seg = tmp_cntr_seg;
	*pln_seg = tmp_pln_seg;
      }
  }
  
  
  /*---------------------------------------------------------------*
   *                                                               *
   * Set defaults for values that could be set by icon attributes  *
   *                                                               *
   *---------------------------------------------------------------*/
  
  
  //
  // Establish a default CONDUCTIVITY if the user specified no value.
  //
  
  if (*conductivity == 0.0)
    {
      /* warn about using default values */
      sprintf(msg, "Default of %lf used\n",
	      DEFAULT_CONDUCTIVITY);
      printf ("No conductivity supplied:  %s\n", msg);
      /* assign in mhos/meter */
      *conductivity = DEFAULT_CONDUCTIVITY;
    }
  

  //
  // Establish a default COUPLING if the user specified no value.
  // Note here that units input will default
  // to units of mils when the user specifies none.
  //
	      
  if (*coupling == 0.0)
  {
    /* make the assignment in terms of meters, first, using default in inches*/
    *coupling = DEFAULT_COUPLING;
    *coupling *= INCHES_TO_METERS;
		
    /* warn about using default values in user's selected units */
    switch(*units)
    {
    case UNITS_MICRONS :
      sprintf(msg, "Default=%lf microns used",*coupling / MICRONS_TO_METERS);
      break;
    case UNITS_INCHES :
      sprintf(msg, "Default=%lf inches used",*coupling / INCHES_TO_METERS);
      break;
    case UNITS_METERS :
      sprintf(msg, "Default=%lf meters used",*coupling);
      break;
    case UNITS_MILS :
    case UNITS_NO_UNITS :
    default :
      sprintf(msg, "Default=%g mils used\n",(float)((*coupling) / MILS_TO_METERS));
      break;
    }
    printf ("Assign a default value of %g to coupling length\n",
	    *coupling);
    printf ("%s\n", msg);
  }
  
  //
  // Establish a default RISETIME if the user specified no value.
  //
  if (*risetime == 0.0)
  {
    /* warn about using default values */
    /* assign in units of seconds */
    *risetime = DEFAULT_RISETIME * 1.0e-12;
    printf ("Assign a default value of %g to risetime\n", *risetime);
    printf(msg, "Default=%g picoseconds used", (float)(DEFAULT_RISETIME));
  }
  

  //
  // Establish a default CONDUCTIVITY if the user specified no value.
  //

  // if (*conductivity == 0.0)
  // {
  //   /* warn about using default values */
  //   sprintf(msg, "Default=%lf used", DEFAULT_CONDUCTIVITY);
  //   cad_output(electro_nocond(msg),ERR_LOC(""));
  //   /* assign in mhos/meter */
  //  *conductivity = DEFAULT_CONDUCTIVITY;
  // }


  //
  // Establish a default if the user specified no value.
  //

  if (*frequency == 0.0)
  {
    // Tue Feb 18 06:05:13 1997 -- Kevin J. Buchs - frequency not used,
    // so there is no good reason to generate this warning!
    /* warn about using default values */
    // sprintf(msg, "Default=%lf used", DEFAULT_FREQUENCY);
    // cad_output(electro_nofreq(msg),ERR_LOC(""));
    *frequency = DEFAULT_FREQUENCY * 1e6;
  }
  
  //
  // Establish a default if the user specified no value.  Note here that 
  // units default to units of mils when the user specifies none.
  //
  if (*top_ground_plane_thickness == 0.0)
  {
    /* should we really warn here - this is an optional field, probably not */
    /* frequently used */
    /* warn about using default values */
    /* sprintf(msg, "Default=%lf used", DEFAULT_GND_THICK); *//* error msg */
    /* cad_output(electro_nothickness(msg),ERR_LOC(""));  */
    
    /* assign in user's units */
    
    switch(*units)
    {
      case UNITS_MICRONS :
        *top_ground_plane_thickness = DEFAULT_GND_THICK / MICRONS_TO_METERS;
        break;
      case UNITS_INCHES :
        *top_ground_plane_thickness = DEFAULT_GND_THICK / INCHES_TO_METERS;
        break;
      case UNITS_METERS :
        *top_ground_plane_thickness = DEFAULT_GND_THICK;
        break;
      case UNITS_MILS :
      case UNITS_NO_UNITS :
      default :
        *top_ground_plane_thickness = DEFAULT_GND_THICK / MILS_TO_METERS;
        break;
    }
  }
  
  //
  // Establish a default if the user specified no value.  Note here that 
  // units default to units of mils when the user specifies none.
  //
  if (*bottom_ground_plane_thickness == 0.0)
  {
    /* should we really warn here - this is an optional field, probably not */
    /* frequently used */
    /* warn about using default values */
    /* sprintf(msg, "Default=%lf used", DEFAULT_GND_THICK); *//* error msg */
    /* cad_output(electro_nothickness(msg),ERR_LOC(""));  */
    
    /* assign in user's units */
    
    switch(*units)
    {
      case UNITS_MICRONS :
      *bottom_ground_plane_thickness = DEFAULT_GND_THICK / MICRONS_TO_METERS;
      break;
      case UNITS_INCHES :
      *bottom_ground_plane_thickness = DEFAULT_GND_THICK / INCHES_TO_METERS;
      break;
      case UNITS_METERS :
      *bottom_ground_plane_thickness = DEFAULT_GND_THICK;
      break;
      case UNITS_MILS :
      case UNITS_NO_UNITS :
      default :
      *bottom_ground_plane_thickness = DEFAULT_GND_THICK / MILS_TO_METERS;
      break;
      
    }
    
  }
  
  /* read graphic file to second "$PINS;" */
  /* (if no icon attrs already read line) */
  
  while (strncmp(line, "$PINS;", 6))
    fgets(line, GPGE_MAX, in);
  
  /* - - - - - - - READ IN ALL PINS AND ASSOCIATED ATTRIBUTES - - - - - - */
  /* 1.14 most attributes have become primitive attributes, the only pin
     attributes currently used are for coords on polygons (polypoints).   */
  /* create list of polypoints, else error */
  
  if(input_pin_attrib(&p_front,in,*units) != SUCCESS)
  {
    return(FAIL);
  }
  
  
  /* - - - - - - - - - - - - INPUT PRIMITIVES - - - - - - - - - - - - - - */
  
  fgets(line, GPGE_MAX, in);                   /* pass "$PRIMITIVES;"     */
  fgets(line, GPGE_MAX, in);                   /* read first data line    */
  
  while (strncmp(line, "$END;", 5))
  {
    
    /* - - - - - - - - - ICONS - ground planes - - - - - - - - - - - - */
    
    if (!strncmp(line, "\tI ", 3))
    {
      
      /* its an ICON */
      if (strncmp(&line[3], "ground_plane", 12))
      {
	/* not ground  */
	char *cp ;
	cp = strchr(line+3, ' ') ; /* trap the icon name  */
	*cp = '\0' ;/* output warning to screen and file *
	
	/* WARNING: extra icon */
	printf ("Extraneous instance of %s in design\n", &line[3]);
      }
      else if (++(*gnd_planes) > 2)
      {
	/* count ground planes */
	printf ("There are %d groundplanes in the design...reset to 2\n",
		gnd_planes);
	*gnd_planes = 2;	/* keep it at 2 */
      }
      
      /* each icon is followed by an attribute indicator, "ATT;"
	 (at least 1 for comp #) - read these lines and skip them until
	 a "$END;" is reached.   */
      
      while(strncmp(fgets(line, GPGE_MAX, in), "\t\t$END;", 7))
	/* nothing - skip the line */ ;
      
      fgets(line, GPGE_MAX, in); /* get the next line */
      continue;			/* restart loop for the next primitive */
      
    }
    
    /* If primitive is not an icon, it could be a rectangle, circle, */
    /* or polygon.  We would find that line[0]=tab.  */
    
    switch (line[1])
    {
      
      /* RECTANGLE:(dielectic, signal, ground wire) */
      
    case RECTANGLE:
      
      /* if low_left AND up_right pin attributes are not supplied
	 for the rectangle use the GPGE TO_SCALE coordinates. */
      
      if (sscanf(line, " R ( %lf , %lf ) ( %lf , %lf ) %d %*s\n",
		 &x0, &y0, &x1, &y1, &color) != 5)
      {
	/* GPGE coords */
	printf ("Graphic file error on line (%s)\n", line);
	return(FAIL);
      }
      
      /* Color key: sig=yellow gnd=blue die=green/white */
      
      switch (color)
      {
	
      case DIE_COLOR1:		/* - - - DIELECTRIC INTERFACE - - */
      case DIE_COLOR2:
      case DIE_COLOR3:
	
	d_temp = (struct dielectric *)malloc(sizeof(struct dielectric));
	d_temp->constant = 0.0; /* flag if none found */
	d_temp->tangent = 0.0;	/* flag if none found */
	
	/* primitive attributes may follow */
	if( ! strncmp(fgets(line,GPGE_MAX,in),"\t\t$ATT;", 7))
	  {
	    if(fill_dielectric(d_temp,line,in,*units) != SUCCESS)
	      {
		return(FAIL);
	      }
	  }

	/* ensure (x0,y0) are lower left coords */
	if (d_temp->y1 < d_temp->y0)
	{ t0 = d_temp->y0;  d_temp->y0 = d_temp->y1;  d_temp->y1 = t0; }
	
	/* ensure (x1,y1) are upper right coords */
	if (d_temp->x1 < d_temp->x0)
	{ t0 = d_temp->x0;  d_temp->x0 = d_temp->x1;  d_temp->x1 = t0; }
	
#ifdef CHECK_SCALE
	if (check_scale(local_scale, &global_scale,
			"Dielectric layer") == TO_SCALE)
	{
	  d_temp->x0=x0; d_temp->y0=y0;	/* gpge */
	  d_temp->x1=x1; d_temp->y1=y1;
	}
#endif
	
	if (d_temp->x0 == d_temp->x1 || d_temp->y0 == d_temp->y1)
	{			/* check coordinates */
	  printf ("Bad dielectric coordinate (%s)\n", line);
	  return(FAIL);
	}
	
	/* keep track of the highest dielectric - to find ground plane */
	if(d_temp->y1 > highest_dielectric) highest_dielectric = d_temp->y1;
	
	/* keep track of the lowest dielectric - to offset to y=0 */
	if(d_temp->y0 < offset) offset = d_temp->y0;
	
	/* keep track of smallest dimension */
	if(d_temp->y1 - d_temp->y0 < minimum_dimension)
	  minimum_dimension = d_temp->y1 - d_temp->y0;
	
	if(d_temp->x1 - d_temp->x0 < minimum_dimension)
	  minimum_dimension = d_temp->x1 - d_temp->x0;
	
#ifdef CHECK_SCALE
	if (global_scale == MIXED_SCALE)
	  break;		/* just parse file, don't save any */
#endif
	
	if (! d_temp->constant)
	{			/* 0 constant found */
	  printf ("No dielectric constant");
	  d_temp->constant = AIR_CONSTANT;
	}
	
#ifdef SORT_DIELECTRICS


	/* insert dielectric layer into sorted list */
	
	if (!*dielectrics)	/* bottom layer */
	  *dielectrics = d_temp;
	else if ((*dielectrics)->y0 > d_temp->y0)
	{
	  d_temp->left = *dielectrics; /* before first */
	  *dielectrics = (*dielectrics)->right = d_temp;
	}
	else
	{			/* find spot in sorted layers */
	  for(d_temp2 = *dielectrics;
	      d_temp2->left;
	      d_temp2 = d_temp2->left)
	    if (d_temp2->left->y0 > d_temp->y0)
	      break;
	  d_temp->left = d_temp2->left;
	  d_temp->right = d_temp2;
	  d_temp2->left = d_temp; /* insert */
	  if (d_temp->left)
	    d_temp->left->right = d_temp;
	}
#else
	
	/* insert dielectric onto plain list */
	
	d_temp->next = *dielectrics;
	*dielectrics = d_temp;
#endif
	
	break;
	
      case SIGNAL_COLOR:	/* - - - - - SIGNAL WIRE - - - - */
      case GND_COLOR:		/* - - - - - GROUND WIRE - - - - */
	
	/* use all pins that fall inside gpge boundary
	   Auto generate signal name if none supplied */
	
	c_temp = (struct contour *)malloc(sizeof(struct contour));
	c_temp->name[0] = '\0';	/* only name signals */
	c_temp->primitive = RECTANGLE;
	c_temp->conductivity = 0.0;
	/* primitive attributes may follow */
	
	if (!strncmp(fgets(line, GPGE_MAX, in),"\t\t$ATT;", 7))
	{
	  if(fill_contour(c_temp,RECTANGLE,line,color,in,*units,
			  &c_conductivity)
	     != SUCCESS)
	  {
	    return(FAIL);
	  }
	}
	
	
#ifdef CHECK_SCALE
	if (check_scale(local_scale, &global_scale,
			(color == SIGNAL_COLOR ?
			 "Rectangular signal contour" :
			 "Rectangular ground contour"))== TO_SCALE)
	{
	  c_temp->x0=x0; c_temp->y0=y0;	/* gpge */
	  c_temp->x1=x1; c_temp->y1=y1;
	}
#endif
	
	/* check and if required, sort, coordinates */
	
	if(c_temp->x0 > c_temp->x1)
	{ t0 = c_temp->x0; c_temp->x0 = c_temp->x1; c_temp->x1 = t0; }
	
	if(c_temp->y0 > c_temp->y1)
	{ t0 = c_temp->y0; c_temp->y0 = c_temp->y1; c_temp->y1 = t0; }
	
	if (c_temp->x0 == c_temp->x1 || c_temp->y0 == c_temp->y1)
	{
	  printf ("Bad coordinate on signal line/ground wire\n");
	  return(FAIL);
	}
	
	/* keep track of smallest dimension */
	
	if(c_temp->y1 - c_temp->y0 < minimum_dimension)
	  minimum_dimension = c_temp->y1 - c_temp->y0;
	
	if(c_temp->x1 - c_temp->x0 < minimum_dimension)
	  minimum_dimension = c_temp->x1 - c_temp->x0;
	
#ifdef CHECK_SCALE
	if (global_scale == MIXED_SCALE)
	  break;		/* just parse file, don't save any */
#endif
	
	if (color==SIGNAL_COLOR)
	{
	  if (!c_temp->name[0])	/* name supplied? */
	    sprintf(c_temp->name,"A%d",++auto_gen);
	  c_temp->next = *signals;
	  *signals = c_temp;
	  (*num_signals)++;
	  c_temp->conductivity = c_conductivity;
	}
	else
	{
	  c_temp->next = *groundwires;
	  *groundwires = c_temp;
	  (*num_grounds)++;
	}
	break;
	
	
	/* WARNING: Unknown color */
	
      default:
	printf ("Extraneous rectangle of wrong color\n");
	if (!strncmp(fgets(line, GPGE_MAX, in),"\t\t$ATT;", 7))
	{
	  while (strncmp(fgets(line, GPGE_MAX, in),"\t\t$END;", 7)) ;
	  /* get next line */
	  fgets(line, GPGE_MAX, in);
	}
	break;			/* - - DONE WITH RECTANGLE COLORS - - - */
      }
      break;			/* - - - - DONE WITH RECTANGLE PRIMITIVES - - - - */
      
      
    case CIRCLE:		/* - - - - CIRCLE:(signal, ground wire) - - - - */
      
      
      /* GPGE coords */
      if (sscanf(line, " A ( %lf , %lf ) ( %lf , %lf ) %d %*s\n",
		 &x0, &y0, &x1, &y1, &color) != 5)
      {
	printf ("Graphic file error on line (%s)\n", &line[1]);
	return(FAIL);
      }
      
#ifdef CHECK_SCALE
      /* if center AND radius pin attributes are not supplied
	 for the circle use the GPGE TO_SCALE coordinates. */
      
      radius = sqrt(pow(fabs(x1-x0),2.0)+pow(fabs(y1-y0),2.0));
#endif
      
      /* signal=yellow, ground=blue */
      
      switch (color)
      {
	
      case SIGNAL_COLOR:	/* - - - - - SIGNAL WIRE - - - - */
      case GND_COLOR:		/* - - - - - GROUND WIRE - - - - */
	
	/* use all pins that fall inside gpge boundary
	   Auto generate signal name if none supplied */
	
	c_temp = (struct contour *)malloc(sizeof(struct contour));
	c_temp->name[0] = '\0';	/* only name signals */
	c_temp->primitive = CIRCLE;
	c_temp->conductivity = 0.0;
	/* primitive attributes may follow */
	
	if (!strncmp(fgets(line, GPGE_MAX, in),"\t\t$ATT;", 7))
	{
	  if(fill_contour(c_temp,CIRCLE,line,color,in,*units,&c_conductivity)
	     != SUCCESS)
	  {
	    return(FAIL);
	  }
	}
	
	/* keep track of smallest dimension, by using radius */
	
	if(c_temp->x1 < minimum_dimension)
	  minimum_dimension = c_temp->x1;
	
	
#ifdef CHECK_SCALE
	if (check_scale(local_scale, &global_scale,
			(color == SIGNAL_COLOR ?
			 "Circular signal contour" :
			 "Circular ground contour")) == TO_SCALE)
	{
	  c_temp->x0=x0; c_temp->y0=y0;	/* gpge */
	  c_temp->x1=radius;	/* save radius here!! */
	}
	
	if (global_scale == MIXED_SCALE)
	  break;		/* just parse file, don't save any */
#endif
	
	if (color==SIGNAL_COLOR)
	{
	  if (!c_temp->name[0]) /* name supplied? */
	    sprintf(c_temp->name,"A%d",++auto_gen);
	  c_temp->next = *signals;
	  *signals = c_temp;
	  (*num_signals)++;
	  c_temp->conductivity = c_conductivity;
	}
	else
	{
	  c_temp->next = *groundwires;
	  *groundwires = c_temp;
	  (*num_grounds)++;
	}
	break;
	
	/* Warning: circle color is unknown */
	
      default:
	
	printf ("Extraneous circle of wrong color\n");
	if (!strncmp(fgets(line, GPGE_MAX, in),"\t\t$ATT;", 7))
	{
	  while(strncmp(fgets(line,GPGE_MAX,in),"\t\t$END;",7))
	    /* do nothing */ ;
	  
	  /* get next line */
	  fgets(line, GPGE_MAX, in);
	}
	break;			/* - - - DONE WITH CIRCLE COLORS - - - */
      }
      break;			/* - - - - DONE WITH CIRCLE PRIMITIVES - - - - */
      
      
      /* - - - POLYGON :(signal, ground wire) - - - - */
      
    case POLYGON:
      
      c_temp = (struct contour *)malloc(sizeof(struct contour));
      c_temp->primitive = POLYGON;
      c_temp->points = NULL;
      c_temp->name[0] = '\0' ;
      c_temp->x0 = 0.0;		/* initialize perimeter */
      c_temp->x1 = 0.0;		/* not used on polys */
      c_temp->conductivity = 0.0;
      /* polygon must have at least 3 pts, extract them from gpge
	 line, take the NOT_SCALE pin attribute polypoint if they
	 exist at EVERY vertex, error out if they exist at SOME
	 of the points, use gpge if NO polypoint attributes. Take
	 the polygon name if one is associated with any vertex
	 */
      
      if (sscanf(line, " G %d", &t1) != 1 || t1 < 3)
      {
	printf ("Graphic file error on line (%s)\n", &line[1]);
	return(FAIL);
      }
      
      /* extract points from line and look for pin coordinates */
      if(extrct_pts(line,c_temp,&color,&p_front,&minimum_dimension)
	 != SUCCESS)
      {
	return(FAIL);
      }
      
      
      /* primitive attributes may follow */
      if (!strncmp(fgets(line, GPGE_MAX, in), "\t\t$ATT;", 7))
	{
	  if(fill_contour(c_temp,POLYGON,line,color,in,*units,&c_conductivity)
	     != SUCCESS)
	    {
	      return(FAIL);
	    }
	}
      
#ifdef CHECK_SCALE
      if (check_scale(local_scale, &global_scale,
		      (color == SIGNAL_COLOR ? "Polygon signal contour" :
		       "Polygon ground contour")) == MIXED_SCALE)
	break;			/* just parse file, don't save any */
#endif
      
      /* place node in proper signal or ground list (or error) */
      if (color==SIGNAL_COLOR)
      {
	if (!c_temp->name[0])	/* make sure signal is named */
	  sprintf(c_temp->name, "A%d",++auto_gen);
	c_temp->next = *signals;
	*signals = c_temp;
	(*num_signals)++;
	c_temp->conductivity = c_conductivity;
      }
      else if (color==GND_COLOR)
      {
	c_temp->next = *groundwires;
	*groundwires = c_temp;
	(*num_grounds)++;
      }
      
      /* polygon is NOT a signal or ground */
      else
      {
	printf ("Extraneous polygon of wrong color\n");
	free(c_temp);           /* release unused primitive */
      }
      
      break;		/* - - - - DONE WITH POLYGON PRIMITIVES - - - - - */
      
      
      /* not a icon, rectangle, circle, or polygon, what is it? */
      
    default:		/* WARNING: unidentifiable, find color after last */
      
      
      sscanf(strrchr(line, ')'), ") %d %*s", &t1); /* 1.13 */
      printf ("Extraneous ploygon of wrong color\n");
      
      if (!strncmp(fgets(line, GPGE_MAX, in),"\t\t$ATT;", 7))
      {
	while (strncmp(fgets(line, GPGE_MAX, in),"\t\t$END;", 7))
	  /* do nothing */ ;
	fgets(line, GPGE_MAX, in); /* get next line */
      }
      
      break;
      
      
    }
  } /* - finished reading the PRIMITIVES section of node.graphic - */
  
  
  /* - - - - - - - - - - - - GENERATE NETLIST - - - - - - - - - - - - - */
  if (nmmtl_gen_netlist_from_icon(netlist, *signals, filename) != SUCCESS)
  {
    printf ("**** Cannot generate the netlist\n");
    return(FAIL);
  }
  
  /* - - - - - - - - - - - - CHECK FOR ERRORS - - - - - - - - - - - - - */
  
  for ( ; p_front; p_front=p_front->left)
  {

    sprintf(msg, "(%lf, %lf)",p_front->x,p_front->y); /* Warn: extra pin */
    printf ("Extraneous pin at %s\n", msg);
    
    for (t_att=p_front->att; t_att; t_att=t_att->next)
    {				/* Warning */
      sprintf(line, " at x=%lf, y=%lf", t_att->x, t_att->y);
      printf ("Extraneous pin attribute %s\n", line);
    }
  }
  
#ifdef CHECK_SCALE
  if (global_scale == MIXED_SCALE)
  {
    printf ("Program termination due to mixed scale modes in diagram\n");
    return(FAIL);
  }
  
  /* The reference point (origin=0,0) must be along the top of the bottom
     ground plane. Use origin_offset to adjust the diagram.
     IF              THEN SUBTRACT FROM ALL PIECES FOR OFFSET
     diagram==TO_SCALE && dielectrics            dielectrics->y0
     diagram==TO_SCALE && !dielectrics           GROUND_THICK, default
     diagram==NOT_SCALE && dielectrics         dielectrics->y0, should be 0.0
     diagram==NOT_SCALE && !dielectrics          0.0, assume correct (1.09)
     */
  
  /* if there exists a dielectric, the bottom most dielectric layer */
  
  if (*dielectrics != NULL)
    *origin_offset = (*dielectrics)->y0; /* use lower left coord, lowest die */
  else
  {
    if (global_scale == TO_SCALE) /* use default */
      *origin_offset = GROUND_THICK;
    else *origin_offset = 0.0;	/* NOT_SCALE w/o die, or 0 contours */
  }
#endif
  
  /* Offset all y dimensions to make top of bottom ground plane at */
  /* y=0.0 */
  
  if(offset != 0.0)
    status = nmmtl_set_offset(offset,*dielectrics,*signals,*groundwires);
  highest_dielectric -= offset;
  
  /* now, check to see if ground planes were really specified by */
  /* drawing rectanges.  If so, the bottom ground plane will be */
  /* now at y=0, since the offset operation was completed. */
  
  /* ordinarily there should be one of each, but the following code */
  /* is extra robust, just in case there are more. */
  
  if(*gnd_planes == 0 && *groundwires != NULL)
  {
    
    /* loop and take off all elements on groundwire list which */
    /* would qualify as lower ground planes.  If there is more */
    /* than one, so warn the user. */
    
    c_prev = NULL;
    c_temp = *groundwires;
    while(c_temp != NULL)
    {
      if(c_temp->primitive == 'R')
      {
	if(c_temp->y1 == 0.0)
	{
	  lower_ground_planes++;
	  if(lower_ground_planes > 1)
	  {
	    printf ("**** Too many lower groundplanes...reset to 1\n");
	  }
	  *gnd_planes = 1;
	  (*num_grounds)--;
	  
	  /* save fullest extent of x dimensions of ground plane */
	  if(ground_x_min > c_temp->x0) ground_x_min = c_temp->x0;
	  if(ground_x_max < c_temp->x1) ground_x_max = c_temp->x1;
	  
	  /* record ground plane thickness */
	  *bottom_ground_plane_thickness = c_temp->y1 - c_temp->y0;
	  
	  if(c_prev == NULL)
	  {
	    /* remove first item from the list */
	    *groundwires = c_temp->next;
	    free(c_temp);
	    c_temp = *groundwires;
	  }
	  else
	  {
	    /* remove from list */
	    c_prev->next = c_temp->next;
	    free(c_temp);
	    c_temp = c_prev->next;
	  }
	}
	else
	{
	  c_prev = c_temp; c_temp = c_temp->next;
	}
      }
      else
      {
	c_prev = c_temp; c_temp = c_temp->next;
      }
    }
  }
  
  
  /* Now try to find an upper ground plane - its x dimensions must */
  /* match exactly */
  
  if(*gnd_planes == 1 && *groundwires != NULL)
  {
    
    /* loop and take off all elements on groundwire list which */
    /* would qualify as an upper ground planes.  If there is more */
    /* than one, so warn the user. */
    
    c_prev = NULL;
    c_temp = *groundwires;
    while(c_temp != NULL)
    {
      if(c_temp->primitive == 'R')
      {
	if(c_temp->y0 == highest_dielectric &&
	   ground_x_min == c_temp->x0 && ground_x_max == c_temp->x1 )
	{
	  (*num_grounds)--;

	  upper_ground_planes++;
	  if(upper_ground_planes > 1)
	  {
	    printf ("**** Too many upper groundplanes...reset to 2\n");
	  }
	  *gnd_planes = 2;
	  
	  /* record ground plane thickness */
	  *top_ground_plane_thickness = c_temp->y1 - c_temp->y0;
	  
	  /* remove from the list of groundwires */
	  if(c_prev == NULL)
	  {
	    /* remove first item from the list */
	    *groundwires = c_temp->next;
	    free(c_temp);
	    c_temp = *groundwires;
	  }
	  else
	  {
	    /* remove from list */
	    c_prev->next = c_temp->next;
	    free(c_temp);
	    c_temp = c_prev->next;
	  }
	}
	else
	{
	  c_prev = c_temp; c_temp = c_temp->next;
	}
      }
      else
      {
	c_prev = c_temp; c_temp = c_temp->next;
      }
    }
  }
  
  
  /* warning to the user if bottom ground plane is missing - it
     will go on and assume one exists, below the lowest dielectric layer */
  
  if(*gnd_planes < 1)
  {
    printf ("**** There isn't a groundplane\n");
  }
  
  *half_minimum_dimension = 0.5 * minimum_dimension;
  
  /* all done, return success */
  return(SUCCESS);
  
}

#ifdef CHECK_SCALE

/* ------------------------------------------------------------------------
   - - - - - - - - - - FUNCTION: check_scale - - - - - - - - - - - - - - -
   
   check that the local and global scale values are the same, else output
   an error. Also use on each pin of polygons.
   
   ------------------------------------------------------------------------
   */

check_scale(int local,int *global,char *err_msg)
{
  
  if (*global == NOT_SET)
    *global = local;
  
  else if ((*global != local && *global != MIXED_SCALE) ||
	   (local == TO_SCALE && *global == MIXED_SCALE))
  {
    *global = MIXED_SCALE;
    print_error(ELECTRO_MIXSCALE,ERR_LOC(""),NULL,err_msg,
		(local==TO_SCALE ? "TO_SCALE" : "NOT_SCALE"),
		(local==TO_SCALE ? "NOT_SCALE" : "TO_SCALE"));
  }
  return(*global);
}

#endif


/*
   
   FUNCTION NAME:  input_pin_attrib
   
   
   FUNCTIONAL DESCRIPTION:
   
   Read in the pin section of the graphic file.  We expect to find pins
   with the POLYPOINT attribute.  Look for this attribute and store its
   value.  Front is front of pin list.
   
   FORMAL PARAMETERS:
   
   struct pins **front,  - front of list of pins
   FILE *in,             - graphic file reading from
   
   RETURN VALUE:
   
   SUCCESS or FAIL
   
   CALLING SEQUENCE:
   
   if(input_pin_attrib(&p_front,in) != SUCCESS)
   
   */

static char hertz[] = "hertz";
static char meters[] = "meter";
static char seconds[] = "second";
static char siemensPmeter[] = "siemen/meter";

static int input_pin_attrib(struct pins **front,FILE *in,int units)
{

  struct pins *p_temp;
  struct polypoints *t_att;
  char line[GPGE_MAX];
  double x0,y0;			/* gpge coords of the pin */
  //
  // Obtain 1st pin, beware of pins without attributes.
  //
  fgets(line, GPGE_MAX, in);
  //
  // Loop until you are through the pins section of the graphic file searching for
  // polypoint attributes.
  //
  while (strncmp(line, "$END;", 5))
  {
    /* first line must be the coordinates of the pin */
    
    if (sscanf(line, " ( %lf , %lf ) %*s\n", &x0, &y0) != 2)
    {
      //
      // Fatal error reading in a pin.  Each pin must have a unique 
      //   x and y position.
      //
      printf ("**** Graphic file error while reading pins (%s)\n",
	      line);
      return(FAIL);
    }
    if (strncmp(fgets(line, GPGE_MAX, in), "\t\t$ATT;", 7))
    {
      //
      // Warn the user that no attributes are present with this pin.
      //
      char msg[80];
      sprintf(msg, "(%lf, %lf)", x0, y0);
      printf ("Pin %s without attributes\n", msg);
      //
      // Read in the next pin coords or $END;
      //
      continue;
    }
    
    p_temp = (struct pins *)malloc(sizeof(struct pins)); /* build list */
    p_temp->x = x0; p_temp->y = y0; /* previously read in */
    p_temp->left = *front; /* add to front of list */
    p_temp->right = NULL; /* rightmost is the front */
    p_temp->att = NULL;	/* attach the attribute */
    if (*front)		/* if a list does exists */
      (*front)->right = p_temp;	/* point to new front */
    *front = p_temp;	/* move first up to right */
    /* scan through the pin's attributes */
    
    //
    // Local character strings and values used in parsing the polypoint lines.
    //
    int polygon_vertex_set=FALSE, polypoint_set=FALSE;
    while(strncmp(fgets(line, GPGE_MAX, in), "\t\t$END;", 7))
    {
      if (!strncmp(line, "\t\t\"POLYGON_VERTEX\"", 18))
      {
	double xval, yval;
	char x_string[GPGE_MAX], y_string[GPGE_MAX];
	//
	// Allocate another polypoint structure.
	//

	t_att = (struct polypoints *)malloc(sizeof(struct polypoints));
	//
	// Does the attribute parse corrrectly into a x,y coordinate pair?
	//
	if (sscanf(line, "%*[^)]) \" ( %[^,] , %[^)] )", x_string, 
		   y_string) != 2)
	{
	  printf ("Graphic file error on line (%s)\n", line);
	  return(FAIL);
	}
	
	if ( (conversion(x_string, meters, xval) != SUCCESS) ||
	    (conversion(y_string, meters, yval) != SUCCESS) )
	{
	  printf ("Conversion of units failed for %s\n", &line[2]);
	  return(FAIL);
	}
	else
	{
	  t_att->x = xval;
	  t_att->y = yval;
	  //
	  // If it has an attribute of correct form then save this pin.
	  //
	  (*front)->att = t_att; /* attach the attribute */
	  polygon_vertex_set=TRUE;
	  if (polypoint_set == TRUE)
	  {
	    printf ("Obsolete attribute \"POLYPOINT\" superseded by \"POLYGON_VERTEX\"\n");
	  }
	}
      }
      else if( !(strncmp(line, "\t\t\"POLYPOINT\"", 13)) )
      {
	if (polygon_vertex_set == TRUE)
	{
	    printf ("Obsolete attribute \"POLYPOINT\" superseded by \"POLYGON_VERTEX\"\n");
	}
	else
	{
	  //
	  // Allocate another polypoint structure.
	  //
	  t_att = (struct polypoints *)malloc(sizeof(struct polypoints));

	  //
	  // Does the attribute parse corrrectly into a x,y coordinate pair?
	  //
	  if (sscanf(line," %*[^)]) \" ( %lf , %lf ) ",&t_att->x,&t_att->y) != 2)
	  {
	    printf ("Graphic file error on line (%s)\n", line);
	    return(FAIL);
	  }
	  else
	  {
	    convert(units,t_att->x);
	    convert(units,t_att->y);
	    polypoint_set = TRUE;
	    (*front)->att = t_att; /* attach the attribute */
	  }
	}
      }
      else
      {
	printf ("Extraneous pin attribute %s\n", &line[2]);
	continue;
      }
    }
    fgets(line, GPGE_MAX, in); /* get next pin coord line */
  }
  return(SUCCESS);
}


/*
   
   FUNCTION NAME: input_icon_attrib
   
   
   FUNCTIONAL DESCRIPTION:
   
   Read the icon attributes section of the graphic file and store them.
   
   FORMAL PARAMETERS:
   
   FILE *in,            - graphic file reading from
   
   - all the following are values for icon attributes
   int *cntr_seg,
   int *pln_seg,
   float *coupling,
   float *risetime,
   float *conductivity,
   float *frequency,
   float *top_ground_plane_thickness,
   float *bottom_ground_plane_thickness,
   int *units
   
   RETURN VALUE:
   
   FAIL or SUCCESS
   
   CALLING SEQUENCE:
   
   if(input_icon_attrib(in,cntr_seg,pln_seg,coupling,risetime,
      conductivity,frequency,top_ground_plane_thickness,
      bottom_ground_plane_thickness,&units)
        != SUCCESS)
   
   Note: GND_T_T, and GND_B_T do not seem to be supported in the program
   and no further code will be added here for new attributes parsing
   for these two attributes.
   
   Note on Note: These attributes were put in place for the ground plane
   thickness and were to be used by the elusive internal inductance program.
   -- Kevin Buchs 6/12/95.
   
   
   */


static int input_icon_attrib(
					 FILE *in,
					 int *cntr_seg,
					 int *pln_seg,
					 float *coupling,
					 float *risetime,
					 float *conductivity,
					 float *frequency,
					 float *top_ground_plane_thickness,
					 float *bottom_ground_plane_thickness,
					 int *units)
{
  int cntr_seg_set=FALSE, pln_seg_set=FALSE, conduct_set=FALSE;
  int coupling_set=FALSE, cplng_in_set=FALSE, edge_rate_ps_set=FALSE;
  int rstm_ps_set=FALSE;
  int units_set=FALSE, freq_mhz_set=FALSE, surf_res_set=FALSE;
  
  int frequency_set=FALSE, conductivity_set=FALSE;
  int contour_segments_set=FALSE, plane_segments_set=FALSE;
  int coupling_length_set=FALSE, risetime_set=FALSE;
  double temp_value;
  
  char line[GPGE_MAX];
  char units_string[GPGE_MAX];
  
  // Default internally converted values.
  
  float surf_resist = 0.0;
  float coupling_in = 0.0;
  
  // Parse each attribute line searching for relevent information.
  
  while (strncmp(fgets(line, GPGE_MAX, in), "\t\t$END;", 7))
  {
    
    
    //---> Attribute = CSEG
    
    if (!strncmp(line, "\t\t\"CSEG\"", 8) )
    {
      if (contour_segments_set == TRUE)
      {
	printf ("Obsolete attribute \"CSEG\" superseded by \"CONTOUR_SEGMENTS\"\n");
      }
      else
      {
	//
	// Only use this old attribute (CSEG) if the new attribute,
	// CONTOUR_SEGMENTS, is not yet specified.
	//
	if (sscanf(line, " %*s %*s \" %d \" %*s\n", cntr_seg)!=1)
	{
	  printf ("Graphic file error on (%s)\n", line);
	  return(FAIL);
	}
	else
	{
	  cntr_seg_set=TRUE;
	}
      }
    }
    
    
    //---> Attribute = CONTOUR_SEGMENTS 
    
    else if (!strncmp(line, "\t\t\"CONTOUR_SEGMENTS\"", 20))
    {
      //
      // The variable 'cntr_seg' is expected to have no units but a scale
      // may be applied.
      //
      char contour_segments_string[GPGE_MAX];
      //
      // Obtain the attribute corresponding to the contour_segments of the MMTL
      //
      if (sscanf(line, " %*s %*s \"%[^\"]\" %*s\n", contour_segments_string)!=1)
      {
	printf ("Graphic file error on (%s)\n", line);
	return(FAIL);
      }
      //
      // Attempt to convert the contour_segments with a scale factor
      // into a straight number.
      //
      if ( (sscanf (contour_segments_string, "%d", cntr_seg)) != 1 )
      {
	printf ("Could not convert contour_segments_string (%s) to integer\n", 
		contour_segments_string);
	return(FAIL);
      }

      contour_segments_set=TRUE;
      if (cntr_seg_set == TRUE)
	{
	  printf ("Obsolete attribute \"CNTR_SEG\" superseded by \"CONTOUR_SEGMENTS\"\n");
	}
    }
    
    
    //---> Attribute = DSEG
    
    else if (!strncmp(line, "\t\t\"DSEG\"", 8) )
    {
      if (plane_segments_set == TRUE)
      {
        printf ("Obsolete attribute \"DSEG\" superseded by \"PLANE_SEGMENTS\"\n");
      }
      else
      {
	//
	// Only use this old attribute (DSEG) if the new attribute,
	// PLANE_SEGMENTS, is not yet specified.
	//
	if (sscanf(line, " %*s %*s \" %d \" %*s\n", pln_seg)!=1)
	{
	  printf ("Graphic file error on (%s)\n", line);
	  return(FAIL);
	}
	else
	{
	  pln_seg_set=TRUE;
	}
      }
    }
    
    
    //---> Attribute = PLANE_SEGMENTS
    
    else if (!strncmp(line, "\t\t\"PLANE_SEGMENTS\"", 18))
    {
      //
      // The variable 'pln_seg' is expected to be in no units but a scale
      // may be applied.
      //
      char plane_segments_string[GPGE_MAX];
      //
      // Obtain the attribute corresponding to the plane_segments of the MMTL.
      //
      if (sscanf(line, " %*s %*s \"%[^\"]\" %*s\n", plane_segments_string)!=1)
      {
	printf ("Graphic file error on (%s)\n", line);
	return(FAIL);
      }
      //
      // Attempt to convert the plane_segments with a scale factor into a
      // straight number.
      //
      if ( (sscanf (plane_segments_string, "%d", pln_seg)) != 1 )
      {
	printf ("Could not convert plane_segments_string (%s) to integer\n", 
		plane_segments_string);
	return(FAIL);
      }
      plane_segments_set=TRUE;
      if (pln_seg_set == TRUE)
	{
          printf ("Obsolete attribute \"PLN_SEG\" superseded by \"PLANE_SEGMENTS\"\n");
	}
    }
    
    
    
    //---> Attribute = CPLNG_IN
    
    
    /* coupling length can be specified by CPLNG_IN which is in inches */
    
    
    else if (!strncmp(line, "\t\t\"CPLNG_IN\"", 12))
    {
      
      // Coupling_Length superscedes Coupling which supercedes Cplng_In
      
      if (coupling_length_set == TRUE)
      {
          printf ("Obsolete attribute \"CPLNG_IN\" superseded by \"COUPLING_LENGTH\"\n");
      }
      
      else if (coupling_set == TRUE)
      {
          printf ("Obsolete attribute \"CPLNG_IN\" superseded by \"COUPLING\"\n");
      }
      
      else if (sscanf(line, " %*s %*s \" %lf \" %*s\n", &coupling_in)!=1)
      {
	printf ("Graphic file failed on line %s\n", line);
	return(FAIL);
      }
      
      else
      {
	cplng_in_set = TRUE;
	
      }
    }
    
    
    //---> Attribute = COUPLING
    
    /* COUPLING which is in the user's choice of units */
    
    else if (!strncmp(line, "\t\t\"COUPLING\"", 12))
    {
      
      // Coupling_Length superscedes Coupling which supercedes Cplng_In
      
      if (coupling_length_set == TRUE)
      {
        printf ("Obsolete attribute \"COUPLING\" superseded by \"COUPLING_LENGTH\"\n");
      }
      
      else
      {
	
	// Only use this old attribute (COUPLING) if the new attribute,
	// COUPLING_LENGTH, is not yet specified.  This is satisfied by
	// the else above.
	
	if (sscanf(line, " %*s %*s \" %lf \" %*s\n", coupling)!=1)
	{
	  printf ("Graphic file error on (%s)\n", line);
	  return(FAIL);
	}
	else
	{
	  coupling_set=TRUE;
	  if(cplng_in_set == TRUE)
	  {
            printf ("Obsolete attribute \"CPLNG_IN\" superseded by \"COUPLING\"\n");
	  }
	}
      }
    }
    
    
    //---> Attribute = COUPLING_LENGTH
    
    else if ( !strncmp(line, "\t\t\"COUPLING_LENGTH\"", 19) )
    {
      
      // This attribute will overwrite the older 'COUPLING' and
      // 'CPLNG_IN' if these are specified.
      
      char coupling_string[GPGE_MAX];
      
      // Obtain the attribute corresponding to the length of the MMTL.
      
      if (sscanf(line, " %*s %*s \"%[^\"]\" %*s\n", coupling_string)!=1)
      {
	printf ("Graphic file error on (%s)\n", line);
	return(FAIL);
      }
      
      // Convert the length, in any units, into meters.
      
      if (conversion(coupling_string, meters, temp_value) != SUCCESS)
      {
	printf ("Graphic file error on (%s)\n", line);
	return(FAIL);
      }
      else
      {
	coupling_length_set=TRUE;
	*coupling = temp_value;
	if (coupling_set == TRUE)
	{
          printf ("Obsolete attribute \"COUPLING\" superseded by \"COUPLING_LENGTH\"\n");
	}
      }
    }
    
    
    //---> Attribute = RISETIME 
    
    else if (!strncmp(line, "\t\t\"RISETIME\"", 12))
    {
      
      char risetime_string[GPGE_MAX];
      
      // The variable 'risetime' is expected, when all conversions are complete
      // to be in units of picoseconds.  This attribute can be specified in anything.
      // Obtain the attribute corresponding to the risetime of the MMTL.
      
      if (sscanf(line, " %*s %*s \"%[^\"]\" %*s\n", risetime_string)!=1)
      {
	printf ("Graphic file error on (%s)\n", line);
	return(FAIL);
      }
      
      // convert the risetime, in any units, into pico seconds.
      
      if (conversion(risetime_string, seconds, temp_value) != SUCCESS)
      {
	printf ("Graphic file error on (%s)\n", line);
	return(FAIL);
      }
      else
      {
	risetime_set=TRUE;
	*risetime = temp_value;
	if (edge_rate_ps_set == TRUE)
	{
          printf ("Obsolete attribute \"EDGE_RATE_PS\" superseded by \"RISETIME\"\n");
	}
	if (rstm_ps_set == TRUE)
	{
          printf ("Obsolete attribute \"RSTM_PS\" superseded by \"RISETIME\"\n");
	}
      }
    }
    
    
    //---> Attribute = RSTM_PS
    
    else if (!strncmp(line, "\t\t\"RSTM_PS\"", 11))
    {
      if (risetime_set == TRUE)
      {
        printf ("Obsolete attribute \"RSTM_PS\" superseded by \"RISETIME\"\n");
      }
      else
      {
	
	// Only use this old attribute (RSTM_PS) if the new attribute
	// (RISETIME) is not specified.
	
	if (sscanf(line, " %*s %*s \" %f \" %*s\n", risetime)!=1)
	{
	printf ("Graphic file error on (%s)\n", line);
	  return(FAIL);
	}
	else
	{
	  *risetime = *risetime * 1.0e-12; /* convert to seconds */
	  rstm_ps_set=TRUE;
	  if(edge_rate_ps_set == TRUE)
	  {
            printf ("Obsolete attribute \"EDGE_RATE_PS\" superseded by \"RSTM_PS\"\n");
	  }
	}
      }
    }
    
    
    //---> Attribute = EDGE_RATE_PS
    
    /* EDGE_RATE_PS for xtalk */
    else if (!strncmp(line, "\t\t\"EDGE_RATE_PS\"", 16))
    {
      if (risetime_set == TRUE)
      {
        printf ("Obsolete attribute \"EDGE_RATE_PS\" superseded by \"RISETIME\"\n");
      }
      else if(rstm_ps_set == TRUE)
      {
        printf ("Obsolete attribute \"EDGE_RATE_PS\" superseded by \"RSTM_PS\"\n");
      }
      else
      {
	
	// Only use this old attribute (EDGE_RATE_PS) if the new
	// attribute (RISETIME) is not specified.
	
	if (sscanf(line, " %*s %*s \" %f \" %*s\n", risetime)!=1)
	{
	printf ("Graphic file error on (%s)\n", line);
	  return(FAIL);
	}
	else
	{
	  *risetime = *risetime * 1.0e-12; /* convert to seconds */
	  edge_rate_ps_set=TRUE;
	}
      }
    }
    
    
    //---> Attribute = FREQ_MHZ
    
    else if (!strncmp(line, "\t\t\"FREQ_MHZ\"", 12) )
    {
      if (frequency_set == TRUE)
      {
        printf ("Obsolete attribute \"FREQ_MHZ\" superseded by \"FREQUENCY\"\n");
      }
      else
      {
	//
	// Only use this old attribute (FREQ_MHZ) if the new attribute
	// (FREQUENCY) is not yet specified.
	//
	if (sscanf(line, " %*s %*s \" %f \" %*s\n", frequency)!=1)
	{
	printf ("Graphic file error on (%s)\n", line);
	  return(FAIL);
	}
	else
	{
	  freq_mhz_set=TRUE;
	  *frequency = *frequency * 1.0e6; /* convert to hertz */
	}
      }
    }
    
    
    //---> Attribute = FREQUENCY 
    
    else if (!strncmp(line, "\t\t\"FREQUENCY\"", 13))
    {
      //
      // The variable 'frequency' is expected to be in Hertz units but a scale
      // may be applied.
      //
      char frequency_string[GPGE_MAX];
      //
      // Obtain the attribute corresponding to the frequency of the MMTL.
      //
      if (sscanf(line, " %*s %*s \"%[^\"]\" %*s\n", frequency_string)!=1)
      {
	printf ("Graphic file error on (%s)\n", line);
	return(FAIL);
      }
      //
      // Attempt to convert the frequency, in any units, into MegaHertz units.
      //
      if (conversion(frequency_string, hertz, temp_value) != SUCCESS)
      {
	printf ("Graphic file error on (%s)\n", line);
	return(FAIL);
      }
      else
      {
	frequency_set=TRUE;
	*frequency = temp_value;
	if (freq_mhz_set == TRUE)
	{
        printf ("Obsolete attribute \"FREQ_MHZ\" superseded by \"FREQUENCY\"\n");
	}
      }
    }
    
    
    //---> Attribute = CONDUCT
    
    /* Conductivity is given by CONDUCT */
    else if (!strncmp(line, "\t\t\"CONDUCT\"", 11) )
    {
      if (conductivity_set == TRUE)
      {
        printf ("Obsolete attribute \"CONDUCT\" superseded by \"CONDUCTIVITY\"\n");
      }
      else
      {
	//
	// Only use this old attribute (CONDUCT)
	// if the new attribute (CONDUCTIVITY) is not yet specified.
	//
	if (sscanf(line, " %*s %*s \" %f \" %*s\n", conductivity)!=1)
	{
	printf ("Graphic file error on (%s)\n", line);
	  return(FAIL);
	}
	else
	{
	  conduct_set=TRUE;
	}
      }
    }
    
    
    //---> Attribute = CONDUCTIVITY 
    
    else if ( !strncmp(line, "\t\t\"CONDUCTIVITY\"", 16) )
    {
      //
      // The variable 'conductivity' is expected to be in units of picoseconds.
      //
      char conductivity_string[GPGE_MAX];
      //
      // Obtain the attribute corresponding to the conductivity of the MMTL metal.
      //
      if (sscanf(line, " %*s %*s \"%[^\"]\" %*s\n", conductivity_string)!=1)
      {
	printf ("Graphic file error on (%s)\n", line);
	return(FAIL);
      }
      //
      // Attempt to convert the conductivity, in any units, into Siemens/meter.
      //
      if (conversion(conductivity_string, siemensPmeter, temp_value) != SUCCESS)
      {
	printf ("Graphic file error on (%s)\n", line);
	return(FAIL);
      }
      else
      {
	conductivity_set=TRUE;
	*conductivity = temp_value;
	if (surf_res_set == TRUE)
	{
        printf ("Obsolete attribute \"SURF_RES\" superseded by \"CONDUCTIVITY\"\n");
	}
	if (conduct_set == TRUE)
	{
        printf ("Obsolete attribute \"CONDUCT\" superseded by \"CONDUCTIVITY\"\n");
	}
      }
    }
    
    
    //---> Attribute = SURF_RES 
    
    else if (!strncmp(line, "\t\t\"SURF_RES\"", 12))
    {
      if (conductivity_set == TRUE)
      {
        printf ("Obsolete attribute \"SURF_RES\" superseded by \"CONDUCTIVITY\"\n");
      }
      else
      {
	//
	// Only use this old attribute if the new value is not yet specified.
	//
	if (sscanf(line, " %*s %*s \" %f \" %*s\n", &surf_resist)!=1)
	{
	printf ("Graphic file error on (%s)\n", line);
	  return(FAIL);
	}
	else
	{
	  surf_res_set=TRUE;
	}
      }
    }
    
    
    //---> Attribute = GND_T_T 
    
    else if (!strncmp(line, "\t\t\"GND_T_T\"", 11))
    {
      if(sscanf(line," %*s %*s \" %f \" %*s\n",top_ground_plane_thickness)!=1)
      {
	printf ("Graphic file error on (%s)\n", line);
	return(FAIL);
      }
    }
    
    
    //---> Attribute = GND_T_B 
    
    else if (!strncmp(line, "\t\t\"GND_T_B\"", 11))
    {
      if(sscanf(line," %*s %*s \" %f \" %*s\n",
		bottom_ground_plane_thickness)!=1)
      {
	printf ("Graphic file error on (%s)\n", line);
	return(FAIL);
      }
    }
    
    
    //---> Attribute = UNITS 
    
    else if (!strncmp(line,"\t\t\"UNITS\"",9))
    {
      char *cp ;
      if(sscanf(line," %*s %*s %s %*s\n",units_string)!=1)
      {
	printf ("Graphic file error on (%s)\n", line);
	return(FAIL);
      }
      for (cp=units_string; *cp!='\0'; cp++) {
	*cp = (char) toupper((int)(*cp));  /* fold to upper case */
      }
      if(strcmp(units_string,"\"MILS\"") == 0)         
	*units = UNITS_MILS;
      else if(strcmp(units_string,"\"MICRONS\"") == 0) 
	*units = UNITS_MICRONS;
      else if(strcmp(units_string,"\"INCHES\"") == 0)  
	*units = UNITS_INCHES;
      else if(strcmp(units_string,"\"METERS\"") == 0)  
	*units = UNITS_METERS;
      else
      {
	printf ("**** Illegal UNITS (%s)...must be MILS, MICRONS, INCHES or METERS\n", units_string);
	return(FAIL);
      }
    }
    
    
    //---> Bottom of attribute checking - none-of-the-above
    
    else
    {
      //
      // Warning: extraneous icon attribute.
      //
      char nme[100], val[100];
      sscanf (line, "%*c%s %*s %s", nme, val);
      printf ("Extraneous icon attribute: %s = %s\n", nme, val);
    }
  }
  
  
  // If conductivity has not been otherwise set then set it from surface resistance.
  
  if (conductivity_set == FALSE)
  {
    
    //  This is the formular relation between surface resistance and conductivity
    //
    //           ( M_PI * frequency * permeability )
    // R  = sqrt ( ------------------------------- )
    //  s        (         conductivity            )
    //
    //                ( M_PI * frequency * permeability )
    // conductivity = ( ------------------------------- )
    //                (            2                    )
    //                (           R                     )
    //                (            s                    )
    
    if(*conductivity == 0.0 && surf_resist != 0.0)
    {
      /* use default frequency (in MHz) if not set */
      if(*frequency == 0.0)
	
	//	    *conductivity = surf_resist * 1.0e-7 * sqrt(DEFAULT_FREQUENCY * 1.0e6);
	
	*conductivity = (M_PI*DEFAULT_FREQUENCY*MU_NAUGHT/
			 (surf_resist*1.0e-7*surf_resist*1.0e-7));
      else
	
	//	    *conductivity = surf_resist * 1.0e-7 * sqrt(*frequency);
	
	*conductivity = (M_PI*(*frequency)*MU_NAUGHT/
			 (surf_resist*1.0e-7*surf_resist*1.0e-7));
    }
    
    
    // Convert conductivity from mho/unit-length to mho/meters.
    
    if(*conductivity != 0.0)
    {
      switch(*units)
      {
      case UNITS_NO_UNITS : /* use mils for the default if not specified. */
      case UNITS_MILS :
	*conductivity /= MILS_TO_METERS;
	break;
      case UNITS_MICRONS :
	*conductivity /= MICRONS_TO_METERS;
	break;
      case UNITS_INCHES :
	*conductivity /= INCHES_TO_METERS;
	break;
      case UNITS_METERS :
	break; /* no conversion needed */
      }
    }
  }
  
  
  // Convert coupling length from given units to meters.
  // Note that only the "old style" 'COUPLING' is set here.
  
  if (coupling_set == TRUE && coupling_length_set == FALSE)
  {
    
    switch(*units)
    {
    case UNITS_NO_UNITS : /* assume mils */
    case UNITS_MILS :
      *coupling *= MILS_TO_METERS;
      break;
    case UNITS_MICRONS :
      *coupling *= MICRONS_TO_METERS;
      break;
    case UNITS_INCHES :
      *coupling *= INCHES_TO_METERS;
      break;
    case UNITS_METERS :
      break; /* no conversion needed */
    }
  }
  
  // Convert coupling length from inches to meters.
  // Note that only the "old style" 'CPLNG_IN' is used here to set coupling
  
  if(cplng_in_set == TRUE && 
     coupling_set == FALSE && 
     coupling_length_set == FALSE)
  {
    *coupling = coupling_in * INCHES_TO_METERS;
  }
  
  return(SUCCESS);
  
}



/*
   
   FUNCTION NAME:  extrct_pts
   
   
   FUNCTIONAL DESCRIPTION:
   
   Extract each (x,y) vertex coordinate from the polygon primitive gpge line.
   If the first vertex extracted has a polypoint pin attribute within
   allowance distance from the actual vertex then store all of the NOT_SCALE
   points (EVERY actual vertex must have a NOT_SCALE polypoint). If one does
   not exist for the first vertex read in then NONE of the others can have
   NOT_SCALE polypoint pin attributes (polypoint attributes must exist for
   all or none of the pins). Store lowest and highest y coords for splitting.
   Calculate the perimeter (x0) around the polygon for interval size setting.
   Create closing polygon point, GPGE does not save it.
   
   FORMAL PARAMETERS:
   
   char *line,                    char array used to store strng read in
   struct contour *polygon,       polygon under investigation
   int *color,                    indicates if it is a signal or ground wire
   struct pins **front,           list of pins
   float *minimum_dimension       used to keep track of smallest length in
   the graphic file
   
   RETURN VALUE:
   
   SUCCESS, FAIL
   
   CALLING SEQUENCE:
   
   if(extrct_pts(line,c_temp,&color,&p_front,&minimum_dimension)
   != SUCCESS)
   
   */

static int extrct_pts(char *line,struct contour *polygon,
				  int *color,
				  struct pins **front,
				  float *minimum_dimension)
{
  char *cp ;
  float length;
  struct polypoints *back, *newp, *ppoint;
  char msg[80],msg1[80];	/* custom string error messages */
  
#ifdef CHECK_SCALE
  int pin_scale;		/* check each pin of the polygon */
  
  *scale = NOT_SET;		/* local scale for entire polygon */
#endif
  
  /* parse each gpge polygon coordinate from the graphic file line */
  cp = line ;
  char *lstcp = line;
  while ( (cp = strchr(cp, '(')) != NULL )
  {

    newp = (struct polypoints *)malloc(sizeof(struct polypoints));
    
#ifdef CHECK_SCALE
    pin_scale = TO_SCALE;       /* initialize flag for each pin */
#endif
    
    if ((sscanf(cp++, "( %lf , %lf ) ",&newp->x,&newp->y)) != 2)
    {
      /* FATAL */ 
	printf ("Graphic file error on (%s)\n", line);
      return(FAIL);
    }
    
    /* look for pin that falls within ALLOWANCE distance of the vertex. */
    /* based on where things were drawn in GPGE, not the attribute values */
    /* Grab the value of the attribute */
    
    /* if couldn't find a pin meeting these requirements - generate message */
    /* and fail */
    
    if ((ppoint=find_pin(front, newp->x, newp->y)) == NULL)
    {
      sprintf(msg,"%lf inches",ALLOWANCE);
      sprintf(msg1,"(%lf,%lf)",newp->x,newp->y);
      printf ("A pin within %s of polygon vertex at %s was not found\n",
	      msg, msg1);
      return(FAIL);
    }
    else
    {
      
      /* found a pin, so update the x and y coordinates from */
      /* the polypoint attribute */
      
      newp->x = ppoint->x; newp->y = ppoint->y;
      
#ifdef CHECK_SCALE
      pin_scale = NOT_SCALE;    /* this pin is NOT_SCALE */
#endif
      
    }
    
#ifdef CHECK_SCALE
    check_scale(pin_scale, scale, "Polygon pin");
#endif
    
    /* if first point in the list, then do some initialization */
    
    if (!polygon->points)
    {
      polygon->points = back = newp;
      polygon->x0 = 0;
      polygon->y1 = polygon->y0 = newp->y; /* initialize y extent points */
    }
    else
    {
      
      /* add segment length to perimeter */
      length = sqrt(pow(fabs(newp->x - back->x), 2.0) +
		    pow(fabs(newp->y - back->y), 2.0));
      polygon->x0 += length;
      
      /* keep track of smallest dimension */
      if(length < *minimum_dimension)
	*minimum_dimension = length;
      
      /* save the extents */
      if (newp->y < polygon->y0) polygon->y0 = newp->y;
      if (newp->y > polygon->y1) polygon->y1 = newp->y;
      
      /* list maintenance */
      back->next = newp;
      back = newp;
    }
    lstcp = cp;
  }
  
  /* color is after coords */
  sscanf(lstcp, " %*[^)] ) %d", color);
  if (*color == GND_COLOR && polygon->name[0])
  {
    /* unused name assigned */
      printf ("Extraneous ground/polygon name  of %s\n", polygon->name);
  }
  
  
  /*
     Close in the polygon:
     
     GPGE does not save the closing polygon point (it deletes a final point
     within some allowance of first point), so recreate the first point as
     the last in the point list for the final segment.
     
     */
  
  /* close poly with 1st point */
  newp = (struct polypoints *)malloc(sizeof (struct polypoints));
  newp->x = polygon->points->x;
  newp->y = polygon->points->y;
  /* add segment length to perimeter */
  length = sqrt(pow(fabs(newp->x - back->x), 2.0) +
		pow(fabs(newp->y - back->y), 2.0));
  polygon->x0 += length;
  
  /* keep track of smallest dimension */
  if(length < *minimum_dimension)
    *minimum_dimension = length;
  
  /* add on to the end of the list */
  back->next = newp;
  /* terminate the list */
  newp->next = NULL;
  
  return(SUCCESS);
  
}


/*
   
   FUNCTION NAME:  find_pin
   
   
   FUNCTIONAL DESCRIPTION:
   
   find a pin that falls in the region. Remove the pin from pin list
   and return attribute list. pins are within allowance of polygon vertex
   
   FORMAL PARAMETERS:
   
   struct pins **front;  the front of the pins list - added to.
   double x0, y0;         vertex is here
   
   
   RETURN VALUE:
   
   polypoint structre constructed from pin or NULL
   
   CALLING SEQUENCE:
   
   if((ppoint=find_pin(front, newp->x, newp->y)) == NULL)
   
   */

static struct polypoints *find_pin(struct pins **front,
				   double x0, double y0)
{
  struct pins *pin;
  struct polypoints *ppoint;
  
  for (pin = *front; pin; pin = pin->left) /* rightmost is front */
    if (x0+ALLOWANCE > pin->x && x0-ALLOWANCE < pin->x &&
	y0+ALLOWANCE > pin->y && y0-ALLOWANCE < pin->y)
    {
      ppoint = pin->att;	/* save attributes, remove pin from list */
      if (pin == *front) *front = pin->left; /* new front of list */
      if (pin->right) pin->right->left = pin->left;
      if (pin->left)  pin->left->right = pin->right;
      free(pin);		/* remove pin so list becomes smaller */
      return(ppoint);		/* return pointer to start of attribute list */
    }
  return(NULL);	/* could not find a pin in the region with coordinates */
}


/*
   
   FUNCTION NAME:  fill_contour
   
   
   FUNCTIONAL DESCRIPTION:
   
   Read all primitive attributes and attach them to the node (signal
   or ground wire - rectangle, polygon or circle). Use attributes that apply.
   
   
   FORMAL PARAMETERS:
   
   struct contour *node,  signal or ground contour to read attributes for
   char prim,             what type: CIRCLE or RECTANGLE or POLYGON
   char *line,            string array for reading in data
   int color,             indicates whether ground or signal
   FILE *in,              graphic file reading from
   
   RETURN VALUE:
   
   SUCCESS, OR FAIL
   
   CALLING SEQUENCE:
   
   if(fill_contour(c_temp,POLYGON,line,color,in) != SUCCESS)
   
   */


static int fill_contour(struct contour *node, char prim,
				    char *line, int color,
				    FILE *in, int units, float *conductivity)
{
  char adequate_dimensions = 0;
  char *ptr;
  int sig_name_set=FALSE, signal_name_set=FALSE;
  int low_left_set=FALSE, lower_left_set=FALSE;
  int up_right_set=FALSE, upper_right_set=FALSE;
  int center_set=FALSE, circle_center_set=FALSE;
  int radius_set=FALSE, circle_radius_set=FALSE;
  char msg[80];			/* custom string error messages */

  *conductivity = 0.0;

  while (strncmp(fgets(line, GPGE_MAX, in), "\t\t$END;", 7))
  {
    
    //---> Attribute = SIG_NAME 
    
    if (color == SIGNAL_COLOR && !strncmp(line, "\t\t\"SIG_NAME\"", 12) )
    {
      if (signal_name_set == TRUE)
      {
        printf ("Obsolete attribute \"SIG_NAME\" superseded by \"SIGNAL_NAME\"\n");
      }
      else
      {
	//
	// Only use this old attribute, SIG_NAME, if the new attribute,
	// SIGNAL_NAME, is not yet specified.
	//
	sscanf(&line[12], " %*[^\"]\" %[^\"]", node->name);
	ptr = strchr(node->name,' '); /* spaces in signal name? */
	if (ptr != NULL)
	{
	  printf ("There is a space in the signal name\n");
	  return(FAIL);
	}
	else
	{
	  sig_name_set=TRUE;
	}
      }
    }
    
    //---> Attribute = SIGNAL_NAME 
    
    else if (color == SIGNAL_COLOR && !strncmp(line, "\t\t\"SIGNAL_NAME\"", 
					       15))
    {
      sscanf(&line[15], " %*[^\"]\" %[^\"]", node->name);
      ptr = strchr(node->name,' ');
      //
      // Spaces are not allowed in signal names and result in FATAL errors.
      //
      if (ptr != NULL)
      {
	  printf ("There is a space in the signal name\n");
	return(FAIL);
      }
      else
      {
	signal_name_set=TRUE;
	if (sig_name_set == TRUE)
	{
        printf ("Obsolete attribute \"SIG_NAME\" superseded by \"SIGNAL_NAME\"\n");
	}
      }
    }
    
    //---> Attribute = CONDUCTIVITY 
    
    else if (color == SIGNAL_COLOR && !strncmp(line, "\t\t\"CONDUCTIVITY\"", 
					       16) )
    {
      double temp_value;

      //
      // The variable 'conductivity' is expected to be in units of picoseconds.
      //
      char conductivity_string[GPGE_MAX];
      //
      // Obtain the attribute corresponding to the conductivity of the MMTL metal.
      //
      if (sscanf(line, " %*s %*s \"%[^\"]\" %*s\n", conductivity_string)!=1)
      {
	printf ("Graphic file error on (%s)\n", line);
	return(FAIL);
      }
      //
      // Attempt to convert the conductivity, in any units, into Siemens/meter.
      //
      if (conversion(conductivity_string, siemensPmeter, temp_value) != 
	  SUCCESS)
	{
	printf ("Graphic file error on (%s)\n", line);
	  return(FAIL);
	}
      else
	{
	  //	conductivity_set = TRUE;
	  *conductivity = temp_value;
	  sprintf (msg, "Conductivity %s = %g siemens/meter",
		   node->name, *conductivity);
	  printf ("%s\n", msg);
	}
    }
    
    //---> Attribute = LOW_LEFT 
    
    else if (prim==RECTANGLE && !strncmp(line, "\t\t\"LOW_LEFT\"", 12))
    {
      if (lower_left_set == TRUE)
      {
        printf ("Obsolete attribute \"LOW_LEFT\" superseded by \"LOWER_LEFT\"\n");
      }
      else
      {
	//
	// Only use this old attribute if the new value is not yet specified.
	//
	if (sscanf(&line[12]," %*[^\"]\" ( %lf , %lf )", &node->x0, &node->y0) != 2)
	{

	  printf ("Bad pin (%s)\n", line);
	  return(FAIL);
	}
	else
	{
	  adequate_dimensions++;
	  convert(units,node->x0);
	  convert(units,node->y0);
	  low_left_set=TRUE;
	}
      }
    }
    
    //---> Attribute = UP_RIGHT 
    
    else if (prim==RECTANGLE && !strncmp(line, "\t\t\"UP_RIGHT\"", 12))
    {
      if (upper_right_set == TRUE)
      {
        printf ("Obsolete attribute \"UP_RIGHT\" superseded by \"UPPER_RIGHT\"\n");
      }
      else
      {
	//
	// Only use this old attribute if the new value is not yet specified.
	//
	if (sscanf(&line[12], " %*[^\"]\" ( %lf , %lf )", &node->x1,
		   &node->y1) != 2)
	{
	  printf ("Bad pin (%s)\n", line);
	  return(FAIL);
	  
	}
	else
	{
	  adequate_dimensions++;
	  convert(units,node->x1);
	  convert(units,node->y1);
	  up_right_set=TRUE;
	}
      }
    }
    
    //---> Attribute = LOWER_LEFT 
    
    else if (!strncmp(line, "\t\t\"LOWER_LEFT\"", 14))
    {
      char x_string[GPGE_MAX];
      char y_string[GPGE_MAX];
      if (sscanf(&line[14]," %*[^\"]\" ( %[^,] , %[^)] )", x_string, y_string)!=2)
      {
	//
	// Fatal error: unable to parse line.
	//
	printf ("Bad pin (%s)\n", line);
	return(FAIL);
      }
      //
      // Attempt to convert the lower_left coordinate, in any units, into
      // meters.
      //
      double xval, yval;
      if ( (conversion(x_string, meters, xval) != SUCCESS) ||
	  (conversion(y_string, meters, yval) != SUCCESS) )
      {
	printf ("Graphic file error on (%s)\n", line);
	return(FAIL);
      }
      else
      {
	//
	// NON SCALE lower_left coord supplied.
	//
	adequate_dimensions++;
	node->x0 = xval;
	node->y0 = yval;
	lower_left_set = TRUE;
	if (low_left_set == TRUE)
	{
        printf ("Obsolete attribute \"LOW_LEFT\" superseded by \"LOWER_LEFT\"\n");
	}
      }
    }
    
    //---> Attribute = UPPER_RIGHT 
    
    else if (!strncmp(line, "\t\t\"UPPER_RIGHT\"", 15))
    {
      char x_string[GPGE_MAX];
      char y_string[GPGE_MAX];
      if (sscanf(&line[15], "%*[^\"]\" ( %[^,] , %[^)] )", x_string, y_string) !=2 )
      {
	//
	// Fatal error: unable to parse line.
	//
	printf ("Bad pin (%s)\n", line);
	return(FAIL);
      }
      //
      // Attempt to convert the upper_right coordinate, in any units, into
      // meters.
      //
      double xval, yval;
      if ( (conversion(x_string, meters, xval) != SUCCESS) ||
	  (conversion(y_string, meters, yval) != SUCCESS) )
      {
	printf ("Graphic file error on (%s)\n", line);
	return(FAIL);
      }
      else
      {
	//
	// NON SCALE upper_right coord supplied.
	//
	adequate_dimensions++;
	node->x1 = xval;
	node->y1 = yval;
	upper_right_set = TRUE;
	if (up_right_set == TRUE)
	{
        printf ("Obsolete attribute \"UP_RIGHT\" superseded by \"UPPER_RIGHT\"\n");
	}
      }
    }
    
    //---> Attribute = RADIUS 
    
    else if (prim==CIRCLE && !strncmp(line, "\t\t\"RADIUS\"", 10))
    {
      if (circle_radius_set == TRUE)
      {
        printf ("Obsolete attribute \"RADIUS\" superseded by \"CIRCLE_RADIUS\"\n");
      }
      else
      {
	//
	// Only use this old attribute, RADIUS, if the new attribute,
	// CIRCLE_RADIUS, is not yet specified.
	//
	if (sscanf(&line[10], " %*[^\"]\" %lf", &node->x1) != 1)
	{
	  printf ("Bad pin (%s)\n", line);
	  return(FAIL);
	}
	else
	{
	  adequate_dimensions++;
	  convert(units,node->x1);
	  radius_set=TRUE;
	}
      }
    }
    
    //---> Attribute = CENTER 
    
    else if (prim==CIRCLE && !strncmp(line, "\t\t\"CENTER\"", 10))
    {
      if (circle_center_set == TRUE)
      {
        printf ("Obsolete attribute \"CENTER\" superseded by \"CIRCLE_CENTER\"\n");
      }
      else
      {
	//
	// Only use this old attribute, CENTER, if the new attribute,
	// CIRCLE_CENTER, is not specified.
	//
	if (sscanf(&line[10], " %*[^\"]\" ( %lf , %lf )", &node->x0,
		   &node->y0) != 2)
	{
	  printf ("Bad pin (%s)\n", line);
	  return(FAIL);
	}
	else
	{
	  adequate_dimensions++;
	  convert(units,node->x0);
	  convert(units,node->y0);
	  center_set=TRUE;
	}
      }
    }
    
    //---> Attribute = CIRCLE_RADIUS 
    
    else if ( (prim==CIRCLE) && !strncmp(line, "\t\t\"CIRCLE_RADIUS\"", 17) )
    {
      char radius_string[GPGE_MAX];
      if (sscanf(&line[17], " %*s \"%[^\"]\"  ", radius_string) != 1)
      {
	//
	// Fatal error: unable to parse line.
	//
	printf ("Bad pin (%s)\n", line);
	return(FAIL);
      }
      //
      // Attempt to convert the circle radius, in any units, into
      // meters.
      //
      double radval;
      if (conversion(radius_string, meters, radval) != SUCCESS)
      {
	printf ("Graphic file error on (%s)\n", line);
	return(FAIL);
      }
      else
      {
	//
	// NON SCALE center coord supplied.
	//
	node->x1 = radval;
	//	      up_rad = FALSE;
	adequate_dimensions++;
	circle_radius_set=TRUE;
	if (radius_set == TRUE)
	{
        printf ("Obsolete attribute \"RADIUS\" superseded by \"CIRCLE_RADIUS\"\n");
	}
      }
    }
    
    //---> Attribute = CIRCLE_CENTER 
    
    else if ( (prim==CIRCLE) && !strncmp(line, "\t\t\"CIRCLE_CENTER\"", 17) )
    {
      char x_string[GPGE_MAX];
      char y_string[GPGE_MAX];
      if (sscanf(&line[17], "%*[^\"]\" ( %[^,] , %[^)] )", x_string, y_string) != 2)
      {
	//
	// Fatal error: unable to parse line.
	//
	  printf ("Bad pin (%s)\n", line);
	return(FAIL);
      }
      //
      // Attempt to convert the circle_center coordinate, in any units, into
      // meters.
      //
      double xval, yval;
      if ( (conversion(x_string, meters, xval) != SUCCESS) ||
	  (conversion(y_string, meters, yval) != SUCCESS) )
      {
	printf ("Graphic file error on (%s)\n", line);
	return(FAIL);
      }
      else
      {
	//
	// NON SCALE radius value supplied.
	//
	adequate_dimensions++;
	node->x0 = xval;
	node->y0 = yval;
	//	      low_cntr = FALSE;
	circle_center_set=TRUE;
	if (center_set == TRUE)
	{
        printf ("Obsolete attribute \"CENTER\" superseded by \"CIRCLE_CENTER\"\n");
	}
      }
    }
    else
    {	       /* Warn user: extraneous primitive attribute (pass \t\t) */
      
      char nme[100], val[100];
      sscanf (line, "%*c%s %*s %s", nme, val);
      printf ("Extraneous primitive attribute: %s = %s\n", 
	      nme, val);
    }
  }
  
  fgets(line, GPGE_MAX, in);	/* get the next primitive line */
  node->primitive = prim;	/* store 'R' or 'A' for output */
  
  /* extrct_pts sets scale for polygons */
  if (prim == POLYGON) return(SUCCESS);
  
  /* for rectangles and circles, emit a warning if too few dimensions */
  if(adequate_dimensions < 2)
  {
    if(prim == RECTANGLE)
    {
      printf ("Missing LOWER_LEFT and/or UPPER_RIGHT attribute on a rectangle\n");
    }
    else if(prim == CIRCLE)
    {
      printf ("Missing CIRCLE_RADIUS and/or CIRCLE_CENTER attribute on a rectangle\n");
    }
    
  }
  
#ifdef CHECK_SCALE
  /* if low_cntr == up_rad == TRUE, scale is unchanged (TO_SCALE) */
  
  if (low_cntr==up_rad && low_cntr==FALSE) /* adjust the node's scale */
    *scale = NOT_SCALE;		/* both coordinates supplied */
  
  else if (low_cntr != up_rad)
  {
      printf ("Missing coordinate attribute on a rectangle or circle\n");
    *scale = MIXED_SCALE;
  }
#endif
  
  
  return(SUCCESS);
}


/*
   
   FUNCTION NAME:  fill_dielectric
   
   
   FUNCTIONAL DESCRIPTION:
   
   Read all primitive attributes for a dielectric structure and attach
   them to the node.
   
   FORMAL PARAMETERS:
   
   struct dielectric *node;  pointer to the dielectric record
   char *line;               character buffer to use
   FILE *in                  graphic file reading
   
   RETURN VALUE:
   
   SUCCESS, FAIL
   
   CALLING SEQUENCE:
   
   if(fill_dielectric(d_temp,line,in) != SUCCESS)
   
   */

static int fill_dielectric(struct dielectric *node,char *line,
				       FILE *in, int units)
{
  char adequate_dimensions = 0;
  int e_cnstnt_set=FALSE, relative_permittivity_set=FALSE;
  int low_left_set=FALSE, lower_left_set=FALSE;
  int up_right_set=FALSE, upper_right_set=FALSE;
  
  while (strncmp(fgets(line, GPGE_MAX, in), "\t\t$END;", 7))
  {
    
    //---> Attribute = E_CNSTNT 
    
    if (!strncmp(line, "\t\t\"E_CNSTNT\"", 12))
    {
      if (relative_permittivity_set == TRUE)
      {
        printf ("Obsolete attribute \"E_CNSTNT\" superseded by \"RELATIVE_PERMITTIVITY\"\n");
      }
      else
      {
	//
	// Only use this old attribute, E_CNSTNT, if the new attribute,
	// RELATIVE_PERMITTIVITY, is not yet specified.
	//
	if (sscanf(&line[12]," %*[^\"]\" %lf ",&node->constant) !=1)
	{
	  printf ("Bad pin (%s)\n", line);
	  return(FAIL);
	}
	else
	{
	  e_cnstnt_set=TRUE;
	}
      }
    }
    
    //---> Attribute = RELATIVE_PERMITTIVITY 
    
    else if (!strncmp(line, "\t\t\"RELATIVE_PERMITTIVITY\"", 25))
    {
      if (sscanf(&line[25]," %*[^\"]\" %f ",&node->constant) !=1)
      {
	//
	// Fatal error: unable to parse line.
	//
	  printf ("Bad pin (%s)\n", line);
	return(FAIL);
      }
      else
      {
	relative_permittivity_set=TRUE;
	if (e_cnstnt_set == TRUE)
	{
        printf ("Obsolete attribute \"E_CNSTNT\" superseded by \"RELATIVE_PERMITTIVITY\"\n");
	}
      }
    }
    
    //---> Attribute = LOW_LEFT 
    
    else if (!strncmp(line, "\t\t\"LOW_LEFT\"", 12))
    {
      if (lower_left_set == TRUE)
      {
        printf ("Obsolete attribute \"LOW_LEFT\" superseded by \"LOWER_LEFT\"\n");
      }
      else
      {
	//
	// Only use this old attribute, LOW_LEFT, if the new attribute,
	// LOWER_LEFT, is not yet specified.
	//
	if (sscanf(&line[12]," %*[^\"]\" ( %lf , %lf )", &node->x0,&node->y0)!=2)
	{
	  printf ("Bad pin (%s)\n", line);
	  return(FAIL);
	}
	else
	{
	  adequate_dimensions++;
	  convert(units,node->x0);
	  convert(units,node->y0);
	  low_left_set=TRUE;
	}
      }
    }
    
    //---> Attribute = UP_RIGHT 
    
    else if (!strncmp(line, "\t\t\"UP_RIGHT\"", 12) )
    {
      if (upper_right_set == TRUE)
      {
        printf ("Obsolete attribute \"UP_RIGHT\" superseded by \"UPPER_RIGHT\"\n");
      }
      else
      {
	//
	// Only use this old attribute, UP_RIGHT, if the new attribute,
	// UPPER_RIGHT, is not yet specified.
	//
	if (sscanf(&line[12],"%*[^\"]\" ( %lf , %lf )",&node->x1,&node->y1) !=2 )
	{
	  printf ("Bad pin (%s)\n", line);
	  return(FAIL);
	}
	else
	{
	  adequate_dimensions++;
	  convert(units,node->x1);
	  convert(units,node->y1);
	  up_right_set=TRUE;
	}
      }
    }
    
    //---> Attribute = LOWER_LEFT 
    
    else if (!strncmp(line, "\t\t\"LOWER_LEFT\"", 14))
    {
      char x_string[GPGE_MAX];
      char y_string[GPGE_MAX];
      if (sscanf(&line[14]," %*[^\"]\" ( %[^,] , %[^)] )", x_string, y_string)!=2)
      {
	//
	// Fatal error: unable to parse line.
	//
	  printf ("Bad pin (%s)\n", line);
	return(FAIL);
      }
      //
      // Attempt to convert the lower_left coordinate, in any units, into
      // meters.
      //
      double xval, yval;
      if ( (conversion(x_string, meters, xval) != SUCCESS) ||
	  (conversion(y_string, meters, yval) != SUCCESS) )
      {
	printf ("Graphic file error on (%s)\n", line);
	return(FAIL);
      }
      else
      {
	//
	// NON SCALE lower_left coord supplied.
	//
	adequate_dimensions++;
	node->x0 = xval;
	node->y0 = yval;
	lower_left_set = TRUE;
	if (low_left_set == TRUE)
	{
        printf ("Obsolete attribute \"LOW_LEFT\" superseded by \"LOWER_LEFT\"\n");
	}
      }
    }
    
    //---> Attribute = UPPER_RIGHT 
    
    else if (!strncmp(line, "\t\t\"UPPER_RIGHT\"", 15))
    {
      char x_string[GPGE_MAX];
      char y_string[GPGE_MAX];
      if (sscanf(&line[15], "%*[^\"]\" ( %[^,] , %[^)] )", 
		 x_string, y_string) !=2 )
      {
	//
	// Fatal error: unable to parse line.
	//
	  printf ("Bad pin (%s)\n", line);
	return(FAIL);
      }
      //
      // Attempt to convert the upper_right coordinate, in any units, into
      // meters.
      //
      double xval, yval;
      if ( (conversion(x_string, meters, xval) != SUCCESS) ||
	  (conversion(y_string, meters, yval) != SUCCESS) )
      {
	printf ("Graphic file error on (%s)\n", line);
	return(FAIL);
      }
      else
      {
	//
	// NON SCALE upper_right coord supplied.
	//
	adequate_dimensions++;
	node->x1 = xval;
	node->y1 = yval;
	upper_right_set = TRUE;
	if (up_right_set == TRUE)
	{
        printf ("Obsolete attribute \"UP_RIGHT\" superseded by \"UPPER_RIGHT\"\n");
	}
      }
    }
    else
    {
      //
      // Warn user: extraneous primitive attribute.
      //
      char nme[100], val[100];
      sscanf (line, "%*c%s %*s %s", nme, val);
      printf ("Extraneous icon attribute: %s = %s\n", nme, val);
    }
  }
  
  fgets(line, GPGE_MAX, in);	/* get the next primitive line */
  
#ifdef CHECK_SCALE
  /* if low==TRUE && up==TRUE, scale is unchanged (TO_SCALE) */
  
  if (low == up && low == FALSE) /* adjust the node's scale */
    *scale = NOT_SCALE;		/* both coordinates supplied */
  
  else if (low != up)
  {
    printf ("Missing LOWER_LEFT and/or UPPER_RIGHT attribute on a dielectric\n");
    *scale = MIXED_SCALE;
  }
#endif
  
  if(adequate_dimensions < 2)
  {
      printf ("Missing LOWER_LEFT and/or UPPER_RIGHT attribute on a dielectric\n");
  }
  
  return(SUCCESS);
}




