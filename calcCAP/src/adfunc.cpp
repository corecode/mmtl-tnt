#include "stdafx.h"

extern  double   eps0;
extern  Vector   eps;
extern  Vector   d;

extern  int      Nc, Nd;
extern  int      Die, Rec, Cir, Gnd, Tra;

extern  Matrix   Gnddi, Recdi, Cirdi, Tradix, Tradiy;

extern  Vector   a, b, c;
extern  Vector   at, bt, ct;

extern  Vector   Circum;
extern  Vector   Diec, Recc, Circ, Gndc, Trac;

extern  int      matr, Np;
extern  int      Nh, Nc4, Nit, J, Nw, Nwx, Nwy, Nws;
extern  double   Pi, Pih, Pit, cf;
extern  double   step_w, hw, hwh;
extern  double   EPS;
extern  Complex  Ic;
extern  int      od;
extern  double   maxga;
extern  double   maxerr;

double rc(int i, int j) {
  if( i == Nd && j == Nd+1 ) {
    return(0.0);
  } else if( i == 1 && j == 0 ) {
    return(-1.0);
  } else {
    return ((eps[i]-eps[j])/(eps[i]+eps[j]));
  }
}

double tc(int i, int j) {
  if( i == Nd && j == Nd+1 ) {
    return(1.0);
  } else if( i == 1 && j == 0 ) {
    return(0.0);
  } else {
    return (2.0*eps[i]/(eps[i]+eps[j]));
  }
}

double grcplus(int i, double gamma) {

  double tmp;

  if( i == Nd ) {
    return(0.0);
  } else if( i < Nd && i > 0 ) {
    tmp = rc(i,i+1)+grcplus(i+1,gamma)*exp(2.0*gamma*(d[i]-d[i+1]));
    tmp = tmp/(1.0+rc(i,i+1)*grcplus(i+1,gamma)*exp(2.0*gamma*(d[i]-d[i+1])));
    return(tmp);
  } else {
    cerr<<"Error in grcplus"<<endl;
    return(-1.0);
  }

}

double grcminus(int i, double gamma) {

  double tmp;

  if( i == 1 ) {
    return(-1.0);
  } else if( i > 1 && i <= Nd ) {
    tmp = rc(i,i-1)+grcminus(i-1,gamma)*exp(2.0*gamma*(d[i-2]-d[i-1]));
    tmp = tmp/(1.0+rc(i,i-1)*grcminus(i-1,gamma)*exp(2.0*gamma*(d[i-2]-d[i-1])));
    return(tmp);
  } else{
    cerr<<"Error in Grcminus"<<endl;
    return(0.0);
  }

}

double splus(int i, double gamma) {

  double tmp;
  
  tmp = 1.0-rc(i+1,i)*grcplus(i+1,gamma)*exp(2.0*gamma*(d[i]-d[i+1]));
  return(tc(i,i+1)/tmp);

}

double sminus(int i, double gamma) {

  double tmp;
  
  tmp = 1.0-rc(i-1,i)*grcminus(i-1,gamma)*exp(2.0*gamma*(d[i-2]-d[i-1]));
  return(tc(i,i-1)/tmp);

}

double Mm(int m, double gamma) {

  double tmp;
  
  tmp = 1.0-grcminus(m,gamma)*grcplus(m,gamma)*exp(2.0*gamma*(d[m-1]-d[m]));
  return(1.0/tmp);
  
}

double Kplus(int m, int n, double gamma) {
  
  int    i;
  double tmp=1.0;

  for( i=m; i<=n-1; i++ ) {
    tmp*=splus(i,gamma);
  }
  
  tmp*=Mm(m,gamma);
  
  return(tmp);
  
}

double Kminus(int m, int n, double gamma) {
  
  int i;
  double tmp=1.0;
  
  for( i=n+1; i<=m; i++ ) {
    tmp*=sminus(i,gamma);
  }
  
  tmp*=Mm(m,gamma);
  
  return(tmp);
  
}

double aKplus(int m, int n) {

  int    i;
  double tmp=1.0;

  for( i=m; i<=n-1; i++ ) {
    tmp*=tc(i,i+1);
  }
  
  return(tmp);
  
}

double aKminus(int m, int n) {

  int    i;
  double tmp=1.0;

  for( i=n+1; i<=m; i++ ) {
    tmp*=tc(i,i-1);
  }
  
  return(tmp);
  
}

double Gsplus(double xs, double xo, double ys, double yo, CmplxMatrix& l, CmplxMatrix& si ) {

  int    i, j, k1, k2, m, n;
  double tpx, K;
  double Tplus, Tminus;
  Complex tmp = 0.0;
  Vector tpy(4), K1(4);
  Vector lbd, c;
  
  m = xlayer(ys);
  n = xlayer(yo);
  
  tpx = xo-xs;
  
  tpy[0] = yo+ys-2.0*d[n];
  tpy[1] = yo-ys+2.0*(d[m-1]-d[n]);
  tpy[2] = -yo+ys;
  tpy[3] = -yo-ys+2.0*d[m-1];
  
  K = aKplus(m, n);
  Tplus = rc(n, n+1);
  Tminus = rc(m, m-1);
  
  K1[0] = K * Tplus;
  K1[1] = K * Tplus * Tminus;
  K1[2] = K;
  K1[3] = K * Tminus;

  for(i=0; i<4; i++) {
    for(j=0; j<od; j++) {
      k1 = (m-1)*(Nd+1)+n;
      k2 = i*od+j;
      tmp += si(k1,k2)*log(sqrt(pow(tpx,2.0)+pow((tpy[i]+l(k1,k2)),2.0)));
    }    
  }
  
  for(i=0; i<4; i++) {
    tmp += K1[i]*log(sqrt(pow(tpx,2.0)+pow(tpy[i],2.0)));
  }

  tmp /= (-2.0*eps[m]*Pi);
  
  return(real(tmp));
  
}

double Gsminus(double xs, double xo, double ys, double yo, CmplxMatrix& l, CmplxMatrix& si) {
  
  int    i, j, m, n, k1, k2;
  double tpx, K;
  double Tplus, Tminus;
  Complex tmp = 0.0;
  Vector tpy(4), K1(4);
  Vector lbd, c;
  
  m = xlayer(ys);
  n = xlayer(yo);

  tpx = xo-xs;

  tpy[0] = yo+ys-2.0*d[m];
  tpy[1] = yo-ys;
  tpy[2] = -yo+ys+2.0*(d[n-1]-d[m]);
  tpy[3] = -yo-ys+2.0*d[n-1];

  K = aKminus(m,n);
  Tplus = rc(m, m+1);
  Tminus = rc(n, n-1);

  K1[0] = K * Tplus;
  K1[1] = K;
  K1[2] = K * Tplus * Tminus;
  K1[3] = K * Tminus;

  for(i=0; i<4; i++) {
    for(j=0; j<od; j++) {
      k1 = (m-1)*(Nd+1)+n-1;
      k2 = i*od+j;
      tmp += si(k1,k2)*log(sqrt(pow(tpx,2.0)+pow((tpy[i]+l(k1,k2)),2.0)));
    }
  }
  
  for(i=0; i<4; i++) {
    tmp += K1[i]*log(sqrt(pow(tpx,2.0)+pow(tpy[i],2.0)));
  }

  tmp /= (-2.0*eps[m]*Pi);
  
  return(real(tmp));

}

Complex kernel( double& t, double& tp, int i, int j, CmplxMatrix& l, CmplxMatrix& c ) {
  
  double  r, rp;
  double  x, y, xp, yp;
  double  g;

  r = csft( t );
  rp = csft( tp );
  
  if( i >= 0 && i < Rec ) {
    recgeom(i,r,x,y);
  } else if( i >= Rec && i < (Rec + Cir) ) {
    cirgeom(i,r,x,y);
  } else if( i >= (Rec + Cir) && i < (Rec + Cir + Tra) ) {
    trageom(i,r,x,y);
  }
  
  if( j >= 0 && j < Rec ) {
    recgeom(j,rp,xp,yp);
  } else if( j >= Rec && j < (Rec + Cir) ) {
    cirgeom(j,rp,xp,yp);
  } else if( j >= (Rec + Cir) && j < (Rec + Cir + Tra) ) {
    trageom(j,rp,xp,yp);
  }
    
  if( y >= yp ) {
    g = Gsplus( xp, x, yp, y, l, c );
  } else {
    g = Gsminus( xp, x, yp, y, l, c );
  }

  return(g);
  
}

int xlayer(double y) {
  
  int i;

  for( i=1; i<=Nd; i++ ) {
    if( y>d[i-1] && y<=d[i] ) {
      return(i);
    }
  }

  return(-1);

}

double sqv( double x ) {

    return( x*x );

}

double csft( double x ) {

    if( x < 0.0 ) {
      return( x + 1.0 );
    } else if( x >= 1.0 ) {
      return( x - 1.0 );
    } else {
      return( x );
    }

}

double sidelen( double x1, double y1, double x2, double y2) {

  return(sqrt(sqv(x1-x2)+sqv(y1-y2)));

}

int recgeom( int i, double& ti, double& xt, double& yt) {

    register double t = ti*Circum[i];
    int ii = i;
    
    if( t < 0.0 || t > Circum[i] ) {

       cerr << " t is out of range in recgeom !!! " << endl;
       exit( 1 );

    }

    if( t >= 0.0 && t <= a[ii] ) {

       xt = Recdi(ii,0) + t;
       yt = Recdi(ii,1);
       return( 1 );

    } else if( t > a[ii] && t <= b[ii] ) {
      
       xt = Recdi(ii,0) + Recdi(ii,2);
       yt = t - a[ii] + Recdi(ii,1);
       return( 2 );
       
    } else if( t > b[ii] && t <= c[ii] ) {

       xt = Recdi(ii,2) - t + b[ii] + Recdi(ii,0);
       yt = Recdi(ii,3) + Recdi(ii,1);
       return( 3 );

    } else if( t > c[ii] && t <= c[ii]+Recdi(ii,3) ) {
      
       xt = Recdi(ii,0);
       yt = Recdi(ii,3) - t + c[ii] + Recdi(ii,1);
       return( 4 );
       
    }

    return( 0 );

}

int cirgeom( int i, double& ti, double& xt, double& yt ) {

    register double t = ti*Circum[i];
    int ii = i - Rec;
    double al;

    if( t < 0.0 || t > Circum[i] ) {

      cerr << " t is out of range in cirgeom !!! " << endl;
      exit( 1 );

    } else {
      
      al = t / Cirdi(ii,2);
      xt  = Cirdi(ii,0) + Cirdi(ii,2) * cos(al);
      yt  = Cirdi(ii,1) + Cirdi(ii,2) * sin(al);
      return( 5 );

    }
    return( 0 );
    
}

int trageom( int i, double& ti, double& xt, double& yt ) {

    register double t = ti*Circum[i];
    double tside;
    int ii = i-Rec-Cir;

    if( t < 0.0 || t > Circum[i] ) {

      cerr << " t is out of range in trageom !!! " << endl;
      exit( 1 );

    }

    if( t >= 0.0 && t < at[ii] ) {

      tside = sidelen(Tradix(ii,0),Tradiy(ii,0),Tradix(ii,1),Tradiy(ii,1));
      xt = Tradix(ii,0)+t*(Tradix(ii,1)-Tradix(ii,0))/tside;
      yt = Tradiy(ii,0)+t*(Tradiy(ii,1)-Tradiy(ii,0))/tside;
      return( 6 );

    } else if( t > at[ii] && t <= bt[ii] ) {

      t -= at[ii];
      tside = sidelen(Tradix(ii,1),Tradiy(ii,1),Tradix(ii,2),Tradiy(ii,2));
      xt = Tradix(ii,1)+t*(Tradix(ii,2)-Tradix(ii,1))/tside;
      yt = Tradiy(ii,1)+t*(Tradiy(ii,2)-Tradiy(ii,1))/tside;
      return( 7 );
      
    } else if( t > bt[ii] && t <= ct[ii] ) {

      t -= bt[ii];
      tside = sidelen(Tradix(ii,2),Tradiy(ii,2),Tradix(ii,3),Tradiy(ii,3));
      xt = Tradix(ii,2)+t*(Tradix(ii,3)-Tradix(ii,2))/tside;
      yt = Tradiy(ii,2)+t*(Tradiy(ii,3)-Tradiy(ii,2))/tside;
      return( 8 );
      
    } else if( t > ct[ii] && t <= Circum[i]) {

      t -= ct[ii];
      tside = sidelen(Tradix(ii,3),Tradiy(ii,3),Tradix(ii,0),Tradiy(ii,0));
      xt = Tradix(ii,3)+t*(Tradix(ii,0)-Tradix(ii,3))/tside;
      yt = Tradiy(ii,3)+t*(Tradiy(ii,0)-Tradiy(ii,3))/tside;
      return( 9 );
    }

    return( 0 );
}

void getsbs( Matrix& uns ) {

  for( int i = 0; i < Nw; i++ ) {

    uns(0,i) = double(i)*step_w;
    uns(1,i) = double(i-3.0)*step_w;
    uns(2,i) = double(i+3.0)*step_w;

  }

}

void intpnt( double xcnt, Vector& tx, Vector& wx, Vector& ptx, Vector& whx,
             double ycnt, Vector& ty, Vector& wy, Vector& pty, Vector& why,
             double h ) {

   const int nx = tx.dim();
   const int ny = ty.dim();
   int i;

   for( i = 0; i < nx; i++ ) {
     
     tx[i] = h*ptx[i]+xcnt;
     wx[i] = whx[i]*h;
      
   }
   
   for( i = 0; i < ny; i++ ) {
     
     ty[i] = h*pty[i]+ycnt;
     wy[i] = why[i]*h;
      
   }

}

void getcoe( CmplxMatrix& li, CmplxMatrix& ci ) {

  int m, n, i, j, k, flg = 0;
  double K, Tplus, Tminus;
  Vector xx, yy;
  CmplxVector lbd, c;
  CmplxVector yy1;
  Vector K1(4), K2(4);

  for( m = 1; m <= Nd; m ++ ) {

    for( n = 1; n <= Nd; n ++ ) {

      if( n >= m ) {

	K = aKplus(m, n);
	Tplus = rc(n, n+1);
	Tminus = rc(m, m-1);
	
	K1[0] = K * Tplus;
	K1[1] = K * Tplus * Tminus;
	K1[2] = K;
	K1[3] = K * Tminus;

	for(i=0; i<4; i++) {
	  
	  xx.resize(2*od);
	  yy.resize(2*od);
	  yy1.resize(2*od);
	  lbd.resize(od);
	  c.resize(od);
	  flg = 0;

	  for(j=0; j<2*od; j++) {
	    
	    xx[j] = (j+1.0)*(maxga)/(double)(2*od);
	    
	    K = Kplus(m,n,xx[j]);
	    Tplus = grcplus(n,xx[j]);
	    Tminus = grcminus(m,xx[j]);
	    
	    K2[0] = K * Tplus;
	    K2[1] = K * Tplus * Tminus;
	    K2[2] = K;
	    K2[3] = K * Tminus;
	    
	    yy[j] = K2[i]-K1[i];
	    
	  }

	  for(j=0; j<2*od; j++) {
	    if( fabs(yy[j]) < 1.0e-10 ) {
	      flg = 1;
	    }
	  }
	  
	  for(j=0; j<od; j++) {
	    if( fabs(yy[2*j]-yy[2*j+1]) < 1.0e-10 ) {
	      flg = 1;
	    }
	  }

	  if(flg == 0) {
	    prony(xx, yy, lbd, c);
	    for( j = 0; j < od; j ++ ) {
	      li((m-1)*(Nd+1)+n,i*od+j) = lbd[j];
	      ci((m-1)*(Nd+1)+n,i*od+j) = c[j];
	    }
	  }
	}
      }

      if(n <= m) {

	K = aKminus(m,n);
	Tplus = rc(m, m+1);
	Tminus = rc(n, n-1);
	
	K1[0] = K * Tplus;
	K1[1] = K;
	K1[2] = K * Tplus * Tminus;
	K1[3] = K * Tminus;

	for(i=0; i<4; i++) {

	  xx.resize(2*od);
	  yy.resize(2*od);
	  lbd.resize(od);
	  c.resize(od);
	  flg = 0;

	  for(j=0; j<2*od; j++) {
	    
	    xx[j] = (j+1.0)*(maxga)/(double)(2*od);

	    K = Kminus(m,n,xx[j]);
	    Tplus = grcplus(m,xx[j]);
	    Tminus = grcminus(n,xx[j]);
	    
	    K2[0] = K * Tplus;
	    K2[1] = K;
	    K2[2] = K * Tplus * Tminus;
	    K2[3] = K * Tminus;
	    
	    yy[j] = K2[i]-K1[i];
	    
	  }

	  for(j=0; j<2*od; j++) {
	    if( fabs(yy[j]) < 1.0e-10 ) {
	      flg = 1;
	    }
	  }
	  
	  for(j=0; j<od; j++) {
	    if( fabs(yy[2*j]-yy[2*j+1]) < 1.0e-10 ) {
	      flg = 1;
	    }
	  }

	  if(flg == 0) {
	    prony(xx, yy, lbd, c);
	    for( j = 0; j < od; j ++ ) {
	      li((m-1)*(Nd+1)+n-1,i*od+j) = lbd[j];
	      ci((m-1)*(Nd+1)+n-1,i*od+j) = c[j];
	    }
	  }
	}
      }
    }
  }
}
