
/*
  
  FACILITY:  NMMTL
  
  MODULE DESCRIPTION:
  
  Contains the function nmmtl_build_gnd_die_list()
  
  AUTHOR(S):
  
  Kevin J. Buchs
  
  CREATION DATE:  Wed Mar 18 09:48:35 1992
  
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
  
  FUNCTION NAME:  nmmtl_build_gnd_die_list
  
  
  FUNCTIONAL DESCRIPTION:
  
  Build up a list of list of dielectric - ground plane intersection points
  
  FORMAL PARAMETERS:
  
  GND_DIE_LIST_P *head  -  pointer to the head of the list - set for first
  element added to the list
  GND_DIE_LIST_P tail   -  on-going pointer to the tail of the list
  DIELECTRICS_P dielectrics - the dielectric that touches the ground plane.
  
  RETURN VALUE:
  
  Updated value of tail
  
  CALLING SEQUENCE:
  
  */

GND_DIE_LIST_P nmmtl_build_gnd_die_list(GND_DIE_LIST_P *head,
					GND_DIE_LIST_P tail,
					DIELECTRICS_P dielectrics)
{
  if(*head == NULL)
  {
    tail = (GND_DIE_LIST_P)malloc(sizeof(GND_DIE_LIST));
    *head = tail;
  }
  else
  {
    tail->next = (GND_DIE_LIST_P)malloc(sizeof(GND_DIE_LIST));
    tail = tail->next;
  }
  tail->next = NULL;
  tail->start = dielectrics->x0;
  tail->end = dielectrics->x1;
  tail->epsilon = dielectrics->constant;
  return(tail);
}
