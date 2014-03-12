
/*
  
  FACILITY:  NMMTL
  
  MODULE DESCRIPTION:
  
  Contains these functions:
  
  nmmtl_interval_c   (conductor)
  nmmtl_interval_self_c (conductor self element)
  nmmtl_interval_c_fs   (conductor in _free_space)
  nmmtl_interval_self_c_fs
  (conductor self element in _free_space)
  nmmtl_interval_d   (dielectric)
  nmmtl_interval_self_d (dielectric self element)
  
  
  AUTHOR(S):
  
  Kevin J. Buchs
  
  CREATION DATE:  Thu Apr  2 15:51:42 1992
  
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
/*
 *******************************************************************
 **  FUNCTION DEFINITIONS
 *******************************************************************
 */


/*
  
  FUNCTION NAME:  nmmtl_interval_c()
  
  
  FUNCTIONAL DESCRIPTION:
  
  Performs source point integration over conductor elements
  
  FORMAL PARAMETERS:
  
	double x,         - global coordinates
	double y,         - global coordinates
  CELEMENTS_P cel, - conductor element
	double *value     - output coeficient values of integration
  int outer_cond_flag - flags that the outer element is a conductor - 
  determines the Green's Function used.
	float normalx,   - normals on outer element
	float normaly,
  
  RETURN VALUE:
  
  SUCCESS,FAILURE
  
  CALLING SEQUENCE:
  
  */

void nmmtl_interval_c(double x,
					double y,
		      CELEMENTS_P cel,
					double *value,
		      int outer_cond_flag,
					float normalx,
					float normaly)
{
  
  int i;
  int Legendre_counter;
	double X,Y;  /* interpolated coordinates */
	double shape[INTERP_PTS];
	double Jacobian;
	double dx1,dx2,dy1,dy2,d1,d2;
	double Greens_Function;
	double nu0,nu1;
  
  
  /* zero out output */
  for(i = 0; i < INTERP_PTS; i++)
    value[i] = 0.0;
  
  for(Legendre_counter = 0; Legendre_counter < Legendre_root_i_max;
      Legendre_counter++)
  {
    nmmtl_shape(Legendre_root_i[Legendre_counter],shape);
    
    /* interpolate x,y coordinate using no_edge shape function */
    X = 0.0;
    Y = 0.0;
    for(i=0;i < INTERP_PTS;i++)
    {
      X += shape[i]*cel->xpts[i];
      Y += shape[i]*cel->ypts[i];
    }
    
    nmmtl_jacobian_c(Legendre_root_i[Legendre_counter],cel,&Jacobian);
    /* if an edge element - recalculate shape using edge effects */
    
    if(cel->edge[0] != NULL || cel->edge[1] != NULL)
    {
      /* if given edge is really an edge, set the true value of nu,
	 otherwise, don't really care */
      nu0 = cel->edge[0] ? cel->edge[0]->nu : 0;
      nu1 = cel->edge[1] ? cel->edge[1]->nu : 0;
      nmmtl_shape_c_edge(Legendre_root_i[Legendre_counter],shape,cel,
			 nu0,nu1);
    }
    
    dx1 = x - X;
    dy1 = y - Y;
    d1 = sqrt(dx1*dx1 + dy1*dy1);
    
    dx2 = x - X; 
    dy2 = y + Y;
    d2 = sqrt(dx2*dx2 + dy2*dy2);
    
    if(outer_cond_flag == TRUE)
    {
      Greens_Function = log(d2/d1);
    }
    else
    {
      Greens_Function = ( dx1*normalx + dy1*normaly ) / ( d1*d1 ) -
	( dx2*normalx + dy2*normaly ) / ( d2*d2 );
    }
    
    for(i=0;i < INTERP_PTS;i++)
    {
      value[i] += Legendre_weight_i[Legendre_counter] * shape[i] * 
	Greens_Function * Jacobian;
    }
  } /* for all Legendre roots */
}


/*
  
  FUNCTION NAME:  nmmtl_interval_self_c()
  
  
  FUNCTIONAL DESCRIPTION:
  
  Performs source point integration over conductor elements for the self
  element
  
  FORMAL PARAMETERS:
  
	double x,         - global coordinates
	double y,         - global coordinates
  CELEMENTS_P cel, - conductor element
	double *value     - output coeficient values of integration
	double point      - the point at which to break the self element
  
  RETURN VALUE:
  
  SUCCESS,FAILURE
  
  CALLING SEQUENCE:
  
  */

void nmmtl_interval_self_c(double x,
				 double y,
			   CELEMENTS_P cel,
				 double *value,
				 double point)
{
  
  int i;
  int Legendre_counter;
	double X,Y;  /* interpolated coordinates */
	double shape[INTERP_PTS];
	double Jacobian;
	double dx1,dx2,dy1,dy2,d1,d2;
	double Greens_Function;
	double alpha; /* coeficient for interval splitting */
	double local_coord;
	double nu0,nu1;
  extern int nmmtl_fpe_handler_argument;
  
  /* set up specific floating point divide-by-zero error handling */
  nmmtl_fpe_handler_argument = NMMTL_FPE_ELETOOSMALL;
  
  /* zero out output */
  for(i = 0; i < INTERP_PTS; i++)
    value[i] = 0.0;
  
  /* This section - starting with the assignment to alpha is replicated
     below, slightly differently, to process the second have of the splitting
     of the self element, there are more efficient ways of doing this, but it
     would not be as clear from the code what is being done.  
     */
  
  alpha = point;
  
  for(Legendre_counter = 0; Legendre_counter < Legendre_root_i_max;
      Legendre_counter++)
  {
    local_coord = point * Legendre_root_i[Legendre_counter];
    
    nmmtl_shape(local_coord,shape);
    
    /* interpolate x,y coordinate using no_edge shape function */
    X = 0.0;
    Y = 0.0;
    for(i=0;i < INTERP_PTS;i++)
    {
      X += shape[i]*cel->xpts[i];
      Y += shape[i]*cel->ypts[i];
    }
    
    nmmtl_jacobian_c(local_coord,cel,&Jacobian);
    
    /* if an edge element - recalculate shape using edge effects */
    
    if(cel->edge[0] != NULL || cel->edge[1] != NULL)
    {
      /* if given edge is really an edge, set the true value of nu,
	 otherwise, don't really care */
      nu0 = cel->edge[0] ? cel->edge[0]->nu : 0;
      nu1 = cel->edge[1] ? cel->edge[1]->nu : 0;
      nmmtl_shape_c_edge(local_coord,shape,cel,
			 nu0,nu1);
    }
    
    dx1 = x - X;
    dy1 = y - Y;
    d1 = sqrt(dx1*dx1 + dy1*dy1);
    
    dx2 = x - X; 
    dy2 = y + Y;
    d2 = sqrt(dx2*dx2 + dy2*dy2);
    
    Greens_Function = log(d2/d1);
    
    for(i=0;i < INTERP_PTS;i++)
    {
      value[i] += Legendre_weight_i[Legendre_counter] * shape[i] * 
	Greens_Function * Jacobian * alpha;
    }
  } /* for all Legendre roots */
  
  
  /* Replicated section - see comment above where previous assignment to
     alpha occurs */
  
  alpha = 1.0 - point;
  
  for(Legendre_counter = 0; Legendre_counter < Legendre_root_i_max;
      Legendre_counter++)
  {
    local_coord = point + (1.0 - point) * Legendre_root_i[Legendre_counter];
    
    nmmtl_shape(local_coord,shape);
    
    /* interpolate x,y coordinate using no_edge shape function */
    X = 0.0;
    Y = 0.0;
    for(i=0;i < INTERP_PTS;i++)
    {
      X += shape[i]*cel->xpts[i];
      Y += shape[i]*cel->ypts[i];
    }
    
    nmmtl_jacobian_c(local_coord,cel,&Jacobian);
    
    /* if an edge element - recalculate shape using edge effects */
    
    if(cel->edge[0] != NULL || cel->edge[1] != NULL)
    {
      /* if given edge is really an edge, set the true value of nu,
	 otherwise, don't really care */
      nu0 = cel->edge[0] ? cel->edge[0]->nu : 0;
      nu1 = cel->edge[1] ? cel->edge[1]->nu : 0;
      nmmtl_shape_c_edge(local_coord,shape,cel,
			 nu0,nu1);
    }
    
    dx1 = x - X;
    dy1 = y - Y;
    d1 = sqrt(dx1*dx1 + dy1*dy1);
    
    dx2 = x - X; 
    dy2 = y + Y;
    d2 = sqrt(dx2*dx2 + dy2*dy2);
    
    Greens_Function = log(d2/d1);
    
    for(i=0;i < INTERP_PTS;i++)
    {
      value[i] += Legendre_weight_i[Legendre_counter] * shape[i] * 
	Greens_Function * Jacobian * alpha;
    }
  } /* for all Legendre roots */
  
  /* Restore default divide-by-zero error handling */
  nmmtl_fpe_handler_argument = NMMTL_FPE_OTHER;
}



/*
  
  FUNCTION NAME:  nmmtl_interval_c_fs()
  
  
  FUNCTIONAL DESCRIPTION:
  
  Performs source point integration over conductor elements
  Just like nmmtl_interval_c, but calls nmmtl_shape_c_edge
  with the free space value for nu to compute this function
  over free space.
  
  FORMAL PARAMETERS:
  
	double x,         - global coordinates
	double y,         - global coordinates
  CELEMENTS_P cel, - conductor element
	double *value     - output coeficient values of integration
  
  RETURN VALUE:
  
  SUCCESS,FAILURE
  
  CALLING SEQUENCE:
  
  */

void nmmtl_interval_c_fs(double x,
			 double y,
			 CELEMENTS_P cel,
			 double *value)
{
  
  int i;
  int Legendre_counter;
	double X,Y;  /* interpolated coordinates */
	double shape[INTERP_PTS];
	double Jacobian;
	double dx1,dx2,dy1,dy2,d1,d2;
	double Greens_Function;
	double nu0,nu1;
  
  
  /* zero out output */
  for(i = 0; i < INTERP_PTS; i++)
    value[i] = 0.0;
  
  for(Legendre_counter = 0; Legendre_counter < Legendre_root_i_max;
      Legendre_counter++)
  {
    nmmtl_shape(Legendre_root_i[Legendre_counter],shape);
    
    /* interpolate x,y coordinate using no_edge shape function */
    X = 0.0;
    Y = 0.0;
    for(i=0;i < INTERP_PTS;i++)
    {
      X += shape[i]*cel->xpts[i];
      Y += shape[i]*cel->ypts[i];
    }
    
    nmmtl_jacobian_c(Legendre_root_i[Legendre_counter],cel,&Jacobian);
    /* if an edge element - recalculate shape using edge effects */
    
    if(cel->edge[0] != NULL || cel->edge[1] != NULL)
    {
      /* if given edge is really an edge, set the true value of nu,
	 otherwise, don't really care */
      nu0 = cel->edge[0] ? cel->edge[0]->free_space_nu : 0;
      nu1 = cel->edge[1] ? cel->edge[1]->free_space_nu : 0;
      nmmtl_shape_c_edge(Legendre_root_i[Legendre_counter],shape,cel,
			 nu0,nu1);
    }
    
    dx1 = x - X;
    dy1 = y - Y;
    d1 = sqrt(dx1*dx1 + dy1*dy1);
    
    dx2 = x - X; 
    dy2 = y + Y;
    d2 = sqrt(dx2*dx2 + dy2*dy2);
    
    Greens_Function = log(d2/d1);
    
    for(i=0;i < INTERP_PTS;i++)
    {
      value[i] += Legendre_weight_i[Legendre_counter] * shape[i] * 
	Greens_Function * Jacobian;
    }
  } /* for all Legendre roots */
}


/*
  
  FUNCTION NAME:  nmmtl_interval_self_c_fs()
  
  
  FUNCTIONAL DESCRIPTION:
  
  Performs source point integration over conductor elements for the self
  element
  
  Just like nmmtl_interval_self_c, but calls nmmtl_shape_c_edge with the
  free space value for nu to compute this function over free space.
  
  
  FORMAL PARAMETERS:
  
	double x,         - global coordinates
	double y,         - global coordinates
  CELEMENTS_P cel, - conductor element
	double *value     - output coeficient values of integration
	double point      - the point at which to break the self element
  
  RETURN VALUE:
  
  SUCCESS,FAILURE
  
  CALLING SEQUENCE:
  
  */

void nmmtl_interval_self_c_fs(double x,
						double y,
			      CELEMENTS_P cel,
						double *value,
						double point)
{
  
  int i;
  int Legendre_counter;
	double X,Y;  /* interpolated coordinates */
	double shape[INTERP_PTS];
	double Jacobian;
	double dx1,dx2,dy1,dy2,d1,d2;
	double Greens_Function;
	double alpha; /* coeficient for interval splitting */
	double local_coord;
	double nu0,nu1;
  extern int nmmtl_fpe_handler_argument;
  
  /* set up specific floating point divide-by-zero error handling */
  nmmtl_fpe_handler_argument = NMMTL_FPE_ELETOOSMALL;
  
  
  
  /* zero out output */
  for(i = 0; i < INTERP_PTS; i++)
    value[i] = 0.0;
  
  /* This section - starting with the assignment to alpha is replicated
     below, slightly differently, to process the second have of the splitting
     of the self element, there are more efficient ways of doing this, but it
     would not be as clear from the code what is being done.  
     */
  
  alpha = point;
  
  for(Legendre_counter = 0; Legendre_counter < Legendre_root_i_max;
      Legendre_counter++)
  {
    local_coord = point * Legendre_root_i[Legendre_counter];
    
    nmmtl_shape(local_coord,shape);
    
    /* interpolate x,y coordinate using no_edge shape function */
    X = 0.0;
    Y = 0.0;
    for(i=0;i < INTERP_PTS;i++)
    {
      X += shape[i]*cel->xpts[i];
      Y += shape[i]*cel->ypts[i];
    }
    
    nmmtl_jacobian_c(local_coord,cel,&Jacobian);
    
    /* if an edge element - recalculate shape using edge effects */
    
    if(cel->edge[0] != NULL || cel->edge[1] != NULL)
    {
      /* if given edge is really an edge, set the true value of nu,
	 otherwise, don't really care */
      nu0 = cel->edge[0] ? cel->edge[0]->free_space_nu : 0;
      nu1 = cel->edge[1] ? cel->edge[1]->free_space_nu : 0;
      nmmtl_shape_c_edge(local_coord,shape,cel,
			 nu0,nu1);
    }
    
    dx1 = x - X;
    dy1 = y - Y;
    d1 = sqrt(dx1*dx1 + dy1*dy1);
    
    dx2 = x - X; 
    dy2 = y + Y;
    d2 = sqrt(dx2*dx2 + dy2*dy2);
    
    Greens_Function = log(d2/d1);
    
    for(i=0;i < INTERP_PTS;i++)
    {
      value[i] += Legendre_weight_i[Legendre_counter] * shape[i] * 
	Greens_Function * Jacobian * alpha;
    }
  } /* for all Legendre roots */
  
  
  /* Replicated section - see comment above where previous assignment to
     alpha occurs */
  
  alpha = 1.0 - point;
  
  for(Legendre_counter = 0; Legendre_counter < Legendre_root_i_max;
      Legendre_counter++)
  {
    local_coord = point + (1.0 - point) * Legendre_root_i[Legendre_counter];
    
    nmmtl_shape(local_coord,shape);
    
    /* interpolate x,y coordinate using no_edge shape function */
    X = 0.0;
    Y = 0.0;
    for(i=0;i < INTERP_PTS;i++)
    {
      X += shape[i]*cel->xpts[i];
      Y += shape[i]*cel->ypts[i];
    }
    
    nmmtl_jacobian_c(local_coord,cel,&Jacobian);
    
    /* if an edge element - recalculate shape using edge effects */
    
    if(cel->edge[0] != NULL || cel->edge[1] != NULL)
    {
      /* if given edge is really an edge, set the true value of nu,
	 otherwise, don't really care */
      nu0 = cel->edge[0] ? cel->edge[0]->free_space_nu : 0;
      nu1 = cel->edge[1] ? cel->edge[1]->free_space_nu : 0;
      nmmtl_shape_c_edge(local_coord,shape,cel,
			 nu0,nu1);
    }
    
    dx1 = x - X;
    dy1 = y - Y;
    d1 = sqrt(dx1*dx1 + dy1*dy1);
    
    dx2 = x - X; 
    dy2 = y + Y;
    d2 = sqrt(dx2*dx2 + dy2*dy2);
    
    Greens_Function = log(d2/d1);
    
    for(i=0;i < INTERP_PTS;i++)
    {
      value[i] += Legendre_weight_i[Legendre_counter] * shape[i] * 
	Greens_Function * Jacobian * alpha;
    }
  } /* for all Legendre roots */
  
  /* Restore default divide-by-zero error handling */
  nmmtl_fpe_handler_argument = NMMTL_FPE_OTHER;
  
}



/*
  
  FUNCTION NAME:  nmmtl_interval_d()
  
  
  FUNCTIONAL DESCRIPTION:
  
  Performs source point integration over dielectric elements
  
  FORMAL PARAMETERS:
  
	double x,         - global coordinates
	double y,         - global coordinates
  DELEMENTS_P del, - dielectric element
	double *value     - output coeficient values of integration
  int outer_cond_flag - flags that the outer element is a conductor - 
  determines the Green's Function used.
	double normalx,   - normals on outer element
	float normaly,
  
  RETURN VALUE:
  
  SUCCESS,FAILURE
  
  CALLING SEQUENCE:
  
  */

void nmmtl_interval_d(double x,
					double y,
		      DELEMENTS_P del,
					double *value,
		      int outer_cond_flag,
					float normalx,
					float normaly)
{
  
  int i;
  int Legendre_counter;
	double X,Y;  /* interpolated coordinates */
	double shape[INTERP_PTS];
	double Jacobian;
	double dx1,dx2,dy1,dy2,d1,d2;
	double Greens_Function;
  
  
  /* zero out output */
  for(i = 0; i < INTERP_PTS; i++)
    value[i] = 0.0;
  
  for(Legendre_counter = 0; Legendre_counter < Legendre_root_i_max;
      Legendre_counter++)
  {
    nmmtl_shape(Legendre_root_i[Legendre_counter],shape);
    
    /* interpolate x,y coordinate using no_edge shape function */
    X = 0.0;
    Y = 0.0;
    for(i=0;i < INTERP_PTS;i++)
    {
      X += shape[i]*del->xpts[i];
      Y += shape[i]*del->ypts[i];
    }
    
    nmmtl_jacobian_d(Legendre_root_i[Legendre_counter],del,&Jacobian);
    
    dx1 = x - X;
    dy1 = y - Y;
    d1 = sqrt(dx1*dx1 + dy1*dy1);
    
    dx2 = x - X; 
    dy2 = y + Y;
    d2 = sqrt(dx2*dx2 + dy2*dy2);
    
    if(outer_cond_flag == TRUE)
    {
      Greens_Function = log(d2/d1);
    }
    else
    {
      Greens_Function = ( dx1*normalx + dy1*normaly ) / ( d1*d1 ) -
	( dx2*normalx + dy2*normaly ) / ( d2*d2 );
    }
    
    
    for(i=0;i < INTERP_PTS;i++)
    {
      value[i] += Legendre_weight_i[Legendre_counter] * shape[i] * 
	Greens_Function * Jacobian;
    }
  } /* for all Legendre roots */
}


/*
  
  FUNCTION NAME:  nmmtl_interval_self_d()
  
  FUNCTIONAL DESCRIPTION:
  
  Performs source point integration over dielectric elements for the self
  element
  
  FORMAL PARAMETERS:
  
	double x,         - global coordinates
	double y,         - global coordinates
  DELEMENTS_P del, - dielectric element
	double *value     - output coeficient values of integration
	double point      - the point at which to break the self element
	float normalx,   - normals on outer element
	float normaly,
  
  RETURN VALUE:
  
  SUCCESS,FAILURE
  
  CALLING SEQUENCE:
  
  */

void nmmtl_interval_self_d(double x,
				 double y,
			   DELEMENTS_P del,
				 double *value,
				 double point,
				 float normalx,
				 float normaly)
{
  
  int i;
  int Legendre_counter;
	double X,Y;  /* interpolated coordinates */
	double shape[INTERP_PTS];
	double Jacobian;
	double dx1,dx2,dy1,dy2,d1,d2;
	double Greens_Function;
	double alpha; /* coeficient for interval splitting */
	double local_coord;
  extern int nmmtl_fpe_handler_argument;
  
  /* set up specific floating point divide-by-zero error handling */
  nmmtl_fpe_handler_argument = NMMTL_FPE_ELETOOSMALL;
  
  
  /* zero out output */
  for(i = 0; i < INTERP_PTS; i++)
    value[i] = 0.0;
  
  /* This section - starting with the assignment to alpha is replicated
     below, slightly differently, to process the second have of the splitting
     of the self element, there are more efficient ways of doing this, but it
     would not be as clear from the code what is being done.  
     */
  
  alpha = point;
  
  for(Legendre_counter = 0; Legendre_counter < Legendre_root_i_max;
      Legendre_counter++)
  {
    local_coord = point * Legendre_root_i[Legendre_counter];
    
    nmmtl_shape(local_coord,shape);
    
    /* interpolate x,y coordinate using no_edge shape function */
    X = 0.0;
    Y = 0.0;
    for(i=0;i < INTERP_PTS;i++)
    {
      X += shape[i]*del->xpts[i];
      Y += shape[i]*del->ypts[i];
    }
    
    nmmtl_jacobian_d(local_coord,del,&Jacobian);
    
    dx1 = x - X;
    dy1 = y - Y;
    d1 = sqrt(dx1*dx1 + dy1*dy1);
    
    dx2 = x - X; 
    dy2 = y + Y;
    d2 = sqrt(dx2*dx2 + dy2*dy2);
    
    Greens_Function = ( dx1*normalx + dy1*normaly ) / ( d1*d1 ) -
      ( dx2*normalx + dy2*normaly ) / ( d2*d2 );
    
    for(i=0;i < INTERP_PTS;i++)
    {
      value[i] += Legendre_weight_i[Legendre_counter] * shape[i] * 
	Greens_Function * Jacobian * alpha;
    }
    
  } /* for all Legendre roots */
  
  
  /* Replicated section - see comment above where previous assignment to
     alpha occurs */
  
  alpha = 1.0 - point;
  
  for(Legendre_counter = 0; Legendre_counter < Legendre_root_i_max;
      Legendre_counter++)
  {
    local_coord = point + (1.0 - point) * Legendre_root_i[Legendre_counter];
    
    nmmtl_shape(local_coord,shape);
    
    /* interpolate x,y coordinate using no_edge shape function */
    X = 0.0;
    Y = 0.0;
    for(i=0;i < INTERP_PTS;i++)
    {
      X += shape[i]*del->xpts[i];
      Y += shape[i]*del->ypts[i];
    }
    
    nmmtl_jacobian_d(local_coord,del,&Jacobian);
    
    dx1 = x - X;
    dy1 = y - Y;
    d1 = sqrt(dx1*dx1 + dy1*dy1);
    
    dx2 = x - X; 
    dy2 = y + Y;
    d2 = sqrt(dx2*dx2 + dy2*dy2);
    
    Greens_Function = ( dx1*normalx + dy1*normaly ) / ( d1*d1 ) -
      ( dx2*normalx + dy2*normaly ) / ( d2*d2 );
    
    for(i=0;i < INTERP_PTS;i++)
    {
      value[i] += Legendre_weight_i[Legendre_counter] * shape[i] * 
	Greens_Function * Jacobian * alpha;
    }
    
  } /* for all Legendre roots */
  
  /* Restore default divide-by-zero error handling */
  nmmtl_fpe_handler_argument = NMMTL_FPE_OTHER;
  
}
