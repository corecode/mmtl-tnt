
/*
  
  FACILITY:  NMMTL
  
  MODULE DESCRIPTION:
  
  Contains the functions:
  nmmtl_jacobian_d
  nmmtl_jacobian_c
  nmmtl_jacobian ( this is only called internally by the other two )
  
  AUTHOR(S):
  
  Kevin J. Buchs
  
  CREATION DATE:  Mon Apr  6 15:51:27 1992
  
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
static void nmmtl_jacobian(double local,
			   double xpts[],
			   double ypts[],
			   double *Jacobian);

/*
 *******************************************************************
 **  FUNCTION DEFINITIONS
 *******************************************************************
 */


/*
  
  FUNCTION NAME:  nmmtl_jacobian_d
  
  
  FUNCTIONAL DESCRIPTION:
  
  Calculate the jacobian for the transformation from global coordinates to 
  local coordinates for a dielectric element.
  
  Calls nmmtl_jacobian - which is more universal.
  
  FORMAL PARAMETERS:
  
  double       local       - the local coordinate to calcuate the Jacobian at
  DELEMENTS_P del         - pointer to the dielectric element structure
  double       *Jacobian   - the value of the jacobian returned.
  
  RETURN VALUE:
  
  None
  
  CALLING SEQUENCE:
  
  nmmtl_jacobian_d(local_coord,del,&Jacobian);
  */

void nmmtl_jacobian_d(double local,DELEMENTS_P del,double *Jacobian)
{
  nmmtl_jacobian(local,del->xpts,del->ypts,Jacobian);
}



/*
  
  FUNCTION NAME:  nmmtl_jacobian_c
  
  
  FUNCTIONAL DESCRIPTION:
  
  Calculate the jacobian for the transformation from global coordinates to 
  local coordinates for a conductor element.
  
  Calls nmmtl_jacobian - which is more universal.
  
  FORMAL PARAMETERS:
  
  double       local       - the local coordinate to calcuate the Jacobian at
  CELEMENTS_P cel         - pointer to the conductor element structure
  double       *Jacobian   - the value of the jacobian returned.
  
  RETURN VALUE:
  
  None
  
  CALLING SEQUENCE:
  
  nmmtl_jacobian_c(local_coord,cel,&Jacobian);
  */

void nmmtl_jacobian_c(double local,CELEMENTS_P cel,double *Jacobian)
{
  nmmtl_jacobian(local,cel->xpts,cel->ypts,Jacobian);
}




/*
  
  FUNCTION NAME:  nmmtl_jacobian
  
  
  FUNCTIONAL DESCRIPTION:
  
  The master universal routine to calculate the Jaboian.
  
  (Notes from FORTRAN sample program follow - the order of the
  points have changed)
  
  Calculating the Jacobian value for coordinate transform based on
  Lagrangian approximating polynomials.
  
  The idea here is that we have mapped an arbitrary curve in the global
  coordinate system (gcs) to a line segment between -1 and +1 in the local
  coordinate system (lcs).  The value of the differential length
  along the curve in gcs is dl and the corresponding length along the
  line segment in lcs is dm.  The relation among these is the Jacobian,
  where the Jacobian must be evaluated at a particular point in the lcs
  and is given by the symbol ajcob:
  
  dl(m) = ajcob(m) * dm(m)
  
  dl can be broken into dx and dy
  
  ajcob(m) = sqrt( (dx/dm(m))**2 + (dy/dm(m))**2 )
  
  This then gets brokken down into a series of basis functions
  and a sum over the basis functions - at node points.
  
  number of nodes per element is one more than basis_order.
  
  To calculate the derivative of the basis function, there will be
  ( 1 + basis order ) basis functions.
  
  L1 = 1 - m, L2 = m, L1' = -1, L2' = 1
  
  basis_order = 1, functions are  f1(m) = L1; f2(m) = L2
  derivatives    f1'(m) = -1;   f2'(m) = 1
  
  basis_order = 2, functions : f1(m) = L1(2*L1 - 1)
  f2(m) = L2(2*L2 - 1)
  f3(m) = 4*L1*L2
  
  derivatives: f1'(m) = -4*L1 + 1
  f2'(m) = 4*L2 - 1
  f3'(m) = 4*L1 - 4*L2
  
  basis_order = 3, f1(m) = 0.5*L1(3*L1 - 1)(3*L1 - 2)
  f2(m) = 0.5*L2(3*L2 - 1)(3*L2 - 2)
  f3(m) = 4.5*L1*L2(3*L1 - 1)
  f4(m) = 4.5*L2*L1(3*L2 - 1)
  
  
  FORMAL PARAMETERS:
  
  double       local       - the local coordinate to calcuate the Jacobian at
  double       xpts[]      - array of global x points
  double       ypts[]      - array of global y points
  double       *Jacobian   - the value of the jacobian returned.
  
  RETURN VALUE:
  
  None
  
  CALLING SEQUENCE:
  
  nmmtl_jacobian(local,cel->xpts,cel->ypts,Jacobian);   
  
  */

static void nmmtl_jacobian(double local,
			   double xpts[],
			   double ypts[],
			   double *Jacobian)
{
  register double delta_x,delta_y,L1,L2;
	double derivative[INTERP_PTS];
  register int i;
  
  /* short cuts, values of L1 and L2 at the point local */
  
  L1 = 1.0 - local;
  L2 = local;
  
  /* calculate the derivative of the basis function, there will be
     ( 1 + basis order ) basis functions. */
  
  /* - hard code the basis order = 2, here are some others for reference 
     
     if(basis_order .eq. 1) then
     
     derivative[0] = -1.
     derivative[1] = 1.
     
     if(basis_order .eq. 3) then
     
     derivative[0] = -0.5*( 27.*L1*L1 - 18.*L1 + 2. )
     derivative[1] =  4.5*( L1*(3.*L1 - 1.) - L2*(6.*L1 - 1.) )
     derivative[2] = -4.5*( L2*(3.*L2 - 1.) - L1*(6.*L2 - 1.) )
     derivative[3] =  0.5*( 27.*L2*L2 - 18.*L2 + 2. )
     
     */
  
  derivative[0] = -4.*L1 + 1.0;
  derivative[1] = 4.*( L1 - L2 );
  derivative[2] = 4.*L2 - 1.0;
  
  /* clear deltas */
  
  delta_x=0.0;
  delta_y=0.0;
  
  /* sum up the global coordinate times the derivative for each of the
     nodal points.  Ref Jackson notes, eqns 21a and 21b. */
  
  for(i=0;i < INTERP_PTS;i++)
  {
    delta_x = delta_x + derivative[i]*xpts[i];
    delta_y = delta_y + derivative[i]*ypts[i];
  }
  
  /* Jacobian is linear displacement - square root of sum of squares of
     delta_x and delta_y */
  
  *Jacobian = sqrt(delta_x*delta_x + delta_y*delta_y);
  
}


