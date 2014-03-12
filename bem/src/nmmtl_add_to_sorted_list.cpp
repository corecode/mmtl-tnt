
/*
  
  FACILITY:  nmmtl
  
  MODULE DESCRIPTION:
  
  Contains nmmtl_add_to_sorted_list()
  
  AUTHOR(S):
  
  Kevin J. Buchs
  
  CREATION DATE:  Thu Mar 12 09:54:14 1992
  
  COPYRIGHT:   Copyright (C) 1992 by Mayo Foundation. All rights reserved.
  
  */


/*
 *******************************************************************
 **  INCLUDE FILES
 *******************************************************************
 */

#include "nmmtl.h"

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

/* if testing this module - make the link simple */
#ifdef TEST_ATSL
#undef malloc
#endif


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
  
  FUNCTION NAME:  nmmtl_add_to_sorted_list
  
  
  FUNCTIONAL DESCRIPTION:
  
  Add a new keyed item of data to a sorted linked list.  The list consists
  of structures of FLK_KEY_LIST, keyed by a double value, and with an optional
  pointer to additional data given by a char *.
  
  FORMAL PARAMETERS:
  
  FLT_KEY_LIST_P *list  - pointer to the sorted list - may need to be created
  double key            - the double key used to sort on
  char *data            - optional additional data, pass NULL if you don'tcare.
  
  RETURN VALUE:
  
  None
  
  CALLING SEQUENCE:
  
  nmmtl_add_to_sorted_list(FLT_KEY_LIST_P *list,double key,char *data);
  */

void nmmtl_add_to_sorted_list(FLT_KEY_LIST_P *list,double key,char *data)
{
  FLT_KEY_LIST_P thelist,newlist;
  
  /* is this an empty list ? */
  if(*list == NULL)
  {
    /* it is, just make this the first element on the list */
    newlist = (FLT_KEY_LIST_P)malloc(sizeof(FLT_KEY_LIST));
    newlist->key = key;
    newlist->data = data;
    newlist->next = (FLT_KEY_LIST_P)NULL; 
    *list = newlist;
    return;
  }
  
  /* else list exists, will search and add entry if doesn't already exist */
  else 
  {
    thelist = *list;
    
    /* check if it belongs at the start of the list */
    if(thelist->key > key)
    {
      /* it does, just make this the first element on the list */
      newlist = (FLT_KEY_LIST_P)malloc(sizeof(FLT_KEY_LIST));
      newlist->key = key;
      newlist->data = data;
      newlist->next = thelist; 
      *list = newlist;
      return;
    }
    
    /* Does it match the first element? */
    if(thelist->key == key) return;
    
    /* Now, traverse the list and determine if it belongs between thelist
       element and the next one */
    
    while(thelist->next != NULL)
    {
      /* check if it belongs between */
      if(thelist->next->key > key)
      {
	/* it does */
	newlist = (FLT_KEY_LIST_P)malloc(sizeof(FLT_KEY_LIST));
	newlist->key = key;
	newlist->data = data;
	newlist->next = thelist->next; 
	thelist->next = newlist;
	return;
      }
      
      /* check if it matched the next */
      if(thelist->next->key == key) return;
      
      thelist = thelist->next;
    }
    
    /* It you have reached this point, you need to add on to the list */  
    
    newlist = (FLT_KEY_LIST_P)malloc(sizeof(FLT_KEY_LIST));
    newlist->key = key;
    newlist->data = data;
    newlist->next = (FLT_KEY_LIST_P)NULL;
    thelist->next = newlist;
    return;
  }
}


/* 
  
  A built in Self Test function for nmmtl_add_to_sorted_list.
  You should be able to compile this module alone with TEST_ATSL defined
  and run the resulting executable.  Good luck!
  
  */

#ifdef TEST_ATSL

main()
{
  FLT_KEY_LIST_P list,test;
  
#define ADD_N_PRINT(KEY) \
  printf("adding %f\n",KEY);\
    nmmtl_add_to_sorted_list(&list,KEY,(char *)NULL);\
      printf("the list: ");\
	for(test = list;test != NULL;test = test->next)\
	{printf("%f,",test->key);}\
	  printf("\n\n");
  
  list = NULL;
  ADD_N_PRINT(1.0);
  ADD_N_PRINT(1.0);
  ADD_N_PRINT(2.0);
  ADD_N_PRINT(3.0);
  ADD_N_PRINT(1.5);
  ADD_N_PRINT(2.6);
  ADD_N_PRINT(3.1);
  ADD_N_PRINT(0.5);
  list = NULL;
  ADD_N_PRINT(1.0);
  ADD_N_PRINT(0.5);
  ADD_N_PRINT(0.4);
  ADD_N_PRINT(0.3);
  ADD_N_PRINT(0.2);
}

#endif
