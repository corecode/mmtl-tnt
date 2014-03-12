/***************************************************************************\
 *                                                                           *
 *   ROUTINE NAME        NMMTL_XTK_CALCULATE				     *
 *                                                                           *
 *   ABSTRACT                                                                *
 *       This routine calculates forward and backward crosstalk given the    *
 *	electrostatic induction, inductance and propagation velocity (from   *
 *	nmmtl_qsp_calculate) and the coupling length and signal risetime.    *
 *	Up to two file pointers (or NULL) can be passed indicating where     *
 *	to write the crosstalk results.					     * 
 *									     *
 *	Note: electrostatice_induction, inductance, propagation_velocity,    *
 *	      forward_xtk and backward_xtk must be allocated using dim2	     *
 *	      from general lib.						     *
 *                                                                           *
 *   CALLING FORMAT                                                          *
 *       status = nmmtl_xtk_calculate(number_conductors, signals,	     *
 *		    electrostatic_induction, inductance, coupling,	     *
 *		    risetime, propagation_velocity, forward_xtk,	     *
 *		    backward_xtk, output_file1, output_file2);		     *
 *                                                                           *
 *   RETURN VALUE                                                            *
 *	SUCCESS or FAIL							     *
 *                                                                           *
 *   INPUT PARAMETERS                                                        *
 *	int number_conductors,		    number of conductors	     *
 *	struct contour *signals,	    signal line info (names)	     *
 *	float **electrostatic_induction,    [B] matrix			     *
 *	float **inductance,		    [L] matrix			     *
 *	float coupling,			    coupling length		     *
 *	float risetime,			    signal risetime		     *
 *	float *propagation_velocity,	    propagation velocity	     *
 *	FILE *output_file1,		    output file ptr #1		     *
 *	FILE *output_file2);		    output file ptr #2		     *
 *                                                                           *
 *   OUTPUT PARAMETERS                                                       *
 *	float **forward_xtk,		    forward crosstalk		     *
 *	float **backward_xtk,		    backward crosstalk		     *
 *                                                                           *
 *   USER FUNCTIONS & SYSTEM SERVICES CALLED                                 *
 *	dim2								     *
 *	free2								     *
 *									     *
 *   AUTHOR          Jeff Prentice                                           *
 *                                                                           *
 *   CREATION DATE       16-MAR-1992					     *
 *                                                                           *
 *        Copyright (C) 1992 by Mayo Foundation.  All rights reserved.       *
 *                                                                           *
 \***************************************************************************/

#include "nmmtl.h"

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
				    FILE *output_file2)
{
  float cap_coef;	/* Mutual Capacitance Coupling Coeficent (Kc) */
  float ind_coef;	/* Mutual Inductive Coupling Coeficient (Kl)  */
  float back_xtk_factor;  /* intermediate value for computing backward xtk */
  int i,j;
  
  /************************************************************************
   *									    *
   * Calculate the forward and backward crosstalk between each unique pair *
   * of lines (i.e. line i and line j where i!=j; the pair ij is the same  *
   * as ji).								    *
   *									    *
   ************************************************************************/
  for (i = 0; i < number_conductors; i++)
  {
    for (j = i + 1; j < number_conductors; j++)
    {
      /************************************************************
       *							    *
       * Calculate the Mutual Capacitance Coupling Coeficient (Kc) *
       * and the Mutual Inductive Coupling Coeficient (Kl) between *
       * the current lines.					    *
       *							    *
       ************************************************************/
      cap_coef = - (electrostatic_induction[j][i] / 
		    sqrt(electrostatic_induction[i][i] *
			 electrostatic_induction[j][j]));
      
      ind_coef = inductance[j][i] / 
	sqrt(inductance[i][i]*inductance[j][j]);
      
      /************************************************************
       *							    *
       * Calculate the Near-end Noise (Backward Crosstalk) on line *
       * i from line j.  The voltage amplitude on the active line  *
       * is assumed to be 1.	The calculation differs depending   *
       * upon signal risetime, coupling length and propogation	    *
       * velocity.						    *
       *							    *
       ************************************************************/
      back_xtk_factor = 2.0 * coupling / propagation_velocity[i];
      if (risetime < back_xtk_factor)	/* long line */
      {
	backward_xtk[i][j] = 0.25 * (cap_coef + ind_coef);
      }
      else				/* short line */
      {
	backward_xtk[i][j] = 0.25 * (cap_coef + ind_coef) *
	  back_xtk_factor / risetime;
      }
      
      /************************************************************
       *							    *
       * Calculate the Far-end Noise (Forward Crosstalk) on line i *
       * from line j.						    *
       *							    *
       ************************************************************/
      forward_xtk[i][j] = 0.5 * coupling *
	pow((double) (electrostatic_induction[i][i]
	     *electrostatic_induction[j][j]
	     *inductance[i][i]
	     *inductance[j][j]), (double) 0.25) *
	       (cap_coef - ind_coef) / risetime;
    }
  }
  
  /********************************************************************
   *									*
   * Output crosstalk to 1st output file (if ptr not NULL).		*
   *									*
   ********************************************************************/
  if (output_file1 != NULL)
    nmmtl_output_crosstalk(output_file1, forward_xtk, backward_xtk, 
			   signals);
  
  /********************************************************************
   *									*
   * Output crosstalk to 2nd output file (if ptr not NULL).		*
   *									*
   ********************************************************************/
  if (output_file2 != NULL)
    nmmtl_output_crosstalk(output_file2, forward_xtk, backward_xtk, 
			   signals);
  
  return(SUCCESS);
}


