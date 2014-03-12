
/*
  
  FACILITY:  nmmtl
  
  MODULE DESCRIPTION:
  
  contains the function nmmtl_qsp_calculate
  
  AUTHOR(S):
  
  Kevin J. Buchs
  
  CREATION DATE:  Mon Mar  2 15:13:26 1992
  
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
extern FILE *plotFile;

double NON_LINEARITY_FACTOR;

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
  
  FUNCTION NAME:  nmmtl_qsp_calculate
  
  
  FUNCTIONAL DESCRIPTION:
  
  Calculates the quasi-static parameters for a cross section of a
  multiconductor multilayer transmission line system.  Implements the
  so called, "Boundary Element Method".  By providing various cross sectional
  data, the function returns the capacitance and inductance matricies and
  the propagation velocity and charactistic impedance arrays.
  
  FORMAL PARAMETERS:
  
  1) electrostatic_induction, inductance, propagation_velocity, and
  characteristic_impedance are outputs and must be allocated before
  calling nmmtl_qsp_calculate using the dim2 (general lib) routine.
  They are of order n x n for first two and order n for other two,
  where n is the number of conductors.
  
  2) if either output_file is NULL, then no output is written to that
  pointer.  Thus the caller may request one or two copies of the output.
  It is up to the calling function to open the file and close the file.
  
  inputs:
  
  struct dielectric *dielectrics,
  struct contour  *signals,
  struct contour  *groundwires,
  int gnd_planes,
  float half_minimum_dimension,
  int cntrs_seg,
  int pln_seg,
  
  outputs:
  
  float **electrostatic_induction,
  float **inductance,
  float *characteristic_impedance,
  float *propagation_velocity,
  float *equivalent_dielectric,
  FILE *output_file1, *output_file2
  
  RETURN VALUE:
  
  returns a status of FAIL or SUCCESS, or other failure status.
  
  CALLING SEQUENCE:
  
  status = nmmtl_qsp_calculate(...);
  
  */

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
				    FILE *output_file2)
{
  
  /* local variables */
  
  int status;
  DIELECTRIC_SEGMENTS_P dielectric_segments = NULL;
  LINE_SEGMENTS_P conductor_ls = NULL;
  CIRCLE_SEGMENTS_P conductor_cs = NULL;
  double top_of_bottom_plane;
  double bottom_of_top_plane;
  double left_of_gnd_planes;
  double right_of_gnd_planes;
  int conductor_counter = 0;       /* start at one, zero is ground */
  COND_PROJ_LIST_P cond_projections = NULL;
  CONDUCTOR_DATA_P conductor_data;
  DELEMENTS_P die_elements = NULL;
  SORTED_GND_DIE_LIST_P lower_sorted_gdl;
  SORTED_GND_DIE_LIST_P upper_sorted_gdl;
  unsigned int node_point_counter = 0, highest_conductor_node = 0;
  char *filename;
  FILE *dump_file = NULL;
  FILE *retrieval_file = NULL;
  EXTENT_DATA extent_data;
  
  /********
  filename = get_logical_name(ER_LOGICAL);
  if(*filename != '\0')
  {
    retrieval_file = fopen(filename,"r");
    if(retrieval_file == NULL)
    {
      printf ("Could not find file %s\n", filename);
    }
    else
    {
      char string[256];
      strcpy(string,"retrieving elements from: ");
      strcat(string,filename);
      printf ("%s\n", string);
    }
  }
  else  // if not retrieving, maybe we're dumping 
  {
    filename = get_logical_name(ED_LOGICAL);
    if(*filename != '\0')
    {
      dump_file = fopen(filename,"w");
      if(dump_file == NULL)
      {
	printf ("Could not find file %s\n", filename);
      }
      else
      {
	char string[256];
	strcpy(string,"dumping elements to: ");
	strcat(string,filename);
	printf ("%s\n", string);
      }
    }
  }
  *****/

  /* don't need to go through the steps of making elements if we are
     reading them from a file */
  
  if(retrieval_file)
  {
    if(nmmtl_retrieve(retrieval_file,&cntr_seg,&pln_seg,
		      &coupling,&risetime,
		      (CONTOURS_P *)NULL,(int *)NULL, 
		      &conductor_counter,&conductor_data,
		      &die_elements,&node_point_counter,
		      &highest_conductor_node) != SUCCESS) return(FAIL);
    
  }
  else
  {
    /* - - - - - - - -  Find the Ground Planes  - - - - - - - - - */
    
    status = nmmtl_find_ground_planes(dielectrics,
				      &top_of_bottom_plane,
				      &bottom_of_top_plane,
				      &left_of_gnd_planes,
				      &right_of_gnd_planes);
    
    if(status != SUCCESS) return(status);
    
    /* - - - - - - - - -  set up some extent data  - - - - - - - - - - - - - */

    /* got this from find_ground_planes */
    extent_data.left_cs_extent = left_of_gnd_planes;
    extent_data.right_cs_extent = right_of_gnd_planes;

    /* ---------------- write out extent data to the plot file ------------- */
    if (plotFile != NULL)
      {
	fprintf(plotFile,"Upper Extent: %e\n",bottom_of_top_plane);
	fprintf(plotFile,"Lower Extent: %e\n",top_of_bottom_plane);
	fprintf(plotFile,"Right Extent: %e\n",right_of_gnd_planes);
	fprintf(plotFile,"Left Extent: %e\n",left_of_gnd_planes);
	fprintf(plotFile,"\n\n");
      }

    /* setup other defaults for conductor region */
    /* setup initial values to be impossible and then they will get adjusted according to a
       maximum type calculation.  We do this to assure that the first value checked will
       satisfy the greater/less than check with the initial value. */

    extent_data.left_cond_extent = right_of_gnd_planes;
    extent_data.right_cond_extent = left_of_gnd_planes;
    extent_data.min_cond_height = bottom_of_top_plane - top_of_bottom_plane;
    extent_data.expand_left = FALSE;
    extent_data.expand_right = FALSE;


        
    /* - - - - - - - -  massage the dielectric segments  - - - - - - - - - - */
    
    status = nmmtl_combine_die(dielectrics,pln_seg,gnd_planes,
			       top_of_bottom_plane,
			       bottom_of_top_plane,left_of_gnd_planes,
			       right_of_gnd_planes,&dielectric_segments,
			       &lower_sorted_gdl,&upper_sorted_gdl);
    
    if(status != SUCCESS) return(status);


    /* - - - - - - - -  process the conductors into elements - - - - - - - - */
    
    if(gnd_planes == 1)
    {
      status = nmmtl_evaluate_conductors(dielectrics,
					 bottom_of_top_plane,
					 cntr_seg,
					 &extent_data,
					 &conductor_counter,
					 &signals,&groundwires,
					 &conductor_ls,&conductor_cs,
					 &dielectric_segments);
    }
    else
    {
      status = nmmtl_evaluate_conductors(dielectrics,
					 0.0,
					 cntr_seg,
					 &extent_data,
					 &conductor_counter,
					 &signals,&groundwires,
					 &conductor_ls,&conductor_cs,
					 &dielectric_segments);
    }
    
    
    if(status != SUCCESS) return(status);

    /* Determine if dielectrc boundary expansion is to be performed.
       This is sort of a macroscopic operation, so it is performed here.
       At this point, it only influences the generation of dielectric elements,
       and the following operations could be buried in the function that does that
       element generation, but it is not.  It is more macroscopic.  It is here.

       Determine if the cross section is qualified for expansion:

       Under normal conditions:
         First, the width of the conductor region must be less than 0.25
       times the height of the lowest conductor above the ground plane. 
       The second condition is that the width of the cross section as
       drawn is less that 2.5 times the height of the lowest conductor
       above the ground plane.

       Under user over-ride:
         We do not check the height of the lowest conductor above the 
       ground plane, any value will do.  The only condition is that 
       the width of the cross section as drawn is less that 2.5 times
       the height of the lowest conductor above the ground plane.
       This user over-ride is determined by the setting of an
       environment variable.  It is checked inside the following compound
       statement.
       */

    if( (extent_data.right_cs_extent - extent_data.left_cs_extent)
        < (2.5 * extent_data.min_cond_height) )
    {
      /* this is a candidate for expansion, setup data for such */
      double conductor_center =
            (extent_data.left_cond_extent + extent_data.right_cond_extent) / 2;
      double half_width = 1.5 * extent_data.min_cond_height;
      char *variable;
      int override = 0;

      /* sample the evironment variable that allows the user to determine:
	 
	    non-linearity expansion factor (by a non-zero value), 
	    override of conductor height checking (by a negative value,
                   absolute value used for non-linearity)
	    disable the expansion feature (by a zero value)
	    */
      variable = getenv(EXPAND_VARIABLE);
      if(variable != NULL) NON_LINEARITY_FACTOR = atof(variable);
      else NON_LINEARITY_FACTOR = DEFAULT_NON_LINEARITY;
      
      /* final checks to see if we will do expansion - done if
          1. the user has overridden the conductor height checking OR
	  2. the conductors are high enough
         at the same time we check for user disable
	 */
      if(( NON_LINEARITY_FACTOR < 0.0 ) ||
	 ( NON_LINEARITY_FACTOR != 0.0 &&
	  (extent_data.right_cond_extent - extent_data.left_cond_extent)
	  < (0.25 * extent_data.min_cond_height)
	 )
	)
      {
	
	extent_data.desired_left = conductor_center - half_width;
	extent_data.desired_right = conductor_center + half_width;

	/* in case the user used a negative value to override */
	NON_LINEARITY_FACTOR = fabs(NON_LINEARITY_FACTOR);

	/* check for the case where the cross section already goes far enough
	   to one side but not the other.  If it does, then leave the expansion
	   flag set to the initialized value of false.
	   */
	if(extent_data.desired_left < extent_data.left_cs_extent)
	  extent_data.expand_left = TRUE;
	if(extent_data.desired_right > extent_data.right_cs_extent)
	  extent_data.expand_right = TRUE;

	printf ("Expanding narrow cross section\n");
      }
      
    }

#ifdef EXTEND_DIAG
    printf("\nextent_data.left_cs_extent    = %f\n",
	   extent_data.left_cs_extent);
    printf("            .right_cs_extent   = %f\n",
	   extent_data.right_cs_extent);
    printf("            .left_cond_extent  = %f\n",
	   extent_data.left_cond_extent);
    printf("            .right_cond_extent = %f\n",
	   extent_data.right_cond_extent);
    printf("            .min_cond_height   = %f\n",
	   extent_data.min_cond_height);
    printf("            .desired_left      = %f\n",
	   extent_data.desired_left);
    printf("            .desired_right     = %f\n",
	   extent_data.desired_right);
    printf("            .expand_left       = %d\n",
	   (int)extent_data.expand_left);
    printf("            .expand_right      = %d\n\n",
	   (int)extent_data.expand_right);
#endif

    /* - - - - - - - -  Generate the Elements  - - - - - - - - - */
    
    status = nmmtl_generate_elements(conductor_counter,&conductor_data,
				     &die_elements,
				     &node_point_counter,
				     &highest_conductor_node,
				     conductor_ls,conductor_cs,
				     dielectric_segments,
				     gnd_planes,
				     upper_sorted_gdl,pln_seg,
				     bottom_of_top_plane,
				     left_of_gnd_planes,right_of_gnd_planes,
				     &extent_data);
    if(status != SUCCESS) return(status);

  }
  
  /* ---------------- write out contour data to the plot file ------------- */
  if (plotFile != NULL)
    {
      struct contour *conductor;
      struct dielectric *dieDieDie;
      conductor = signals;
      while (conductor != 0)
	{
	  fprintf(plotFile,"Contour: Signal\n");
	  if (conductor->primitive == 'A')
	    {
	      fprintf(plotFile,"Contour Type: Circle\n");
	      fprintf(plotFile,"Radius: %e\n",conductor->x1);
	      fprintf(plotFile,"Origin: %e %e\n",conductor->x0,conductor->y0);
	    }
	  if (conductor->primitive == 'R')
	    {
	      fprintf(plotFile,"Contour Type: Polygon\n");
	      fprintf(plotFile,"Number of Points: 4\n");
	      fprintf(plotFile,"Point: %e %e\n",conductor->x0,conductor->y0);
	      fprintf(plotFile,"Point: %e %e\n",conductor->x0,conductor->y1);
	      fprintf(plotFile,"Point: %e %e\n",conductor->x1,conductor->y1);
	      fprintf(plotFile,"Point: %e %e\n",conductor->x1,conductor->y0);
	    }
	  if (conductor->primitive == 'G')
	    {
	      int numberOfPoints;
	      struct polypoints *point;
	    
	      numberOfPoints = 0;
	      point = conductor->points;
	      while (point != 0)
		{
		  numberOfPoints++;
		  point = point->next;
		}

	      fprintf(plotFile,"Contour Type: Polygon\n");
	      fprintf(plotFile,"Number of Points: %d\n",numberOfPoints);

	      point = conductor->points;
	      while (point != 0)
		{
		  fprintf(plotFile,"Point: %e %e\n",point->x,point->y);
		  point = point->next;
		}
	    }
	  fprintf(plotFile,"\n");
	  conductor = conductor->next;
	}      

      conductor = groundwires;
      while (conductor != 0)
	{
	  fprintf(plotFile,"Contour: Ground\n");
	  if (conductor->primitive == 'A')
	    {
	      fprintf(plotFile,"Contour Type: Circle\n");
	      fprintf(plotFile,"Radius: %e\n",conductor->x1);
	      fprintf(plotFile,"Origin: %e %e\n",conductor->x0,conductor->y0);
	    }
	  if (conductor->primitive == 'R')
	    {
	      fprintf(plotFile,"Contour Type: Polygon\n");
	      fprintf(plotFile,"Number of Points: 4\n");
	      fprintf(plotFile,"Point: %e %e\n",conductor->x0,conductor->y0);
	      fprintf(plotFile,"Point: %e %e\n",conductor->x0,conductor->y1);
	      fprintf(plotFile,"Point: %e %e\n",conductor->x1,conductor->y1);
	      fprintf(plotFile,"Point: %e %e\n",conductor->x1,conductor->y0);
	    }
	  if (conductor->primitive == 'G')
	    {
	      int numberOfPoints;
	      struct polypoints *point;
	    
	      numberOfPoints = 0;
	      point = conductor->points;
	      while (point != 0)
		{
		  numberOfPoints++;
		  point = point->next;
		}

	      fprintf(plotFile,"Contour Type: Polygon\n");
	      fprintf(plotFile,"Number of Points: %d\n",numberOfPoints);

	      point = conductor->points;
	      while (point != 0)
		{
		  fprintf(plotFile,"Point: %e %e\n",point->x,point->y);
		  point = point->next;
		}
	    }
	  fprintf(plotFile,"\n");
	  conductor = conductor->next;
	}      

      dieDieDie = dielectrics;
      while (dieDieDie != 0)
	{
	  fprintf(plotFile,"Contour: Dielectric\n");
	  fprintf(plotFile,"Contour Type: Polygon\n");
	  fprintf(plotFile,"Number of Points: 4\n");
	  fprintf(plotFile,"Point: %e %e\n",dieDieDie->x0,dieDieDie->y0);
	  fprintf(plotFile,"Point: %e %e\n",dieDieDie->x0,dieDieDie->y1);
	  fprintf(plotFile,"Point: %e %e\n",dieDieDie->x1,dieDieDie->y1);
	  fprintf(plotFile,"Point: %e %e\n",dieDieDie->x1,dieDieDie->y0);
	  fprintf(plotFile,"\n");
	  dieDieDie = dieDieDie->next;
	}      

    }

  if(dump_file)
  {
    nmmtl_dump(dump_file,cntr_seg,pln_seg,coupling,risetime,
	       signals,conductor_counter,conductor_data,
	       die_elements,node_point_counter,
	       highest_conductor_node);
  }

  else
  {

    
    /* - - - - - - - -  Do the kernel calculations  - - - - - - - - - */
    
    status = nmmtl_qsp_kernel(conductor_counter,conductor_data,die_elements,
			      node_point_counter,highest_conductor_node,
			      half_minimum_dimension,
			      electrostatic_induction,
			      inductance,characteristic_impedance,
			      propagation_velocity,equivalent_dielectric,
			      output_file1,output_file2,
			      signals);
    
    
  }
  
  return(status);
  
}

