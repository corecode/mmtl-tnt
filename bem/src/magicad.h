#ifdef USE_MAGICAD_ANYWAY_YOU_FOOLISH_PERSON
/****************************************************************************\
*                                                                            *
*   ROUTINE NAME    magicad.h					             *
*									     *
*   ABSTRACT  This is the include file for C++. It defines constants	     *
*	      used throughout the MAGICAD system's C++ programs. Each	     *
*	      constant MUST have the same value as the constant in the	     *
*	      related FORTRAN and Pascal include files (although those	     *
*	      files may not be as complete as this file). See structure.h    *
*	      for additional C++ definitions.				     *
*									     *
*   CALLING FORMAT  #include <magicad.h>	                             *
*									     *
*   AUTHOR          David Endry						     *
*									     *
*   CREATION DATE   2-13-87						     *
*									     *
*	Copyright (C) 1987 by Mayo Foundation.  All rights reserved.	     *
*	Copyright (C) 1988 by Mayo Foundation.  All rights reserved.	     *
*	Copyright (C) 1989 by Mayo Foundation.  All rights reserved.	     *
*	Copyright (C) 1990 by Mayo Foundation.  All rights reserved.	     *
*	Copyright (C) 1991 by Mayo Foundation.  All rights reserved.	     *
*	Copyright (C) 1995 by Mayo Foundation.  All rights reserved.	     *
*									     *
*   MODIFICATION HISTORY						     *
*      2.00 Original (from structure.h)                        DDE  2-13-87  *
*      2.01 Added graphics constants			       DDE  2-17-87  *
*      2.02 Moved the constant CAD_MANAGER from the site		     *
*	   dependant section of structure.h to this file.		     *
*	   This constant is now a call to the function			     *
*	   get_cad_manager_name. Now we will not have to		     *
*	   recompile to change the CAD Manager.                CLB  3-31-87  *
*      2.03 Added the contant to point to the log file that		     *
*	   will be kept by the mcms_access_exit_handler.       CLB  4-22-87  *
*      2.04 changed VMS_FAIL code, add VMS_FILNOTFND, change		     *
*	    versions to 2.xx				       BRS  4-23-87  *
*      2.05 SIZE_ATTR_VAL was 256, too large with pickables    DDE  6-01-87  *
*      2.06 allow the word BACKUP in addition to 'b'	       DDE  7-20-87  *
*      2.07 Added constants for cross reference file.          CLB  7-23-87  *
*      2.08 Added includes for stdlib and string.              CLB  8-10-87  *
*      2.09 The functions is_integer and is_float are now		     *
*	   obsolete.  These definitions are here so that		     *
*	   the functions that use them will still compile.		     *
*	   These definitions will go away after the current		     *
*	   chip is done (became is_int, is_real).              CLB  8-11-87  *
*      2.10 Changed the format an the #include stdlib it was		     *
*	   still including version 1 of this #!%$@ file.       CLB  8-15-87  *
*      2.11 "system" purged stdlib, change #include to normal  BRS  8-26-87  *
*      2.12 added constants for the string_hash() and			     *
*	   number_hash() functions.                            JMR  9-02-87  *
*      2.13 add VMS_EXIT, VMS_BACKUP			       BRS  9-08-87  *
*      2.14 add TIMESTAMP_SIZE			       	       JMR  9-17-87  *
*	    also removed redundant graphics constants			     *
*      2.15 removed is_integer, is_float re-defines 2.09       DDE 11-13-87  *
*      2.16 handle MiXeD CaSe on special input values          DDE 11-16-87  *
*      2.17 Added Physical node definitions.                   CLB  1-07-88  *
*      2.18 Made using graphics test for empty window list.    JMR  3-25-88  *
*      2.19 Re-did the list_input macro.                       CLB  6-09-88  *
*                                                                            *
\****************************************************************************/

#ifndef magicad_h
#define magicad_h

/****************************************************************************\
*	ALPHA Cross Development Definitions				     *
\****************************************************************************/
#ifdef __ALPHA
/*#pragma nomember_alignment */
#endif

/****************************************************************************\
*	Redefine non-portable VAX-C constructs				     *
\****************************************************************************/
#define globaldef
#define globalref	extern 


/* *************************************************************************
*   We always redefine globalvalue, but ctype.h and errodefs.h depend on
*   the VMS globalvalue symbol, so include those first 
****************************************************************************/
#include <ctype.h>
#ifdef VMS
#include <errnodef.h>
#endif

/*////////////////////////////////////////////////////////////////////////
// on hpux we need to include sys/stdsyms to setup the POSIX + XOPEN standards
////////////////////////////////////////////////////////////////////////*/
#ifdef __hpux
#include <sys/stdsyms.h>
#endif

/****************************************************************************\
*	Include system definitions					     *
\****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef __cplusplus
#include <iostream.h>
#endif
#include <limits.h>
#if !defined(PATH_MAX) || defined(__hpux)
#undef PATH_MAX
#define PATH_MAX 1023
#endif
#if !defined(FILENAME_MAX) || defined(__hpux)
#undef FILENAME_MAX
#define FILENAME_MAX 256
#endif



#ifdef _WIN32
enum boolean {FALSE=0,TRUE=1};
#define NO_VFORK
#ifdef __cplusplus
inline ostream & operator <<(ostream &s,boolean value)
{
  int intValue = (int) value;
  s << intValue;
  return s;
}
#endif
#endif

#include <float.h>
#ifndef NO_VFORK
#define fork vfork
#endif
#ifdef __cplusplus
#include <rw/defs.h>
#endif

#ifndef _WIN32
#ifdef TRUE
#undef TRUE 
#endif
#ifdef FALSE 
#undef FALSE
#endif
enum boolean {FALSE=0,TRUE=1};
#define TRUE TRUE
#define FALSE FALSE
#endif
#include <assert.h>
/****************************************************************************\
*	Redefine memory allocation functions to use MagiCAD specific	     *
*	versions which do error checking and exit on failure		     *
\****************************************************************************/
#include <malloc.h>
#ifdef calloc		/* check for prior art - take care of proper	*/
#undef calloc		/*  redefinition in MAGICAD_ALLOC.C.		*/
#undef malloc
#undef realloc
#undef free
#endif
void *magicad_calloc(size_t nmemb, size_t size, char *fname, int line);
void *magicad_malloc(size_t size, char *fname, int line);
void *magicad_realloc(void *ptr, size_t size, char *fname, int line);
void magicad_free(void *ptr, char *fname, int line);
#define calloc(n,s)	magicad_calloc((n),(s),__FILE__,__LINE__)
#define malloc(s)	magicad_malloc((s),__FILE__,__LINE__)
#define realloc(p,s)	magicad_realloc((p),(s),__FILE__,__LINE__)
#define free(p)		magicad_free((p),__FILE__,__LINE__)


/****************************************************************************\
*	Constants and sizes used throughout the CAD			     *
\****************************************************************************/
#define NODE "MC_NODE"          /* the name of the environment variable   */
                                /* that should hold the current Node name */

#define DESIGN "MC_DESIGN"      /* the name of the environment variable     */
                                /* that should hold the path to the current */
                                /* node

#define	NOPRINT		0	/* don't print error messages */
#define	PRINT		1	/* print error messages */
#define RECALL_LIMIT    20      /* 1.42 max # of command recall lines used */
				/* in text_line_input & graphic_line_input */
#define MAX_LEVELS	100     /* the max levels of the design 1.21 */
#define NEW_HEADER      1       /* 1.54 header_room() new header mode */
#define FREE_HEADER     2       /* 1.54 header_room() free tt_header mode */
#define SIZE_HEADER     100     /* 1.51 header for paging */
#define SIZE_EQUIV_NAME 140     /* 1.49 140 is random, the true max len of the
				   equivalent name string (logical name value)
				   is 255 (System Services SYS-450 V4.4) */
#define	SIZE_FILE_SPEC	PATH_MAX        /* max size of full file spec (1.25 was 110) */
#define SIZE_FILE_TYPE	FILENAME_MAX	/* size of type or extension of VMS 
                                           file specifications */
#define SIZE_COMMENT	512	/* max size of the history.comment field */

#define SIZE_NODE_NAME	FILENAME_MAX -1  /* size of the node name field, used for
				   node.node_name,
				   hierarchy.node_name,
				   hierarchy.child_name,
				   hierarchy.child_location,
				   history.node_name */
#define SIZE_DATE	23	/* size of date field, used for
				   node.creation_date,
				   history.date_time */
#define SIZE_USERNAME	15	/* size of the username field, used for
				   node.creator,
				   node.lock_owner,
				   history.user */

#define GPGE_MAX 2512	        /* Maximum length of a line in graphic files */

#define SIZE_ATTR_TYPE 25       /* PRIMITIVE, PIN, INSTANCE, ICON.... */
#define SIZE_ATTR_NAME 40       /* max length of GPGE attribute name */
#define SIZE_ATTR_VAL  1999     /* max length of any attribute value (1/25/96)*/


/****************************************************************************\
* 1.01 constants used throughout the CAD by graphics programs and compilers  *
\****************************************************************************/
#define CIRCLE 'A'              /* primitives marked A are circles    */
#define RECTANGLE 'R'           /* primitives marked R are rectangles */
#define POLYGON 'G'             /* primitives marked G are polygons   */
#define CURVE 'C'               /* primitives marked C are curves     */
#define POLYLINE 'P'            /* primitives marked P are polylines  */
#define LINE 'L'                /* primitives marked L are lines      */


/****************************************************************************\
*   following are function calls to determine if we are using the cad	     *
*   system  and to see if operating in debug mode.  The logical names	     *
*   must be defined in the table "cad", and can be set to any value.	     *
*   Will be true if they are are defined, and false if not defined.	     *
*   Use "define/table=cad log-name value".  1.11			     *
\****************************************************************************/
#define	USING_GRAPHICS	are_graphics_enabled()          /* 2.18 */
#define CAD_MANAGER     get_cad_manager_name()          /* 2.02 - Used for  */
							/* sending mail.    */

#define MAIL_MESS       0       /* make mail.c mail a message   1.16 */
#define MAIL_FILE       1       /* make mail.c mail a file  */
#define MAIL_FILE_DEL   2       /* make mail.c mail a file and delete it */


/****************************************************************************\
*	constants for net and component name sizes			     *
\****************************************************************************/
#define SIZE_NET_NAME 512	/* maximum length +1 of net name */
#define SIZE_COMPONENT_NAME 512	/* maximum length +1 of a component name */
#define SIZE_PATH_ENDTYPE 512	/* maximum length +1 of path's end type */
#define SIZE_VIATYPE 512	/* maximum length +1 of via type */

/****************************************************************************\
*	help constants							     *
\****************************************************************************/
#define HLP_PROMPT      0x21    /* make get_help prompt 1.18 */
#define HLP_NOPROMPT    0x20    /* get_help will just display 1.18  */


/****************************************************************************\
*	Confirm flags for output 1.48					     *
\****************************************************************************/
#define CONF_NEVER       0  /* Never confirm */
#define CONF_OVER        1  /* Confirm if graphics window in Over-write mode */
#define CONF_NOCONF      2  /* Confirm if whindow is set not to confirm */
#define CONF_OVER_NOCONF 3  /* Confirm if over-write and no-confirm window */
#define CONF_ALWAYS      4  /* Confirm always */


/****************************************************************************\
*	Special Input Test Macros					     *
*   1.40 used in get_input, placed here because most test functions use them *
*   1.41 BACKUP character was '-', now 'b' so it can be used in DCL code too *
*   1.44 added list_input for get_node, get_design_dir... when '*' is input  *
*   2.16 define a function to check special input values, adhere to new rules*
*	 rules in cadroot:[doc.general]special_input.doc.		     *
*   WILDCARD_INPUT function defined to check for wildcard values '*' and '%' *
*   in the first character ONLY of input.				     *
\****************************************************************************/
#define  NULL_INPUT(str)   (str[0] == '\0')
#define	 BACKUP_INPUT(str) special_input(str, 'B')
#define  EXIT_INPUT(str)   special_input(str, 'E')
#define  HELP_INPUT(str)   special_input(str, 'H')
#define  SEARCH_INPUT(str)   special_input(str, 'S')
#define  LIST_INPUT(str)   (str[0] == '?')
#define  WILDCARD_INPUT(str)  ((str[0] == '*') || (str[0] == '%'))


/****************************************************************************\
*   2.07 - Define constants for Key it use when accessing the		     *
*          cross reference file.					     *
\****************************************************************************/
#define  CROSSREF_PATH   1
#define  CROSSREF_UNIQUE 2


/****************************************************************************\
*   2.12 - Define constants for string and number hash functions.	     *
*   These constants define the modes of operation for the hash functions.    *
\****************************************************************************/
#define HASH_INITIALIZE  0
#define HASH_RESET       1
#define HASH_PUT         2
#define HASH_FETCH       3
#define HASH_REMOVE      4


/****************************************************************************\
*   2.14 - size of timestamp needed for timestamp() function and	     *
*          newer_timestamp() function					     *
\****************************************************************************/
#define TIMESTAMP_SIZE   7


/****************************************************************************\
*	Return status for the function compare_times()			     *
\****************************************************************************/
#define FIRST 1
#define SECOND 2


/****************************************************************************\
*	Physical node types						     *
\****************************************************************************/
#define PHYSICAL_FLOORPLAN_NODE   16
#define PHYSICAL_TECHNOLOGY_NODE  32
#define PHYSICAL_SOCKET_NODE      64


/****************************************************************************\
*	ERR_LOC macro used for generating get_error() debug strings.	     *
*	Evaluates to a (char *) containing file spec, line number and	     *
*	a message string.						     *
\****************************************************************************/
char *error_location( char *file_spec, int line_number, char *message ) ;

#define ERR_LOC(s) error_location( __FILE__, __LINE__, s )


/****************************************************************************\
*    fopen() Macros							     *
*    These macros can be substituted for "r" and "w" in fopen calls	     *
*    to take advantage of RMS options that improve file performance.	     *
*									     *
*	READ_MODE		for reading all files			     *
*	WRITE_MODE		for writing small files (<15 blocks)	     *
*	WRITE_BLOCK_MODE	for writing large files (>15 blocks)	     *
*									     *
*    READ_MODE_OPTIONS, WRITE_MODE_OPTIONS and WRITE_BLOCK_MODE_OPTIONS	     *
*    are for passing to file open functions such as access_n_open(),	     *
*    where the "r" or "w" is not needed.				     *
*									     *
*	"ctx=rec"	forces record mode even for stream_LF files	     *
*	"mbf=2"		use two buffers for reading			     *
*	"rop=RAH"	read-ahead buffering				     *
*	"rop=WBH"	write-behind buffering				     *
*	"deq=21"	default extent quantity 21 blocks		     *
*	"fop=cbt,tef"	contiguous best-try, truncate at EOF		     *
\****************************************************************************/

#ifdef VMS
#define READ_MODE	 "r","ctx=rec","mbf=2","rop=RAH"
#define WRITE_MODE	 "w","mbf=2","rop=WBH"
#define WRITE_BLOCK_MODE "w","mbf=2","rop=WBH","fop=cbt,tef","deq=21","rfm=var"
#define READ_MODE_OPTIONS	 "ctx=rec","mbf=2","rop=RAH"
#define WRITE_MODE_OPTIONS	 "mbf=2","rop=WBH"
#define WRITE_BLOCK_MODE_OPTIONS "mbf=2","rop=WBH","fop=cbt,tef","deq=21","rfm=var"
#else
#define READ_MODE	 "r"
#define WRITE_MODE	 "w"
#define WRITE_BLOCK_MODE "w"
#define READ_MODE_OPTIONS		""
#define WRITE_MODE_OPTIONS		""
#define WRITE_BLOCK_MODE_OPTIONS	""
#endif




#ifdef __cplusplus
/******************************************************************************
*                        Very useful simple classes
******************************************************************************/
class generic_string
{
  char *ptr;

public:
  generic_string() {ptr = 0;}
  generic_string(const generic_string &old)
  {
    if (old.ptr != 0)
      {
	ptr = new char[strlen(old.ptr)+1];
	strcpy(ptr,old.ptr);
      }
    else
      ptr = 0;
  }
  generic_string(const char *old)
  {
    ptr = new char[strlen(old)+1];
    strcpy(ptr,old);
  }
  ~generic_string() {if (ptr != 0) delete [] ptr;}
  generic_string & operator=(const generic_string &old)
  {
    if (ptr != 0) delete [] ptr;
    if (old.ptr != 0)
      {
	ptr = new char[strlen(old.ptr)+1];
	strcpy(ptr,old.ptr);
      }
    else
      ptr = 0;
    return *this;
  }
  boolean operator > (const generic_string &inName)
  {
    if (strcmp(ptr,inName.ptr) > 0)
      return TRUE;
    else
      return FALSE;
  }
  boolean operator == (const generic_string &inName)
  {
    if (strcmp(ptr,inName.ptr) == 0)
      return TRUE;
    else
      return FALSE;
  }
  operator const char* () const { return ptr;}
friend ostream & operator<< (ostream &s,generic_string &o)
  {
    s << o;
    return s;
  }
};

/******************************************************************************
*                        ErrorMessages
******************************************************************************/

extern FILE *caderr; /* defined in output */
extern FILE *cadLog; /* defined in output */
/******************************************************************/
/*                these are for the cad message facillity         */
/*****************************************************************/

extern int cad_error_total;
extern int cad_warning_total;
extern int cad_message_total;


class magicad_message
{
protected:
  char *ostring;
  long id;
  char sev;

public:
  magicad_message() : ostring(0),id(0),sev('F') {}
  char *message() const {return ostring;}
  long identifier() const {return id;}
  char severity() const {return sev;} 

  boolean operator == (const magicad_message & other) const
  {
    if (other.identifier() == identifier())
      return TRUE;
    else 
      return FALSE;
  }

/*  boolean operator ==(const long other) const
  {
    if (other == identifier())
      return TRUE;
    else 
      return FALSE;
  }
  */
  boolean operator !=(const magicad_message & other) const
  {
    if (other.identifier() != identifier())
      return TRUE;
    else 
      return FALSE;
  }

/*  boolean operator !=(const long other) const
  {
    if (other != identifier())
      return TRUE;
    else 
      return FALSE;
  }
  */
  operator long() const  {return identifier();}
  
};




class magicad_message
{
protected:
  char *ostring;
  long id;
  char sev;

public:
  magicad_message() : ostring(0),id(0),sev('F') {}
  char *message() const {return ostring;}
  long identifier() const {return id;}
  char severity() const {return sev;} 

  boolean operator == (const magicad_message & other) const
  {
    if (other.identifier() == identifier())
      return TRUE;
    else 
      return FALSE;
  }

/*  boolean operator ==(const long other) const
  {
    if (other == identifier())
      return TRUE;
    else 
      return FALSE;
  }
  */
  boolean operator !=(const magicad_message & other) const
  {
    if (other.identifier() != identifier())
      return TRUE;
    else 
      return FALSE;
  }

/*  boolean operator !=(const long other) const
  {
    if (other != identifier())
      return TRUE;
    else 
      return FALSE;
  }
  */
  operator long() const  {return identifier();}
  
};




/****************************************************************************\
*	Return status for functions					     *
\****************************************************************************/

const static char STR_MAGICAD_FAIL[] = "MAGICAD-I-FAIL A cad function has failed";
class magicad_fail : public magicad_message
{
public:
  magicad_fail()
  {
    ostring = (char *) STR_MAGICAD_FAIL;
    id = 0;
    sev = 'I';
  }
};

const magicad_fail FAIL;

const static char STR_MAGICAD_SUCCESS[] = "MAGICAD-I-SUCCESS A cad function has succeeded";
class magicad_success : public magicad_message
{
public:
  magicad_success()
  {
    ostring = (char *) STR_MAGICAD_SUCCESS;
    id = 1;
    sev = 'I';
  }
};
const magicad_success SUCCESS;

const static char STR_MAGICAD_EXIT[] = "MAGICAD-I-EXIT The user has asked to exit";
class magicad_exit : public magicad_message
{
public:
  magicad_exit()
  {
    ostring = (char *)STR_MAGICAD_EXIT;
    id = 2;
    sev = 'I';
  }
};
const magicad_exit EXIT;

const static char STR_MAGICAD_BACKUP[] = "MAGICAD-I-BACKUP The user has asked to go back one prompt";
class magicad_backup : public magicad_message
{
public:
  magicad_backup()
  {
    ostring = (char *) STR_MAGICAD_BACKUP;
    id = 3;
    sev = 'I';
  }
};
const magicad_backup BACKUP;

const static char STR_MAGICAD_REPROMPT[] = "MAGICAD-I-REPROMPT A cad function has asked that the user be reprompted";
class magicad_reprompt : public magicad_message
{
public:
  magicad_reprompt()
  {
    ostring = (char *) STR_MAGICAD_REPROMPT;
    id = 4;
    sev = 'I';
  }
};
const magicad_reprompt REPROMPT;

/* this class is meant to be used to throw
 instances of bugs in the use of a class
 */
const static char STR_BADOPER[] = "CAD-F-BADOPER A bug exists in this programs use of the list class\n-> The program:\n-->%s\n";
class BADOPER : public magicad_message
{

public:
  BADOPER(char * arg0);
};

typedef int ReturnStatus;



/******************************************************************************
*                        prototypes
******************************************************************************/

char *error_location( const char *file_spec, int line_number, 
		      const char *message_text );

magicad_message get_date(char *date );


magicad_message get_help(const char *topic, const char *library = "", 
			 int flag = HLP_NOPROMPT);

magicad_message get_input(const char *prompt, const char *default_,char *dest,
			  const char *hlp_topic,
			  const char *help_library, int prmpt_flg, PFI test, 
			  int maxlen,
			  int window=0, int remove_spaces=FALSE);

char *get_logical_name(const char *name, int case_flag=FALSE);

magicad_message get_outfile(char *filename,FILE **fptr,char *tt_header=0);


int in_string(const char *string,const char *sub_string);

boolean special_input(const char *str, char key);

char *string_lower (char *string);

char *string_upper (char *string);

magicad_message truncate_string (char *string);

inline int cad_output(int message,char *debug_string=NULL)
{
  char sev = message.severity();
  if ((sev == 'F') || (sev == 'E'))
    cad_error_total++;
  if ((sev == 'W'))
    cad_warning_total++;

  cad_message_total++;

  cad_output(caderr,text_error_window,CONF_OVER_NOCONF,
	     "%s\n",message.message());
  if (cadLog != 0)
    cad_output(cadLog,text_error_window,CONF_OVER_NOCONF,
	       "%s\n",message.message());
      
  return message;
}



#define throw(arg) PrintAndExit(arg)
extern int text_error_window;


inline void PrintAndExit(magicad_message message)
{

  cad_output(caderr,text_error_window,CONF_OVER_NOCONF,
	     "%s\n",message.message());
  exit(1);
}
/*  endif for magicad.h */
#endif

#endif

#endif /* ifdef 0 */

#ifndef MAGICAD_H_FIXED
#define MAGICAD_H_FIXED 1

#include <stdio.h>

enum boolean {FALSE=0,TRUE=1};



/****************************************************************************\
*	Test Function Types						     *
*	Variable length argument list macros require a type definition	     *
*	for a pointer to a function returning int, and the preprocessor	     *
*	is not capable of dealing with the definition (see K&R 6.7)	     *
\****************************************************************************/
typedef int (*PFI)(char *) ;
typedef PFI (*PFPFI)(char *, int) ;

#define SUCCESS 1
#define FAIL 0



#define GPGE_MAX 2512	        /* Maximum length of a line in graphic files */

#define SIZE_ATTR_TYPE 25       /* PRIMITIVE, PIN, INSTANCE, ICON.... */
#define SIZE_ATTR_NAME 40       /* max length of GPGE attribute name */
#define SIZE_ATTR_VAL  1999     /* max length of any attribute value (1/25/96)*/


/****************************************************************************\
* 1.01 constants used throughout the CAD by graphics programs and compilers  *
\****************************************************************************/
#define CIRCLE 'A'              /* primitives marked A are circles    */
#define RECTANGLE 'R'           /* primitives marked R are rectangles */
#define POLYGON 'G'             /* primitives marked G are polygons   */
#define CURVE 'C'               /* primitives marked C are curves     */
#define POLYLINE 'P'            /* primitives marked P are polylines  */
#define LINE 'L'                /* primitives marked L are lines      */



/****************************************************************************\
*	ERR_LOC macro used for generating get_error() debug strings.	     *
*	Evaluates to a (char *) containing file spec, line number and	     *
*	a message string.						     *
\****************************************************************************/
char *error_location( char *file_spec, int line_number, char *message ) ;

#define ERR_LOC(s) error_location( __FILE__, __LINE__, s )




int cad_output (const char *fmt, ...);

int cad_output (
		FILE *fptr,	
		int window_name,
		int confirm_flag,
		const char *fmt,
		...	
		);




#endif
