
/*
  
  FACILITY:  
  
  MODULE DESCRIPTION:
  
  
  
  AUTHOR(S):
  
  Kevin J. Buchs
  
  CREATION DATE:  1-AUG-1991 11:54:13
  
  COPYRIGHT:   Copyright (C) 1991,1992 by Mayo Foundation. All rights reserved.
  
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
  
  FUNCTION NAME:  nmmtl_find_ground_planes
  
  
  FUNCTIONAL DESCRIPTION:
  
  Returns the locations of the ground planes in terms of top of the bottom and
  bottom of top, and the left and right edges.
  
  FORMAL PARAMETERS:
  
  struct dielectric *dielectrics,  - list of dielectrics to search
  int gnd_planes,                  - number of ground planes to seek (1 or 2)
  double *top_of_bottom_plane,      - return coordinate values
  double *bottom_of_top_plane,        ""
  double *left,                       ""
  double *right,                      ""
  
  RETURN VALUE:
  
  SUCCESS
  
  CALLING SEQUENCE:
  
  status = nmmtl_find_ground_planes(dielectrics,gnd_planes,
  &top_of_bottom_plane,&bottom_of_top_plane,
  &left,&right);
  
  */


int nmmtl_find_ground_planes(struct dielectric *dielectrics,
					 double *top_of_bottom_plane,
					 double *bottom_of_top_plane,
					 double *left, double *right)
{
  if(dielectrics == NULL)
  {
    fprintf(stderr,"ELECTRO-F-ZERODIE No dielectric layer found - at least 1 is required\n");
    return(FAIL);
  }
  *top_of_bottom_plane = dielectrics->y0;
  *bottom_of_top_plane = dielectrics->y1;
  *left = dielectrics->x0;
  *right = dielectrics->x1;
  dielectrics = dielectrics->next;
  while(dielectrics != NULL)
  {
    if(dielectrics->y0 < *top_of_bottom_plane)
      *top_of_bottom_plane = dielectrics->y0;
    if(dielectrics->y1 > *bottom_of_top_plane)
      *bottom_of_top_plane = dielectrics->y1;
    if(dielectrics->x0 < *left)
      *left = dielectrics->x0;
    if(dielectrics->x1 > *right)
      *right = dielectrics->x1;
    dielectrics = dielectrics->next;
  }
  
#ifdef DIAG_GND_PLANES
  printf("ground planes:\n top of bottom: %g\n bottom of top: %g\n\
 left: %g\n right: %g\n",*top_of_bottom_plane,*bottom_of_top_plane,
	  *left,*right);
#endif
  return(SUCCESS);
}    
