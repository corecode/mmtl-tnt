/***************************************************************************\
 * 									    *
 *   ROUTINE NAME    nmmtl_gen_netlist_from_icon			    *
 * 									    *
 *   ABSTRACT								    *
 *	Verifies that for every signal name XXXX in the object section	    *
 *	there exists two pins named XXXX-N (Near-end pin) and XXXX-F (Far-  *
 *	end) and then generates a netlist for the Icon.	If no Icon exists   *
 *	then a netlist is not generated.  This routine is used by new MMTL  *
 *	and is the same as electro_generate_netlist used by gendata except  *
 *	that the "struct contour" passed to each is different.		    *
 * 									    *
 *   CALLING FORMAT							    *
 *	status = nmmtl_gen_netlist_from_icon(netlist, signals, basefile)    *
 *									    *
 *   RETURN VALUE							    *
 *       SUCCESS    No errors occured (Netlist may or may not have been	    *
 *		    generated)						    *
 *	FAIL	    Error generating Netlist				    *
 * 									    *
 *   INPUT PARAMETERS							    *
 *	NETLIST_REC *netlist;	    icon netlist data			    *
 *      struct contour *signals;    list of x-section signals		    *
 *	char *basefile,		    base file name     			    *
 *									    *
 *   OUTPUT PARAMETERS							    *
 *	None								    *
 * 									    *
 *   USER FUNCTIONS & SYSTEM SERVICES CALLED				    *
 *	get_error							    *
 *	mcms_access							    *
 *	mcms_deaccess							    *
 *									    *
 *   AUTHOR          Jeffrey A. Prentice				    *
 * 									    *
 *   CREATION DATE       03/25/92					    *
 *									    *
 *        Copyright (C) 1992 by Mayo Foundation.  All rights reserved.	    *
 * 									    *
 \***************************************************************************/

#include "nmmtl.h"
#include <time.h>
#include <limits.h>
#include <string.h>

/************************************************************************
 *                                                                       *
 *   Error Message constants.                                            *
 *                                                                       *
 ************************************************************************/

int nmmtl_gen_netlist_from_icon(
NETLIST_REC	*netlist,	/* icon netlist data		*/
struct contour	*signals,	/* list of x-section signals	*/
char		*basefile     	/* base file name (no ext)	*/
)
{
  struct contour  *signal_ptr;    /* ptrs for traversing lists    */
  ATTRIBUTE_REC   *attr_ptr;	    
  PIN_REC	    *pin_ptr;	    
  int		    i,found,	    /* counter and flag		    */
  end;	                            /* current end of signal line   */
  time_t	    t;		    /* Holds time of day in seconds since   */
                                    /* January 1, 1970, 12:00 a.m.	    */
  char month[4], date[3], day[4], /* These strings hold values showing    */
  year[5], clock[9];         /* the date and time		    */
  
  FILE *net = NULL;		    /* basename.NETLIST			    */
  int type_exists;		    /* indicates if attr EM_TYPE is defined */
  char filename[PATH_MAX];

  /********************************************************************
   *									*
   * Check whether icon exists.  If not, do not generate a netlist.	*
   *									*
   ********************************************************************/
  if (netlist->exists == FALSE)
    return(SUCCESS);
  
  /********************************************************************
   *									*
   * Open the netlist file.						*
   *									*
   ********************************************************************/
  sprintf (filename, "%s.netlist", basefile);

  if ( (net = fopen (filename, "w")) == NULL )
  {
    printf ("**** Could not open the netlist file %s for write\n", filename);
    return(FAIL);  /* Fatal error; could not open .netlist file */
  }
  
  /********************************************************************
   *									*
   * Get date and time and output netlist header line.			*
   *									*
   ********************************************************************/
  t = time(NULL);
  sscanf( ctime(&t), "%s %s %s %s %s", day, month, date, clock, year);
  fprintf(net, "%s-%s-%s %s MMTL %s %s\n", date, month,
	  year, clock, "1.0", filename);
  
  /********************************************************************
   *									*
   * See if the icon attribute "EM_TYPE" has been defined.  If not	*
   * add attribute to list and assign a value of "tran_line".		*
   *									*
   ********************************************************************/
  for (attr_ptr = netlist->attributes, type_exists = FALSE;
       attr_ptr != NULL;
       attr_ptr = attr_ptr->next)
  {
    if (! strcmp(attr_ptr->name, "EM_TYPE")) 
    {
      type_exists = TRUE;
      break;
    }
  }
  
  if (! type_exists)
  {
    attr_ptr = (ATTRIBUTE_REC *)malloc(sizeof(ATTRIBUTE_REC));
    attr_ptr->name = strcpy(((char *)malloc(strlen("EM_TYPE") + 1)),"EM_TYPE");
    attr_ptr->value = strcpy(((char *)malloc(strlen("tran_line") + 1)), 
			     "tran_line");
    attr_ptr->next = netlist->attributes;
    netlist->attributes = attr_ptr;
    netlist->attribute_count++;
  }
  
  /********************************************************************
   *									*
   * Write the netlist index. It includes the number of references (0),*
   * global attributes, inputs (0), outputs (0), bidirectionals,	*
   * internal nets (0), and instances.					*
   *									*
   ********************************************************************/
  fprintf(net, "0 %d 0 0 %d 0 0\n", netlist->attribute_count, 
	  netlist->pin_count);
  
  /********************************************************************
   *									*
   * Output the global Icon attributes.				*
   *									*
   ********************************************************************/
  for (attr_ptr = netlist->attributes;
       attr_ptr != NULL;
       attr_ptr = attr_ptr->next)
  {
    fprintf(net, "%s \"%s\"\n", attr_ptr->name, attr_ptr->value);
  }
  
  /********************************************************************
   *									*
   * Output the bidirectional pins. Output the Near-end pins and then  *
   * the Far-end pins in order of the x-section signal names.		*
   *									*
   ********************************************************************/
  for (end = NEAR_END, i = 0; 
       i < 2;
       end = FAR_END, i++)
  {
    for (signal_ptr = signals; 
	 signal_ptr != NULL ; 
	 signal_ptr = signal_ptr->next)
    {
      for (pin_ptr = netlist->pins; 
	   pin_ptr != NULL; 
	   pin_ptr = pin_ptr->next)
      {
	/********************************************************
	 *							*
	 * Match the signal name portion of the pin name to the  *
	 * the signal name of the x-section.  If a match is	*
	 * found output this pin name and its attributes.	*
	 *							*
	 ********************************************************/
	found = FALSE;
	if ((! strcmp(signal_ptr->name, pin_ptr->sig_name))	&&
	    (pin_ptr->tl_end == end))
	{
	  found = TRUE;
	  fprintf(net, "%d %s\n", pin_ptr->attribute_count, 
		  pin_ptr->name);
	  for (attr_ptr = pin_ptr->attributes;
	       attr_ptr != NULL;
	       attr_ptr = attr_ptr->next)
	  {
	    fprintf(net, "%s \"%s\"\n", 
		    attr_ptr->name, 
		    attr_ptr->value);
	  }
	  break;
	}
      }
      /************************************************************
       *								*
       * No Near or Far-end pin name matches the current x-section *
       * name.  Output a message and return.			*
       *								*
       ************************************************************/
      if (! found)
      {
	printf ("No Near or Far-end pin name matches %s\n",
		signal_ptr->name);
	
	/********************************************************
	 *							*
	 * Deaccess and close the .NETLIST file.			*
	 *							*
	 ********************************************************/

	fclose(net);
	
	return(FAIL);
      }
    }
  }
  
  /********************************************************************
   *									*
   * Deaccess and close the .NETLIST file.				*
   *									*
   ********************************************************************/
  fclose(net);

  printf ("Netlist has been generated\n");
  return(SUCCESS);	    
}
