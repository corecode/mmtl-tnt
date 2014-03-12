/***************************************************************************\
 * 									    *
 *   ROUTINE NAME    electro_read_icon_section				    *
 * 									    *
 *   ABSTRACT								    *
 *	Reads the pertinent information from the Icon section of the	    *
 *	graphic file and places it in a structure.  Also verifies that	    *
 *	all pins have names and the IOTYPE of bidirectional. If the Icon    *
 *	does not exist, a flag is set in the netlist structure, a warning   *
 *	is given and gendata continues.					    *
 * 									    *
 *   CALLING FORMAT							    *
 *	status = electro_read_icon_section(fp, netlist, err)		    *
 *									    *
 *   RETURN VALUE							    *
 *      SUCCESS     No errors in Icon section (Icon may or may not exist)   *
 *	FAIL	    Error reading Icon or invalid Icon			    *
 * 									    *
 *   INPUT PARAMETERS							    *
 *	FILE *fp    graphic input file ptr				    *
 *	FILE *err   error output file ptr				    *
 *									    *
 *   OUTPUT PARAMETERS							    *
 *	NETLIST_REC *netlist	 netlist structure			    *
 * 									    *
 *   USER FUNCTIONS & SYSTEM SERVICES CALLED				    *
 *	get_error							    *
 *									    *
 *   AUTHOR          Jeffrey A. Prentice				    *
 * 									    *
 *   CREATION DATE       02/21/91					    *
 *									    *
 *        Copyright (C) 1991 by Mayo Foundation.  All rights reserved.	    *
 * 									    *
 \***************************************************************************/

#include "magicad.h"
#include "electro_gendata_netlist.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

/************************************************************************
 *                                                                       *
 *   Error Message constants.                                            *
 *                                                                       *
 ************************************************************************/

int electro_read_icon_section(FILE *fp, NETLIST_REC *netlist, FILE *err)
/* FILE *fp;		 graphic input file	*/
/* NETLIST_REC *netlist; netlist structure	*/
/* FILE *err;		 error output file	*/
{
  char    line[GPGE_MAX],
  attr_name[SIZE_ATTR_NAME],	/* attribute name		    */	
  attr_value[SIZE_ATTR_VAL],	/* attribute value		    */
  sig_name[SIZE_ATTR_NAME],	/* signal name part of pin name	    */
  tl_end[SIZE_ATTR_NAME];	/* Near, Far-end part of pin name   */
  
  ATTRIBUTE_REC   *attr_ptr;		
  PIN_REC	    *pin_ptr;
  int	    type_exists;	 /* indicates if attr EM_TYPE assigned */
  
  /********************************************************************
   *									*
   * Set flag indicating icon exists.  Will be changed if it is	        *
   * determined that it does not.					*
   *									*
   ********************************************************************/
  netlist->exists = TRUE;
  
  /********************************************************************
   *									*
   * Find the Icon attribute section.					*
   *									*
   ********************************************************************/
  while (strncmp(fgets(line, GPGE_MAX, fp), "\t\t$IATT;", 8)) ;
  
  /********************************************************************
   *									*
   * Read the Icon attributes.						*
   *									*
   ********************************************************************/
  for (netlist->attribute_count = 0, type_exists = FALSE;
       strncmp(fgets(line, GPGE_MAX, fp), "\t\t$END;", 7);
       netlist->attribute_count++)
  {
    attr_name[0] = '\0';
    attr_value[0] = '\0';
    sscanf(line, " \"%[^\"]\" (%*f , %*f) \"%[^\"]\" ", attr_name,
	   attr_value);
    attr_ptr = (ATTRIBUTE_REC *) malloc(sizeof(ATTRIBUTE_REC));
    attr_ptr->name = strcpy(((char *) malloc(strlen(attr_name) + 1)), attr_name);
    attr_ptr->value = strcpy(((char *) malloc(strlen(attr_value) + 1)), attr_value);
    attr_ptr->next = netlist->attributes;
    netlist->attributes = attr_ptr;
    
    if (! strcmp(attr_name, "EM_TYPE")) type_exists = TRUE;
  }
  if (! type_exists)
  {
    attr_ptr = (ATTRIBUTE_REC *) malloc(sizeof(ATTRIBUTE_REC));
    attr_ptr->name =strcpy(((char *) malloc(strlen("EM_TYPE") + 1)), "EM_TYPE");
    attr_ptr->value = strcpy(((char *) malloc(strlen("tran_line") + 1)), 
			     "tran_line");
    attr_ptr->next = netlist->attributes;
    netlist->attributes = attr_ptr;
    netlist->attribute_count++;
  }
  
  /********************************************************************
   *									*
   * Find the Pin attribute section.					*
   *									*
   ********************************************************************/
  while (strncmp(fgets(line, GPGE_MAX, fp), "$PINS;", 6)) ;
  
  /********************************************************************
   *									*
   * Read the pins if there are any.  Continue until the "$END;"	*
   * record is found.							*
   *									*
   ********************************************************************/
  netlist->pin_count = 0;
  while (strncmp(fgets(line, GPGE_MAX, fp), "$END;", 5))
  {
    /****************************************************************
     *								     *
     * Read the pin attributes.  The attributes "NAME" and "IOTYPE"  *
     * must be assigned.					     *
     *								     *
     ****************************************************************/
    if (! strncmp(line, "\t\t$ATT;", 7))
    {
      pin_ptr = (PIN_REC *) malloc(sizeof(PIN_REC));
      pin_ptr->attribute_count = 0;
      pin_ptr->attributes = NULL;
      pin_ptr->name = NULL;
      pin_ptr->iotype = NULL;
      pin_ptr->sig_name = NULL;
      pin_ptr->tl_end = NEAR_END;
      pin_ptr->next = netlist->pins;
      netlist->pins = pin_ptr;
      
      while (strncmp(fgets(line, GPGE_MAX, fp), "\t\t$END;", 7))
      {
	attr_name[0] = '\0';
	attr_value[0] = '\0';
	sscanf(line, " \"%[^\"]\" (%*f , %*f) \"%[^\"]\" ", 
	       attr_name, attr_value);
	/********************************************************
	 *							 *
	 * The attribute "NAME" should be of the form: XXXXX-Z   *
	 * where XXXXX matches a signal line name in the object  *
	 * section and Z is N for Near-end or F for Far-end.     *
	 * So, parse this attribute.				 *
	 *							 *
	 ********************************************************/
	if (! strcmp(attr_name, "NAME"))
	{
	  pin_ptr->name = strcpy(((char *) malloc(strlen(attr_value) + 1)), 
				 attr_value);
	  sig_name[0] = '\0';
	  tl_end[0] = '\0';		
	  sscanf(attr_value, "%[^-]-%s", sig_name, tl_end);
	  pin_ptr->sig_name = strcpy(((char *) malloc(strlen(sig_name) + 1)), 
				     sig_name);
	  if ( toupper(*tl_end) == 'N' )
	    pin_ptr->tl_end = NEAR_END;
	  else if ( toupper(*tl_end) == 'F' )
	    pin_ptr->tl_end = FAR_END;
	  else
	  {
	    printf ("**** Bad pin-name: %s\n", pin_ptr->name);
	    return(FAIL);
	  }		    
	}
	else
	  /********************************************************
	   *							   *
	   * The attribute is "IOTYPE".  Verify that it is	   *
	   * bidirectional ("b").				   *
	   *							   *
	   ********************************************************/
	  if (! strcmp(attr_name, "IOTYPE"))
	  {
	    if (! strcmp(attr_value, "b"))
	    {
	      pin_ptr->iotype =strcpy(((char *) malloc(strlen(attr_value) + 1))
				      , attr_value);
	    }
	    else
	    {
	      printf ("**** Bad pin-type: %s\n", attr_value);
	      return(FAIL);
	    }
	  }
	
	/********************************************************
	 *							*
	 * The attribute is something else.  Add to the pin	*
	 * attribute list.					*
	 *							*
	 ********************************************************/
	  else
	  {
	    attr_ptr = (ATTRIBUTE_REC *) malloc(sizeof(ATTRIBUTE_REC));
	    attr_ptr->name = strcpy(((char *) malloc(strlen(attr_name) + 1)), 
					     attr_name);
	    attr_ptr->value = strcpy(((char *) malloc(strlen(attr_value) + 1)),
				     attr_value);
	    attr_ptr->next = pin_ptr->attributes;
	    pin_ptr->attributes = attr_ptr;
	    pin_ptr->attribute_count++;
	  }
      }        
      
      /************************************************************
       *							   *
       * If the attributes "NAME" and "IOTYPE" were not specified  *
       * output error message and return.			   *
       *							   *
       ************************************************************/
      if (pin_ptr->name == NULL) 
      {
	printf ("**** No pin-name!");
	return(FAIL);
      }
      else if (pin_ptr->iotype == NULL)
      {
	printf ("**** No io-type for pin %s\n", pin_ptr->name);
	return(FAIL);
      } 
      netlist->pin_count++;    
    }
  }
  
  /********************************************************************
   *								       *
   * If no pins were found then for all practical purposes the Icon    *
   * does not exist.  Set the flag and continue accordingly.	       *
   *								       *
   ********************************************************************/
  if (netlist->pin_count == 0)
  {
    netlist->exists = FALSE;
    printf ("No ICON section defined!\n");
  }
  return( SUCCESS);
}	











