/***************************************************************************\
* 									    *
*   ROUTINE NAME		graphic.h				    *
* 									    *
*   ABSTRACT	This file includes All the various graphic include files.   *
* 									    *
*   AUTHOR          Cary Bates						    *
* 									    *
*   CREATION DATE	28-AUG-1985					    *
* 									    *
*        Copyright (C) 1986 by Mayo Foundation.  All rights reserved.	    *
*        Copyright (C) 1989 by Mayo Foundation.  All rights reserved.	    *
* 									    *
*   MODIFICATION HISTORY						    *
*      1.00 Original                                         CLB 08-28-86   *
*      1.01 Constant generic window names                    DDE 11-07-86   *
*      1.02 Moved generic window names grcons                JMR 11-10-86   *
* 									    *
\***************************************************************************/

#ifndef graphic_h
#define graphic_h

/**** This holds graphic Constants */

#ifndef grcons_h
#include "grcons.h"
#endif

/***** This file holds all the structures and macros for calling the */
/***** Graphic_output function.  */

#ifndef graphic_output_h
#include "graphic_output.h"
#endif

#ifndef graphic_prototype_h
#include "graphic_prototype.h"
#endif

#endif
