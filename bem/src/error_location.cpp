
/****************************************************************************\
*                                                                            *
*  ROUTINE NAME			error_location				     *
*									     *
*  ABSTRACT								     *
*	Generate debug string for get_error() messages using file spec,	     *
*	line number and message string arguments passed in.  The format	     *
*	of the generated string is:					     *
*									     *
*	    File_spec  Line xxxx  :  message_text			     *
*									     *
*	This routine is usually called using the ERR_LOC macro defined	     *
*	in magicad_c.constants, and passed immediately to get_error as	     *
*	the debug string.  This makes the file spec the full VMS file	     *
*	spec including device and version information, and the line	     *
*	number is the line of the get_error call.			     *
*									     *
*									     *
*  CALLING FORMAT							     *
*									     *
*	debug_string = error_location( file_spec, line_number, message ) ;   *
*									     *
*	(or if using ERR_LOC macro and get_error() function)		     *
*									     *
*	get_error( ERR_CODE, NULL, PRINT, ERR_LOC("getting values") );	     *
*									     *
*									     *
*  RETURN VALUE								     *
*	char *debug_string	    pointer to debug string		     *
*									     *
*  INPUT PARAMETERS							     *
*	char *file_spec		    file specification  (__FILE__)	     *
*	int  line_number	    line number		(__LINE__)	     *
*	char *message		    message text appended to debug string    *
*									     *
*  OUTPUT PARAMETERS							     *
*	none								     *
*									     *
*  USER FUNCTIONS & SYSTEM SERVICES CALLED				     *
*	none								     *
*									     *
*  CALLED BY								     *
*	ERR_LOC macro							     *
*									     *
*  AUTHOR		TECHENTIN					     *
*  CREATION DATE	Friday, 19-MAY-1989				     *
*									     *
*	Copyright (C) 1989 by Mayo Foundation.  All rights reserved.	     *
*                                                                            *
\****************************************************************************/

#include "magicad.h"
#include <stdio.h>
#include <limits.h>

char *error_location(char * file_spec,int  line_number,char * message_text )
{
  static char debug_string_buffer[PATH_MAX + 256] ;
  
  if (message_text == 0)
    sprintf( debug_string_buffer, "%s  Line %d  :  ", file_spec, line_number) ;
  else
    sprintf( debug_string_buffer, "%s  Line %d  :  %s", file_spec, 
	     line_number, message_text ) ;

  return( debug_string_buffer ) ;
    
}
