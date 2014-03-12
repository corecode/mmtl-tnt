#ifndef tpdlistSortingFunctions_h
#define tpdlistSortingFunctions_h


#ifndef __alpha
#ifndef node_database_h
#include <node_database.h>
#endif
#endif
/////////////////////////////////////////////////////////////////////
// Functions for sorting a list and maintaining a sorted list
////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////
// sortedInsertion
//   add an item to a sorted {or null} list
//
// return value:
//  returns the numeric position of the insertion {numbering 
//  starting at 1\}
///////////////////////////////////////////////////////////////////

template <class Type> 
int sortedInsertion(Type *itemPtr,RWTPtrDlist<Type> &preSortedList);

/////////////////////////////////////////////////////////////////////
// bubbleSort
//
//  performs a bubble sort on a list, note: assumes the operator >
//  is defined for the Type to be sorted
////////////////////////////////////////////////////////////////////


template <class Type> void bubbleSort(RWTPtrDlist<Type> &unSortedList);  

#endif


