/***********************************************************/
/***                                                     ***/
/***   Resistance and Inductance using Coifman wavelets  ***/
/***            for the method of moments                ***/
/***                                                     ***/
/***               by Zhichao Zhang                      ***/
/***                    2001.3                           ***/
/***                                                     ***/
/***********************************************************/
/***********************************************************
Abstract:
The frequency-dependent resistance and inductance of uniform
transmission lines are calculated with a hybrid technique that
combines a cross-section coupled circuit method with a surface
integral equation approach.  The coupled circuit approach is
most applicble for low-frequency calculations, while the
integral equation approach is best for high frequencies.  The
low-frequency method consists in subdividing the cross section
of each conductor into triangular filaments, each with an
assumed uniform currect distribution.  The resistance and
mutual inductance between the filaments are clculated, and a
matrix is inverted to give the overall resistance and
inductance of the conductors.  The high-frequency method
expresses the resistance and inductance of each conductor in
terms of the current at the surface of that conductor and the
derivative of that current normal to the surface.  A coupled
integral equation is then derived to relate these qualities
through the diffusion equation inside the conductors and
La;lace's equation outside.  The method of moments with
pulse basis functions is used to solve the integral equations.
An interpolation between the results of these two methods gives
very good results over the entire frequency range, even when few
basis functions are used.  Results for a variety of configurations
are shown and are compared with experimental data and other
numerical techniques.
**************************************************************/


#include "stdafx.h"

#ifdef HAVE_SYS_TIMES_H
#include <sys/times.h>
#endif
#include <time.h>
#include <stdio.h>
#include <string.h>

/********************** Global variables *******************/

   int     Nc, Die, Rec, Cir, Tri, Ell, Pol, Gnd, Tra;
   Matrix  Gnddi, Recdi, Cirdi, Tradix, Tradiy;             /* dimension matrix */

   Vector  ag, bg, cg;      /* gnd parameters **************/
   Vector  a, b, c;         /* rec parameters **************/
   Vector  at, bt, ct;      /* tra parameters **************/

   Vector  Circum;          /* Circumference of conductors */
   Vector  Diec;            /* circum of die conductors ****/
   Vector  Recc;            /* circum of rec conductors ****/
   Vector  Circ;            /* circum of cir conductors ****/
   Vector  Trac;            /* circum of tra conductors ****/
   Vector  Tric;            /* circum of tri conductors ****/
   Vector  Ellc;            /* circum of ell conductors ****/
   Vector  Polc;            /* circum of pol conductors ****/
   Vector  Gndc;            /* circum of gnd conductors ****/

   double  freq, sqrtoms, sigma;
   double  omega, oms, omegamu, sft;
   double  tmp1, tmp2, tmp3, tmp4;

   int     matr, Mt, Np;
   int     Ns, Nsr, Ngx, Ngy, Nxt, Nyt, Ngr, Nrt;
   int     Nh, Nc4, Nit, J, Nw, Nwx, Nwy, Nws;
   double  hsrw, EPS, Uniti;
   double  Pi = 4.0*atan(1.0);
   double  Pih, Pit, wn, mu, cf;
   double  step_w, hw, hwh, mmax;
   Complex Ic;
   Vector  Iq_sq;

/********************** Main program ***********************/
char fileIn[1024];
int  append;

int main ( int argc, char *argv[] )
{

  /************ to setup global parameters *********/

  char fileName[1024], fileOut[1024], fileRL[1024], fileXmgrR[1024];
  char fileXmgrL[1024], strg[20];

  //---------------------------------------------------------------
  // Get the input and output file names from the command-line.
  //---------------------------------------------------------------
  if ( argc > 0)
    strcpy (fileName, argv[1]);
  else
    strcpy (fileName, "data");

  append = 0;
  if ( argc > 1 )
    {
      if ( ! strcmp (argv[2], "append") )
	{
	  append = 1;
	  cout << "Append to existing data in file" << endl;
	}
    }

  if ( strlen(fileName) < 2 )
    strcpy (fileName, "data");

  sprintf (fileIn, "%s.in", fileName);
  sprintf (fileOut, "%s.out", fileName);
  sprintf (fileRL, "%s.out.rl", fileName);
  sprintf (fileXmgrR, "%s.out.resistance", fileName);
  sprintf (fileXmgrL, "%s.out.inductance", fileName);
  cout << "Input file : " << fileIn << endl;
  cout << "Output file: " << fileOut << endl;
  cout << "R file     : " << fileRL << endl;
  cout << "Xmgr files : " << fileXmgrR << endl;
  cout << "           : " << fileXmgrL << endl;
  cout << " " << endl;

  //----------------------------------------------------------------
  // Get the data.
  //----------------------------------------------------------------
  getparam();


  time_t start_time, end_time;
  time_t tme1, tme2;

  time(&start_time);
  tme1 = start_time;

  /************ define variables *******************/

  const   int  NN = 2*Nc*Nw+Nc;
  int          i, j, m, n, k, n0, j1, i1, ix, i2, j2, k2;
  int          jn, kn, jm, km, jx, iy;
  double       t, tp, xt, yt, dlu;
  Complex      tmp, tmp0, sum, sum0, tmpsum, suc;
  Vector       xcnt(Nws), ycnt(Nws);
  Vector       ptr(Ngr), whr(Ngr), tr(Nrt), wr(Nrt);
  Vector       twr(Nrt), wwr(Nrt);
  Vector       twx(Nwx), wwx(Nwx), twy(Nwy), wwy(Nwy);
  Vector       ptwx(Nwx), whwx(Nwx), ptwy(Nwy), whwy(Nwy);
  Vector       sxc4(Nc4), sc4(Nc4), wxc4(Nc4), wc4(Nc4);
  Vector       ts(Nw), tn(Nw), Indx;
  Matrix       uns(3, Nw);
  CmplxVector  Rhsw(NN), Jnj(NN), xc;
  CmplxMatrix  Pw(NN, NN), Pwt(NN, NN);

  Vector       Res(Nc), Ind(Nc), sum1(Nc);
  Matrix       Res1(Nc, Nc), Ind1(Nc, Nc), matout;
  CmplxVector  sum2(Nc);
  double       rsum, isum, tmpsum1, tmpsum2, tst, tempc;
  Complex      tmpv0, tmpvi, tmpu0, tmpui, tmp0v0, tmp0vi, tmp0u0, tmp0ui;
  Complex      sumv0, sumvi, sumu0, sumui, temsum;

  ofstream *xmgrRdata;
  ofstream *xmgrLdata;
  ofstream *rldata;
  ofstream *outdata;


  // Check if the file exists.  If it doesn't, the xmgr files must be
  // initialized.

  int init = 0;
  if ( ! fopen (fileXmgrR, "r") )
    {
      cout << fileXmgrR << " does not exist so must initialize" << endl;
      init = 1;
    }
  if ( append )
    {
      outdata = new ofstream(fileOut, ios::app | ios::out);
      rldata = new ofstream(fileRL, ios::app | ios::out);
      xmgrRdata = new ofstream(fileXmgrR, ios::app | ios::out);
      xmgrLdata = new ofstream(fileXmgrL, ios::app | ios::out);
    }
  else
    {
      init = 1;

      outdata = new ofstream( fileOut, ios::out);

      rldata = new ofstream( fileRL, ios::out);

      xmgrRdata = new ofstream( fileXmgrR, ios::out);

      xmgrLdata = new ofstream( fileXmgrR, ios::out);
    }

  if ( init )
    {
      cout << "Initializing the xmgr files" << endl;
      *xmgrRdata << "@ title \"Resistance\"" << endl;
      *xmgrRdata << "@ subtitle \"" << fileXmgrR << "\"" << endl;
      *xmgrRdata << "@ yaxis label \"Ohm/m\"" << endl;
      *xmgrRdata  << "@ xaxis label \"Frequency (Hz)\"" << endl;
      *xmgrRdata << "@ legend string 0 \"Signal1,Signal1\"" << endl;
      *xmgrRdata << "@ legend string 1 \"Signal1,Signal2\"" << endl;
      *xmgrRdata << "@ legend string 2 \"Signal2,Signal1\"" << endl;
      *xmgrRdata << "@ legend string 3 \"Signal2,Signal2\"" << endl;
      *xmgrRdata << "@ type nxy" << endl;
      cout  << "@ title \"Resistance\"" << endl;
      cout  << "@ subtitle \"" << fileXmgrR << "\"" <<  endl;
      cout  << "@ yaxis label \"nH/m\"" << endl;
      cout  << "@ xaxis label \"Frequency (Hz)\"" << endl;
      cout  << "@ legend string 0 \"Signal1,Signal1\"" << endl;
      cout  << "@ legend string 1 \"Signal1,Signal2\"" << endl;
      cout  << "@ legend string 2 \"Signal2,Signal1\"" << endl;
      cout  << "@ legend string 3 \"Signal2,Signal2\"" << endl;
      cout  << "@ type nxy" << endl;

      *xmgrLdata << "@ title \"Inductance\"" << endl;
      *xmgrLdata << "@ subtitle \"" << fileXmgrL << "\"" << endl;
      *xmgrLdata << "@ yaxis label \"Ohm/m\"" << endl;
      *xmgrLdata << "@ legend string 0 \"Signal1,Signal1\"" << endl;
      *xmgrLdata << "@ legend string 1 \"Signal1,Signal2\"" << endl;
      *xmgrLdata << "@ legend string 2 \"Signal2,Signal1\"" << endl;
      *xmgrLdata << "@ legend string 3 \"Signal2,Signal2\"" << endl;
      *xmgrLdata << "@ type nxy" << endl;
      cout  << "@ subtitle \"" << fileXmgrL << "\"" << endl;
      cout  << "@ yaxis label \"nH/m\"" << endl;
      cout  << "@ legend string 0 \"Signal1,Signal1\"" << endl;
      cout  << "@ legend string 1 \"Signal1,Signal2\"" << endl;
      cout  << "@ legend string 2 \"Signal2,Signal1\"" << endl;
      cout  << "@ legend string 3 \"Signal2,Signal2\"" << endl;
      cout  << "@ type nxy" << endl;
    }

  //  cout << "start" << endl;

  getcoif4( sxc4, sc4, wxc4, wc4, Nit );
  getsbs( uns );

  /** points and weights for Gauss the method */

  gauleg( -1.0, 1.0, ptwx, whwx );
  gauleg( -1.0, 1.0, ptwy, whwy );

  for(i = 0; i < Nw; i++) {

    tn[i] = i*step_w;

  }

  //  time(&tme2);
  //  cout << "time 1: " << tme2 - tme1 << endl;
  //  tme1 = tme2;


  int idx, idx2, jdx, jdx2;

  /***** to create system matrix **************/

  for(i1 = 0; i1 < Nc; i1 ++) {

    //    cout << i1 << endl;
    double hcircum = 0.5 * Circum[i1];
    for(i = 0; i < Nw; i ++) {

      idx = i1*Nw+i;
      idx2 =(Nc+i1)*Nw+i;
      for(j1 = 0; j1 < Nc; j1 ++) {

	tmp2 = step_w*Circum[j1];

	for(j = 0; j < Nw; j ++) {

	  jdx = j1*Nw+j;
	  jdx2 = (Nc+j1)*Nw+j;

	  sumv0 = 0.0;
	  sumu0 = 0.0;
	  sumvi = 0.0;
	  sumui = 0.0;

	  if(i1 == j1 && i == j) {

	    for(m = 0; m < Nws; m ++) {
	      tmp3 = double(m) + 0.5;
	      tmp3 *= hw;
	      //	      xcnt[m] = uns(1,j) + hw * tmp3;
	      //	      ycnt[m] = uns(1,i) + hw * tmp3;
	      xcnt[m] = uns(1,j) + tmp3;
	      ycnt[m] = uns(1,i) + tmp3;

	    }

	    for(iy = 0; iy < Nws; iy ++) {

	      for(jx = 0; jx < Nws; jx ++) {

		intpnt( xcnt[jx], twx, wwx, ptwx, whwx,
			ycnt[iy], twy, wwy, ptwy, whwy,
			hwh );

		tmpv0 = 0.0;
		tmpu0 = 0.0;
		tmpvi = 0.0;
		tmpui = 0.0;

		for(m = 0; m < Nwy; m ++) {

		  tmp0v0 = 0.0;
		  tmp0u0 = 0.0;
		  tmp0vi = 0.0;
		  tmp0ui = 0.0;

		  for( n = 0; n < Nwx; n++ ) {
		    tmp3 = wwx[n] * scalcoif4( twx[n], J, j, sc4 );
		    tmp0v0 += kernel_v0( twy[m], twx[n], i1, j1 ) *
		      tmp3;
		    tmp0u0 += kernel_u0( twy[m], twx[n], i1, j1 ) *
		      tmp3;
		    tmp0vi += kernel_vi( twy[m], twx[n], i1, j1 ) *
		      tmp3;
		    tmp0ui += kernel_ui( twy[m], twx[n], i1, j1 ) *
		      tmp3;

		  }

		  tmp3 = wwy[m] * scalcoif4( twy[m], J, i, sc4 );
		  tmpv0 += tmp0v0 * tmp3;
		  tmpu0 += tmp0u0 * tmp3;
		  tmpvi += tmp0vi * tmp3;
		  tmpui += tmp0ui * tmp3;

		}

		sumv0 += tmpv0;
		sumu0 += tmpu0;
		sumvi += tmpvi;
		sumui += tmpui;

	      }
	    }
	    tmp1 = Circum[j1] * Circum[i1];
	    sumv0 = sumv0 * tmp1;
	    sumvi = sumvi * tmp1;
	    sumu0 = sumu0 * tmp1 - hcircum;
	    sumui = sumui * tmp1 + hcircum;

	  } else if (i1 == j1 && i != j) {

	    tmp3 = tmp2 * Circum[i1];
	    sumv0 = kernel_v0( tn[i], tn[j], i1, j1 ) * tmp3;
	    sumvi = kernel_vi( tn[i], tn[j], i1, j1 ) * tmp3;
	    sumu0 = kernel_u0( tn[i], tn[j], i1, j1 ) * tmp3;
	    sumui = kernel_ui( tn[i], tn[j], i1, j1 ) * tmp3;

	  } else {

	    tmp3 = tmp2 * Circum[i1];
	    sumv0 = kernel_v0( tn[i], tn[j], i1, j1 ) * tmp3;
	    sumvi = 0.0;
	    sumu0 = kernel_u0( tn[i], tn[j], i1, j1 ) * tmp3;
	    sumui = 0.0;

	  }

	  Pwt(idx, jdx) = sumv0;
	  Pwt(idx2, jdx) = sumvi;
	  Pwt(idx, jdx2) = -sumu0;
	  Pwt(idx2, jdx2) = -sumui;

	}
      }
    }
  }

  //  time(&tme2);
  //  cout << "time 2: " << tme2 - tme1 << endl;
  //  tme1 = tme2;

  for(i = 0; i < Nc; i ++) {

    if(i < Gnd) {
      tempc = Gndc[i];
    } else if(i >= Gnd && i < (Gnd + Rec)) {
      tempc = Recc[i-Gnd];
    } else if(i >= (Gnd + Rec) && i < (Gnd + Rec + Cir)) {
      tempc = Circ[i-Gnd-Rec];
    } else if(i >= (Gnd + Rec + Cir) && i < (Gnd + Rec + Cir + Tra)) {
      tempc = Trac[i-Gnd-Rec-Cir];
    }

    for(j = 0; j < Nw; j ++) {

      Pwt(2*Nw*Nc+i, i*Nw+j) = tempc * tempc;
      Pwt(i*Nw+j, 2*Nw*Nc+i) = tempc / power(2.0, double(J)/2.0);

    }

  }

  //  time(&tme2);
  //  cout << "time 3: " << tme2 - tme1 << endl;
  //  tme1 = tme2;


  //  cout << NN
  //       << "X"
  //       << NN
  //       << " System Matrix Created !!! "
  //       << endl;

  /* Solve equation for different current combination */

  for(i2 = 1; i2 < Nc; i2 ++) {

    for(j2 = i2; j2 < Nc; j2 ++) {

      ///      cout << i2 << endl;
      //      cout << j2 <<endl;

      if(i2 == j2) {

	Iq_sq[0] = -1.0;

	for (k2 = 1; k2 < Nc; k2 ++) {
	  if(k2 == j2) {
	    Iq_sq[k2] = 1.0;
	  } else {
	    Iq_sq[k2] = 0.0;
	  }
	}

      } else {

	for (k2 = 0; k2 < Nc; k2 ++) {
	  if(k2 == i2) {
	    Iq_sq[k2] = 1.0;
	  } else if(k2 == j2){
	    Iq_sq[k2] = -1.0;
	  } else {
	    Iq_sq[k2] = 0.0;
	  }
	}

      }

      for(i = 0; i < 2*Nc*Nw; i ++) {
	Rhsw[i] = 0.0;
      }

      for(i = 0; i < Nc; i ++) {
	if(i < Gnd) {
	  tempc = Gndc[i];
	} else if(i >= Gnd && i < (Gnd + Rec)) {
	  tempc = Recc[i-Gnd];
	} else if(i >= (Gnd + Rec) && i < (Gnd + Rec + Cir)) {
	  tempc = Circ[i-Gnd-Rec];
	} else if(i >= (Gnd + Rec + Cir) && i < (Gnd + Rec + Cir + Tra)) {
	  tempc = Trac[i-Gnd-Rec-Cir];
	}
	Rhsw[2*Nc*Nw+i] = (-1.0)*Ic*Iq_sq[i]*oms*tempc/power(2.0,double(J)/2.0);
      }

      //  time(&tme2);
      //  cout << "time 4: " << tme2 - tme1 << endl;
      //  tme1 = tme2;

  //      cout << endl
  //	   << "Right hand side vector created!! "
  //	   << endl;

      for(i = 0; i < NN; i ++) {

	for(j = 0; j < NN; j ++) {

	  Pw(i,j) = Pwt(i,j);

	}

      }

      /*  By Using LU Decomposition  */
      if(matr == 1) {
	ludcmp( Pw, Indx, dlu );
	lubksb( Pw, Rhsw, Indx );
      }

      /*  By Using Stab */
      if(matr == 2) {
	xc.resize(NN);
	bi_cgstab_c( Pw, xc, Rhsw, EPS );
	Rhsw = xc;
      }

      //      cout << " System of linear equations for N = "
      //	   << NN
      //	   << " solved !!! "
      //	   << endl;

      /* To calculate the resistance and inductance ****/

      for(i = 0; i < Nw; i ++) {

	ts[i] = double(i)/double(Nw);

      }

      for(ix = 0; ix < Nc; ix ++) {

	for(i = 0; i < Nw; i ++) {

	  tmp = 0.0;
	  tmp0 = 0.0;

	  for(j = i-7; j <= i+4; j ++) {

	    if(j < 0) {
	      tst = ts[i] + 1.0;
	      j1 = j + Nw;
	    } else if (j > Nw-1) {
	      tst = ts[i] - 1.0;
	      j1 = j - Nw;
	    } else {
	      tst = ts[i];
	      j1 = j;
	    }

	    tmp += scalcoif4( tst, J, j1, sc4 ) * Rhsw[ix*Nw+j1];
	    tmp0 += scalcoif4( tst, J, j1, sc4 ) * Rhsw[(ix+Nc)*Nw+j1];

	  }

	  Jnj[ix*Nw+i] = tmp;
	  Jnj[(ix+Nc)*Nw+i] = tmp0;

	}

      }

      for (i = 0; i < Nc; i ++) {

	Jnj[2*Nc*Nw+i] = Rhsw[2*Nc*Nw+i];

      }

      for (i = 0; i < Nc; i ++) {

	sum1[i] = 0.0;
	Res[i] = 0.0;
	Ind[i] = 0.0;

      }

      //  time(&tme2);
      //  cout << "time 5: " << tme2 - tme1 << endl;
      //  tme1 = tme2;


      rsum = 0.0;
      isum = 0.0;

      for (i = 0; i < Nc; i ++) {
	tmp4 = Circum[i] / double(Nw);
	tmpsum1 = 0.0;
	tmpsum2 = 0.0;
	sum2[i] = cmplx( 0.0, 0.0 );

	for (j = 0; j < Nw; j ++) {

	  tmpsum1 += imag(Jnj[(i+Nc)*Nw+j] * conjg(Jnj[i*Nw+j])) * tmp4;
	  tmpsum2 += real(Jnj[2*Nc*Nw+i] * conjg(Jnj[i*Nw+j])) * tmp4;
	  sum2[i] += Jnj[i*Nw+j] * Circum[i] / double(Nw);

	}

	rsum += tmpsum1;
	isum += tmpsum2;

	tmp4 = cabs(sum2[i]);
	sum1[i] = tmp4 * tmp4;

      }

      for (i = 0; i < Nc; i++) {

	Res[i] = omegamu * rsum / sum1[i];
	Ind[i] = (-1) * mu * isum / sum1[i];

      }

      if(i2 == j2) {
	Res1(i2,i2) = Res[i2];
	Ind1(i2,i2) = Ind[i2];

      } else {
	Res1(i2,j2) = Res[i2];
	Ind1(i2,j2) = Ind[i2];
	Res1(j2,i2) = Res[j2];
	Ind1(j2,i2) = Ind[j2];

      }

    }

  }

  //  time(&tme2);
  //  cout << "time 6: " << tme2 - tme1 << endl;
  //  tme1 = tme2;

  /* Generate mutual resistance and inductance */

  for(i2 = 1; i2 < Nc; i2 ++) {

    for(j2 = 1; j2 < Nc; j2 ++) {

      if(i2 != j2) {

	Res1(i2,j2) = (Res1(i2,i2)+Res1(j2,j2)-Res1(i2,j2))/2.0;
	Ind1(i2,j2) = (Ind1(i2,i2)+Ind1(j2,j2)-Ind1(i2,j2))/2.0;

      }

    }

  }


  time(&end_time);
  //  cout << "time 7: " << end_time - tme1 << endl;
  //  tme1 = tme2;


  //  cout << "Start-time: " << start_time << endl;
  //  cout << "  End-time: " << end_time << endl;
  cout << "  Run-time: " << (end_time - start_time) << " seconds" << endl;

#ifdef HAVE_SYS_TIMES_H
  tms run_times;

  times(&run_times);

  cout << "User CPU time: " << run_times.tms_utime/CLOCKS_PER_SEC << endl;
  cout << " Sys CPU time: " << run_times.tms_stime/CLOCKS_PER_SEC << endl;
#endif

  /* output data into file */

  *outdata << "--------------------------------------------------------"
	   << endl;
  *outdata << "Resistance/Inductance Values" << endl;
  *outdata << "Input file: " << fileIn << endl;
  *outdata << "--------------------------------------------------------"
	   << endl;

  *outdata << "Frequency: " << freq << "     Conductivity: " <<
    sigma << "\n" << endl;

  *outdata << Nc-1 << ""
	   << "X" << ""
	   << Nc-1 << ""
	   << " Resistance Matrix (Ohm/m):" << endl;
  *outdata <<endl;

  for(i2 = 1; i2 < Nc; i2++) {

    for(j2 = 1; j2 < Nc; j2++) {

      *outdata << Res1(i2,j2) << "     ";
      *rldata << freq << " " << (Ind1(i2,j2) * 1e9) << " " <<
	Res1(i2,j2) << endl;

    }

    *outdata << endl;
    *outdata << endl;

    *xmgrRdata << freq;
    for(i2 = 1; i2 < Nc; i2++)
      {

      for(j2 = 1; j2 < Nc; j2++)
	*xmgrRdata << " " << Res1(i2,j2);

      }
    *xmgrRdata << endl;


  }

  *outdata << Nc-1 << ""
	   << "X" << ""
	   << Nc-1 << ""
	   << " Inductance Matrix (H/m):" << endl;
  *outdata <<endl;

  for(i2 = 1; i2 < Nc; i2 ++) {

    for(j2 = 1; j2 < Nc; j2 ++) {

      *outdata << Ind1(i2,j2) << "     ";

    }
    *outdata << endl;
    *outdata << endl;

  }

  *outdata << endl;
  *outdata << endl;

  *xmgrLdata << freq;
  for(i2 = 1; i2 < Nc; i2++)
    {

      for(j2 = 1; j2 < Nc; j2++)
	*xmgrLdata << " " << Ind1(i2,j2) * 1e9;

    }
  *xmgrLdata << endl;

  *outdata << endl;
#ifdef HAVE_SYS_TIMES_H
  *outdata << "User CPU time: " << run_times.tms_utime/CLOCKS_PER_SEC << endl;
  *outdata << " Sys CPU time: " << run_times.tms_stime/CLOCKS_PER_SEC << endl;
#endif

  outdata->close();
  rldata->close();
  xmgrRdata->close();
  xmgrLdata->close();

  cout << "Calculation Completed." << endl;

  return 0;

}
