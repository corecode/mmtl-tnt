/***************************************************************************\
 * 									    *
 *   ROUTINE NAME	NMMTL_OUTPUT_MATRICES				    *
 * 									    *
 *   ABSTRACT								    *
 *	Write Electrostatic Induction and Inductance matrices to output.    *
 * 									    *
 *   CALLING FORMAT							    *
 *	nmmtl_output_matrices(output_fp, electrostatic_induction,	    *
 *	    inductance, signals);					    *
 * 									    *
 *   RETURN VALUE							    *
 * 									    *
 *   INPUT PARAMETERS							    *
 *	FILE *output_fp;		output file ptr			    *
 *	float **electrostatic_induction;[B] matrix			    *
 *	float **inductance;		[L] matrix			    *
 *	struct contour *signals;	signal line info (names)	    *
 * 									    *
 *   OUTPUT PARAMETERS							    *
 * 									    *
 *   USER FUNCTIONS & SYSTEM SERVICES CALLED				    *
 * 									    *
 *   AUTHOR          Jeff Prentice 					    *
 * 									    *
 *   CREATION DATE	17-MAR-1992					    *
 * 									    *
 *        Copyright (C) 1992 by Mayo Foundation.  All rights reserved.	    *
 * 									    *
 \***************************************************************************/

#include "nmmtl.h"

void nmmtl_output_matrices(FILE *output_fp, float **electrostatic_induction,
			   float **inductance, struct contour *signals)
{
  struct contour *sig_line1, *sig_line2;    /* signal ptrs */
  int i,j;				    /* array indices */
  
  /********************************************************************
   *                                                                   *
   * Output Electrostatic Induction matrix [B].			*
   *                                                                   *
   ********************************************************************/
  fprintf(output_fp, "\nMutual and Self Electrostatic Induction:\n");
  fprintf(output_fp, "B(Active Signal , Passive Signal) Farads/Meter\n");
  for (sig_line1 = signals, i = 0;
       sig_line1 != NULL;
       sig_line1 = sig_line1->next, i++)
  {
    for (sig_line2 = signals, j = 0;
	 sig_line2 != NULL;
	 sig_line2 = sig_line2->next, j++)
    {
      fprintf(output_fp, "B( ::%s , ::%s )= %15.7e\n",
	      sig_line1->name, sig_line2->name, 
	      electrostatic_induction[i][j]);
    }
  }
  
  /********************************************************************
   *                                                                   *
   * Output Induction matrix [L].					*
   *                                                                   *
   ********************************************************************/
  fprintf(output_fp, "\nMutual and Self Inductance:\n");
  fprintf(output_fp, "L(Active Signal , Passive Signal) Henrys/Meter\n");
  for (sig_line1 = signals, i = 0;
       sig_line1 != NULL;
       sig_line1 = sig_line1->next, i++)
  {
    for (sig_line2 = signals, j = 0;
	 sig_line2 != NULL;
	 sig_line2 = sig_line2->next, j++)
    {
      fprintf(output_fp, "L( ::%s , ::%s )= %15.7e\n",
	      sig_line1->name, sig_line2->name, inductance[i][j]);
    }
  }
}




