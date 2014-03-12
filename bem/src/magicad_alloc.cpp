/****************************************************************************\
*                                                                            *
*		    MAGICAD MEMORY ALLOCATION FUNCTIONS			     *
*                                                                            *
*  MODULE NAME		magicad_alloc					     *
*									     *
*  ABSTRACT								     *
*	This module contains functions to allocate			     *
*	memory in the MagiCAD Environment.  The functions		     *
*	will check for errors and, if necessary, print			     *
*	an error message, deaccess all files and terminate.		     *
*									     *
*	The functions are functional replacements for the		     *
*	standard library calls to calloc, malloc and realloc.		     *
*	The functions are prototyped in magicad_c.constants,		     *
*	where the library function names are redefined to		     *
*	use these replacements.  Note that the function name		     *
*	redefinitions in magicad_c.constants are undone here		     *
*	to prevent recursive preprocessor definitions.			     *
*									     *
*	Two extra arguments, file name and line number,			     *
*	are supplied by the caller.  These values are used to		     *
*	build the debug string passed to get_error, and will		     *
*	indicate the source file and line number of the			     *
*	routine calling malloc() etc.  Note that the macro		     *
*	definitions in magicad_c.constants supply the			     *
*	preprocessor macros __FILE__ and __LINE__ automatically.	     *
*									     *
*  CALLING FORMAT							     *
*	pointer = magicad_malloc( size, fname, line ) ;			     *
*	pointer = magicad_calloc( nmemb, size, fname, line ) ;		     *
*	pointer = magicad_realloc( ptr, size, fname, line ) ;		     *
*	magicad_free( ptr ) ;						     *
*									     *
*  RETURN VALUE								     *
*	void *pointer	    This can be a pointer to anything		     *
*									     *
*  INPUT PARAMETERS							     *
*	void	*ptr;		    pointer to be reallocated or freed	     *
*	size_t	size ;		    size of memory to be allocated	     *
*	size_t	nmemb ;		    number of elements to allocate	     *
*	char	*fname ;	    file name of calling routine	     *
*	int	line ;		    line number of calling routine	     *
*									     *
*  OUTPUT PARAMETERS							     *
*	none								     *
*									     *
*  AUTHOR		TECHENTIN					     *
*  CREATION DATE	Friday, 30-AUG-1991				     *
*									     *
*	Copyright (C) 1991 by Mayo Foundation.  All rights reserved.	     *
*	Copyright (C) 1992 by Mayo Foundation.  All rights reserved.	     *
*                                                                            *
\****************************************************************************/


/****************************************************************************\
*                                                                            *
*	Some constants are required, but we cannot include		     *
*	magicad_c.constants, because it redefines calloc()		     *
*	and friends (which is one too many levels of indiscretion).  ;-)     *
*                                                                            *
\****************************************************************************/

#include <stddef.h>	    /*  included for NULL  */
#include <stdio.h>
#include<stdlib.h>
#include<string.h>
#define SIZE_COMMENT 512

int cad_output (const char *fmt,...);

class magicad_message
{

protected:
  char message_string[SIZE_COMMENT];
  char format_string[SIZE_COMMENT];
  long id;

public:
  magicad_message(long number,char *string) 
  : 
  id(number)
  {
    strcpy(format_string,string);
  }
  char *message()
  {
    return message_string;
  }
  long identifier() {return id;}
};
inline void cad_output(magicad_message message)
{
  cad_output(message.message());
}

#include "cad_messages.hxx"
#define PRINT	    1
#define VMS_FAIL    3


/****************************************************************************\
*	Define the following for Version 5 VMS VAXC RTL.  See "VMS Version   *
*	5.0 Release Notes", AA-LB22A-TE, pp. 9-20.			     *
\****************************************************************************/
#ifdef VAXC
#ifdef calloc
#undef calloc
#undef malloc
#undef realloc
#undef free
#endif
#ifdef __ALPHA
#define	malloc	decc$malloc_opt
#define	calloc	decc$calloc_opt
#define	free	decc$free_opt
#define	realloc	decc$realloc_opt
void *decc$calloc_opt(size_t nmemb, size_t size);
void *decc$malloc_opt(size_t size);
void *decc$realloc_opt(void *ptr, size_t size);
void decc$free_opt(void *ptr);
#else
#define	malloc	vaxc$malloc_opt
#define	calloc	vaxc$calloc_opt
#define	free	vaxc$free_opt
#define	realloc	vaxc$realloc_opt
void *vaxc$calloc_opt(size_t nmemb, size_t size);
void *vaxc$malloc_opt(size_t size);
void *vaxc$realloc_opt(void *ptr, size_t size);
void vaxc$free_opt(void *ptr);
#endif
#endif

#include <stdlib.h>
#include "malloc.h"



void *magicad_calloc( size_t nmemb, size_t size, char *fname, int line )
{
    register void *pointer ;	/*  temporary pointer	*/

#ifdef MAGICAD_DEBUG
    pointer = debug_calloc( fname, line, nmemb, size ) ;
#else
    pointer = calloc(nmemb,size);
#endif

    if (pointer == NULL)
    {
	cad_output( cad_outofmemory(nmemb*size ));
/*	mcms_deaccess_all() ;*/
	exit( VMS_FAIL ) ;
    } 

    return( pointer ) ;
}

void *magicad_malloc( size_t size, char *fname, int line )
{
    register void *pointer ;	/*  temporary pointer	*/

#ifdef MAGICAD_DEBUG
    pointer = debug_malloc(fname, line, size ) ;
#else
    pointer = malloc( size ) ;
#endif

    if (pointer == NULL)
    {
	cad_output( cad_outofmemory(size ));
/*	mcms_deaccess_all() ;*/
	exit( VMS_FAIL ) ;
    } 

    return( pointer ) ;
}

void *magicad_realloc( void *ptr, size_t size, char *fname, int line )
{
    register void *pointer ;	/*  temporary pointer	*/

#ifdef MAGICAD_DEBUG
    pointer = debug_realloc( fname, line, ptr, size ) ;
#else
    pointer = realloc( ptr, size ) ;
#endif

    if (pointer == NULL)
    {
	cad_output( cad_outofmemory(size) );
/*	mcms_deaccess_all() ; */
	exit( VMS_FAIL ) ;
    } 

    return( pointer ) ;
}

void magicad_free( void *pointer, char *fname, int line )
{
#ifdef MAGICAD_DEBUG
  debug_free( fname, line, pointer ) ;
#else
  free( pointer);
#endif
}








