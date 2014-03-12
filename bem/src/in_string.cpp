/* *********************************************************************** *
 * ROUTINE NAME           in_string                                        *
 *                                                                         *
 * ABSTRACT      To simplify string searching. This function searches for  *
 * 	an entire substring (one or more QUOTED characters or "") within   *
 *      another string. This is different from the C functions strchr,     *
 *      strpbrk because it searches for a substring not just a character.  * 
 *      It returns the index of the substring in the main string or -1.	   *
 *                                                                         *
 * ENVIRONMENT	status = in_string( string, sub_string )		   *
 *      return status: index of the start of substring within main string  *
 *                     -1 if not found                                     *
 *                                                                         *
 * INPUTS   char *string, *sub_string					   *
 *                                                                         *
 * OUTPUTS  none. (see return status)					   *
 *                                                                         *
 * FUNCTIONS CALLED							   *
 *									   *
 * AUTHOR          David Endry                                             *
 * CREATION DATE   8-19-85                                                 *
 *                                                                         *
 * MODIFICATION HISTORY                                                    *
 *	1.00	Original			DDE     8-19-85            *
 *      1.01    General cleanup                 DDE     3-31-86            *
 *      1.02    Insert 	copyright notice        DDE     9-10-86            *
 *      1.03    Modified #includes to work with CMS	RWT  10-12-88	   *
 *                                                                         *
 *    Copyright (C) 1986 by Mayo Foundation.  All rights reserved.         *
 *    Copyright (C) 1988 by Mayo Foundation.  All rights reserved.         *
 *    Copyright (C) 1990 by Mayo Foundation.  All rights reserved.         *
 * *********************************************************************** *
 */
#include "magicad.h"
int in_string(const char *string,const char *sub_string)
{
     int i, j, k;              /* loop indices and pointers into the strings */

     for (i=0; string[i]!='\0'; i++) {	        /* search entire string */
          for (k=0, j=i; sub_string[k]!='\0' && string[j]==sub_string[k];
               k++, j++) ;                      /* compare along the strings */
          if (sub_string[k]=='\0')              /* reached end of sub string */
               return(i);                       /* return index into string */
     }
     return(-1);				/* match not found */
}
