
/*
  
  FACILITY:  NMMTL
  
  MODULE DESCRIPTION:
  
  Contains the function nmmtl_dump
  
  AUTHOR(S):
  
  Kevin J. Buchs
  
  CREATION DATE:  Mon Jun  1 15:21:29 1992
  
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
  
  FUNCTION NAME:  nmmtl_dump
  
  
  FUNCTIONAL DESCRIPTION:
  
  Just put out a dump file of conductor and dielectric elements.
  
  FORMAL PARAMETERS:
  
  FILE *dump_file                  - where to write dumpy things to.
  int cntr_seg,                  - cseg parameter
  int pln_seg,                   - dseg parameter
  struct contour *signals          - signals data structure
  int conductor_counter,           - how many conductors (gnd not included)
  CONDUCTOR_DATA_P conductor_data, - array of data on conductors
  DELEMENTS_P die_elements         - list of dielectric elements        
  unsigned int node_point_counter           - highest node number
  unsigned int highest_conductor_node       - highest node for a conductor
  
  RETURN VALUE:
  
  None
  
  CALLING SEQUENCE:
  
  nmmtl_dump(dump_file,conductor_counter,cd,*die_elements);
  
  */

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
		unsigned int highest_conductor_node)
{
  CELEMENTS_P ce;
  int cntr,i;
  
  fprintf(dump_file,"%d %d %g %g\n",cntr_seg,pln_seg,
	  coupling,risetime);
  
  for(;signals != NULL; signals = signals->next)
  {
    fprintf(dump_file,"=%s\n",signals->name);
  }
  fputs("\n",dump_file);
  
  fprintf(dump_file,"%d %d %d\n",node_point_counter,highest_conductor_node,
	  conductor_counter);
  for(cntr=0;cntr <= conductor_counter;cntr++)
  {
    fprintf(dump_file,"%d %d \n",
	    conductor_data[cntr].node_start,
	    conductor_data[cntr].node_end);
    for(ce = conductor_data[cntr].elements;ce != NULL;ce = ce->next)
    {
      fprintf(dump_file,"%g ",ce->epsilon);
      if(ce->edge[0]) fputs("1 ",dump_file);
      else fputs("0 ",dump_file);
      if(ce->edge[1]) fputs("1 ",dump_file);
      else fputs("0 ",dump_file);
      if(ce->edge[0]) fprintf(dump_file,"%g %g ",ce->edge[0]->nu,
			      ce->edge[0]->free_space_nu);
      if(ce->edge[1]) fprintf(dump_file,"%g %g ",ce->edge[1]->nu,
			      ce->edge[1]->free_space_nu);
      fputs("\n",dump_file);
      for(i = 0; i < 3; i++)
      {
	fprintf(dump_file,"%d %-23.21g %-23.21g\n",ce->node[i],ce->xpts[i],ce->ypts[i]);
      }
    }
    fputs(".\n",dump_file);
  }
  
  for(; die_elements != NULL; die_elements = die_elements->next)
  {
    fprintf(dump_file,"%g %g %g %g\n",
	    die_elements->epsilonplus,die_elements->epsilonminus,
	    die_elements->normalx,die_elements->normaly);
    for(i = 0; i < 3; i++)
    {
      fprintf(dump_file,"%d %-23.21g %-23.21g\n",die_elements->node[i],
	      die_elements->xpts[i],die_elements->ypts[i]);
    }
  }
  
  /* signify end of dielectric elements with a zero epsilon value */
  
  fputs(".\n",dump_file);
  
}

