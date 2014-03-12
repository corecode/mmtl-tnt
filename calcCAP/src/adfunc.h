/*      adfunc.h         */

double rc(int, int);
double tc(int, int);
double grcplus(int, double);
double grcminus(int, double);
double splus(int, double);
double sminus(int, double);
double Mm(int, double);
double Kplus(int, int, double);
double aKplus(int, int);
double Kminus(int, int, double);
double aKminus(int, int);
//double Ggplus(int, int, double);
double Gsplus(double, double, double, double, CmplxMatrix&, CmplxMatrix& );
//double Ggminus(int, int, double);
double Gsminus(double, double, double, double, CmplxMatrix&, CmplxMatrix& );
int    xlayer(double);
double sqv( double );
double csft( double );
double sidelen( double, double, double, double );
int    recgeom( int, double&, double&, double& );
int    cirgeom( int, double&, double&, double& );
int    trageom( int, double&, double&, double& );
void   getsbs( Matrix& );
void   intpnt( double, Vector&, Vector&, Vector&, Vector&,
             double, Vector&, Vector&, Vector&, Vector&,
             double );
Complex kernel( double&, double&, int, int, CmplxMatrix&, CmplxMatrix& );
void    getcoe( CmplxMatrix&, CmplxMatrix& );
