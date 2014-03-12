/***************************************************************************\
* 									    *
*   ROUTINE NAME	graphic_output.h				    *
* 									    *
*   ABSTRACT	This include file Holds the structure and Macros for the    *
* 		graphic_output.						    *
* 									    *
* 									    *
*   AUTHOR          Cary Bates						    *
* 									    *
*   CREATION DATE	28-AUG-1986					    *
* 									    *
*        Copyright (C) 1986 by Mayo Foundation.  All rights reserved.	    *
*        Copyright (C) 1989 by Mayo Foundation.  All rights reserved.	    *
* 									    *
*   MODIFICATION HISTORY						    *
*      1.00 Original                                         CLB  8-28-86   *
* 									    *
\***************************************************************************/

#ifndef graphic_output_h
#define graphic_output_h

typedef struct graphic_text_window
{
    int    name;    /* Name of graphical I/O window */
    float  vxmin, vxmax, vymin, vymax;  /* Virtual window boundaries */
    float  wxmin, wxmax, wymin, wymax;  /* World window boundaries */
    float  left_margin,   /*  These margins delimit where the text will  */ 
           right_margin,  /*  be drawn within the I/O window. They are   */
           top_margin,    /*  specified in world coordinates.            */
           bottom_margin; 
    float  char_height,   /* These define the height and width of a          */
           char_width;    /* character in world coordinates.                 */
    float  char_gap;      /* Defines the amount of space between charac-     */
                          /* ters as   space =  char_gap * char_width.       */
    float  line_gap;      /* Defines the amount of space between lines       */
                          /* as  space = line_gap * char_height.             */
    float  aspect_ratio;  /* The aspect ratio of the device                  */
    int    border_color;  /* Color of the border around the I/O window       */
    int    text_color;    /* Color of the text within the I/O window         */
    int    background_color; /* Background Color within the I/O window       */
    int    cursor_color;  /* The color to draw the cursor.                   */
    char   text_type;     /* Text precision level to use. see GRCONS.H       */
    char   font;          /* Text font to use. see GRCONS.H                  */
    char   mode_flag;     /* Operational mode of window. Discussed below     */
    char   *log_file;     /* name of optional file that text can be sent     */
                          /* to as the screen. If null then ignored.         */
    int    (*ret_funct)(const char *,...);/* Output function to be used if a retained        */
                          /* segment is open                                 */
    int    (*redraw_funct)(); /* function for redrawing the graphic screen.  */
    struct graphic_text_window *next; /* points to next window in global list*/
                          /*-------------------------------------------------
			   * The following variable are used internally by
			   * the graphical I/O routines. These variables must 
                           * be set to the value designated by the '[]'
			   *-------------------------------------------------*/
    int    line_count;    /* Current line being written to in window. [0]    */
    float  locx,locy;     /* Current character position being written to in
			     window.   [left_margin]                         */
    int    stop_count;    /* Controls when confirming is done. [0]           */
    char   **(*lines);   /* memory to hold the text that is currently in
			     the I/O window.  [0]                            */
    char   test_file;     /* for opening the logfile                         */
} GRAPHIC_TEXT_WINDOW;

/* These constants define how the window is to operate. The flags operate
   independently of one another and can be combined to produce many
   different modes of operation. For example, If you want your output
   window to scroll, yet you want it to prompt you at every pagefull, then
   you would set the flags GRAOUT_SCROLL and GRAOUT_CONFIRM. If you also wanted
   consecutive output calls to resume where the last one left off, then
   you would set the GRAOUT_CONTINUE flag also like this:
       abc = malloc(sizeof(struct graphic_text_window));
             ...
       abc->mode_flag = GRAOUT_SCROLL | GRAOUT_CONFIRM | GRAOUT_CONTINUE;
   */
       
#define GRAOUT_SCROLL 0x0001          /* Causes window to scroll when full   */
#define GRAOUT_CLEAR  0x0002          /* Window is erased before each call   */
#define GRAOUT_CONFIRM 0x0004         /* User hits return before info is lost*/
#define GRAOUT_IGNORE_NEWLINES 0x0008 /* Newline chars are ignored           */
#define GRAOUT_CONTINUE 0x0010        /* Output continues where last line
				         left off.                           */



#define _DEFINE_GRAOUT_STRUCT( str ) struct graphic_text_window str;


#define _LOAD_GRAOUT_FEEDBACK(str)\
 str.name    =  1;\
 str.vxmin   =  -0.5;\
 str.vxmax   =  1.0;\
 str.vymin   =  -1.0;\
 str.vymax   =  -0.8125;\
 str.wxmin   =   0.0;\
 str.wxmax   =  12.0;\
 str.wymin   =   0.0;\
 str.wymax   =   1.0;\
 str.left_margin   =  0.1;\
 str.right_margin  =  11.9;\
 str.top_margin    =  0.99;\
 str.bottom_margin =  0.01;\
 str.char_height   =  0.21;\
 str.char_width   =  0.2;\
 str.char_gap =  0.0;\
 str.line_gap =  0.5;\
 str.border_color      =   GREEN;\
 str.text_color        =   WHITE;\
 str.cursor_color      =   GREEN;\
 str.background_color  =   BLUE;\
 str.text_type  =  2;\
 str.font =  1;\
 str.mode_flag =  (GRAOUT_CLEAR | GRAOUT_SCROLL | GRAOUT_CONTINUE);\
 str.log_file =  0;\
 str.ret_funct =  printf;\
 str.next =  0;\
 str.line_count  =  0;\
 str.locx   =  0.0;\
 str.locy   =  0.0;\
 str.stop_count = 0;\
 str.lines = 0;

#endif
