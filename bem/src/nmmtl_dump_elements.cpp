
/*
  
  FACILITY:  NMMTL
  
  MODULE DESCRIPTION:
  
  Contains the function nmmtl_dump_elements()
  
  AUTHOR(S):
  
  Kevin J. Buchs
  
  CREATION DATE:  Thu Mar 19 11:38:10 1992
  
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
  
  FUNCTION NAME:  nmmtl_dump_elements
  
  
  FUNCTIONAL DESCRIPTION:
  
  Just put out a dump file of conductor and dielectric elements.
  
  FORMAL PARAMETERS:
  
  int conductor_counter,           - how many conductors (gnd not included)
  CONDUCTOR_DATA_P conductor_data, - array of data on conductors
  DELEMENTS_P die_elements         - list of dielectric elements        
  
  RETURN VALUE:
  
  None
  
  CALLING SEQUENCE:
  
  nmmtl_dump_elements(conductor_counter,cd,*die_elements);
  
  */

void nmmtl_dump_elements(int conductor_counter,
			 CONDUCTOR_DATA_P conductor_data,
			 DELEMENTS_P die_elements)
{
  CELEMENTS_P ce;
  int cntr,i;
  
  fputs("\n\n---------------------------------------------\n\n",dump_file);
  
  fputs("\nDump of Elements\n\n  Conductor Elements\n\n",dump_file);
  for(cntr=0;cntr <= conductor_counter;cntr++)
  {
    fprintf(dump_file,"    Conductor %d, node_start %d, node_end %d\n\n",
	    cntr,conductor_data[cntr].node_start,
	    conductor_data[cntr].node_end);
    fputs("       Elements\n\n",dump_file);
    for(ce = conductor_data[cntr].elements;ce != NULL;ce = ce->next)
    {
      for(i = 0; i < 3; i++)
      {
	fprintf(dump_file,"        node %d, x=%f, y=%f, node=%d\n",
		i,ce->xpts[i],ce->ypts[i],ce->node[i]);
      }
      
      fprintf(dump_file,"        epsilon=%f",ce->epsilon);
      if(ce->edge[0])
      {
	fprintf(dump_file," edge[0]: nu=%f, free_space_nu=%f,",
		ce->edge[0]->nu,ce->edge[0]->free_space_nu);
      }
      if(ce->edge[1])
      {
	fprintf(dump_file," edge[1]: nu=%f, free_space_nu=%f,",
		ce->edge[1]->nu,ce->edge[1]->free_space_nu);
      }
      fputs("\n",dump_file);
    }
    fputs("\n",dump_file);
  }
  
  fputs("\n\n  Dielectric Elements\n\n",dump_file);
  for(; die_elements != NULL; die_elements = die_elements->next)
  {
    for(i = 0; i < 3; i++)
    {
      fprintf(dump_file,"        node %d, x=%f, y=%f, node=%d\n",
	      i,die_elements->xpts[i],die_elements->ypts[i],
	      die_elements->node[i]);
    }
    fprintf(dump_file,"        epsilon(+)=%f, epsilon(-)=%f\n",
	    die_elements->epsilonplus,die_elements->epsilonminus);
    fprintf(dump_file,"        normalx=%f, normaly=%f\n\n",
	    die_elements->normalx,die_elements->normaly);
  }    
  fputs("\n\nEnd of Element Dump\n",dump_file);
  fputs("------------------------------------------\n\n",dump_file);
}

