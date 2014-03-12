/*************** To set global variables ********************/

#include "stdafx.h"

   extern  double  Pi;          /* Pi number ****************/
   extern  double  Pih;         /* Pi/2.0 *******************/
   extern  double  Pit;         /* Pi*2.0 *******************/
   extern  double  wn;          /* wave number **************/
   extern  double  mu;          /* impedance of free space **/

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
   extern  int     Die, Rec, Cir, Tri, Ell, Pol, Gnd, Tra;

   extern  Matrix  Gnddi;
   extern  Matrix  Recdi;       /* rectangular conductors ***/
   extern  Matrix  Cirdi;
   extern  Matrix  Tradix;
   extern  Matrix  Tradiy;

   extern  Vector  ag, bg, cg;
   extern  Vector  a, b, c;
   extern  Vector  at, bt, ct;

   extern  Vector  Circum;      /* circumference of conduct**/

   extern  Vector  Diec;        /* circum of die conductors */
   extern  Vector  Recc;        /* circum of rec conductors */
   extern  Vector  Circ;        /* circum of cir conductors */
   extern  Vector  Trac;        /* circum of tra conductors */
   extern  Vector  Tric;        /* circum of tri conductors */
   extern  Vector  Ellc;        /* circum of ell conductors */
   extern  Vector  Polc;        /* circum of pol conductors */
   extern  Vector  Gndc;        /* circum of gnd conductors */

   extern  double  hw;
   extern  double  hwh;

   extern  double  freq, sqrtoms;
   extern  double  sigma;
   extern  double  omega;
   extern  double  oms, omegamu;
   extern  double  sft;

   extern  Matrix  uns;
   extern  int     matr;
   extern  double  EPS;
   extern  Vector  Iq_sq;
   extern  double  Uniti;

void getparam() {

    int i;
    extern char fileIn[1024];

    fstream fp(fileIn, ios::in);

    if( fp.fail() ) {

       cerr << "getparam"
            << ": can't open input file: " << fileIn
            << endl;

       exit(1);

    }

    fp >> Gnd;        fp.ignore(100,'\n');
    fp >> Rec;        fp.ignore(100,'\n');
    fp >> Cir;        fp.ignore(100,'\n');
    fp >> Tra;        fp.ignore(100,'\n');

    if( Gnd > 1 ) {
      cerr << "The Number of Ground Plane should be one!" << endl;
      exit(1);
    } else if ( Gnd < 1 ) {
      cerr << "You need to specify one ground plane!" << endl;
      exit(1);
    }

    Die = 0;
    Tri = 0;
    Ell = 0;
    Pol = 0;
    Nc = Die+Rec+Cir+Tra+Tri+Ell+Pol+Gnd;
  
    Circum.resize(Nc);

    ag.resize(Gnd);
    bg.resize(Gnd);
    cg.resize(Gnd);

    a.resize(Rec);
    b.resize(Rec);
    c.resize(Rec);

    at.resize(Tra);
    bt.resize(Tra);
    ct.resize(Tra);

    Diec.resize(Die);
    Recc.resize(Rec);
    Circ.resize(Cir);
    Trac.resize(Tra);
    Tric.resize(Tri);
    Ellc.resize(Ell);
    Polc.resize(Pol);
    Gndc.resize(Gnd);
    
    Gnddi.resize(Gnd,4);
    Recdi.resize(Rec,4);
    Cirdi.resize(Cir,3);
    Tradix.resize(Tra,4);
    Tradiy.resize(Tra,4);

    Iq_sq.resize(Nc);

    if( Gnd > 0 ) {

      for( i = 0; i < Gnd; i ++ ) {
	fp >> Gnddi(i,0);     fp.ignore(100,'\n');
	fp >> Gnddi(i,1);     fp.ignore(100,'\n');
	fp >> Gnddi(i,2);     fp.ignore(100,'\n');
	fp >> Gnddi(i,3);     fp.ignore(100,'\n');
	fp >> Iq_sq[i];       fp.ignore(100,'\n');
      }

    }

    if( Rec > 0 ) {

      for( i = 0; i < Rec; i ++ ) {
	fp >> Recdi(i,0);     fp.ignore(100,'\n');
	fp >> Recdi(i,1);     fp.ignore(100,'\n');
	fp >> Recdi(i,2);     fp.ignore(100,'\n');
	fp >> Recdi(i,3);     fp.ignore(100,'\n');
	fp >> Iq_sq[i+Gnd];   fp.ignore(100,'\n');
      }

    }

    if( Cir > 0 ) {

      for( i = 0; i < Cir; i ++ ) {
	fp >> Cirdi(i,0);         fp.ignore(100,'\n');
	fp >> Cirdi(i,1);         fp.ignore(100,'\n');
	fp >> Cirdi(i,2);         fp.ignore(100,'\n');
	fp >> Iq_sq[i+Gnd+Rec];   fp.ignore(100,'\n');
      }

    }

    if( Tra > 0 ) {

      for( i = 0; i < Tra; i ++ ) {
	fp >> Tradix(i,0);            fp.ignore(100,'\n');
	fp >> Tradiy(i,0);            fp.ignore(100,'\n');
	fp >> Tradix(i,1);            fp.ignore(100,'\n');
	fp >> Tradiy(i,1);            fp.ignore(100,'\n');
	fp >> Tradix(i,2);            fp.ignore(100,'\n');
	fp >> Tradiy(i,2);            fp.ignore(100,'\n');
	fp >> Tradix(i,3);            fp.ignore(100,'\n');
	fp >> Tradiy(i,3);            fp.ignore(100,'\n');
	fp >> Iq_sq[i+Gnd+Rec+Cir];   fp.ignore(100,'\n');
      }
    }
  
    fp >> Nh;       fp.ignore(100,'\n');
    fp >> Nit;      fp.ignore(100,'\n');
    fp >> J;        fp.ignore(100,'\n');

    fp >> Nwx;      fp.ignore(100,'\n');
    fp >> Nwy;      fp.ignore(100,'\n');
    fp >> Nws;      fp.ignore(100,'\n');

    fp >> Np;       fp.ignore(100,'\n');
    fp >> EPS;      fp.ignore(100,'\n');
    fp >> matr;     fp.ignore(100,'\n');

    fp >> freq;     fp.ignore(100,'\n');
    fp >> sigma;    fp.ignore(100,'\n');

    fp.close();      


    Uniti=1.0;
    Pi = 4.0*atan(1.0);
    Pih = Pi/2.0;
    Pit = Pi*2.0;
    wn = 2.0*Pi;
    mu = 120.0*Pi;
    
    if (Gnd > 0) {
      for ( i = 0; i < Gnd; i ++ ) {
	
	Gndc[i] = 2.0*(Gnddi(i,2)+Gnddi(i,3));
	Circum[i] = Gndc[i];
	
	ag[i] = Gnddi(i,2);
	bg[i] = Gnddi(i,2)+Gnddi(i,3);
	cg[i] = 2*Gnddi(i,2)+Gnddi(i,3);
	
      }
    }

    if (Rec > 0) {
      for ( i = 0; i < Rec; i ++ ) {
	
	Recc[i] = 2.0*(Recdi(i,2)+Recdi(i,3));  
	Circum[i+Gnd] = Recc[i];

	a[i] = Recdi(i,2);
	b[i] = Recdi(i,2)+Recdi(i,3);
	c[i] = 2*Recdi(i,2)+Recdi(i,3);
	
      }
    }

    if (Cir > 0) {
      for ( i = 0 ; i < Cir; i ++ ) {
	
	Circ[i] = 2.0*Pi*Cirdi(i,2);
	Circum[Gnd+Rec+i] = Circ[i];

      }
    }

    if (Tra > 0) {
      for ( i = 0 ; i < Tra; i ++ ) {
      
	at[i] = sidelen(Tradix(i,0),Tradiy(i,0),Tradix(i,1),Tradiy(i,1));
	bt[i] = at[i]+sidelen(Tradix(i,1),Tradiy(i,1),Tradix(i,2),Tradiy(i,2));
	ct[i] = bt[i]+sidelen(Tradix(i,2),Tradiy(i,2),Tradix(i,3),Tradiy(i,3));
	
	Trac[i] = ct[i]+sidelen(Tradix(i,0),Tradiy(i,0),Tradix(i,3),Tradiy(i,3));
	Circum[Gnd+Rec+Cir+i] = Trac[i];
	
      }
    }
    
    Ic = cmplx( 0.0, 1.0 );
    
    Nc4 = 11*Nh+1;
    step_w = 1.0/power( 2.0, J );
    Nw = int(power( 2.0, J ));
    hw = 11.0*step_w/double(Nws);
    hwh = hw/2.0;

    omega = 2.0*Pi*freq;
    mu = 4.0*Pi*1.0e-7;
    omegamu = omega*mu;

    oms = omega*mu*sigma;
    sqrtoms = sqrt(oms);

    cout << freq << endl;
    cout << sigma << endl;

}

