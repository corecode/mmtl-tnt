
/*
  
  FACILITY:  NMMTL
  
  MODULE DESCRIPTION:
  
  Contains the function nmmtl_output_headers()
  
  AUTHOR(S):
  
  Kevin J. Buchs
  
  CREATION DATE:  Thu Apr 23 13:12:03 1992
  
  COPYRIGHT:   Copyright (C) 1992 by Mayo Foundation. All rights reserved.
  
  */


/*
 *******************************************************************
 **  INCLUDE FILES
 *******************************************************************
 */

#include "nmmtl.h"
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <string.h>

#ifdef HAVE_PWD_H
#include <pwd.h>
#endif

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
  FUNCTION NAME:  getlogin

  This compatibility function is supplied for broken
  systems like Windows where you can't call simple
  posix functions like getlogin().
*/
#ifndef HAVE_GETLOGIN
#include <stdlib.h>
char *getlogin() {
  char *name ;
  name = getenv("USERNAME");
  if ( name == NULL ) {
    name = getenv("USER");
  }
  return name;
}
#endif



/*
  
  FUNCTION NAME:  nmmtl_output_headers()
  
  
  FUNCTIONAL DESCRIPTION:
  
  Puts header information into the file.
  
  FORMAL PARAMETERS:
  
  FILE *output_file,
  char *filename,
  int num_signals,
  int num_grounds
  float coupling,
  float risetime
  
  RETURN VALUE:
  
  None
  
  CALLING SEQUENCE:
  
  nmmtl_output_headers(output_file1, filename,
                 num_signals,num_grounds,num_ground_planes,
		 coupling,risetime);
  
  */

void nmmtl_output_headers(FILE *output_file,
			  char *filename,
			  int num_signals,
			  int num_grounds,
			  int num_ground_planes,
			  float coupling,
			  float risetime,
			  int cntr_seg,
			  int pln_seg)
{
  char date[50];   /*  for getting and formatting date/time  */
  time_t tm ;

  struct passwd *user_entry;
  char *userName;
  static char nullString[] = "";

  // look up the user name 
  userName = getlogin() ;
  if ( userName == NULL ) {
    userName = "Username Unknown";
  }

  time(&tm) ;
  strftime(date,sizeof(date),"%Y %m %d %H:%M:%S", localtime(&tm) );


  fprintf(output_file,"\n%s %s %s\n\n",date,userName,"NMMTL_2DLF");
  fprintf(output_file,"File = %s\n",filename);
  fprintf(output_file,
	  "Number of Signal Lines  = %3d\n", num_signals);
  fprintf(output_file,
	  "Number of Ground Planes = %3d\n", num_ground_planes);
  fprintf(output_file,
	  "Number of Ground Wires  = %3d\n", num_grounds);
  fprintf(output_file,"Coupling Length = %9.5f meters\n",coupling);
  fprintf(output_file,"Rise Time = %10.4f picoseconds\n",risetime*1.0e12);
  fprintf(output_file,"Contour (conductor) segments [cseg] = %d\n",cntr_seg);
  fprintf(output_file,"Ground Plane/Dielectric segments [dseg] = %d\n",pln_seg);
  
}

