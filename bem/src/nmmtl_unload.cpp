
/*
  
  FACILITY:  NMMTL
  
  MODULE DESCRIPTION:
  
  Contains the nmmtl_unload() function
  
  AUTHOR(S):
  
  Kevin J. Buchs
  
  CREATION DATE:  Thu Apr  2 11:57:24 1992
  
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
  
  FUNCTION NAME:  nmmtl_unload
  
  
  FUNCTIONAL DESCRIPTION:
  
  nmmtl_load() sets up the potential vector to set a particular signal
  line to one volt potential at one time.  nmmtl_unload is for un-doing this.
  
  FORMAL PARAMETERS:
  
  float *potential_vector            - vector of potential on all elements
  int conductor_number               - the number of the conductor to unset to
  one volt potential
  CONDUCTOR_DATA_P conductor_data,   - array of data on conductors
  
  RETURN VALUE:
  
  none 
  
  CALLING SEQUENCE:
  
    nmmtl_unload(potential_vector,ic,conductor_data);
  
  
  */

void nmmtl_unload(float *potential_vector,int conductor_number,
		  CONDUCTOR_DATA_P conductor_data)
{
  int i;
  CELEMENTS_P cel;
  
  cel = conductor_data[conductor_number].elements;
  while(cel != NULL)
  {
    for(i=0; i < INTERP_PTS; i++)
    {
      potential_vector[cel->node[i]] = 0.0;
    }
    cel = cel->next;
  }
}

