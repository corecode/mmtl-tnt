#ifndef _bicgstab__h_
#define _bicgstab__h_


int  bi_cgstab_d( Matrix&, Vector&, Vector&, double );
int  bi_cgstab_c( CmplxMatrix&, CmplxVector&, CmplxVector&, double );

int  bi_cgstab_d_sparse( Matrix&, Vector&, Vector&, double, double );
int  bi_cgstab_c_sparse( CmplxMatrix&, CmplxVector&, CmplxVector&, double, double );

int  bi_cg_d( Matrix&, Vector&, Vector&, double );
int  bi_cg_c( CmplxMatrix&, CmplxVector&, CmplxVector&, double );


int  bi_cgstab_d_precond( Matrix&, Vector&, Vector&, double );
int  bi_cgstab_c_precond( CmplxMatrix&, CmplxVector&, CmplxVector&, double );

int  bi_cgstab_d_sparse_precond( Matrix&, Vector&, Vector&, double, double );
int  bi_cgstab_c_sparse_precond( CmplxMatrix&, CmplxVector&, CmplxVector&, double, double );

int  bi_cgstab_c_sparse_simple( CmplxMatrix&, CmplxVector&, CmplxVector&, double );
int  bi_cgstab_c_sparse_new( CmplxVector&, IVectorl&, CmplxVector&, CmplxVector&, double );


#endif
