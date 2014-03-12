/***********************************************************/
/***                                                     ***/ 
/***   Resistance and Inductance using Coifman wavelets  ***/
/***            for the method of moments                ***/
/***                                                     ***/
/***********************************************************/

#include "stdafx.h"

/********************** Global variables *******************/

   int     Nc;              /* number of conductors ********/
   int     Die, Rec, Cir, Tri, Ell, Pol, Gnd, Tra;

   Matrix  Gnddi;
   Matrix  Recdi;           
   Matrix  Cirdi;
   Matrix  Tradix;          /* Trapezoid x coordinates *****/
   Matrix  Tradiy;

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

   double  freq, sqrtoms,sigma;
   double  omega,oms,omegamu;
   double  sft;

   int     matr, Mt, Np;
   int     Ns, Nsr, Ngx, Ngy, Nxt, Nyt, Ngr, Nrt;
   int     Nh, Nc4, Nit, J, Nw, Nwx, Nwy, Nws;
   double  hsrw;
   double  Pi, Pih, Pit, wn, mu, cf;
   double  step_w, hw, hwh, mmax;
   Complex Ic;
   double  EPS;
   Vector  Iq_sq;
   double  Uniti;

/********************** Main program ***********************/

int main()
{

    /************ to setup global parameters *********/
    
    getparam();

    /************ global variables *******************/

    int          i, j, m, n, k, n0, j1, i1, ix, i2, j2, k2;
    int          jn, kn, jm, km, jx, iy;
    double       dlu;
    double       t, tp, xt, yt;
    Complex      tmp, tmp0, sum, sum0, tmpsum, suc;
    Vector       xcnt(Nws), ycnt(Nws);
    Vector       ptr(Ngr), whr(Ngr), tr(Nrt), wr(Nrt);
    Vector       twr(Nrt), wwr(Nrt);
    Vector       twx(Nwx), wwx(Nwx), twy(Nwy), wwy(Nwy);
    Vector       ptwx(Nwx), whwx(Nwx), ptwy(Nwy), whwy(Nwy);
    Vector       sxc4(Nc4), sc4(Nc4), wxc4(Nc4), wc4(Nc4);
    Vector       ts(Nw), tn(Nw);
    Vector       Indx;
    Matrix       uns(3,Nw);
    CmplxVector  Rhsw(2*Nc*Nw+Nc), Jnj(2*Nc*Nw+Nc), xc;
    CmplxMatrix  Pw(2*Nc*Nw+Nc, 2*Nc*Nw+Nc), Pwt(2*Nc*Nw+Nc,2*Nc*Nw+Nc);
    Matrix       matout;

    Vector       Res(Nc),Ind(Nc);
    Matrix       Res1(Nc,Nc),Ind1(Nc,Nc);
    Vector       sum1(Nc);
    CmplxVector  sum2(Nc);
    double       rsum,isum;
    double       tmpsum1;
    double       tmpsum2;
    Complex      temsum;
    Complex      tmpv0,tmpvi,tmpu0,tmpui,tmp0v0,tmp0vi,tmp0u0,tmp0ui;
    Complex      sumv0,sumvi,sumu0,sumui;
    double       tst;
    double       tempc;
    
    ofstream  outdata( "data.out", ios::out );

    cout<<"start"<<endl;

    getcoif4( sxc4, sc4, wxc4, wc4, Nit );
    getsbs( uns );
    
    /** points and weights for Gauss the method */
    
    gauleg( -1.0, 1.0, ptwx, whwx );
    gauleg( -1.0, 1.0, ptwy, whwy );

    for( i = 0; i < Nw; i++ ) {
      
      tn[i] = i*step_w;
      
    }
    
    /***** to create system matrix **************/

    for( i1 = 0; i1 < Nc; i1++ ) {
      
      cout<<i1<<endl;
      
      for( i = 0; i < Nw; i++ ) {
	
	for( j1 = 0; j1 < Nc; j1++ ) {
	  
	  for( j = 0; j < Nw; j++ ) {
 
	    sumv0 = 0.0;
	    sumu0 = 0.0;
	    sumvi = 0.0;
	    sumui = 0.0;

	    if( i1 ==j1 && i == j ) {
	      
	      for( m = 0; m < Nws; m++ ) {
		
		xcnt[m] = uns(1,j)+hw*(double(m)+0.5);
		ycnt[m] = uns(1,i)+hw*(double(m)+0.5);
		
	      }
	      
	      for( iy = 0; iy < Nws; iy++ ) {
		
		for( jx = 0; jx < Nws; jx++ ) {
		  
		  intpnt( xcnt[jx], twx, wwx, ptwx, whwx,
			  ycnt[iy], twy, wwy, ptwy, whwy,
			  hwh );
		  
		  tmpv0 = 0.0;
		  tmpu0 = 0.0;
		  tmpvi = 0.0;
		  tmpui = 0.0;
		  
		  for( m = 0; m < Nwy; m++ ) {
		    
		    tmp0v0 = 0.0;
		    tmp0u0 = 0.0;
		    tmp0vi = 0.0;
		    tmp0ui = 0.0;
		    
		    for( n = 0; n < Nwx; n++ ) {
		      
		      tmp0v0 += kernel_v0( twy[m], twx[n], i1, j1 )*
			wwx[n]*scalcoif4( twx[n], J, j, sc4 );
		      tmp0u0 += kernel_u0( twy[m], twx[n], i1, j1 )*
			wwx[n]*scalcoif4( twx[n], J, j, sc4 );
		      tmp0vi += kernel_vi( twy[m], twx[n], i1, j1 )*
			wwx[n]*scalcoif4( twx[n], J, j, sc4 );
		      tmp0ui += kernel_ui( twy[m], twx[n], i1, j1 )*
			wwx[n]*scalcoif4( twx[n], J, j, sc4 );
		      
		    }
		    
		    tmpv0 += tmp0v0*wwy[m]*scalcoif4( twy[m],J,i,sc4 );
		    tmpu0 += tmp0u0*wwy[m]*scalcoif4( twy[m],J,i,sc4 );
		    tmpvi += tmp0vi*wwy[m]*scalcoif4( twy[m],J,i,sc4 );
		    tmpui += tmp0ui*wwy[m]*scalcoif4( twy[m],J,i,sc4 );
		    
		  }
		  
		  sumv0 += tmpv0;
		  sumu0 += tmpu0;
		  sumvi += tmpvi;
		  sumui += tmpui;
		  
		}
		
	      }
	      
	      sumv0 = sumv0*Circum[j1]*Circum[i1];
	      sumvi = sumvi*Circum[j1]*Circum[i1];
	      sumu0 = sumu0*Circum[j1]*Circum[i1]-0.5*Circum[i1];
	      sumui = sumui*Circum[j1]*Circum[i1]+0.5*Circum[i1];
	      
	    } else if ( i1 == j1 && i != j ) {

	      sumv0 = kernel_v0( tn[i], tn[j], i1, j1 )*step_w*Circum[j1]*Circum[i1];
	      sumvi = kernel_vi( tn[i], tn[j], i1, j1 )*step_w*Circum[j1]*Circum[i1];
	      sumu0 = kernel_u0( tn[i], tn[j], i1, j1 )*step_w*Circum[j1]*Circum[i1];
	      sumui = kernel_ui( tn[i], tn[j], i1, j1 )*step_w*Circum[j1]*Circum[i1];
	      
	    } else {

	      sumv0 = kernel_v0( tn[i], tn[j], i1, j1 )*step_w*Circum[j1]*Circum[i1];
	      sumvi = 0.0;
	      sumu0 = kernel_u0( tn[i], tn[j], i1, j1 )*step_w*Circum[j1]*Circum[i1];
	      sumui = 0.0;

	    }
	    
	    Pwt(i1*Nw+i,j1*Nw+j) = sumv0;
	    Pwt((Nc+i1)*Nw+i,j1*Nw+j) = sumvi;
	    Pwt(i1*Nw+i,(Nc+j1)*Nw+j) = (-1.0)*sumu0;
	    Pwt((Nc+i1)*Nw+i,(Nc+j1)*Nw+j) = (-1.0)*sumui;

	  }
	}
      }
    }
    
    for( i = 0; i < Nc; i++ ) {
      
      if( i < Gnd ) {
	tempc=Gndc[i];
      } else if( i >= Gnd && i < (Gnd + Rec) ) {
	tempc=Recc[i-Gnd];
      } else if( i >= (Gnd + Rec) && i < (Gnd + Rec + Cir) ) {
	tempc=Circ[i-Gnd-Rec];
      } else if( i >= (Gnd + Rec + Cir) && i < (Gnd + Rec + Cir + Tra) ) {
	tempc=Trac[i-Gnd-Rec-Cir];
      }

      for( j = 0; j < Nw; j++ ) {

	Pwt(2*Nw*Nc+i,i*Nw+j)=sqv(tempc);
	Pwt(i*Nw+j,2*Nw*Nc+i)=tempc/power(2.0,double(J)/2.0);

      }
      
    }

    cerr << 2*Nc*Nw+Nc
	 << "X"
	 << 2*Nc*Nw+Nc
	 << " System Matrix Created !!! " 
         << endl;
    
    /* Solve equation for different current combination */

    for( i2 = 1; i2 < Nc; i2++ ) {
      
      for( j2 = i2; j2 < Nc; j2++ ) {

	cout<<i2<<endl;
	cout<<j2<<endl;

	if( i2==j2 ) {

	  Iq_sq[0] = -1.0;

	  for ( k2=1; k2 < Nc; k2++) {
	    if( k2==j2 ) {
	      Iq_sq[k2]=1.0;
	    } else {
	      Iq_sq[k2]=0.0;
	    }
	  }
	  
	} else {

	  for ( k2=0; k2 < Nc; k2++) {
	    if( k2==i2 ) {
	      Iq_sq[k2]=1.0;
	    } else if( k2==j2 ){
	      Iq_sq[k2]=-1.0;
	    } else {
	      Iq_sq[k2]=0.0;
	    }
	  }

	}

	for(i=0;i<2*Nc*Nw;i++) {
	  Rhsw[i] = 0.0;
	} 

	for(i=0;i<Nc;i++) {
	  if( i < Gnd ) {
	    tempc=Gndc[i];
	  } else if( i >= Gnd && i < (Gnd + Rec) ) {
	    tempc=Recc[i-Gnd];
	  } else if( i >= (Gnd + Rec) && i < (Gnd + Rec + Cir) ) {
	    tempc=Circ[i-Gnd-Rec];
	  } else if( i >= (Gnd + Rec + Cir) && i < (Gnd + Rec + Cir + Tra) ) {
	    tempc=Trac[i-Gnd-Rec-Cir];
	  }
	  Rhsw[2*Nc*Nw+i]=(-1.0)*Ic*Iq_sq[i]*oms*tempc/power(2.0,double(J)/2.0);	  
	}
            	
	cerr << endl
	     << " Right hand side vector created!! "
	     << endl;

	for( i = 0; i < 2*Nc*Nw+Nc; i++ ) {
	  
	  for( j = 0; j < 2*Nc*Nw+Nc; j++ ) {
	    
	    Pw(i,j)=Pwt(i,j);

	  }

	}
      
	/*  By Using LU Decomposition  */
	if( matr == 1 ) {
	  
	  ludcmp( Pw, Indx, dlu );
	  lubksb( Pw, Rhsw, Indx );
	}
	
	/*  By Using Stab */
	if( matr == 2 ) {
	  
	  xc.resize(2*Nc*Nw+Nc);
	  bi_cgstab_c( Pw, xc, Rhsw, EPS );
	  Rhsw = xc;
	}
	
	cerr << " System of linear equations for N = " 
	     << 2*Nc*Nw+Nc                
	     << " solved !!! " 
	     << endl;
	
	/* To calculate the resistance and inductance ****/
	
	for( i = 0; i < Nw; i++ ) {
	  
	  ts[i] = double(i)/double(Nw);
	  
	}
	
	for( ix = 0; ix < Nc; ix++ ) {        // to be revised by Nc
	  
	  for( i = 0; i < Nw; i++ ) {
	    
	    tmp = 0.0;
	    tmp0 = 0.0;
	    
	    for( j = i-7; j <= i+4; j++ ) {
	      
	      if( j < 0 ) {
		tst = ts[i] + 1.0;
		j1 = j + Nw;
	      } else if ( j > Nw-1) {
		tst = ts[i] - 1.0;
		j1 = j - Nw;
	      } else {
		tst = ts[i];
		j1 = j;
	      }
	      
	      tmp += scalcoif4( tst, J, j1, sc4 )*Rhsw[ix*Nw+j1];
	      tmp0 += scalcoif4( tst, J, j1, sc4 )*Rhsw[(ix+Nc)*Nw+j1];
	      
	    }
	    
	    Jnj[ix*Nw+i]=tmp;
	    Jnj[(ix+Nc)*Nw+i]=tmp0;
	    
	  }
	  
	}

	for ( i = 0; i < Nc; i++ ) {

	  Jnj[2*Nc*Nw+i]=Rhsw[2*Nc*Nw+i];

	}
	
	for ( i = 0; i < Nc; i++ ) {
	  
	  sum1[i] = 0.0;
	  Res[i] = 0.0;
	  Ind[i] = 0.0;
	  
	}
	
	rsum = 0.0;
	isum = 0.0;
	
	for ( i=0; i<Nc; i++ ) {
	  
	  tmpsum1 = 0.0;
	  tmpsum2 = 0.0;
	  sum2[i] = cmplx( 0.0, 0.0 );
	  
	  for ( j=0; j<Nw; j++ ) {
	    
	    tmpsum1 += imag(Jnj[(i+Nc)*Nw+j]*conjg(Jnj[i*Nw+j]))*Circum[i]/double(Nw);
	    tmpsum2 += real(Jnj[2*Nc*Nw+i]*conjg(Jnj[i*Nw+j]))*Circum[i]/double(Nw);
	    sum2[i] += Jnj[i*Nw+j]*Circum[i]/double(Nw);
	    
	  }
	  
	  rsum += tmpsum1;
	  isum += tmpsum2;
	  
	  sum1[i]=sqv(cabs(sum2[i]));
	  
	}
	
	for ( i=0; i<Nc; i++ ) {
	  
	  Res[i]=omegamu*rsum/sum1[i];
	  Ind[i]=(-1)*mu*isum/sum1[i];

	}   
	
	if( i2 == j2 ) {
	  Res1(i2,i2)=Res[i2];
	  Ind1(i2,i2)=Ind[i2];

	} else {
	  Res1(i2,j2)=Res[i2];
	  Ind1(i2,j2)=Ind[i2];
	  Res1(j2,i2)=Res[j2];  
	  Ind1(j2,i2)=Ind[j2];  

	}
	
      }

    }

    for( i2 = 1; i2 < Nc; i2++ ) {
      
      for( j2 = 1; j2 < Nc; j2++ ) {

	if(i2!=j2) {

	  Res1(i2,j2)=(Res1(i2,i2)+Res1(j2,j2)-Res1(i2,j2))/2.0;
	  Ind1(i2,j2)=(Ind1(i2,i2)+Ind1(j2,j2)-Ind1(i2,j2))/2.0;
	  
	}
	
      }
  
    }

    outdata << Nc-1 << ""
	    << "X" << ""
	    << Nc-1 << ""
	    << " Resistance Matrix (Ohm/m):" << endl;
    outdata <<endl;

    for( i2 = 1; i2 < Nc; i2++ ) {
      
      for( j2 = 1; j2 < Nc; j2++ ) {
	
	outdata <<Res1(i2,j2)<< "     ";
	
      }

      outdata <<endl;
      outdata <<endl;

    }

    outdata << Nc-1 << ""
	    << "X" << ""
	    << Nc-1 << ""
	    << " Inductance Matrix (H/m):" << endl;
    outdata <<endl;

    for( i2 = 1; i2 < Nc; i2++ ) {

      for( j2 = 1; j2 < Nc; j2++ ) {

	outdata <<Ind1(i2,j2)<< "     ";

      }
      outdata <<endl;
      outdata <<endl;

    }

    cout<<"Calculation Completed."<<endl;

    return 0;

}   
