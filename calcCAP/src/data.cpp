/*************** To set global variables ********************/

#include "stdafx.h"

extern  double  Pi;          /* Pi number ****************/
extern  double  Pih;         /* Pi/2.0 *******************/
extern  double  Pit;         /* Pi*2.0 *******************/

extern  int     Np;
extern  int     Nh;          /* for Coifman, L = 4 *******/
extern  int     Nc4;         /* for Coifman, L = 4 *******/
extern  int     Nit;         /* number of iterations *****/
extern  int     J;           /* maximum resolution level */

extern  Complex Ic;          /* cmplx( 0.0, 1.0 ) ********/

extern  double  step_w;      /* 1.0 / power( 2.0, J ) ****/
extern  int     Nw;          /* number of wavelets *******/
extern  int     Nwx;         /* number of Gauss points ***/
extern  int     Nwy;         /* number of Gauss points ***/
extern  int     Nws;         /* number of subdivisons ****/

extern  int     Nc;          /* number of conductors *****/

extern  int     Die;         /* number of diele layer ****/
extern  int     Rec;         /* number of rectangles *****/
extern  int     Cir;         /* number of circles ********/
extern  int     Gnd;         /* number of ground planes **/
extern  int     Tra;         /* number of trapezoid ******/

extern  Matrix  Gnddi;       /* ground parameters ********/
extern  Matrix  Recdi;       /* rectangular parameters ***/
extern  Matrix  Cirdi;       /* circular parameters ******/
extern  Matrix  Diedi;       /* dielectric parameters ****/
extern  Matrix  Tradix;      /* trapezoid x parameters ***/
extern  Matrix  Tradiy;      /* trapecoid y parameters ***/

extern  Vector  a, b, c;     /* rectangle parameters *****/
extern  Vector  at, bt, ct;  /* trapezoid parameters *****/

extern  Vector  Circum;      /* circumference of conduct**/

extern  Vector  Diec;        /* circum of die conductors */
extern  Vector  Recc;        /* circum of rec conductors */
extern  Vector  Circ;        /* circum of cir conductors */
extern  Vector  Trac;        /* circum of tra conductors */
extern  Vector  Gndc;        /* circum of gnd conductors */

extern  double  hw;
extern  double  hwh;

extern  Matrix  uns;
extern  Vector  Iq_sq;
extern  int     matr;
extern  double  EPS;

extern  int     Nd;
extern  double  eps0;
extern  Vector  eps;
extern  Vector  d;

extern  int     od;
extern  double  maxga;

void getparam(char *inFile) {

    int i, j;
    double zerobase;

    fstream fp(inFile, ios::in);

    if( fp.fail() ) {

       cerr << "getparam"
            << ": can't open input file: " 
	    << inFile 
            << endl;

       exit(1);

    }

    fp >> Gnd;        fp.ignore(100,'\n');
    fp >> Rec;        fp.ignore(100,'\n');
    fp >> Cir;        fp.ignore(100,'\n');
    fp >> Tra;        fp.ignore(100,'\n');
    fp >> Nd;         fp.ignore(100,'\n');

    Nd += 1;

    if( Gnd > 1 ) {
      cerr << "The Number of Ground Plane should be less than one!" << endl;
      exit(1);
    } else if ( Gnd < 1 ) {
      cerr << "You need to specify one ground plane!" << endl;
      exit(1);
    }

    Die = 0;
    Nc = Rec + Cir + Tra;

    eps.resize(Nd+1);
    d.resize(Nd+1);

    Circum.resize(Nc);

    a.resize(Rec);
    b.resize(Rec);
    c.resize(Rec);

    at.resize(Tra);
    bt.resize(Tra);
    ct.resize(Tra);

    Recc.resize(Rec);
    Circ.resize(Cir);
    Trac.resize(Tra);
    Gndc.resize(Gnd);
    
    Gnddi.resize(Gnd,4);
    Recdi.resize(Rec,4);
    Cirdi.resize(Cir,3);
    Tradix.resize(Tra,4);
    Tradiy.resize(Tra,4);
    Diedi.resize(Nd,4);
    
    Iq_sq.resize(Nc);
    
    if( Gnd == 1 ) {
      for( i = 0; i < Gnd; i++ ) {

	fp >> Gnddi(i,0);         fp.ignore(100,'\n');
	fp >> Gnddi(i,1);         fp.ignore(100,'\n');
	fp >> Gnddi(i,2);         fp.ignore(100,'\n');
	fp >> Gnddi(i,3);         fp.ignore(100,'\n');

      }
    }
    
    if( Rec > 0 ) {
      for( i = 0; i < Rec; i++ ) {

	fp >> Recdi(i,0);         fp.ignore(100,'\n');
	fp >> Recdi(i,1);         fp.ignore(100,'\n');
	fp >> Recdi(i,2);         fp.ignore(100,'\n');
	fp >> Recdi(i,3);         fp.ignore(100,'\n');

      }
    }
    
    if( Cir > 0 ) {
      for( i = 0; i < Cir; i++ ) {

	fp >> Cirdi(i,0);         fp.ignore(100,'\n');
	fp >> Cirdi(i,1);         fp.ignore(100,'\n');
	fp >> Cirdi(i,2);         fp.ignore(100,'\n');

      }
    }
    
    if( Tra > 0 ) {
      for( i = 0; i < Tra; i++ ) {

	fp >> Tradix(i,0);        fp.ignore(100,'\n');
	fp >> Tradiy(i,0);        fp.ignore(100,'\n');
	fp >> Tradix(i,1);        fp.ignore(100,'\n');
	fp >> Tradiy(i,1);        fp.ignore(100,'\n');
	fp >> Tradix(i,2);        fp.ignore(100,'\n');
	fp >> Tradiy(i,2);        fp.ignore(100,'\n');
	fp >> Tradix(i,3);        fp.ignore(100,'\n');
	fp >> Tradiy(i,3);        fp.ignore(100,'\n');

      }
    }

    if( Nd-1 > 0 ) {
      for( i = 0; i < Nd-1; i++ ) {

	fp >> Diedi(i,0);     fp.ignore(100,'\n');
	fp >> Diedi(i,1);     fp.ignore(100,'\n');
	fp >> Diedi(i,2);     fp.ignore(100,'\n');
	fp >> Diedi(i,3);     fp.ignore(100,'\n');
	fp >> eps[i+1];       fp.ignore(100,'\n');

      }
      
      Diedi(Nd-1,0) = Diedi(Nd-2,0);
      Diedi(Nd-1,1) = Diedi(Nd-2,1) + Diedi(Nd-2,3);
      Diedi(Nd-1,2) = Diedi(Nd-2,2);
      Diedi(Nd-1,3) = 1.0e3;
      eps[Nd] = 1.0;
      eps[0] = 1.0;   //anything is ok
      d[0] = Gnddi(0,1)+Gnddi(0,3);
      
      for(i=0; i<Nd; i++) {
	d[i+1] = Diedi(i,1)+Diedi(i,3);
      }
    } else if( Nd == 1) {
      Diedi(0,0) = Gnddi(0,0);
      Diedi(0,1) = Gnddi(0,1) + Gnddi(0,3);
      Diedi(0,2) = Gnddi(0,2);
      Diedi(0,3) = 1.0e3;
      eps[0] = 1.0;   //anything is ok
      eps[1] = 1.0;
      d[0] =  Gnddi(0,1)+Gnddi(0,3);
      d[1] =  1.0e3;
    }
    
    eps0 = 8.84e-12;
    eps *= eps0;

    fp >> Nh;       fp.ignore(100,'\n');
    fp >> Nit;      fp.ignore(100,'\n');
    fp >> J;        fp.ignore(100,'\n');

    fp >> Nwx;      fp.ignore(100,'\n');
    fp >> Nwy;      fp.ignore(100,'\n');
    fp >> Nws;      fp.ignore(100,'\n');

    fp >> Np;       fp.ignore(100,'\n');
    fp >> EPS;      fp.ignore(100,'\n');
    fp >> matr;     fp.ignore(100,'\n');
    fp >> od;       fp.ignore(100,'\n');
    fp >> maxga;    fp.ignore(100,'\n');

    fp.close();

    //cerr << "maxga=" << maxga << endl;

    Pi = 4.0*atan(1.0);
    Pih = Pi/2.0;
    Pit = Pi*2.0;

    if (Rec > 0) {
      for ( i = 0; i < Rec; i++ ) {
	Recc[i] = 2.0*(Recdi(i,2)+Recdi(i,3));
	Circum[i] = Recc[i];
	
	a[i] = Recdi(i,2);
	b[i] = Recdi(i,2)+Recdi(i,3);
	c[i] = 2*Recdi(i,2)+Recdi(i,3);
      }
    }
    
    if (Cir > 0) {
      for ( i = 0; i < Cir; i++ ) {
	Circ[i] = 2.0*Pi*Cirdi(i,2);
	Circum[Rec+i] = Circ[i];
      }
    }
    
    if (Tra > 0) {
      for ( i = 0; i < Tra; i++ ) {
	at[i] = sidelen(Tradix(i,0),Tradiy(i,0),Tradix(i,1),Tradiy(i,1));
	bt[i] = at[i]+sidelen(Tradix(i,1),Tradiy(i,1),Tradix(i,2),Tradiy(i,2));
	ct[i] = bt[i]+sidelen(Tradix(i,2),Tradiy(i,2),Tradix(i,3),Tradiy(i,3));
	
	Trac[i] = ct[i]+sidelen(Tradix(i,0),Tradiy(i,0),Tradix(i,3),Tradiy(i,3));
	Circum[Rec+Cir+i] = Trac[i];
      }
    }
    
    Ic = cmplx( 0.0, 1.0 );
    
    Nc4 = 11*Nh+1;
    step_w = 1.0/power( 2.0, J );
    Nw = int(power( 2.0, J ));
    //hw = 11.0*step_w/double(Nws);
    hw = 6.0*step_w/double(Nws);
    hwh = hw/2.0;

    //cerr << d << eps << endl;

}
