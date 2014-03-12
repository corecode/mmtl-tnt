#ifndef _sparse__h_
#define _sparse__h_


void  sprsin_d( Matrix&, double, Vector&, IVectorl& );
void  sprsax_d( Vector&, IVectorl&, Vector&, Vector& );
Vector  sprsax_d_v( Vector&, IVectorl&, Vector& );  


void  sprsin_c( CmplxMatrix&, double, CmplxVector&, IVectorl& );
void  sprsax_c( CmplxVector&, IVectorl&, CmplxVector&, CmplxVector& );
CmplxVector  sprsax_c_v( CmplxVector&, IVectorl&, CmplxVector& );  


#endif
