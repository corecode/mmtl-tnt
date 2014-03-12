
/*
  
  FACILITY:  NMMTL
  
  MODULE DESCRIPTION:
  
  Contains the function nmmtl_retrieve
  
  AUTHOR(S):
  
  Kevin J. Buchs
  
  CREATION DATE:  Mon Jun  1 15:21:29 1992
  
  COPYRIGHT:   Copyright (C) 1992 by Mayo Foundation. All rights reserved.
  
  */


/*
 *******************************************************************
 **  INCLUDE FILES
 *******************************************************************
 */

#include "nmmtl.h"
#include <string.h>

/*
 *******************************************************************
 **  STRUCTURE DECLARATIONS AND TYPE DEFINTIONS
 *******************************************************************
 */
/*
 *******************************************************************
 **  MACRO DEFINITIONS
 *******************************************************************
 */
/*
 *******************************************************************
 **  PREPROCESSOR CONSTANTS
 *******************************************************************
 */
/*
 *******************************************************************
 **  GLOBALS
 *******************************************************************
 */
/*
 *******************************************************************
 **  FUNCTION DECLARATIONS
 *******************************************************************
 */
/*
 *******************************************************************
 **  FUNCTION DEFINITIONS
 *******************************************************************
 */


/*
  
  FUNCTION NAME:  nmmtl_retrieve
  
  
  FUNCTIONAL DESCRIPTION:
  
  Retreive data from a dump file of conductor and dielectric elements.
  
  FORMAL PARAMETERS:
  
  FILE *retrieve_file                - where to write dumpy things to.
  int *cntr_seg,                     - cseg parameter
  int *pln_seg,                      - dseg parameter
  float *coupling                    - coupling length
  float *risetime                    - risetime
  struct contour **psignals          - signals data structure
  int *pconductor_counter,           - how many conductors (gnd not included)
  CONDUCTOR_DATA_P *pconductor_data, - array of data on conductors
  DELEMENTS_P *pdie_elements         - list of dielectric elements        
  unsigned int *pnode_point_counter           - highest node number
  unsigned int *phighest_conductor_node       - highest node for a conductor
  
  RETURN VALUE:
  
  SUCCESS, or FAIL
  
  CALLING SEQUENCE:
  
  status = 
  nmmtl_retrieve(retrieve_file,conductor_counter,cd,*die_elements);
  
  */

int nmmtl_retrieve(FILE *retrieve_file,
			       int *cntr_seg,
			       int *pln_seg,
			       float *coupling,
			       float *risetime,
			       CONTOURS_P *psignals,
			       int *sig_cnt,
			       int *pconductor_counter,
			       CONDUCTOR_DATA_P *pconductor_data,
			       DELEMENTS_P *pdie_elements,
			       unsigned int *pnode_point_counter,
			       unsigned int *phighest_conductor_node)
{
  CELEMENTS_P ce;
  int cntr,i,edge0,edge1;
  char line[256];
  CONTOURS_P sigs;
  CONDUCTOR_DATA_P conductor_data;
  DELEMENTS_P die_elements;
  int temp[2];
  float ftemp[2];
  
  
  if(fscanf(retrieve_file,"%d %d %g %g\n",
	    &temp[0],&temp[1],&ftemp[0],&ftemp[1]) != 4) return(FAIL);
  *cntr_seg = temp[0];
  *pln_seg = temp[1];
  *coupling = ftemp[0];
  *risetime = ftemp[1];
  
  /* skip if null otherwise skip what comes after this section */
  if(psignals != NULL)
  {
    if(fgets(line,255,retrieve_file) == NULL) return(FAIL);
    i = 0;
    while(line[0] == '=')
    {
      if(*psignals == NULL)
      {
	sigs = (struct contour *)malloc(sizeof(struct contour));
	*psignals = sigs;
      }
      else
      {
	sigs->next = (struct contour *)malloc(sizeof(struct contour));
	sigs = sigs->next;
      }
      /* null out newline */
      if(strchr(line+1,'\n') != 0) *(strchr(line+1,'\n')) = '\0';
      
      strcpy(sigs->name,line+1);
      sigs->next = NULL;
      i++;
      if(fgets(line,255,retrieve_file) == NULL) return(FAIL);
    }
    *sig_cnt = i;
    return(SUCCESS);
  }
  else
  {
    if(fgets(line,255,retrieve_file) == NULL) return(FAIL);
    while(line[0] == '=')
      if(fgets(line,255,retrieve_file) == NULL) return(FAIL);
  }  
  
  if(fscanf(retrieve_file,"%d %d %d\n",pnode_point_counter,
	    phighest_conductor_node,
	    pconductor_counter) != 3)
  {
    return(FAIL);
  }
  
  
  *pconductor_data = (CONDUCTOR_DATA_P)malloc(
                     sizeof(CONDUCTOR_DATA)*(*pconductor_counter+1));
  conductor_data = *pconductor_data;
  
  for(cntr=0;cntr <= *pconductor_counter;cntr++)
  {
    if(fscanf(retrieve_file,"%d %d \n",
	      &conductor_data[cntr].node_start,
	      &conductor_data[cntr].node_end) != 2) return(FAIL);
    
    conductor_data[cntr].elements = NULL;
    if(fgets(line,255,retrieve_file) == NULL) return(FAIL);
    while(line[0] != '.')
    {
      if(conductor_data[cntr].elements == NULL)
      {
	ce = (CELEMENTS_P)malloc(sizeof(CELEMENTS));
	conductor_data[cntr].elements = ce;
      }
      else
      {
  
	ce->next = (CELEMENTS_P)malloc(sizeof(CELEMENTS));
	ce = ce->next;
      }
      
      ce->next = NULL;
      ce->edge[0] = NULL;
      ce->edge[1] = NULL;
      
      sscanf(line,"%f %d %d",&ce->epsilon,&edge0,&edge1);
      
      /* skip over first three fields */
      i = 0;
      while(line[i] != ' ') i++;
      i++;
      while(line[i] != ' ') i++;
      i++;
      while(line[i] != ' ') i++;
      i++;
      
      if(edge0)
      {
  
	ce->edge[0] = (EDGEDATA_P)malloc(sizeof(EDGEDATA));
	sscanf(&line[i],"%f %f",&ce->edge[0]->nu,
	       &ce->edge[0]->free_space_nu);
	/* skip these two fields */
	while(line[i] != ' ') i++;
	i++;
	while(line[i] != ' ') i++;
	i++;
      }
      if(edge1)
      {
  
	ce->edge[1] = (EDGEDATA_P)malloc(sizeof(EDGEDATA));
	sscanf(&line[i],"%f %f",&ce->edge[1]->nu,
	       &ce->edge[1]->free_space_nu);
	/* skip these two fields */
	while(line[i] != ' ') i++;
	i++;
	while(line[i] != ' ') i++;
	i++;
      }
      for(i = 0; i < 3; i++)
      {
	if(fscanf(retrieve_file,"%d %f %f\n",&ce->node[i],
		  &ce->xpts[i],&ce->ypts[i]) != 3) return(FAIL);
	
      }
      if(fgets(line,255,retrieve_file) == NULL) return(FAIL);
    }
  }
  
  
  if(fgets(line,255,retrieve_file) == NULL) return(FAIL);
  while(line[0] != '.')
  {
    if(*pdie_elements == NULL)
    {
      die_elements = (DELEMENTS_P)malloc(sizeof(DELEMENTS));
      *pdie_elements = die_elements;
    }
    else
    {
      die_elements->next = (DELEMENTS_P)malloc(sizeof(DELEMENTS));
      die_elements = die_elements->next;
    }
    
    die_elements->next = NULL;
    if(sscanf(line,"%f %f %f %f\n",
	      &die_elements->epsilonplus,&die_elements->epsilonminus,
	      &die_elements->normalx,&die_elements->normaly) != 4)
      return(FAIL);
    
    for(i = 0; i < 3; i++)
    {
      fscanf(retrieve_file,"%d %f %f\n",&die_elements->node[i],
	     &die_elements->xpts[i],&die_elements->ypts[i]);
    }
    if(fgets(line,255,retrieve_file) == NULL) return(FAIL);
  }
  return(SUCCESS);
}

