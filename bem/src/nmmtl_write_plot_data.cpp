
/*
  
  FACILITY:  NMMTL
  
  MODULE DESCRIPTION:
  
  contains code to write data for field plots in a postprocessor
  
  AUTHOR(S):
  
  Andrew J Staniszewski
  
  CREATION DATE:  Tuesday Aug 1 1995
  
  COPYRIGHT:   Copyright (C) 1995 by Mayo Foundation. All rights reserved.
  
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
  
  FUNCTION NAME:   nmmtl_write_plot_data
  
  
  FUNCTIONAL DESCRIPTION:

  writes out plot data for the field line post processor

  
  */

void nmmtl_write_plot_data(
			   CONTOURS_P signal,
			   int conductor_counter,
			   DELEMENTS_P die_elements,
			   CONDUCTOR_DATA_P conductor_data,
			   float *sigma_vector,
			   FILE *outputFile
			 )
{
  int cond_num;
  CELEMENTS_P cel;
  DELEMENTS_P die;
  int i;
  
  fprintf(outputFile,"Start Solution Output:\n");
  fprintf(outputFile,"Active Line: %s\n",signal->name);
  fprintf(outputFile,"\n");
  for(cond_num = 0;cond_num <= conductor_counter; cond_num++)
    {
      cel=conductor_data[cond_num].elements;
      while(cel != NULL)
	{
	  fprintf(outputFile,"Element Type: Conductor\n");
	  
	  fprintf(outputFile,"X Points:");
	  for (i = 0; i < INTERP_PTS; i++)
	    fprintf(outputFile," %e",cel->xpts[i]);
	  fprintf(outputFile,"\n");
	  
	  fprintf(outputFile,"Y Points:");
	  for (i = 0; i < INTERP_PTS; i++)
	    fprintf(outputFile," %e",cel->ypts[i]);
	  fprintf(outputFile,"\n");
	  
	  if (cel->edge[0])
	    {
	      fprintf(outputFile,"Edge: 0 %e\n",cel->edge[0]->nu);
	    }
	  if (cel->edge[1])
	    {
	      fprintf(outputFile,"Edge: 1 %e\n",cel->edge[1]->nu);
	    }
	  fprintf(outputFile,"Charge Values:");
	  for (i = 0; i < INTERP_PTS; i++)
	    fprintf(outputFile," %e",sigma_vector[cel->node[i]]);
	  fprintf(outputFile,"\n");
	  fprintf(outputFile,"\n");
	  
	  cel = cel->next;
	} /* while looping on elements of particular conductor */
    } /* for all conductors */

  die = die_elements;
  while (die != NULL)
    {
      fprintf(outputFile,"Element Type: Dielectric\n");
      fprintf(outputFile,"X Points:");
      for (i = 0; i < INTERP_PTS; i++)
	fprintf(outputFile," %e",die->xpts[i]);
      fprintf(outputFile,"\n");

      fprintf(outputFile,"Y Points:");
      for (i = 0; i < INTERP_PTS; i++)
	fprintf(outputFile," %e",die->ypts[i]);
      fprintf(outputFile,"\n");

      fprintf(outputFile,"Charge Values:");
      for (i = 0; i < INTERP_PTS; i++)
	fprintf(outputFile," %e",sigma_vector[die->node[i]]);
      fprintf(outputFile,"\n");

      fprintf(outputFile,"\n");

      die = die->next;
    } 
  fprintf(outputFile,"End Solution Output:\n");
}





