#include "dim.h"

/* ***********************************************************************
 * ROUTINE NAME  free2  
 *
 *
 * ABSTRACT  frees a matrix created by dim2
 *           
 *            
 *
 * ENVIRONMENT  free(void **pa)
 *
 *
 *
 * 
 * INPUTS  
 *    pa                             the pointer to the row ptrs for the
 *                                     matrix      
 * OUTPUTS  
 *          
 *         
 * FUNCTIONS CALLED 
 *         
 *         
 *         
 *                                    
 * AUTHOR          Andrew Staniszewski
 *                                                        
 * MODIFICATION HISTORY                                   
 * 1.00     Original                          AJS       3-8-91   
 *                                                                 
 * ***********************************************************************
 */

void free2(void **pa)
{
  free(*pa);
  free(pa);
  return;
}
