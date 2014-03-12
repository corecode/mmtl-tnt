/***************************************************************************\
 * 									    *
 *   ROUTINE NAME	NMMTL_OUTPUT_CROSSTALK				    *
 * 									    *
 *   ABSTRACT								    *
 *	Write forward and backward crosstalk to specified file.		    *
 * 									    *
 *   CALLING FORMAT							    *
 *	nmmtl_output_crosstalk(output_fp, forward_xtk,			    *
 *	    backward_xtk, signals);					    *
 * 									    *
 *   RETURN VALUE							    *
 * 									    *
 *   INPUT PARAMETERS							    *
 *	FILE *output_fp;		output file ptr			    *
 *	float **forward_xtk;						    *
 *	float **backward_xtk;						    *
 *	struct contour *signals;	signal line info (names)	    *
 * 									    *
 *   OUTPUT PARAMETERS							    *
 * 									    *
 *   USER FUNCTIONS & SYSTEM SERVICES CALLED				    *
 * 									    *
 *   AUTHOR          Jeff Prentice 					    *
 * 									    *
 *   CREATION DATE	04-MAR-1992					    *
 * 									    *
 *        Copyright (C) 1991 by Mayo Foundation.  All rights reserved.	    *
 * 									    *
 \***************************************************************************/

#include "nmmtl.h"

void nmmtl_output_crosstalk(FILE *output_fp, float **forward_xtk,
			    float **backward_xtk, struct contour *signals)
{
  
  struct contour *sig_line1, *sig_line2;    /* signal ptrs */
  int i,j;				    /* array indices */
  
  /********************************************************************
   *                                                                   *
   * Output forward crosstalk.						*
   *                                                                   *
   ********************************************************************/
  fprintf(output_fp, "\nFar-End (Forward) Cross Talk:\n");
  fprintf(output_fp, "FXT(Active Signal, Passive Signal)\n");
  for (sig_line1 = signals, i = 0;
       sig_line1 != NULL;
       sig_line1 = sig_line1->next, i++)
  {
    for (sig_line2 = signals, j = 0;
	 sig_line2 != NULL;
	 sig_line2 = sig_line2->next, j++)
    {
      if (j > i)
      {
	if(forward_xtk[i][j] != 0.0)
	  fprintf(output_fp, "FXT( ::%s , ::%s )= %11.5e = %11.5f dB\n",
		  sig_line1->name, sig_line2->name, forward_xtk[i][j],
		  (20.0 * log10(fabs(forward_xtk[i][j]))));
	else
	  fprintf(output_fp, "FXT( ::%s , ::%s )= %11.5e = infinite dB\n",
		  sig_line1->name, sig_line2->name, forward_xtk[i][j]);
      }
    }
  }
  
  /********************************************************************
   *                                                                   *
   * Output backward crosstalk.					*
   *                                                                   *
   ********************************************************************/
  fprintf(output_fp, "\nNear-End (Backward) Cross Talk:\n");
  fprintf(output_fp, "BXT(Active Signal, Passive Signal)\n");
  for (sig_line1 = signals, i = 0;
       sig_line1 != NULL;
       sig_line1 = sig_line1->next, i++)
  {
    for (sig_line2 = signals, j = 0;
	 sig_line2 != NULL;
	 sig_line2 = sig_line2->next, j++)
    {
      if (j > i)
      {
	if(backward_xtk[i][j] != 0.0)
	  fprintf(output_fp, "BXT( ::%s , ::%s )= %11.5e = %11.5f dB\n",
		  sig_line1->name, sig_line2->name, backward_xtk[i][j],
		  (20.0 * log10(fabs(backward_xtk[i][j]))));
	else
	  fprintf(output_fp, "BXT( ::%s , ::%s )= %11.5e = infinite dB\n",
		  sig_line1->name, sig_line2->name, backward_xtk[i][j]);
      }
    }
  }
  fprintf(output_fp, 
	  "\nNOTE: Cross talk results assume there are no reflections.\n");
}
