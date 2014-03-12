#ifndef _diffrm__h_
#define _diffrm__h_


void    set_wavelets( Vector&, Vector&, const int& );

void    filtermat_all( Matrix&, Matrix&, Matrix&, Matrix&, int&, int&, Vector&, Vector& );

void    fastwt( CmplxMatrix&, CmplxMatrix&, CmplxMatrix&, CmplxMatrix&, CmplxMatrix&, Vector&, Vector& );
void    nonstdform( CmplxMatrix&, CmplxMatrix&, int&, Vector&, Vector& );

void    fwt_vec_decomp( CmplxVector&, CmplxVector&, CmplxVector&, Vector&, Vector& );
void    std_vec_decomp( CmplxVector&, CmplxVector&, int&, Vector&, Vector& );
void    fwt_vec_reconstr( CmplxVector&, CmplxVector&, CmplxVector&, Vector&, Vector& );
void    std_vec_reconstr( CmplxVector&, CmplxVector&, int&, Vector&, Vector& );

void    getsbs( Matrix&, const int&, const int& );
void    fastwt_row( CmplxMatrix&, CmplxMatrix&, CmplxMatrix&, Vector&, Vector& );
void    fastwt_col( CmplxMatrix&, CmplxMatrix&, CmplxMatrix&, Vector&, Vector& );
void    stdform( CmplxMatrix&, CmplxMatrix&, int&, Vector&, Vector& );

void    fastwt( CmplxMatrix&, Matrix&, int&, Vector&, Vector& );

void    std_vec_decomp_new( CmplxVector&, int&, Vector&, Vector& );
void    std_vec_reconstr_new( CmplxVector&, int&, Vector&, Vector& );
void    stdform_new( CmplxMatrix&, int&, Vector&, Vector& );
void    stdform_new_fast( CmplxMatrix&, int&, Vector&, Vector& );


#endif
