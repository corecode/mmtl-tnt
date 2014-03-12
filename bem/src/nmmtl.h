/*
  
  FILE NAME:  nmmtl.h 

  ABSTRACT:  This include file contains the definitions that are common to 
  NMMTL programs.							 
  
  USAGE:  #include "nmmtl.h"
  
  USED BY
  
  AUTHOR:  Kevin Buchs
  
  CREATION DATE:  07/17/91						 
  
  Copyright (C) 1991 by Mayo Foundation. All rights reserved.
  
  */

#ifndef nmmtl_h
#define nmmtl_h

/* Preprocessor directives */

/* make structure members naturally aligned */
#if defined (VAXC) || defined (DECC)
#pragma member_alignment
#endif

/***********************************************
 *                                              *
 *               Include Files                  *
 *                                              *
 ***********************************************/

#ifndef magicad_h
#include "magicad.h"                  /* defines some general constants */
#endif

#ifndef math_h
#include <math.h>                     /* for the pow() and fabs() functions */
#endif

#ifndef float_h
#include <float.h>
#endif

#ifndef graphic_h
#include "graphic.h"		      /* graphic constants */
#endif

#ifdef __TURBOC__
#include "math_lib.h"             /* info on calling NSWC routines */
#include "egn.h"
#else
#include "math_library.h"             /* info on calling NSWC routines */
#include "electro_gendata_netlist.h"
#endif

#ifndef dim_h
#include "dim.h"                      /* to allow dynamic 2D array allocation*/
#endif

/* Remove clashes with other include files */

#ifdef __GNUCPPLIB__
#ifdef PI
#undef PI
#endif
#endif



/************** Conditional compilation flags *****************/


/* don't check for element size being no larger than half a conductor
   dimension */
#define NO_HALF_MIN_CHECKING 1


/* Declare which math routines we will use */

/* define to use IMSL LU factorization and solution route */
/* #define IMSL_LU_ROUTE 1 */
/* define to use IMSL Matrix inversion route */
/* #define IMSL_INV_ROUTE 1 */

/* define to use NSWC LU factorization and solution route */
#define NSWC_LU_ROUTE 1
/* define to use NSWC Matrix inversion route */
#define NSWC_INV_ROUTE 1

/* uncomment this if you want a diagnostic dump file */
/* #define NMMTL_DUMP_DIAG 1 */

/*                Macros                       */

#define ABS(exp) (((exp) < 0) ? -1*(exp) : (exp))

/************************************************
 *						*
 *                Constants.			*
 *						*
 ************************************************/

/* Help library */
#define HELP_LIBRARY_LOC "nmmtl_prompt_2dlf"

/* These constants are used to distinguish GPGE primitives */
#define DIE_COLOR1  GREEN        /* dielectric interfaces may be 2 (green) */
#define DIE_COLOR2  WHITE        /* dielectric interfaces may be 0 (white) */
#define DIE_COLOR3  OLD_WHITE    /* for old graphic files with white=7 */
#define GND_COLOR   BLUE         /* ground wires must be 4 (dark blue) */
#define SIGNAL_COLOR YELLOW      /* signal wires must be 3 (yellow) */
#define DIELECT 'D'
#define CONTOUR 'C'              /* either a SIGNAL OR a GROUND */
#define SIGNAL 'S'
#define GROUND 'G'

/* various flags */

#define REPEAT 1                 /* final line segments repeat last point */
#define NOCNST 0.0               /* flags no dielectric constant to output */
#define NO_COMMENT ".a"          /* mcms_access() no comment log (abort) */
#define ALLOWANCE .0005
#define LINE_LEN 100             /* longest line in an electro_data_ file */
#define EDGE_EFFECTS 1
#define NO_EDGE_EFFECTS 0

/* program control constants */

#define DEFAULT_NON_LINEARITY 1.1 /* default for how fast the non-linear expansion elements scale up */

/* physical constants */

#define AIR_CONSTANT 1.0         /* default dielectric constant (air) */
#define PI 3.141592654L
#define RADIANS_TO_DEGREES 57.29577951L
#define SPEED_OF_LIGHT 2.997925E+08L /* meters/second */
#define EPSILON_NAUGHT 1.0e-9/(36.0*PI)
#define MU_NAUGHT PI*4.0e-7L

/* compound physical constants */
#define C_SQUARED_INVERTED 0.11111111e-16 /* speed of light squared then
 					     inverted */
#define ASSEMBLE_CONST_1 1.0/(2.0*PI*AIR_CONSTANT)

/* define the file extensions used */
#define GEN_IN  "graphic"             /* graphical representation of design */
#define MMTL_OUT "nmmtl" /* final output of nmmtl */

/* logical names (i.e. environment variables) */
#define ER_LOGICAL "NMMTL_ELEMENTS_RETRIEVAL"
#define ED_LOGICAL "NMMTL_ELEMENTS_DUMP"
#define EXPAND_VARIABLE "NMMTL_EXPAND_NL"

/* various icon attribute defaults */
#define DEFAULT_CNTR_SEG 6    /* default number of segments on contours */
#define DEFAULT_PLN_SEG  15    /* default number of segments on planes */
#define DEFAULT_RISETIME 1000.0L  /* risetime if icon attribute not used */
#define DEFAULT_COUPLING 1.0L	 /* coupling len if icon attribute not used */
#define DEFAULT_TO_SCALE .000079375 /* scaling factor if icon attribute not
				       used AND drawing is to scale */
#define DEFAULT_NOT_SCALE .0000254  /* scaling factor if icon attribute not
				       used AND drawing is not to scale */
#define DEFAULT_FREQUENCY 100.0L	 /* frequency if icon attribute not used */
#define DEFAULT_SURF_RESIST 2.61L /* default surface resistance */
#define DEFAULT_CONDUCTIVITY 5.8e7L /* mho/meter for Copper */
#define DEFAULT_GND_THICK 2.54e-5L /* default ground plane thickness meters */
#define DEFAULT_LEFT_EDGE 0.0L    /* default left edge of structure */
#define DEFAULT_RIGHT_EDGE 50.0L  /* default right edge of structure */


/* What should we set the slope to for a vertical line? */
/* this is now defined in nmmtl_qsp_kernel */
extern const float INFINITE_SLOPE;

/* 1.06 The file cad$common:gpge_electro.attributes sets the maximum size for
   the attributes entered. It states that the SIG_NAME primitive attribute
   can be only 10 characters long. SIZE_SIG_NAME must be at least that big. */
#define SIZE_SIG_NAME 30

#define SIZE_RESP 10       /* Response in TERMINATE for Menus */

/* possible orientations of dielectric-dielectric interface */
#define VERTICAL_ORIENTATION 0
#define HORIZONTAL_ORIENTATION 1

/* Floating point exception handler constants */
#define NMMTL_FPE_ELETOOSMALL 1
#define NMMTL_FPE_OTHER 0

/* UNITS CHOICES */

#define UNITS_NO_UNITS 0
#define UNITS_MILS 1
#define UNITS_MICRONS 2
#define UNITS_INCHES 3
#define UNITS_METERS 4


/* UNITS Conversions constants */

#define MILS_TO_METERS     2.54e-5L
#define MICRONS_TO_METERS  1.0e-6L
#define INCHES_TO_METERS   2.54e-2L

/* which end of segments are you dealing with */

#define INITIAL_ENDPOINT  0
#define TERMINAL_ENDPOINT 1
#define NO_ENDPOINT       2

/* Numerical algorithm constants */

#define INTERP_ORDER 2
#define INTERP_PTS (INTERP_ORDER + 1)


/* Various global external variables - tables of Legendre Numbers declared
   in legendre.h and included in nmmtl_main.c */

/* used in nmmtl_assemble* */
#define Legendre_root_a_max 10
#define Legendre_root_a LEGENDRE_10_ROOTS
extern double Legendre_root_a[];
#define Legendre_weight_a LEGENDRE_10_WEIGHTS
extern double Legendre_weight_a[];

/* used in nmmtl_load */
#define Legendre_root_l_max 10
#define Legendre_root_l LEGENDRE_10_ROOTS
extern double Legendre_root_l[];
#define Legendre_weight_l LEGENDRE_10_WEIGHTS
extern double Legendre_weight_l[];

/* used in nmmtl_charge */
#define Legendre_root_c_max 10
#define Legendre_root_c LEGENDRE_10_ROOTS
extern double Legendre_root_c[];
#define Legendre_weight_c LEGENDRE_10_WEIGHTS
extern double Legendre_weight_c[];

/* used in nmmtl_interval */
#define Legendre_root_i_max 6
#define Legendre_root_i LEGENDRE_6_ROOTS
extern double Legendre_root_i[];
#define Legendre_weight_i LEGENDRE_6_WEIGHTS
extern double Legendre_weight_i[];


extern char *range_value;
int range_int(char *value);




/*
  Structures
  */

/* forward references to make C++ happy */
struct pins;
struct polypoints;
struct contour;
struct dielectric;
struct dielectric_sub_segments;
struct dielectric_segments;
struct circle_segments;
struct line_segments;
struct delements;
struct edgedata;
struct elements;
struct conductor_data;
struct point;
struct linesegment;
struct arc;
struct fpksl;
struct gnd_die_list;

/*
  
  Define what is the biggest pointer type on this system - such that it
	could allow enough storage to hold a pointer to any item - char, float,
  struct.  Merely for enhanced portability.
  
  */

#ifdef VMS
typedef char *BIGPOINTER;
#else
/* For now there is only one option - code others here as needed */
typedef char *BIGPOINTER;
#endif


/*
  Structure pins
  
  linked list of pins and attributes, are used only for polygon polypoints.
  
  */

typedef struct pins
{
  struct pins *left, *right; /* for easy removal of used pins */
  struct polypoints *att;    /* points to pins list of polypoints */
  double x, y;                /* position of pin ties it to a polygon */
} PINS;

/*
  
  Structure CONTOUR
  
  This structure is used to provide an ordered list of all signals or
  grounds.  primitive is a keyletter from this set: 'R'=rectangle,
  'A'=circle or 'G'=polygon.  Name is the assigned or auto generated
  name.  The x0,y0,x1,y1 fields have different uses depending on
  primitive type:
  
  rectangle: (x0,y0)=low_left, (x1,y1)=up_right
  circle   : (x0,y0)=center, x1=radius
  polygon  : y0=lowest y, y1=highest y, x0=perimeter.
  
  Points is a linked list of coordinates for POLYGONS ONLY.  The next
  pointer is used to make a linked list of signals or all grounds.
  Conductivity is a pointer used for the self-inductance program
  and should be set to NULL.
  */

typedef struct contour
{
  struct contour *next;
  struct polypoints *points;
  float conductivity;
  double x0,y0,x1,y1;
  char primitive;
  char name[SIZE_SIG_NAME];
} CONTOURS, *CONTOURS_P;


/*
  
  Structure POLYPOINTS
  
  This is used to form a linked list of coordinates for POLYGONS ONLY
  where point and point->next are the endpoints of a line segment.
  */

typedef struct polypoints
{
  struct polypoints *next;
  double x, y;
} POLYPOINTS, *POLYPOINTS_P;

/*
  
  Structure DIELECTRIC
  
  This structure contains data on a dielectric interface.  The lower
  left is given by (x0,y0) and the upper right is given by (x1,y1)
  Constant gives the dielectric constant, and tanget gives the loss
  tangent (2dly only - set to 0.0 for now).  Next points to the next
  dielectric (rectangular) layer on this list.
  */

typedef struct dielectric
{
  struct dielectric *next;
  double x0, y0, x1, y1;
  float constant, tangent;
} DIELECTRICS, *DIELECTRICS_P;


/*
  Data Structures used for segmentation:
  
  In all of these structures the field "divisions" is the number of
  elements to break a segment into.  This number is proportionally
  divided each time a segment split is performed.  It is rounded up to
  the next nearest integer.
  
  Dielectric Sub Segments
  
  Used to build up matching pairs of horizontal or vertical boundaries.
  At holds the y coordinate for horizontal sub segments, and the x coordinate
  for vertical sub segments.  Start and end contain the endpoints.
  
  */

typedef struct dielectric_sub_segments
{
  struct dielectric_sub_segments *next;
  double at;
  double start, end;
  float epsilon;
  int divisions;
} DIELECTRIC_SUB_SEGMENTS;

/*
  Dielectric Segments
  
  
  Originally contains the matching boundaries between dielectric
  layers, but is further segmented by conductors intersecting the
  segments.  The normals of dielectric elements will be taken to be
  up or left.  Epsilonplus is on the normal side of the interface.
  End_in_conductor is 1 if initial endpoint of segment intersects a
  conductor boundary and then proceeds into the conductor.
  End_in_conductor is 2 if same is true for the terminal endpoint
  and 3 if true for both.  It is zero if it is true for neither.
  
  */

typedef struct dielectric_segments
{
  struct dielectric_segments *next;
  double at, start, end;
  double length;
  float epsilonplus, epsilonminus;
  int divisions;
  int segment_number;
  unsigned char end_in_conductor;
  unsigned char orientation; /* one of VERTICAL_ORIENTATION, HORIZONTAL_ORI */
} DIELECTRIC_SEGMENTS, *DIELECTRIC_SEGMENTS_P;


/*
  Circle Segments
  
  Any conductors or ground wires defined as circles are represented by
  segments or
  arcs, which are further fractured by intersections with dielectric
  interfaces.  Epsilon starts at 0.0, and is set appropriately after
  that.  One epsilon for each end.  Radians is the total swing of the
  arc in radians.
  */

typedef struct circle_segments
{
  struct circle_segments *next;
  double centerx,centery,radius;
  double startangle,endangle;
  double radians;
  float epsilon[2];
  int divisions;
  int conductor;
} CIRCLE_SEGMENTS, *CIRCLE_SEGMENTS_P;


/*          
  Line Segments
  
  Holds sides of rectangular and polygonal conductors and ground
  wires, also further
  fractured by intersections with dielectric interfaces.  Epsilon
  and theta2 are initialized to 0.0 and then set to appropriate
  values later.  If theta2 is not set, then segment does not end or
  start on an edge.  Nu and free_space_nu is initialized to -1.0.
  Edge_pair is set to the other segments which form edges with this
  segment.  Theta2, nu, and edge_pair are arrays where the first
  entry is for the initial end of the segment, defined by
  (startx,starty), and the second entry is for the terminal end of
  the segment, defined by (endx,endy).  interior is the direction to
  the inside of the conductor.  Left is a positive turn and is given
  by a +1, and right is given by a -1.  Conductor is the conductor
  number to which the segments belong.
  */

typedef struct line_segments
{
  struct line_segments *next;
  struct line_segments *edge_pair[2];
  double startx,starty,endx,endy;
  double length;
  float theta2[2];
  float nu[2],free_space_nu[2];
  float epsilon[2];
  int interior;
  int divisions;
  int conductor;
} LINE_SEGMENTS, *LINE_SEGMENTS_P;

/*
  
  Elements
  
  These are what segments are divided into before numerical processing.
  
  */

/* Dielectric elements */

typedef struct delements
{
  struct delements *next;
  double xpts[INTERP_PTS],ypts[INTERP_PTS];
  float epsilonplus,epsilonminus;
  float normalx,normaly;
  int node[INTERP_PTS];
  
} DELEMENTS, *DELEMENTS_P;


/* Conductor elements
   
   For celement: next is a pointer to the next element for the same conductor.
   
   all conductor element lists start from an array of conductor_data 
   structures which points to the first celement, and then you can
   follow the next pointer to access the rest of the list.
   
	 */

/* Edge data needed by conductor elements */

typedef struct edgedata
{
  float nu, free_space_nu;
  
} EDGEDATA, *EDGEDATA_P;

/* the actual conductor elements */

typedef struct celements
{
  struct celements *next;
  EDGEDATA_P edge[2];
  double xpts[INTERP_PTS],ypts[INTERP_PTS];
  float epsilon;
  int node[INTERP_PTS];
  
} CELEMENTS, *CELEMENTS_P;

/* the head of the conductor element lists - will be used in an array */

typedef struct conductor_data
{
  CELEMENTS_P elements;
  int node_start, node_end;
  
} CONDUCTOR_DATA, *CONDUCTOR_DATA_P;

          

/*
  Point
  
  A simple point structure - good for intersections, etc
  
  */

typedef struct point
{
  double x,y;
  
} POINT, *POINT_P;


/*
  LineSegments
  
  A simple line segment
  
  */

typedef struct linesegment
{
  double x[2],y[2];
  
} LINESEG, *LINESEG_P;


/*
  Arc
  
  A simple arc
  
  */

typedef struct arc
{
  double center_x,center_y,radius;
  double start_angle, end_angle;
  
}  ARC, *ARC_P;


/*
  
  fpksl
  
  Floating point keyed sorted list - used for generating a sorted linked
  list based on a floating point (double) key.
  
  */
typedef struct fpksl
{
  double key;
  BIGPOINTER data;
  struct fpksl *next;
  
} FLT_KEY_LIST, *FLT_KEY_LIST_P;

/*
  COND_PROJ_LIST
  */
/* use the FLT_KEY_LIST to hold Conductor projection data */
typedef FLT_KEY_LIST COND_PROJ_LIST;
typedef FLT_KEY_LIST_P COND_PROJ_LIST_P;

/*
  SORTED_GND_DIE_LIST
  */
/* use the FLT_KEY_LIST to hold Ground plane - Dielectric intersection data */
typedef FLT_KEY_LIST SORTED_GND_DIE_LIST;
typedef FLT_KEY_LIST_P SORTED_GND_DIE_LIST_P;


/*
  
  gnd_die_list
  
  Unsorted List of dielectric regions along ground planes.  Compare to 
  SORTED_GND_DIE_LIST.
  */

typedef struct gnd_die_list
{
  double start,end;
  float epsilon;
  struct gnd_die_list *next;
} GND_DIE_LIST, *GND_DIE_LIST_P;


/*

   extent_data

   data on extent of cross section and entire conductor region

   */

typedef struct extent_data
{
  /* cross section extents */
  double left_cs_extent, right_cs_extent;

  /* conductor extents */
  double left_cond_extent, right_cond_extent, min_cond_height;

  /* desired cross section extents */
  double desired_left, desired_right;

  /* flags to indicate that expansion should be performed */
  boolean expand_left, expand_right;

} EXTENT_DATA, *EXTENT_DATA_P;


/****************************************
 *                                       *
 *   Function Prototypes                 *
 *                                       *
 ****************************************/

int electro_read_icon_section(FILE *fp, NETLIST_REC *netlist, FILE *err);

int electro_generate_netlist(NETLIST_REC *netlist, struct contour *signals, 
			     char *node, char *dir, int version, FILE *err);


int nmmtl_qsp_calculate(struct dielectric *dielectrics,
				    struct contour  *signals,
				    struct contour  *groundwires,
				    int gnd_planes,
				    float half_minimum_dimension,
				    int cntr_seg,
				    int pln_seg,
				    float coupling,
				    float risetime,
				    float **electrostatic_induction,
				    float **inductance,
				    float *characteristic_impedance,
				    float *propagation_velocity,
				    float *equivalent_dielectric,
				    FILE *output_file1,
				    FILE *output_file2);

/*
  Notes:
  
  1) electrostatic_induction, inductance, equivalent_dielectric, 
  propagation_velocity, and
  characteristic_impedance are outputs and must be allocated before
  calling nmmtl_qsp_calculate using the dim2 (general lib) routine.
  They are of order n x n for first two and order n for other two,
  where n is the number of conductors.
  
  2) if output_file1 and output_file2 is NULL, then no output is written.
  It is up to the calling function to open the file and close the file.
  The strategy here is to pass in two file pointer if you need two
  copies of the results for a particular run and one if you need one copy,
  and zero if the output is handled the by the calling function.  An 
  example is the prompting interface to MMTL, which will want a full report
  put in the regular results file for the last version of swept geometry
  type run.  It may also want long reports for each interation in the
  sweeping.  Then it would pass in one file pointer to all runs, except
  the last, into which it would pass two file pointers.  If shorter output
  is desired, it can be handled by the calling routine and only one file
  pointer is passed in to the last version and no file pointers to all other 
  versions.
  
  3) returns a status of FAIL or SUCCESS.
  */

int nmmtl_xtk_calculate(int number_conductors,
				    struct contour *signals,
				    float **electrostatic_induction,
				    float **inductance,
				    float coupling,
				    float risetime,
				    float *propagation_velocity,
				    float **forward_xtk,
				    float **backward_xtk,
				    FILE *output_file1,
				    FILE *output_file2);

/*
  Notes:
  
  1) forward_xtk, and backward_xtk are outputs and must be allocated 
  using the dim2 routine (general lib) before calling.  They are of 
  order n x n where n is the number of conductors.
  
  2) nmmtl_xtk_calculate depends on electrostatic_induction, inductance 
  and propagation_velocity calculated from nmmtl_qsp_calculate, but it 
  may be called repeatedly with different coupling and risetime parameters
  without the need to call nmmtl_qsp_calculate repeatedly.
  
  3) if output_file1 and output_file2 is NULL, then no output is written.
  It is up to the calling function to open the file and close the file.
  The strategy here is to pass in two file pointer if you need two
  copies of the results for a particular run and one if you need one copy,
  and zero if the output is handled the by the calling function.  An 
  example is the prompting interface to MMTL, which will want a full report
  put in the regular results file for the last version of swept geometry
  type run.  It may also want long reports for each interation in the
  sweeping.  Then it would pass in one file pointer to all runs, except
  the last, into which it would pass two file pointers.  If shorter output
  is desired, it can be handled by the calling routine and only one file
  pointer is passed in to the last version and no file pointers to all other 
  versions.
  
  4) returns a status of FAIL or SUCCESS.
  */

/* nmmtl_add_to_sorted_list.c */
void nmmtl_add_to_sorted_list(FLT_KEY_LIST_P *list,double key,char *data);

/* nmmtl_angle_of_intersections.c */
float nmmtl_angle_of_intersection(float x1, float y1, float x2, float y2);

/* nmmtl_assemble.cxx */
void nmmtl_assemble(int conductor_counter,
		    CONDUCTOR_DATA_P conductor_data,
		    DELEMENTS_P die_elements,
		    float length_scale,
		    float **assemble_matrix);

/* nmmtl_assemble_free_space.cxx */
void nmmtl_assemble_free_space(int conductor_counter,
			       CONDUCTOR_DATA_P conductor_data,
			       float **assemble_matrix);


/* nmmtl_build_gnd_die_list.cxx */
GND_DIE_LIST_P nmmtl_build_gnd_die_list(GND_DIE_LIST_P *head,
                                        GND_DIE_LIST_P tail,
                                        DIELECTRICS_P dielectrics);

/* nmmtl_charge.cxx */
void nmmtl_charge(float *sigma_vector,
		  int conductor_counter,
		  CONDUCTOR_DATA_P conductor_data,
		  float *electrostatic_induction);

void nmmtl_charge_free_space(float *sigma_vector,
			     int conductor_counter,
			     CONDUCTOR_DATA_P conductor_data,
			     float *electrostatic_induction);

/* nmmtl_charimp_propvel_calculate.cxx */
int nmmtl_charimp_propvel_calculate(int number_conductors,
                                    struct contour *signals,
                                    float **electrostatic_induction,
                                    float **inductance,
                                    float **cap_abs_diel,
                                    float *characteristic_impedance,
                                    float *propagation_velocity,
                                    float *equivalent_dielectric,
                                    FILE *output_file1,
				    FILE *output_file2);

/* nmmtl_circle_segments.c */
int nmmtl_cirseg_endpoint(CIRCLE_SEGMENTS_P segment,POINT_P point);

double nmmtl_cirseg_angle_to_normal(CIRCLE_SEGMENTS_P cirseg,
				   POINT_P point,
				   LINESEG_P seg,
				   int seg_index);

double nmmtl_cirseg_point_angle(CIRCLE_SEGMENTS_P cirseg,double point_x,
			       double point_y);

void nmmtl_cirseg_angle_point(CIRCLE_SEGMENTS_P cirseg,double angle,
			      double *point_x,double *point_y);

/* nmmtl_combine_die.cxx */
int nmmtl_combine_die(struct dielectric *dielectrics,
		      int plane_segments,
		      int gnd_planes,double top_of_bottom_plane,
		      double bottom_of_top_plane,double left_of_gnd_planes,
		      double right_of_gnd_planes,
		      struct dielectric_segments **dielectric_segments,
		      SORTED_GND_DIE_LIST_P *lower_sorted_gdl,
		      SORTED_GND_DIE_LIST_P *upper_sorted_gdl);

/* nmmtl_containment.c */
int nmmtl_seg_in_die_rect(DIELECTRICS_P die_rect,LINESEG_P line);

int nmmtl_circle_in_die_rect(DIELECTRICS_P die,POINT_P center,double radius);

/* nmmtl_convert_units.cxx */
int nmmtl_convert_units(float conversion,
                                    struct dielectric *dielectrics,
                                    struct contour *signals,
                                    struct contour *groundwires);

/* nmmtl_det_arc_intersections.cxx */
int nmmtl_determine_arc_intersectio(CIRCLE_SEGMENTS_P *circle_segments, DIELECTRIC_SEGMENTS_P *dielectric_segments);

/* nmmtl_det_intersections.cxx */
int nmmtl_determine_intersections(LINE_SEGMENTS_P *line_segments,
			      DIELECTRIC_SEGMENTS_P *dielectric_segments);

/* nmmtl_dump.cxx */
void nmmtl_dump(FILE *dump_file,
		int cntr_seg,
		int pln_seg,
		float coupling,
		float risetime,
		struct contour *signals,
                int conductor_counter,
                CONDUCTOR_DATA_P conductor_data,
                DELEMENTS_P die_elements,
                unsigned int node_point_counter,
                unsigned int highest_conductor_node);

/* nmmtl_dump_dielectric_segments.cxx */
int nmmtl_dump_dielectric_segment(struct dielectric_segments *seg);

/* nmmtl_dump_elements.cxx */
void nmmtl_dump_elements(int conductor_counter,
                         CONDUCTOR_DATA_P conductor_data,
                         DELEMENTS_P die_elements);

/* nmmtl_dump_geometry.cxx */
void nmmtl_dump_rectangle(CONTOURS_P contour);
                       
void nmmtl_dump_polygon(CONTOURS_P contour);

void nmmtl_dump_circle(CONTOURS_P contour);

void nmmtl_dump_geometry(int cntr_seg,int pln_seg,
                         float coupling,float risetime,
                         float conductivity,float frequency,
                         float half_minimum_dimension,
                         int gnd_planes,
                         float top_ground_plane_thickness,
                         float bottom_ground_plane_thickness,
                         struct dielectric *dielectrics,
                         struct contour *signals,
                         struct contour *groundwires);

/* nmmtl_dump_segments.c */
int nmmtl_dump_segments(DIELECTRIC_SEGMENTS_P ds,
			LINE_SEGMENTS_P ls,
			CIRCLE_SEGMENTS_P cs);

/* nmmtl_evaluate_circles.cxx */
int nmmtl_evaluate_circles(int cntr_seg,
#ifndef NO_HALF_MIN_CHECKING
				       float half_minimum_dimension,
#endif
				       int conductor_counter,
				       CONTOURS_P contour,
				       CIRCLE_SEGMENTS_P *segments,
				       EXTENT_DATA_P extent_data);

/* nmmtl_evaluate_conductors.cxx */
int 
    nmmtl_evaluate_conductors(struct dielectric *dielectrics,
			      double air_starts,
			      int cntr_seg,
			      EXTENT_DATA_P extent_data,
			      int *conductor_counter,
			      CONTOURS_P *signals,
			      CONTOURS_P *groundwires,
			      LINE_SEGMENTS_P *conductor_ls,
			      CIRCLE_SEGMENTS_P *conductor_cs,
			      DIELECTRIC_SEGMENTS_P *dielectric_segments);

/* nmmtl_evaluate_polygons.cxx */
int nmmtl_evaluate_polygons(int cntr_seg,
#ifndef NO_HALF_MIN_CHECKING
					float half_minimum_dimension,
#endif
					int conductor_counter,
					CONTOURS_P contour,
					LINE_SEGMENTS_P *segments,
          EXTENT_DATA_P extent_data);

/* nmmtl_evaluate_rectangles.cxx */
int nmmtl_evaluate_rectangles(int cntr_seg,
#ifndef NO_HALF_MIN_CHECKING
					  float half_minimum_dimension,
#endif
					  int conductor_counter,
					  CONTOURS_P contour,
					  LINE_SEGMENTS_P *segments,
            EXTENT_DATA_P extent_data);

/* nmmtl_fill_die_gaps.cxx */
int nmmtl_fill_die_gaps(int orientation,int *segment_number,
			double top_stack,
			struct dielectric_sub_segments **seg1,
			struct dielectric_sub_segments **seg2,
			struct dielectric_segments **dielectric_segments);

/* nmmtl_find_ground_planes.cxx */
/*
  Notes:
  
  1) inputs are list of dielectrics and number of ground planes.
  The top_of_bottom_plane and bottom_of_top_plane are return values
  and must have been allocated.  If gnd_planes is 1, then
  bottom_of_top_plane is not assigned.
  */
int nmmtl_find_ground_planes(struct dielectric *dielectrics,
					 double *top_of_bottom_plane,
					 double *bottom_of_top_plane,
					 double *left, double *right);

/* nmmtl_form_die_subseg.cxx */
int nmmtl_form_die_subseg(int plane_segments,
                                      struct dielectric *dielectrics,
                                      double top_of_bottom_plane,
                                      double bottom_of_top_plane,
                                      double left_of_gnd_planes,
                                      double right_of_gnd_planes,
                                      struct dielectric_sub_segments **top_seg,
                                   struct dielectric_sub_segments **bottom_seg,
                                     struct dielectric_sub_segments **left_seg,
                                    struct dielectric_sub_segments **right_seg,
                                      SORTED_GND_DIE_LIST_P *lower_sorted_gdl,
                                      SORTED_GND_DIE_LIST_P *upper_sorted_gdl);

/* nmmtl_find_nu.cxx */
float nmmtl_find_nu(float epsilon1,float epsilon2,float theta1,float theta2);

float nmmtl_nu_function(float *nu);

/* nmmtl_intersections.c */
POINT_P nmmtl_cd_intersect(CONTOURS_P this_contour,
			   DIELECTRIC_SEGMENTS_P dieseg);

int nmmtl_rect_seg_inter(CONTOURS_P contour,
			 LINESEG segment,
			 POINT_P intersection);

int nmmtl_poly_seg_inter(CONTOURS_P contour,
			 LINESEG segment,
			 POINT_P intersection);

int nmmtl_circle_seg_inter(CONTOURS_P contour,
			   LINESEG segment,
			   POINT_P intersection);

int nmmtl_arc_seg_inter(ARC arc,
			LINESEG seg,
			POINT_P intersection1,
			POINT_P intersection2,
			int *tangent);

int nmmtl_in_arc_range(ARC arc,double root_x,double root_y);

int nmmtl_cirseg_seg_inter(CIRCLE_SEGMENTS_P cirseg,
			   LINESEG_P seg,
			   POINT_P intersection1,
			   POINT_P intersection2,
			   int *tangent); 

int nmmtl_in_cirseg_range(CIRCLE_SEGMENTS_P cirseg,double root_x,double root_y);

int nmmtl_seg_seg_inter(LINESEG_P segment1,
			LINESEG_P segment2,
			int *colinear,
			POINT_P intersection,
			POINT_P intersection2);

int nmmtl_in_seg_range(LINESEG_P segment,double x,double y);

/* nmmtl_evaluate_polygons.c */
int nmmtl_evaluate_polygons(int cntr_seg,
					float half_minimum_dimension,
					int conductor_counter,
					CONTOURS_P contour,
					LINE_SEGMENTS_P *segments);

/* nmmtl_form_die_subseg.cxx */
int
    nmmtl_form_die_subseg(int plane_segments,
			  struct dielectric *dielectrics,
			  double top_of_bottom_plane,
			  double bottom_of_top_plane,
			  double left_of_gnd_planes,
			  double right_of_gnd_planes,
			  struct dielectric_sub_segments **top_seg,
			  struct dielectric_sub_segments **bottom_seg,
			  struct dielectric_sub_segments **left_seg,
			  struct dielectric_sub_segments **right_seg,
			  SORTED_GND_DIE_LIST_P *lower_sorted_gdl,
			  SORTED_GND_DIE_LIST_P *upper_sorted_gdl);

/* nmmtl_fpe_handler.cxx */
void nmmtl_fpe_handler(void *arg);

/* nmmtl_gen_netlist_from_icon.cxx */
int nmmtl_gen_netlist_from_icon(NETLIST_REC *netlist, 
				struct contour *signals,
				char *basename);

/* nmmtl_genel.cxx */
int nmmtl_generate_elements(int conductor_counter,
					CONDUCTOR_DATA_P *conductor_data,
					DELEMENTS_P *die_elements,
					unsigned int *node_point_counter,
					unsigned int *highest_conductor_node,
					LINE_SEGMENTS_P conductor_ls,
					CIRCLE_SEGMENTS_P conductor_cs,
					DIELECTRIC_SEGMENTS_P dielectric_segments,
					int gnd_planes,
					SORTED_GND_DIE_LIST_P upper_sorted_gdl,
					int pln_seg,
					double bottom_of_top_plane,
					double left_of_gnd_planes,
					double right_of_gnd_planes,
					EXTENT_DATA_P extent_data);

/* nmmtl_genel_ccs.cxx */
int nmmtl_generate_elements_ccs(CIRCLE_SEGMENTS_P *ccsp,
				CELEMENTS_P *head,
				CELEMENTS_P *tail,
				unsigned int *node_point_counter);

/* nmmtl_genel_cls.cxx */
int nmmtl_generate_elements_cls(LINE_SEGMENTS_P *clsp,
				CELEMENTS_P *head,
				CELEMENTS_P *tail,
				unsigned int *node_point_counter);

/* nmmtl_genel_die.cxx */
DELEMENTS_P nmmtl_generate_elements_die(DIELECTRIC_SEGMENTS_P ds,
                                        unsigned int *node_point_counter,
                                        int *number_elements,
                                        EXTENT_DATA_P extent_data);

/* nmmtl_genel_gnd.cxx */
int nmmtl_generate_elements_gnd(CELEMENTS_P *gnd_plane_list_head,
				unsigned int *node_point_counter,
				int gnd_planes,
				int pln_seg,
				double bottom_of_top_plane,
				double left_of_gnd_planes,
				double right_of_gnd_planes,
#ifdef GND_PLANE_COND_PROJECTION
				COND_PROJ_LIST_P cond_projections,
#endif
				SORTED_GND_DIE_LIST_P upper_sorted_gdl);

/* nmmtl_intersections.cxx */
POINT_P nmmtl_cd_intersect(CONTOURS_P contour,
			   DIELECTRIC_SEGMENTS_P dieseg);

int nmmtl_rect_seg_inter(CONTOURS_P contour,
			 LINESEG segment,
			 POINT_P intersection);

int nmmtl_poly_seg_inter(CONTOURS_P contour,
			 LINESEG segment,
			 POINT_P intersection);

int nmmtl_circle_seg_inter(CONTOURS_P contour,
			   LINESEG segment,
			   POINT_P intersection);

int nmmtl_arc_seg_inter(ARC arc,
			LINESEG seg,
			POINT_P intersection1,
			POINT_P intersection2,
			int *tangent);

int nmmtl_in_arc_range(ARC arc,double root_x,double root_y);

int nmmtl_cirseg_seg_inter(CIRCLE_SEGMENTS_P cirseg,
			   LINESEG_P seg,
			   POINT_P intersection1,
			   POINT_P intersection2,
			   int *tangent);

int nmmtl_in_cirseg_range(CIRCLE_SEGMENTS_P cirseg,double root_x,double root_y);

int nmmtl_seg_seg_inter(LINESEG_P segment1,
			LINESEG_P segment2,
			int *colinear,
			POINT_P intersection,
			POINT_P intersection2);

int nmmtl_in_seg_range(LINESEG_P segment,double x,double y);

/* nmmtl_interval.cxx */
void nmmtl_interval_c(double x,
		      double y,
		      CELEMENTS_P cel,
		      double *value,
		      int outer_cond_flag,
		      float normalx,
		      float normaly);

void nmmtl_interval_self_c(double x,
			   double y,
			   CELEMENTS_P cel,
			   double *value,
			   double point);

void nmmtl_interval_c_fs(double x,
			 double y,
			 CELEMENTS_P cel,
			 double *value);

void nmmtl_interval_self_c_fs(double x,
			      double y,
			      CELEMENTS_P cel,
			      double *value,
			      double point);

void nmmtl_interval_d(double x,
		      double y,
		      DELEMENTS_P del,
		      double *value,
		      int outer_cond_flag,
		      float normalx,
		      float normaly);

void nmmtl_interval_self_d(double x,
			   double y,
			   DELEMENTS_P del,
			   double *value,
			   double point,
			   float normalx,
			   float normaly);

/* nmmtl_jacobian.cxx */
void nmmtl_jacobian_d(double local,DELEMENTS_P del,double *Jacobian);

void nmmtl_jacobian_c(double local,CELEMENTS_P cel,double *Jacobian);

/* nmmtl_load.cxx */
void nmmtl_load(float *potential_vector,
		int conductor_number,
		CONDUCTOR_DATA_P conductor_data);

void nmmtl_load_free_space(float *potential_vector,
			   int conductor_number,
			   CONDUCTOR_DATA_P conductor_data);

/* nmmtl_merge_die_subseg.cxx */
int
    nmmtl_merge_die_subseg(int orientation,int *segment_number,
			   double top_stack,
			   struct dielectric_sub_segments **seg1,
			   struct dielectric_sub_segments **seg2,
			   struct dielectric_segments **dielectric_segments);

/* nmmtl_new_die_seg.cxx */
int
     nmmtl_new_die_seg(struct dielectric_segments **dielectric_segments,
		       struct dielectric_sub_segments *list1,
		       struct dielectric_sub_segments *list2,
		       int orientation,int segment_number,
		       double overlap_left, double overlap_right);

/* nmmtl_nl_expand.cxx */
void nmmtl_nl_expand(double xstart, double xend, double incr_start,
		     float epsilonplus,
                     float epsilonminus,float normaly,double y,
                     unsigned int *node_point_counter,
                     DELEMENTS_P *element_p,
                     int *number_elements,unsigned int common_node);

/* nmmtl_orphans.cxx */
int nmmtl_orphans(CIRCLE_SEGMENTS_P *conductor_cs,
			      LINE_SEGMENTS_P *conductor_ls,
			      DIELECTRICS_P dielectrics,
			      double air_starts,
			      DIELECTRIC_SEGMENTS_P *dielectric_segments);

/* nmmtl_output_crosstalk.c */
void nmmtl_output_crosstalk(FILE *output_fp, float **forward_xtk, 
			    float **backward_xtk, struct contour *signals);


/* nmmtl_output_charimp_propvel.c */
void nmmtl_output_charimp_propvel(FILE *output_fp, /* output file ptr */
				  /* data to be output */
				  float *characteristic_impedance,
				  float *propagation_velocity,
				  float *equivalent_dielectric,
				  float even_odd[4],
				  /* signal line info */
				  struct contour *signals);
/* nmmtl_output_headers.cxx */
void nmmtl_output_headers(FILE *output_file,
			  char *filename,
			  int num_signals,
			  int num_grounds,
			  int num_ground_planes,
			  float coupling,
			  float risetime,
			  int cntr_seg,
			  int pln_seg);

/* nmmtl_output_matrices.cxx */
void nmmtl_output_matrices(FILE *output_fp, float **electrostatic_induction,
			   float **inductance, struct contour *signal);

/* nmmtl_overlap_parallel_set.cxx */
int nmmtl_overlap_parallel_seg(struct dielectric_sub_segments *list1,
			       struct dielectric_sub_segments *list2,
			       double *overlap_left, double *overlap_right,
			       int *left_overhang, int *right_overhang);

/* nmmtl_parse_graphic.cxx */
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
			int *units);

/* nmmtl_projections.cxx */
void nmmtl_project_polygon(COND_PROJ_LIST_P *cond_projections,
			   CONTOURS_P contour);

void nmmtl_project_rectangle(COND_PROJ_LIST_P *cond_projections,
			     CONTOURS_P contour);

void nmmtl_project_circle(COND_PROJ_LIST_P *cond_projections,
			  CONTOURS_P contour);

/* nmmtl_qsp_kernel.cxx */
int nmmtl_qsp_kernel(int conductor_counter,
				 CONDUCTOR_DATA_P conductor_data,
				 DELEMENTS_P die_elements,
				 unsigned int node_point_counter,
				 unsigned int highest_conductor_node,
				 double length_scale,
				 float **electrostatic_induction,
				 float **inductance,
				 float *characteristic_impedance,
				 float *propagation_velocity,
				 float *equivalent_dielectric,
				 FILE *output_file1,
				 FILE *output_file2,
				 CONTOURS_P signals);

/* nmmtl_retrieve.cxx */
int nmmtl_retrieve(FILE *retrieve_file,
			       int *cntr_seg,
			       int *pln_seg,
			       float *coupling,
			       float *risetime,
			       CONTOURS_P *psignals,
			       int *sig_cnt,
			       int *pconductor_counter,
			       CONDUCTOR_DATA_P *pconductor_data,
			       DELEMENTS_P *pdie_elements,
			       unsigned int *pnode_point_counter,
			       unsigned int *phighest_conductor_node);

/* nmmtl_set_offset.cxx */
int nmmtl_set_offset(double offset,struct dielectric *dielectrics,
				 struct contour *signals,
				 struct contour *groundwires);

/* nmmtl_shape.c */
void nmmtl_shape_c_edge(double point,
			double *shape,
			CELEMENTS_P cel,
			float nu0,
			float nu1);

void nmmtl_shape(double point,
		 double *shape);

/* nmmtl_sort_gnd_die_list.cxx */
void nmmtl_sort_gnd_die_list(GND_DIE_LIST_P lower_gdl_head,
			     SORTED_GND_DIE_LIST_P *lower_sorted_gdl,
			     GND_DIE_LIST_P upper_gdl_head,
			     SORTED_GND_DIE_LIST_P *upper_sorted_gdl,
			     double left, double right);

/* nmmtl_unload.cxx */
void nmmtl_unload(float *potential_vector,
		  int conductor_number,
		  CONDUCTOR_DATA_P conductor_data);

/* helper function used inside gi_test_real_or_sweep.cxx */
/* Probably used elsewhere */
int is_real(char *string);

#endif



