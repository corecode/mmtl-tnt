#ifndef _systsolv__h_
#define _systsolv__h_


void ludcmp( CmplxMatrix&, Vector&, double& );
void lubksb( CmplxMatrix&, CmplxVector&, Vector& );
void luinvm( CmplxMatrix&, CmplxMatrix&, Vector& ); 
void improve( CmplxMatrix&, CmplxMatrix&, Vector&, 
              CmplxVector&, CmplxVector& );

Complex ludetr( CmplxMatrix&, double& );


void ludcmp_real( Matrix&, Vector&, double& );
void lubksb_real( Matrix&, Vector&, Vector& );
void luinvm_real( Matrix&, Matrix&, Vector& ); 
void improve_real( Matrix&, Matrix&, Vector&, 
                   Vector&, Vector& );

double ludetr_real( Matrix&, double& );

void get_l_u_real( Matrix&, Matrix&, const Matrix& );
void get_l_real( Matrix&, const Matrix&, const Matrix& );
void solve_u_known_real( Matrix&, const Matrix&, const Matrix& );
void solve_l_known_real( Matrix&, const Matrix&, const Matrix& );


void ludcmp_sparse( CmplxMatrix&, Vector&, double& );


#endif
