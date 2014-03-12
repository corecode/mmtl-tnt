/***************************************************************************\
 *                                                                           
 *   ROUTINE NAME        NMMTL_CHARIMP_PROPVEL_CALCULATE		       
 *                                                                           
 *   ABSTRACT                                                                
 *       This routine calculates the Characteristic Impedance,	    	    
 *	Propagation Velocity and Equivalent Dielectric Constant		    
 *	for each signal line.						    
 *									    
 *	Up to two file pointers (or NULL) can be passed indicating where    
 *	to write the results.						    
 *									    
 *	Note: electrostatic_induction, inductance and cap_abs_diel must be  
 *	      allocated using dim2 from general lib.			    
 *                                                                           
 *   CALLING FORMAT                                                          
 *       status = nmmtl_charimp_propvel_calculate(number_conductors, signals,
 *		    electrostatic_induction, inductance, cap_abs_diel,	    
 *		    characteristic_impedance, propagation_velocity,	    
 *		    equivalent_dielectric, output_file1, output_file2);	    
 *                                                                           
 *   RETURN VALUE                                                            
 *	SUCCESS or FAIL							    
 *                                                                           
 *   INPUT PARAMETERS                                                        
 *	int number_conductors,		    number of conductors	    
 *	struct contour *signals,	    signal line info (names)	    
 *	float **electrostatic_induction,    [B] matrix			    
 *	float **inductance,		    [L] matrix			    
 *	float **cap_abs_diel,		    capacitance in absence of diels 
 *	FILE *output_file1,		    output file ptr #1		    
 *	FILE *output_file2);		    output file ptr #2		    
 *                                                                           
 *   OUTPUT PARAMETERS                                                       
 *	float *characteristic_impedance,    characteristic impedance	    
 *	float *propagation_velocity,	    propagation velocity	    
 *	float *equivalent_dielectric	    equivalent dielectric constant  
 *                                                                           
 *   USER FUNCTIONS & SYSTEM SERVICES CALLED                                 
 *									    
 *   AUTHOR          Jeff Prentice                                           
 *                                                                           
 *   CREATION DATE       17-MAR-1992					    
 *                                                                           
 *        Copyright (C) 1992 by Mayo Foundation.  All rights reserved.       
 *                                                                           
 \***************************************************************************/

#include "nmmtl.h"

int nmmtl_charimp_propvel_calculate(int number_conductors,
						struct contour *signals,
						float **electrostatic_induction,
						float **inductance,
						float **cap_abs_diel,
						float *characteristic_impedance,
						float *propagation_velocity,
						float *equivalent_dielectric,
						FILE *output_file1,
						FILE *output_file2)
{
  int i;  /* loop index */

  /* special values if two conductors for odd and even modes */
  float even_odd[4];
  /* char_imp_odd, char_imp_even, prop_vel_odd, prop_vel_even;   */


  /********************************************************************
   *									*
   * Calculate special odd and even values for case of two conductors	*
   * Use -1.0 flags for un-calculatable results				*
   *									*
   ********************************************************************/
  if(number_conductors == 2)
  {
    double term;
    
    /* char_imp_odd */
    term = (inductance[0][0] - inductance[0][1]) /
		       (electrostatic_induction[0][0] -
			electrostatic_induction[0][1]);
    if(term < 0) even_odd[0] = -1.0;
    else even_odd[0] = sqrt(term);

    /* char_imp_even */
    term = (inductance[0][0] + inductance[0][1]) /
           (electrostatic_induction[0][0] + electrostatic_induction[0][1]);
    if(term < 0) even_odd[1] = -1.0;
    else even_odd[1] = sqrt(term);

    /* prop_vel_odd */
    term = (electrostatic_induction[0][0] - electrostatic_induction[0][1]) *
           (inductance[0][0] - inductance[0][1]);
    if(term < 0) even_odd[2] = -1.0;
    else even_odd[2] = 1.0 / sqrt(term);

    /* prop_vel_even */ 
    term = (electrostatic_induction[0][0] + electrostatic_induction[0][1]) *
           (inductance[0][0] + inductance[0][1]);
    if(term < 0) even_odd[3] = -1.0;
    else even_odd[3] = 1.0 / sqrt(term);
  }

  /********************************************************************
   *									*
   * Calculate Characteristic Impedance, Propagation Velocity and	*
   * Equivalent Dielectric Constant for each signal line.		*
   *									*
   ********************************************************************/
  for (i = 0; i < number_conductors; i++)
  {
    /****************************************************************
     *								*
     * Calculate the Characteristic Impedance.			*
     *								*
     ****************************************************************/
    characteristic_impedance[i] =  
      sqrt(inductance[i][i] / electrostatic_induction[i][i]);

    /****************************************************************
     *								*
     * Calculate the Equivalent Dielectric Constant for signal i     *
     * which is used for calculating Propagation Velocity.		*
     *								*
     ****************************************************************/
    equivalent_dielectric[i] = electrostatic_induction[i][i] 
      / cap_abs_diel[i][i];

    /****************************************************************
     *								*
     * Calculate the Propogation Velocity.				*
     *								*
     ****************************************************************/
    propagation_velocity[i] =  SPEED_OF_LIGHT / 
      sqrt(equivalent_dielectric[i]);
  }

  /********************************************************************
   *									*
   * Output Characteristic Impedance, Propagation Velocity and		*
   * Equivalent Dielectric Constant to 1st output file (if ptr not	*
   * NULL).								*
   *									*
   ********************************************************************/
  if (output_file1 != NULL)
    nmmtl_output_charimp_propvel(output_file1, characteristic_impedance,
				 propagation_velocity, equivalent_dielectric,
				 even_odd, signals);

  /********************************************************************
   *									*
   * Output Characteristic Impedance, Propagation Velocity and		*
   * Equivalent Dielectric Constant to 2nd output file (if ptr not	*
   * NULL).								*
   *									*
   ********************************************************************/
  if (output_file2 != NULL)
    nmmtl_output_charimp_propvel(output_file2, characteristic_impedance,
				 propagation_velocity, equivalent_dielectric,
				 even_odd, signals);

  return(SUCCESS);
}
