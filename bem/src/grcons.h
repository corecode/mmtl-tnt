/***************************************************************************\
* 									    *
*   ROUTINE NAME		grcons.h				    *
* 									    *
*   ABSTRACT	This is an include file of constants defined for the        *
* 		DI3000 graphics driver routines.			    *
* 									    *
*   AUTHOR      Sharon Zahn						    *
* 									    *
*   CREATION DATE							    *
* 									    *
*        Copyright (C) 1986 by Mayo Foundation.  All rights reserved.	    *
*        Copyright (C) 1989 by Mayo Foundation.  All rights reserved.	    *
* 									    *
*   MODIFICATION HISTORY						    *
*      1.00 Original                                         SKZ  MM-DD-YY  *
*      1.01 Updates and additions                            JMR            *
*      1.02 Added tablet def                                 CLB   7-02-86  *
*      1.03 Added linestyle definitions                      JMR   7-11-86  *
*      1.04 Added simulation fill pattern definitions        JMR   9-14-86  *
*           Also added text precision definitions                           * 
*      1.05 Added graphic window name constants              JMR   11-10-86 *
*      1.06 Added constants for reset_window.                CLB   12-30-86 *
* 									    *
\***************************************************************************/

#ifndef grcons_h
#define grcons_h

/*---------------------------------------------------------------------------
 * Graphic Constants File
 *---------------------------------------------------------------------------
 */


/*-----------------------------------------------------------------------
 *   Constant for Reset window                1.06
 *-----------------------------------------------------------------------
*/


/* colors for set_color, def_color */
/*  #define NORMAL    0  */
#define RED       1
#define GREEN     2
#define YELLOW    3
#define BLUE      4
#define MAGENTA   5
#define CYAN      6
/* Define WHITE to be the 'normal' color of the device.  For most devices,   */
/*    the 'normal' color will be either black (hardcopy devices) or white    */
/*    (screen displays).                                                     */
#define WHITE     0
/* Define the old white color to be 7 as previously defined.                 */
#define OLD_WHITE 7
#define BLACK     8

/* Simulated fill Hatching Patterns    1.04 */
#define EMPTY_HATCH        0
#define SOLID_HATCH        47


/* string justification for char_justification, def_char_justification */

#define LEFT     1
#define CENTER   2
#define RIGHT    3
#define BOTTOM   1
#define TOP      3
#define PATHRIGHT 1
#define PATHDOWN  2


/* Character spacing sizes */

#define THIN_GAP     0.2

#define SOLID_LINE          0
#define DOUBLE_DASHED_LINE  1

/* switches for set_visibility, et cetera */

#define OFF   0
#define ON    1

#define xVisible   1
#define xInvisible 0

/* Fill patterns for polygons, rectangles, circles et cetera */

#define HATCH 2
#define SOLID 1
#define EMPTY 0

// Font (not really used since text is drawn as lines and polylines.
#define SIMPLEX_BLOCK  1

// Used for flip/rotate.
const int MIRROR_X  = 1;
const int MIRROR_Y  = 2;
const int MIRROR_XY = 3;
const int ROTATE_IT = 4;


#endif
