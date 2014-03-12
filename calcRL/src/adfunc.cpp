/********* Functions needed for the main program *********/

#include "stdafx.h"

   extern  int      Nc, Die, Rec, Cir, Tri, Ell, Pol, Gnd, Tra;
   extern  Matrix   Gnddi, Recdi, Cirdi, Tradix, Tradiy;

   extern  Vector   ag, bg, cg;
   extern  Vector   a, b, c;
   extern  Vector   at, bt, ct;

   extern  Vector   Circum;
   extern  Vector   Diec, Recc, Circ, Tric, Ellc, Polc, Gndc, Trac;

   extern  double   freq, sqrtoms, sigma, omega, oms, omegamu;
   extern  int      matr, Mt, Np;
   extern  int      Ns, Nsr, Ngx, Ngy, Nxt, Nyt, Ngr, Nrt;
   extern  int      Nh, Nc4, Nit, J, Nw, Nwx, Nwy, Nws;
   extern  double   hsrw, EPS;
   extern  double   Pi, Pih, Pit, wn, mu, cf;
   extern  double   step_w, hw, hwh, mmax;
   extern  Complex  Ic;

/*****************************

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
********************/

/* To calculate the length of a side */
/********************
double sidelen(double x1, double y1, double x2, double y2 ) {

  return(sqrt(sqv(x1-x2)+sqv(y1-y2)));

}
*************************/


/* To calculate the area of trapezoid with the coodinates */
double traparea( int i ) {

  int j, j1, flag;
  double side1, side2, height;
  double x[4], y[4], xmax;

  for(j = 0; j < 4; j ++) {
    y[j] = Tradix(i,j);
  }
  
  height = fabs(Tradiy(i,0)-Tradiy(i,2));
  
  for(j = 0; j < 4; j ++) {
    xmax = -100;
    for(j1 = 0; j1 < 4; j1 ++) {
      if(y[j1] > xmax) {
	xmax = y[j1];
	flag = j1;
      }
    }
    x[j] = xmax;
    y[flag] = -200;
  }

  side1 = fabs(x[0]-x[3]);
  side2 = fabs(x[1]-x[2]);

  return((side1+side2)*height/2.0);

}

/*  Determine the coordinates of rectangular shape */

int gndgeom( int i, double& ti, double& xt, double& yt ) {

    register double t = ti*Circum[i];

    if(t < 0.0 || t > Circum[i]) {

       cerr << " t is out of range in gndgeom !!! " << endl;
       exit( 1 );

    }

    if(t >= 0.0 && t <= ag[i]) {

       xt = Gnddi(i,0)+t;
       yt = Gnddi(i,1);
       return( 1 );

    } else if(t > ag[i] && t <= bg[i]) {
      
       xt = Gnddi(i,0)+Gnddi(i,2);
       yt = t-ag[i]+Gnddi(i,1);
       return( 2 );
       
    } else if(t > bg[i] && t <= cg[i]) {

       xt = Gnddi(i,2)-t+bg[i]+Gnddi(i,0);
       yt = Gnddi(i,3)+Gnddi(i,1);
       return( 3 );

    } else if(t > cg[i] && t <= cg[i]+Gnddi(i,3)) {

       xt = Gnddi(i,0);
       yt = Gnddi(i,3)-t+cg[i]+Gnddi(i,1);

       return( 4 );
    }

    return( 0 );
}

int recgeom( int i, double& ti, double& xt, double& yt ) {

    register double t = ti*Circum[i];
    int ii = i - Gnd;

    if(t < 0.0 || t > Circum[i]) {

       cerr << " t is out of range in recgeom !!! " << endl;
       exit( 1 );

    }

    if(t >= 0.0 && t <= a[ii]) {

       xt = Recdi(ii,0)+t;
       yt = Recdi(ii,1);
       return( 1 );

    } else if(t > a[ii] && t <= b[ii]) {
      
       xt = Recdi(ii,0)+Recdi(ii,2);
       yt = t-a[ii]+Recdi(ii,1);
       return( 2 );
       
    } else if(t > b[ii] && t <= c[ii]) {

       xt = Recdi(ii,2)-t+b[ii]+Recdi(ii,0);
       yt = Recdi(ii,3)+Recdi(ii,1);
       return( 3 );

    } else if(t > c[ii] && t <= c[ii]+Recdi(ii,3)) {

       xt = Recdi(ii,0);
       yt = Recdi(ii,3)-t+c[ii]+Recdi(ii,1);

       return( 4 );
    }

    return( 0 );
}

int cirgeom(int i, double& ti, double& xt, double& yt, double& al) {

    register double t = ti*Circum[i];
    int ii = i-Gnd-Rec;

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
    int ii=i-Gnd-Rec-Cir;

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

    uns(0,i) = i;
    uns(1,i) = double(i-4.0)*step_w;
    uns(2,i) = double(i+7.0)*step_w;

  }

}

// determine the coefficients in the gauss quadrature.
void intpnt( double xcnt, Vector& tx, Vector& wx, Vector& ptx, Vector& whx,
             double ycnt, Vector& ty, Vector& wy, Vector& pty, Vector& why,
             double h ) {

   const int nx = tx.dim();
   const int ny = ty.dim();
   int i;

   for( i = 0; i < nx; i++ ){
      
      tx[i] = h*ptx[i]+xcnt;
      wx[i] = whx[i]*h;
      
   }
   
   for( i = 0; i < ny; i++ ) {

      ty[i] = h*pty[i]+ycnt;
      wy[i] = why[i]*h;

   }

}

// calculate the G0
Complex kernel_v0( double& t, double& tp, int i, int j ) {

  //    register Complex res;
    double tt, ttp;

    tt = csft( t );
    ttp = csft( tp );
    
    //    res = g0mn( i, j, tt, ttp );

    return( g0mn( i, j, tt, ttp ) );
}

// calculate the (partial G0/partial n')
Complex kernel_u0( double& t, double& tp, int i, int j ) {

  //    register Complex res;
    double tt, ttp;

    tt = csft( t );
    ttp = csft( tp );
    
    //    res = g0pmn( i, j, tt, ttp );

    return( g0pmn( i, j, tt, ttp ) );
} 

// calculate the Gi
Complex kernel_vi( double& t, double& tp, int i, int j ) {

  //    register Complex res;
    double tt, ttp;

    tt = csft( t );
    ttp = csft( tp );
    
    //    res = gimn( i, j, tt, ttp );

    return( gimn( i, j, tt, ttp ) );
} 

// calculate the (partial Gi/partial n')
Complex kernel_ui( double& t, double& tp, int i, int j ) {

  //    register Complex res;
    double tt, ttp;

    tt = csft( t );
    ttp = csft( tp );
    
    //    res = gipmn( i, j, tt, ttp );

    return( gipmn( i, j, tt, ttp ) );
} 


// Pi = 4.0*atan(1.0);
static double s_2Pi = 8.0 * atan(1.0);

// calculate G0
double g0(double x, double y, double xp, double yp) {

  //  double rou2;
  //  double g0;
  double tmp1 = x - xp;
  double tmp2 = y - yp;
  
  //  rou2 = tmp1 * tmp1 + tmp2 * tmp2;
  //  g0 = -1.0/(4.0*Pi)*log(tmp1 * tmp1 + tmp2 * tmp2);                   
  return(-1.0/(4.0*Pi)*log(tmp1 * tmp1 + tmp2 * tmp2));

}

// calculate derivative of G0 in term of x
double g0x(double x, double y, double xp, double yp) {

    //  double rou2;
    //  double g0x;
  double tmp1 = x - xp;
  double tmp2 = y - yp;
  
  //  rou2 = tmp1 * tmp1 + tmp2 * tmp2;
  //  g0x = -tmp1/(s_2Pi*(tmp1 * tmp1 + tmp2 * tmp2));
  return(-tmp1/(s_2Pi*(tmp1 * tmp1 + tmp2 * tmp2)));

}

// calculate derivative of G0 in term of y
double g0y(double x, double y, double xp, double yp) {

  //  double rou2;
  //  double g0y;
  double tmp1 = x - xp;
  double tmp2 = y - yp;
  
  //  rou2 = tmp1 * tmp1 + tmp2 * tmp2;
  //  g0y = -tmp2/(s_2Pi*(tmp1 * tmp1 + tmp2 * tmp2));
  return(-tmp2/(s_2Pi*(tmp1 * tmp1 + tmp2 * tmp2)));

}

// calculate value of G0
double g0mn(int i, int j, double r, double rp) {

  double x,y,xp,yp,al,alp;
  double g0mn;

  if( i < Gnd ) {
    gndgeom(i,r,x,y);
  } else if( i >= Gnd && i < (Gnd + Rec) ) {
    recgeom(i,r,x,y);
  } else if( i >= (Gnd + Rec) && i < (Gnd + Rec + Cir) ) {
    cirgeom(i,r,x,y,al);
  } else if( i >= (Gnd + Rec + Cir) && i < (Gnd + Rec + Cir + Tra) ) {
    trageom(i,r,x,y);
  }

  if( j < Gnd ) {
    gndgeom(j,rp,xp,yp);
  } else if( j >= Gnd && j < (Gnd + Rec) ) {
    recgeom(j,rp,xp,yp);
  } else if( j >= (Gnd + Rec) && j < (Gnd + Rec + Cir) ) {
    cirgeom(j,rp,xp,yp,alp);
  } else if( j >= (Gnd + Rec + Cir) && j < (Gnd + Rec + Cir + Tra) ) {
    trageom(j,rp,xp,yp);
  }

  g0mn = g0(x,y,xp,yp);
  return(g0mn);

}

// calculate the derivative 
double g0pmn(int i, int j, double r, double rp) {

  double x, y, xp, yp, al, alp;
  int n, np;
  double g0pmn;
  Vector tmpv(2);
  int    ii = j-Gnd-Rec-Cir;

  if( i < Gnd ) {
    n = gndgeom(i,r,x,y);
  } else if( i >= Gnd && i < (Gnd + Rec) ) {
    n = recgeom(i,r,x,y);
  } else if( i >= (Gnd + Rec) && i < (Gnd + Rec + Cir) ) {
    n = cirgeom(i,r,x,y,al);
  } else if( i >= (Gnd + Rec + Cir) && i < (Gnd + Rec + Cir + Tra) ) {
    n = trageom(i,r,x,y);
  }

  if( j < Gnd ) {
    np = gndgeom(j,rp,xp,yp);
  } else if( j >= Gnd && j < (Gnd + Rec) ) {
    np = recgeom(j,rp,xp,yp);
  } else if( j >= (Gnd + Rec) && j < (Gnd + Rec + Cir) ) {
    np = cirgeom(j,rp,xp,yp,alp);
  } else if( j >= (Gnd + Rec + Cir) && j < (Gnd + Rec + Cir + Tra) ) {
    np = trageom(j,rp,xp,yp);
  }
  
  switch( np ) {
  case 1:
    g0pmn = g0y(x,y,xp,yp);
    break;
  case 2:
    g0pmn = -g0x(x,y,xp,yp);
    break;
  case 3:
    g0pmn = -g0y(x,y,xp,yp);
    break;
  case 4:
    g0pmn = g0x(x,y,xp,yp);
    break;
  case 5:                                                      // circle
    g0pmn = -g0x(x,y,xp,yp)*cos(alp) - g0y(x,y,xp,yp)*sin(alp);
    break;
  case 6:                                                      
    tmpv[0] = -g0x(x,y,xp,yp);
    tmpv[1] = -g0y(x,y,xp,yp);
    g0pmn = tmpv * tangle(Tradix(ii,0),Tradiy(ii,0),Tradix(ii,1),Tradiy(ii,1),Tradix(ii,2),Tradiy(ii,2));
    break;
  case 7:                                                      
    tmpv[0] = -g0x(x,y,xp,yp);
    tmpv[1] = -g0y(x,y,xp,yp);
    g0pmn = tmpv * tangle(Tradix(ii,1),Tradiy(ii,1),Tradix(ii,2),Tradiy(ii,2),Tradix(ii,3),Tradiy(ii,3));
    break;
  case 8:                                                      
    tmpv[0] = -g0x(x,y,xp,yp);
    tmpv[1] = -g0y(x,y,xp,yp);
    g0pmn = tmpv * tangle(Tradix(ii,2),Tradiy(ii,2),Tradix(ii,3),Tradiy(ii,3),Tradix(ii,0),Tradiy(ii,0));
    break;
  case 9:                                                      // 6-9 Trapezoid
    tmpv[0] = -g0x(x,y,xp,yp);
    tmpv[1] = -g0y(x,y,xp,yp);
    g0pmn = tmpv * tangle(Tradix(ii,3),Tradiy(ii,3),Tradix(ii,0),Tradiy(ii,0),Tradix(ii,1),Tradiy(ii,1));
    break;    
  default:
    cerr << np << endl;
    cerr << "unidentified number" << endl;
  }

  return(g0pmn);
  
  
}

// functions about G1(r,rp)
// calculate the real part of Gi
double gir(double x, double y, double xp, double yp) {

  extern double ker(double);

  double rou;
  //  double gir;
  double tmpx = x - xp;
  double tmpy = y - yp;

  rou = sqrt(tmpx*tmpx + tmpy*tmpy)*sqrtoms;
  //  gir = 1.0/(s_2Pi)*ker(rou);
  return(1.0/(s_2Pi)*ker(rou));

}

// calculate the derivative of real part of Gi in term of x
double girx(double x, double y, double xp, double yp) {

  extern double kerp(double);

  double rou;  // ,rouoms;
  //  double girx;
  double tmpx = x - xp;
  double tmpy = y - yp;

  rou = sqrt(tmpx*tmpx + tmpy*tmpy);
  //  rouoms = sqrtoms*rou;
  //  girx = sqrtoms*kerp(sqrtoms*rou)*tmpx/(s_2Pi*rou);
  return(sqrtoms*kerp(sqrtoms*rou)*tmpx/(s_2Pi*rou));

}

// calculate the derivative of real part of Gi in term of y
double giry(double x, double y, double xp, double yp) {

  extern double kerp(double);
  double rou;  // ,rouoms;
  //  double giry;
  double tmpx = x - xp;
  double tmpy = y - yp;

  rou = sqrt(tmpx*tmpx + tmpy*tmpy);
  //  rouoms = rou*sqrtoms;
  // giry = sqrtoms*kerp(rou*sqrtoms)*tmpy/(s_2Pi*rou);
  return(sqrtoms*kerp(rou*sqrtoms)*tmpy/(s_2Pi*rou));

}

// calculate the value of Gi
double girmn(int i, int j, double r, double rp) {

  double x,y,xp,yp,al,alp;
  //  double girmn;

  if( i < Gnd ) {
    gndgeom(i,r,x,y);
  } else if( i >= Gnd && i < (Gnd + Rec) ) {
    recgeom(i,r,x,y);
  } else if( i >= (Gnd + Rec) && i < (Gnd + Rec + Cir) ) {
    cirgeom(i,r,x,y,al);
  } else if( i >= (Gnd + Rec + Cir) && i < (Gnd + Rec + Cir + Tra) ) {
    trageom(i,r,x,y);
  }

  if( j < Gnd ) {
    gndgeom(j,rp,xp,yp);
  } else if( j >= Gnd && j < (Gnd + Rec) ) {
    recgeom(j,rp,xp,yp);
  } else if( j >= (Gnd + Rec) && j < (Gnd + Rec + Cir) ) {
    cirgeom(j,rp,xp,yp,alp);
  } else if( j >= (Gnd + Rec + Cir) && j < (Gnd + Rec + Cir + Tra) ) {
    trageom(j,rp,xp,yp);
  }

  //  girmn = gir(x,y,xp,yp);
  return(gir(x,y,xp,yp));

}

// calculate the derivative of Gi
double girpmn(int i, int j, double r, double rp) {

  double x,y,xp,yp,al,alp;
  double girpmn;
  int n,np;
  Vector tmpv(2);
  int ii=j-Gnd-Rec-Cir;

  if( i < Gnd ) {
    n = gndgeom(i,r,x,y);
  } else if( i >= Gnd && i < (Gnd + Rec) ) {
    n = recgeom(i,r,x,y);
  } else if( i >= (Gnd + Rec) && i < (Gnd + Rec + Cir) ) {
    n = cirgeom(i,r,x,y,al);
  } else if( i >= (Gnd + Rec + Cir) && i < (Gnd + Rec + Cir + Tra) ) {
    n = trageom(i,r,x,y);
  }

  if( j < Gnd ) {
    np = gndgeom(j,rp,xp,yp);
  } else if( j >= Gnd && j < (Gnd + Rec) ) {
    np = recgeom(j,rp,xp,yp);
  } else if( j >= (Gnd + Rec) && j < (Gnd + Rec + Cir) ) {
    np = cirgeom(j,rp,xp,yp,alp);
  } else if( j >= (Gnd + Rec + Cir) && j < (Gnd + Rec + Cir + Tra) ) {
    np = trageom(j,rp,xp,yp);
  }

  switch( np ) {
  case 1:
    girpmn = giry(x,y,xp,yp);
    break;
  case 2:
    girpmn = -girx(x,y,xp,yp);
    break;
  case 3:
    girpmn = -giry(x,y,xp,yp);
    break;
  case 4:
    girpmn = girx(x,y,xp,yp);
    break;
  case 5:                                                      // circle
    girpmn = -girx(x,y,xp,yp)*cos(alp) - giry(x,y,xp,yp)*sin(alp);
    break;
  case 6:                                                      
    tmpv[0] = -girx(x,y,xp,yp);
    tmpv[1] = -giry(x,y,xp,yp);
    girpmn = tmpv * tangle(Tradix(ii,0),Tradiy(ii,0),Tradix(ii,1),Tradiy(ii,1),Tradix(ii,2),Tradiy(ii,2));
    break;
  case 7:                                                      
    tmpv[0] = -girx(x,y,xp,yp);
    tmpv[1] = -giry(x,y,xp,yp);
    girpmn = tmpv * tangle(Tradix(ii,1),Tradiy(ii,1),Tradix(ii,2),Tradiy(ii,2),Tradix(ii,3),Tradiy(ii,3));
    break;
  case 8:                                                      
    tmpv[0] = -girx(x,y,xp,yp);
    tmpv[1] = -giry(x,y,xp,yp);
    girpmn = tmpv * tangle(Tradix(ii,2),Tradiy(ii,2),Tradix(ii,3),Tradiy(ii,3),Tradix(ii,0),Tradiy(ii,0));
    break;
  case 9:                                                      // 6-9 Trapezoid
    tmpv[0] = -girx(x,y,xp,yp);
    tmpv[1] = -giry(x,y,xp,yp);
    girpmn = tmpv * tangle(Tradix(ii,3),Tradiy(ii,3),Tradix(ii,0),Tradiy(ii,0),Tradix(ii,1),Tradiy(ii,1));
    break;    
  default:
    cerr << np << endl;
    cerr << "unidentified number" << endl;
  }

  return(girpmn);

}

// calculate the imaginary part of Gi
double gii(double x, double y, double xp, double yp) {

  extern double kei(double);
  double rou;
  double gii;

  rou = sqrt(sqv(x-xp)+sqv(y-yp))*sqrtoms;
  gii = -1.0/(s_2Pi)*kei(rou);

  return(gii);

}

// calculate the derivative of imaginary part of Gi in term of x
double giix(double x, double y, double xp, double yp) {

  extern double keip(double);
  double rou,rouoms;
  double giix;

  rou = sqrt(sqv(x-xp)+sqv(y-yp));
  rouoms = rou*sqrtoms;
  if(rouoms>0.0)
    giix = -sqrtoms*keip(sqrtoms*rou)*(x-xp)/(s_2Pi*rou);
  else
    giix = 0.0;
  return(giix);

}

// calculate the derivative of imaginary part of Gi in term of y
double giiy(double x, double y, double xp, double yp) {

  extern double keip(double);
  double rou,rouoms;
  double giiy;

  rou = sqrt(sqv(x-xp)+sqv(y-yp));
  rouoms = rou*sqrtoms;
  if(rouoms > 0.0)
    giiy = -sqrtoms*keip(sqrtoms*rou)*(y-yp)/(s_2Pi*rou);
  else
    giiy = 0.0;
  return(giiy);

}

// calculate the value of imaginary part of Gi
double giimn(int i, int j, double r, double rp) {

  double x,y,xp,yp,al,alp;
  double giimn;

  if( i < Gnd ) {
    gndgeom(i,r,x,y);
  } else if( i >= Gnd && i < (Gnd + Rec) ) {
    recgeom(i,r,x,y);
  } else if( i >= (Gnd + Rec) && i < (Gnd + Rec + Cir) ) {
    cirgeom(i,r,x,y,al);
  } else if( i >= (Gnd + Rec + Cir) && i < (Gnd + Rec + Cir + Tra) ) {
    trageom(i,r,x,y);
  }

  if( j < Gnd ) {
    gndgeom(j,rp,xp,yp);
  } else if( j >= Gnd && j < (Gnd + Rec) ) {
    recgeom(j,rp,xp,yp);
  } else if( j >= (Gnd + Rec) && j < (Gnd + Rec + Cir) ) {
    cirgeom(j,rp,xp,yp,alp);
  } else if( j >= (Gnd + Rec + Cir) && j < (Gnd + Rec + Cir + Tra) ) {
    trageom(j,rp,xp,yp);
  }

  giimn = gii(x,y,xp,yp);
  return(giimn);

}

// calculate the derivative of imaginary part of Gi
double giipmn(int i, int j, double r, double rp) {

  double x,y,xp,yp,al,alp;
  double giipmn;
  int    n,np;
  Vector tmpv(2);
  int    ii = j-Gnd-Rec-Cir;

  if( i < Gnd ) {
    n = gndgeom(i,r,x,y);
  } else if( i >= Gnd && i < (Gnd + Rec) ) {
    n = recgeom(i,r,x,y);
  } else if( i >= (Gnd + Rec) && i < (Gnd + Rec + Cir) ) {
    n = cirgeom(i,r,x,y,al);
  } else if( i >= (Gnd + Rec + Cir) && i < (Gnd + Rec + Cir + Tra) ) {
    n = trageom(i,r,x,y);
  }

  if( j < Gnd ) {
    np = gndgeom(j,rp,xp,yp);
  } else if( j >= Gnd && j < (Gnd + Rec) ) {
    np = recgeom(j,rp,xp,yp);
  } else if( j >= (Gnd + Rec) && j < (Gnd + Rec + Cir) ) {
    np = cirgeom(j,rp,xp,yp,alp);
  } else if( j >= (Gnd + Rec + Cir) && j < (Gnd + Rec + Cir + Tra) ) {
    np = trageom(j,rp,xp,yp);
  }

  switch( np ) {
  case 1:
    giipmn = giiy(x,y,xp,yp);
    break;
  case 2:
    giipmn = -giix(x,y,xp,yp);
    break;
  case 3:
    giipmn = -giiy(x,y,xp,yp);
    break;
  case 4:
    giipmn = giix(x,y,xp,yp);
    break;
  case 5:                                                      // circle
    giipmn = -giix(x,y,xp,yp)*cos(alp) - giiy(x,y,xp,yp)*sin(alp);
    break;
  case 6:                                                      
    tmpv[0] = -giix(x,y,xp,yp);
    tmpv[1] = -giiy(x,y,xp,yp);
    giipmn = tmpv * tangle(Tradix(ii,0),Tradiy(ii,0),Tradix(ii,1),Tradiy(ii,1),Tradix(ii,2),Tradiy(ii,2));
    break;
  case 7:                                                      
    tmpv[0] = -giix(x,y,xp,yp);
    tmpv[1] = -giiy(x,y,xp,yp);
    giipmn = tmpv * tangle(Tradix(ii,1),Tradiy(ii,1),Tradix(ii,2),Tradiy(ii,2),Tradix(ii,3),Tradiy(ii,3));
    break;
  case 8:                                                      
    tmpv[0] = -giix(x,y,xp,yp);
    tmpv[1] = -giiy(x,y,xp,yp);
    giipmn = tmpv * tangle(Tradix(ii,2),Tradiy(ii,2),Tradix(ii,3),Tradiy(ii,3),Tradix(ii,0),Tradiy(ii,0));
    break;
  case 9:                                                      // 6-9 Trapezoid
    tmpv[0] = -giix(x,y,xp,yp);
    tmpv[1] = -giiy(x,y,xp,yp);
    giipmn = tmpv * tangle(Tradix(ii,3),Tradiy(ii,3),Tradix(ii,0),Tradiy(ii,0),Tradix(ii,1),Tradiy(ii,1));
    break;    
  default:
    cerr << np << endl;
    cerr << "unidentified number" << endl;
  }

  return(giipmn);

}

// calculate the value of Gi,combine the real and imaginary parts
Complex gimn(int i, int j, double r, double rp) {

  Complex gimn;
  
  gimn = girmn(i,j,r,rp)+Ic*giimn(i,j,r,rp);
  return(gimn);
  
}

// calculate the derivative of Gi, combine the real and imaginary parts.

Complex gipmn(int i, int j, double r, double rp) {

  Complex gipmn;
  
  gipmn = girpmn(i,j,r,rp)+Ic*giipmn(i,j,r,rp);
  return(gipmn);
  
}

Vector tangle( double x1, double y1, double x2, double y2, double x3, double y3 ) {

  Vector v1(2), v2(2), v(2);

  v1[0] = x1-x2;     
  v1[1] = y1-y2;     

  v1 = v1/norm2(v1);

  v2[0] = x3-x2;     
  v2[1] = y3-y2;     

  v2 = v2/norm2(v2);

  v = (v1*v2)*v1 - v2;
  v = v/norm2(v);

  return(v);

}

