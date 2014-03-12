#ifndef dim_h
#define dim_h

#ifndef magicad_h
#include "magicad.h"
#endif

#include <stdio.h>
#include <stdlib.h>

void **dim2(int rows, int columns, unsigned size);

void free2(void **pa);

void ***dim3(int grid, int rows, int columns, unsigned size);

void free3(void ***pa);

#endif
