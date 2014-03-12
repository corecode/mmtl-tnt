
/*
  
  FACILITY:  NMMTL
  
  MODULE DESCRIPTION:
  
  Contains these functions:
  
  nmmtl_shape_c_edge  -  for conductor elements when edge effects are
  to be considered.
  nmmtl_shape         -  for conductor elements in other cases and for 
  dielectric elements all the time.
  
  AUTHOR(S):
  
  Kevin J. Buchs
  
  CREATION DATE:  Mon Apr  6 16:24:09 1992
  
  COPYRIGHT:   Copyright (C) 1992 by Mayo Foundation. All rights reserved.
  
  */


/*
 *******************************************************************
 **  INCLUDE FILES
 *******************************************************************
 */

#include "nmmtl.h"

/* right now this module is hardcoded for 3 intepolation points.  
   Since I am lazy - put this in to remind you to change the code if the
   number of interpolation points INTERP_PTS changes */

#if INTERP_PTS != 3
COMPILATION ERROR ERROR ---      INTERP_PTS != 3       --- ERROR ERROR
#endif
  
  
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
    
    FUNCTION NAME:  nmmtl_shape_c_edge
    
    
    FUNCTIONAL DESCRIPTION:
    
    Calculates the shape function for the conductor element by calling
    nmmtl_shape and then additional processing is performed for the 
    edge adjustments.  This additional processing calculates an exponential
    based factor to be multiplied with the regular shape function.  Two
    broad sections handle the edge being on either side.  If on both sides,
    then both sections are processed.
    
    FORMAL PARAMETERS:
    
    double point         - the local coordinate point at which to evaluate
    the shape function
    double *shape        - the coeficients of the shape function
    CELEMENTS_P cel     - pointer to the conductor element
    float nu0,nu1       - the nu value to use for edge[0] and edge[1]
    different for free space
    
    RETURN VALUE:
    
    None
    
    CALLING SEQUENCE:
    
    nmmtl_shape_c_edge(Legendre_root_a[Legendre_counter],
    shape,cel,nu0,nu1);
    
    */
  
  void nmmtl_shape_c_edge(double point,
			  double *shape,
			  CELEMENTS_P cel,
			  float nu0,
			  float nu1)
{
  register int i;
  register double X,Y; /* interpolated points */
  register double numerator,denominator;
  register double deltax,deltay,factor;
  
  
  /* first get the ordinary shape function */
  
  nmmtl_shape(point,shape);
  
  /* then process the edge effects:
     
     We need to include the edge effects and use the parameter
     nu to modify the shape function (the basis function values).
     This does some sort of differential type operation - raising the
     difference between two lengths to the vth power and multiplying
     shp(i) times that.  These lengths are based on the global coordinate
     system.  This sort of allows scaling for the dimensions of the 
     element.  
     
     If we assume that the [0] end of the conductor element is the 
     edge, then the basis functions become modified as below:
     
     | p - p0  | (nu - 1)
     'N0  =  ------------         *  N0
     | p2 - p0 |
     
     | p - p0  | (nu - 1)
     'N1  =  ------------         *  N1
     | p1 - p0 |
     
     | p - p0  | (nu - 1)
     'N2  =  ------------         *  N2
     | p2 - p0 |
     
     Where:
     
     The node points are 1 ([0] end), 2 (middle), 3 ([1] end).
     N1, N2, N3 are plain shape functions.
     'N1, 'N2, 'N3 are modified shape functions.
     p is displacement in global coordinates to point the shape
     function is to be evaluated.
     p1,p2,p3 are displacements to the node points.
     nu is the constant precalculated for a particular edge 
     geometry.
     || are displacements, and it is raised to the (nu - 1) power
     
     If we have both ends as edges, then the transformations on the N's
     listed above are done twice with p0 and p2 switched the second
     time.
     
     */
  
  
  /* Execute this section if the [0] end of the element is an edge */
  
  if(cel->edge[0] != NULL)
  {
    
    X = 0.0;
    Y = 0.0;
    
    /* numerator is  p - p0.  First find p by interpolation.  We need to
       work in x,y coordinates */
    
    /* compute interpolation for x and y */
    
    for(i = 0; i < INTERP_PTS; i++)
    {
      X += shape[i] * cel->xpts[i];
      Y += shape[i] * cel->ypts[i];
    }
    
    /* Now subtract the p0 value */
    
    X -= cel->xpts[0];
    Y -= cel->ypts[0];
    
    /* now find the vector displacement */
    
    numerator=sqrt(X*X+Y*Y);
    /* handle the unusual case where it goes to zero - but we don't 
       want it to.  

       This limit used to be 1e-5, but was found to be
       too large, so it was reduced to 1e-20.  This is probably not the
       right way to do this - we should check the magnitude of the 
       numerator/denominator and fix that at some minimum.  I can't
       remember how 1e-5 was chosen.
            Thu Nov 23 05:32:55 1995 -- Kevin Buchs 
    */

    if(numerator < 1.0e-20) numerator = 1.0e-20;
    
    
    /* Calculate the denominator and compute the effect on the shape 
       functions.
       
       The denominator is  pi - p0 except for i=0, where it is p2 - p0.
       */
    
    /*   Hard code this loop expanded for three points ***    
     *
     *    for(i = 0; i < INTERP_PTS; i++)
     *    {
     *      if(i == 0)
     *      {
     *	/ * vector displacement of p2 - p0  * /
     *	
     *	denominator = sqrt(pow( (cel->xpts[2] - cel->xpts[0]), 2 ) +
     *			   pow( (cel->ypts[2] - cel->ypts[0]), 2 ));
     *      }
     *      else
     *      {
     *	/ * vector displacement of pi - p0 * /
     *	
     *	denominator = sqrt(pow( (cel->xpts[i] - cel->xpts[0]), 2 ) +
     *			   pow( (cel->ypts[i] - cel->ypts[0]), 2 ));
     *      }
     *      
     *
     *  
     *      
     *      / * actually modify the shape function here with the exponential
     *	 factor * /
     *      
     *      shape[i] *= pow( (numerator/denominator), (nu0 - 1.0) );
     *    }
     *
     *****/
    
    /* i = 0 */
    deltax = cel->xpts[2] - cel->xpts[0];
    deltay = cel->ypts[2] - cel->ypts[0];
    denominator = sqrt(deltax*deltax + deltay*deltay);
    factor = pow( (numerator/denominator), (nu0 - 1.0) );
    shape[0] *= factor;
    /* i = 2 */
    shape[2] *= factor;
    /* i = 1 */
    deltax = cel->xpts[1] - cel->xpts[0];
    deltay = cel->ypts[1] - cel->ypts[0];
    denominator = sqrt(deltax*deltax + deltay*deltay);
    shape[1] *= pow( (numerator/denominator), (nu0 - 1.0) );
    
    
  }
  
  /* Execute this section if the [1] end of the element is an edge */
  
  if(cel->edge[1] != NULL)
  {
    
    /* numerator is  p - p2.  First find p by interpolation.  We need to
       work in x,y coordinates */
    /* compute interpolation for x and y */
    
    X = 0.0;
    Y = 0.0;
    
    for(i = 0; i < INTERP_PTS; i++)
    {
      X += shape[i] * cel->xpts[i];
      Y += shape[i] * cel->ypts[i];
    }
    
    /* Now subtract the p2 value */
    
    X -= cel->xpts[2];
    Y -= cel->ypts[2];
    
    /* now find the vector displacement */
    
    numerator=sqrt(X*X+Y*Y);

    /* handle the unusual case where it goes to zero - but we don't 
       want it to.

       This limit used to be 1e-5, but was found to be
       too large, so it was reduced to 1e-20.  This is probably not the
       right way to do this - we should check the magnitude of the 
       numerator/denominator and fix that at some minimum.  I can't
       remember how 1e-5 was chosen.
          Thu Nov 23 05:38:44 1995 -- Kevin Buchs
    */

    if(numerator < 1.0e-20) numerator = 1.0e-20;
    
    
    /* Calculate the denominator and compute the effect on the shape 
       functions.
       
       The denominator is  pi - p2 except for i=2, where it is p0 - p2. */
    
    /* Hard code this loop expanded for three points
     *
     *    for(i = 0; i < INTERP_PTS; i++)
     *    {
     *      if(i == 2)
     *      {
     *	/ * vector displacement of p0 - p2 * /
     *	
     *	denominator = sqrt(pow( (cel->xpts[0] - cel->xpts[2]), 2 ) +
     *			   pow( (cel->ypts[0] - cel->ypts[2]), 2 ));
     *      }
     *      else
     *      {
     *	/ * vector displacement of pi - p2 * /
     *	
     *	denominator = sqrt(pow( (cel->xpts[i] - cel->xpts[2]), 2 ) +
     *			   pow( (cel->ypts[i] - cel->ypts[2]), 2 ));
     *      }
     *      
     *      / * actually modify the shape function here with the exponential
     *	 factor * /
     *      
     *      shape[i] *= pow( (numerator/denominator), (nu1 - 1.0) );
     *    }
     *******/
    
    /* i = 0 */
    deltax = cel->xpts[0] - cel->xpts[2];
    deltay = cel->ypts[0] - cel->ypts[2];
    denominator = sqrt(deltax*deltax + deltay*deltay);
    factor = pow( (numerator/denominator), (nu0 - 1.0) );
    shape[0] *= factor;
    /* i = 2 */
    shape[2] *= factor;
    /* i = 1 */
    deltax = cel->xpts[1] - cel->xpts[2];
    deltay = cel->ypts[1] - cel->ypts[2];
    denominator = sqrt(deltax*deltax + deltay*deltay);
    // the following should contain nu1 not nu0
    shape[1] *= pow( (numerator/denominator), (nu0 - 1.0) );
    
  }
}



/*
  
  FUNCTION NAME:  nmmtl_shape
  
  
  FUNCTIONAL DESCRIPTION:
  
  Calculate the coeficients for the shape (basis/interpolation) polynomials
  at the given point.
  
  This is called for dielectric elements in all cases.  It is called for
  conductor elements when edge effects are not to be considered.  It is called
  indirectly when edge effects are to be considered via the function
  nmmtl_shape_c_edge.
  
  This should be optimized in many cases to be a simple table lookup.
  
  (Notes from FORTRAN sample program follow - the order of the
  points have changed)
  
  Calculating the shape function in local coordinates.  What this means
  is calculating the value the basis functions.  This is
  quite simple with the basis functions being the Lagrangian Interpolation
  functions.  To use these you just need to match a coeficient to
  a corresponding node point.
  
  number of nodes per element is one more than basis_order.
  
  The Lagrangian polynomials
  
  L1 = 1 - m, L2 = m, L1' = -1, L2' = 1
  
  basis_order = 1, functions are  f1(m) = L1; f2(m) = L2
  
  basis_order = 2, functions : f1(m) = L1(2*L1 - 1)
  f2(m) = L2(2*L2 - 1)
  f3(m) = 4*L1*L2
  
  basis_order = 3, f1(m) = 0.5*L1(3*L1 - 1)(3*L1 - 2)
  f2(m) = 0.5*L2(3*L2 - 1)(3*L2 - 2)
  f3(m) = 4.5*L1*L2(3*L1 - 1)
  f4(m) = 4.5*L2*L1(3*L2 - 1)
  
  
  
  FORMAL PARAMETERS:
  
  double point,   - point at which to calculate the shape function in local coord
  double *shape,  - returned coeficients from the shape polynomials
  
  RETURN VALUE:
  
  None
  
  CALLING SEQUENCE:
  
  nmmtl_shape(point,shape);
  
  */
void nmmtl_shape(double point,
		 double *shape)
{
  
	register double L1,L2;
  
  
  /* shortcuts for the L1 and L2 used in Lagrangian polynomials evaluated at
     point x */
  
  L1 = 1.0 - point;
  L2 = point;
  
  /* Evaluate the Lagrangian polynomials here at the point. */
  
  /* - kept for posterity -
     if(basis_order .eq. 1) then
     shp(1)=L1
     shp(2)=L2
     else if(basis_order .eq. 2) then
     shp(1)=L1*(2.0*L1-1.0)
     shp(2)=L2*(2.0*L2-1.0)
     shp(3)=4.0*L1*L2
     else
     shp(1)=0.5*L1*(3.0*L1-1.0)*(3.0*L1-2.0)
     shp(2)=0.5*L2*(3.0*L2-1.0)*(3.0*L2-2.0)
     shp(3)=4.5*L1*L2*(3.0*L1-1.0)
     shp(4)=4.5*L2*L1*(3.0*L2-1.0)
     end if
     */
  
  shape[0] = L1*(2.0*L1-1.0);
  shape[1] = 4.0*L1*L2;
  shape[2] = L2*(2.0*L2-1.0);
  
}



