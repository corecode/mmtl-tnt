#include "dim.h"

/* ***********************************************************************
 * ROUTINE NAME dim2 
 *
 *
 * ABSTRACT  dynamic allocation of two dimesional array using pointer 
 *           deferencing
 *            
 *
 * ENVIRONMENT  ptr = (type **) dim2(int rows,int columns, unsigned size)
 *                       The return value will be null if there wasn't
 *                       enough memory
 *
 *
 * 
 * INPUTS  
 *    rows                          number of rows in the matrix
 *    columns                       number of columns in the matrix      
 *    size                          size in sizeof(data type) / sizeof(char)
 *                                  
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
 * 5.3      Fix for Ansi Compatability        AJS       7-14-91
 *                                                                 
 * ***********************************************************************
 */

void **dim2(int rows, int columns, unsigned size)

{
  int i;
  void **prow;
  char *pdata;
  
  pdata = (char *) calloc(rows * columns, size);
  prow = (void **) malloc(rows *  sizeof(void *));
  for (i=0; i < rows; i++)
    {
      prow[i] = pdata;
      pdata += columns * size;
    }
  return prow;
}
