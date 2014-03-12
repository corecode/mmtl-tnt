/****************************************************************************\
*                                                                            *
*  ROUTINE NAME                 electro_gendata_netlist.h		     *
*                                                                            *
*  ABSTRACT                                                                  *
*       Definitions for the GENDATA to read the icon section of the	     *
*	graphic file and generate a netilist.				     *
*                                                                            *
*  CALLING FORMAT                                                            *
*       #include "electro_gendata_netlist.h"				     *
*                                                                            *
*  CALLED BY                                                                 *
*    electro_gendata_2dlf.c						     *
*    electro_gendata_2dly.c						     *
*                                                                            *
*  AUTHOR               Jeff Prentice                                        *
*                                                                            *
*  CREATION DATE        21-Feb-1991					     *
*                                                                            *
*       Copyright (C) 1989 by Mayo Foundation.  All rights reserved.         *
*                                                                            *
\****************************************************************************/

#ifndef electro_gendata_netlist_h
#define electro_gendata_netlist_h

/************************************************
*						*
* Constants.					*
*						*
************************************************/
#define NEAR_END 0
#define FAR_END 1
    

/* forward references for C++ */ 
struct attribute;
struct pin;

/************************************************
*						*
* Attribute data structure.			*
*						*
************************************************/
typedef struct attribute {
    char		*name;	/* attribute name   */
    char		*value;	/* attribute value  */
    struct attribute	*next;
} ATTRIBUTE_REC;

/************************************************
*						*
* Pin data structure.	    			*
*						*
************************************************/
typedef struct pin {
    char                *name;              /* pin name			    */
    char		*iotype;	    /* pin iotype		    */
    char		*sig_name;	    /* signal part of the pin name  */
    struct attribute    *attributes;        /* list of pin attributes	    */
    struct pin          *next;
    int			tl_end;		    /* end part of pin name	    */
    int                 attribute_count;    /* # of pin attributes	    */
} PIN_REC;

/************************************************
*						*
* Netlist data structure			*
*						*
************************************************/
typedef struct netlist {
    struct attribute	*attributes;	    /* list of icon attributes	    */
    struct pin		*pins;		    /* list of icon pins	    */
    int			exists;		    /* indicates if the icon exists */
    int			attribute_count;    /* # of icon attributes	    */
    int			pin_count;	    /* # of pins in the icon	    */
} NETLIST_REC;

#endif 



