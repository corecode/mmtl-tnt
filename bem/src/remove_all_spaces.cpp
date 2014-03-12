/****************************************************************************\
* 									     *
*   ROUTINE NAME remove_all_spaces()					     *
* 									     *
*   ABSTRACT								     *
*     This function removes all of the blanks in a string and returns a	     *
*     pointer to the modified string.					     *
* 									     *
*   CALLING FORMAT							     *
*      remove_all_spaces(string);					     *
* 									     *
*   RETURN VALUE: a pointer to the modified string			     *
* 									     *
*   INPUT PARAMETERS:							     *
*     string - the string to remove spaces from.			     *
* 									     *
*   OUTPUT PARAMETERS							     *
*     string - the string after all spaces have been removed.		     *
* 									     *
*   USER FUNCTIONS & SYSTEM SERVICES CALLED				     *
* 									     *
*   AUTHOR          Jeff Ryan						     *
* 									     *
*   CREATION DATE							     *
* 									     *
*        Copyright (C) 1987 by Mayo Foundation.  All rights reserved.	     *
*        Copyright (C) 1990 by Mayo Foundation.  All rights reserved.	     *
* 									     *
*   MODIFICATION HISTORY						     *
*      1.00 Original                                         JMR  15-Jun-87  *
* 									     *
\****************************************************************************/
#include "magicad.h"
#include <ctype.h>
#include <string.h>

char *remove_all_spaces(char *string)

{
/*---------------------------------------------------------------------------
 * Local Variable Declaration
 *---------------------------------------------------------------------------
 */
    int i; /* index to next character in old string */
    int j; /* index to next character in new string */
/*---------------------------------------------------------------------------
 * Start of remove_all_spaces
 *---------------------------------------------------------------------------
 */
    /***********************************************************\
    *								*
    * Copy string over the top of itself, excluding any space   *
    * characters from the copy.					*
    *								*
    \***********************************************************/
    for (j=i=0; i<strlen(string); i++) 
        if (!isspace(string[i])) 
	    string[j++] = string[i];
    string[j] = '\0';  /* add null after last character in new string */

    return(string);
    
}

