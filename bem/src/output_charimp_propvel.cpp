/***************************************************************************\
 * 									    *
 *   ROUTINE NAME	NMMTL_OUTPUT_CHARIMP_PROPVEL			    *
 * 									    *
 *   ABSTRACT								    *
 *	Writes Characteristic Impedance, Propagation Velocity and	    *
 *	Equivalent Dielectric Constant to output file.			    *
 * 									    *
 *   CALLING FORMAT							    *
 *	nmmtl_output_charimp_propvel(output_fp, characteristic_impedance,   *
 *	    propagation_velocity, equivalent_dielectric,even_odd,signals);  *
 * 									    *
 *   RETURN VALUE							    *
 * 									    *
 *   INPUT PARAMETERS							    *
 *	FILE *output_fp;		output file ptr			    *
 *	float *characeristic_impedance;	characteristic impedance	    *
 *	float *propagation_velocity;	propagation velocity		    *
 *	float *equivalent_dielectric;   equivalent dielectric constant	    *
 *      float even_odd[4];              even/odd values for 2 conductors    *
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

void nmmtl_output_charimp_propvel(FILE *output_fp, /* output file ptr */
				  /* data to be output */
				  float *characteristic_impedance,
				  float *propagation_velocity,
				  float *equivalent_dielectric,
				  float even_odd[4],
				  /* signal line info */
				  struct contour *signals)
{

  struct contour *sig_line1;	/* signal ptr */
  int i;			/* array indices */

  /********************************************************************
   *                                                                   *
   * Output Characteristic Impedance.					*
   *                                                                   *
   ********************************************************************/
  fprintf(output_fp, "\nCharacteristic Impedance (Ohms):\n");
  for (sig_line1 = signals, i = 0;
       sig_line1 != NULL;
       sig_line1 = sig_line1->next, i++)
  {
    fprintf(output_fp, "For Signal Line ::%s= %g\n", sig_line1->name,
	    characteristic_impedance[i]);
  }

  /* odd/even char. imped. if 2 conductors */
  if(i == 2 && even_odd != 0 && even_odd[0] != -1.0 && even_odd[1] != -1.0)
  {
    fprintf(output_fp, "\nCharacteristic Impedance Odd/Even (Ohms):\n");
    fprintf(output_fp, "  odd= %g\n even= %g\n",even_odd[0],even_odd[1]);
  }

  /********************************************************************
   *                                                                  *
   * Output Equivalent Dielectric Constant.			      *
   *                                                                  *
   ********************************************************************/
  fprintf(output_fp, "\nEffective Dielectric Constant:\n");
  for (sig_line1 = signals, i = 0;
       sig_line1 != NULL;
       sig_line1 = sig_line1->next, i++)
  {
    fprintf(output_fp, "For Signal Line ::%s= %g\n", sig_line1->name,
	    equivalent_dielectric[i]);
  }

  /********************************************************************
   *                                                                   *
   * Output propagation velocity.					*
   *                                                                   *
   ********************************************************************/
  fprintf(output_fp, "\nPropagation Velocity (meters/second):\n");
  for (sig_line1 = signals, i = 0;
       sig_line1 != NULL;
       sig_line1 = sig_line1->next, i++)
  {
    fprintf(output_fp, "For Signal Line ::%s= %15.7e\n", sig_line1->name,
	    propagation_velocity[i]);
  }

  /* odd/even prop. vel. if 2 conductors */
  if(i == 2 && even_odd != 0 && even_odd[2] != -1.0 && even_odd[3] != -1.0)
  {
    fprintf(output_fp, "\nPropagation Velocity Odd/Even (meters/second):\n");
    fprintf(output_fp, "  odd= %g\n even= %g\n",even_odd[2],even_odd[3]);
  }

  /********************************************************************
   *                                                                   *
   * Output propagation delay (just inverse of velocity above          *
   *                                                                   *
   ********************************************************************/
  fprintf(output_fp, "\nPropagation Delay (seconds/meter):\n");
  for (sig_line1 = signals, i = 0;
       sig_line1 != NULL;
       sig_line1 = sig_line1->next, i++)
  {
    fprintf(output_fp, "For Signal Line ::%s= %15.7e\n", sig_line1->name,
	    1.0/propagation_velocity[i]);
  }

  /* odd/even prop. delay if 2 conductors */
  if(i == 2 && even_odd != 0 && even_odd[2] != -1.0 && even_odd[3] != -1.0)
  {
    fprintf(output_fp, "\nPropagation Delay Odd/Even (seconds/meter):\n");
    fprintf(output_fp, "  odd= %g\n",1.0/even_odd[2]);
    fprintf(output_fp, " even= %g\n",1.0/even_odd[3]);
  }

}

