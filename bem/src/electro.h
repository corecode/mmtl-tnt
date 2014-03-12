#ifndef electro_h
#define electro_h

#ifndef magicad_h
#include "magicad.h"
#endif

#define LIGHT_SPEED	299792458
#define MU_0		M_PI*4.0e-7
#define EPS_0		1.0/(MU_0*LIGHT_SPEED*LIGHT_SPEED)
/**/
/* Boltzmann Constant, k, in J/K */
/**/
#define BoltzmannConstant 1.380622e-23
/**/
/* Planck Constant, hbar, in J sec */
/**/
#define PlanckConstant 1.0545726663e-34
/**/
/* Elementary Charge, e, in C */
/**/
#define ElementaryCharge 1.6021773349e-19


#ifndef electro_prototype_h
#include "electro_prototype.h"
#endif


#endif

