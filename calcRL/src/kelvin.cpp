/****Calculate the value of Kelvin Function****/
#include "stdafx.h"

extern  double Pi;
extern  Complex Ic;

Complex seta(double);
Complex dphi(double);

//-------------------------------------------
// Pi = 4.0 * atan(1.0);
// s_pi_4 = Pi / 4.0;
//-------------------------------------------
static double s_pi_4 = atan(1.0);
static double s_sq_2 = sqrt(2.0);

// return the modified Bessel function I0(x) for real x
double ber(double x) {

  double ber;

  if(fabs(x) <= 8.0) {
    double r2, r3, r4, r5, r6, r7;
    double x8;  //,p1,p2,p3,p4,p5,p6,p7,p8;

    /****
    p1 = 1.0;
    p2 = 64.0;
    p3 = 113.77777774;
    p4 = 32.36345652;
    p5 = 2.64191397;
    p6 = 0.08349609;
    p7 = 0.00122552;
    p8 = 0.00000901;
    ********/
    x8 = x/8.0;
    r2 = x8 * x8 * x8 * x8;
    r3 = r2 * r2;
    r4 = r3 * r2;
    r5 = r4 * r2;
    r6 = r5 * r2;
    r7 = r6 * r2;
    ber = 1.0 - 64.0*r2 + 113.77777774*r3 - 
      32.36345652*r4 + 2.64191397*r5 - 
      0.08349609*r6 +  0.00122552*r7 - 0.00000901*r7*r2;
  } else {
    cerr << "Ber function error" << endl;
  }
  return (ber);
}

double bei(double x) {

  double bei;

  if(fabs(x) <= 8.0) {
    double tp, r1, r2, r3, r4, r5, r6;
    double x8;  //,p1,p2,p3,p4,p5,p6,p7;
    
    /****
    p1 = 16.0;
    p2 = 113.77777774;
    p3 = 72.81777742;
    p4 = 10.56765779;
    p5 = 0.52185615;
    p6 = 0.01103667;
    p7 = 0.00011346;
    *****/
    x8 = x/8.0; 
    r1 = x8 * x8;
    tp = x8 * x8 * x8 * x8;
    r2 = r1 * tp;
    r3 = r2 * tp;
    r4 = r3 * tp;
    r5 = r4 * tp;
    r6 = r5 * tp;
    bei = 16.0*r1 - 113.77777774*r2 + 72.81777742*r3 - 
      10.56765779*r4 + 0.52185615*r5 - 0.01103667*r6 + 
      0.00011346*r6*tp;
  } else {
    cerr << "Bei function error" << endl;
  }
  return(bei);

}

double ker(double x) {

  double ker,c1;

  if(x <= 0.0) {
    cerr << "Ker function error" << endl;
  } else if(x <= 8.0) {
    double x8,c1;  // ,p1,p2,p3,p4,p5,p6,p7,p8;
    double r2, r3, r4, r5, r6, r7;
     
    /******
    p1 = 0.57721566;
    p2 = 59.05819744;
    p3 = 171.36272133;
    p4 = 60.60977451;
    p5 = 5.65539121;
    p6 = 0.19636347;
    p7 = 0.00309699;
    p8 = 0.00002458;
    ******/

    x8 = x/8.0;
    r2 = x8 * x8 * x8 * x8;
    r3 = r2 * r2;
    r4 = r3 * r2;
    r5 = r4 * r2;
    r6 = r5 * r2;
    r7 = r6 * r2;
    c1 = log(x/2.0);
    ker = -c1*ber(x) + s_pi_4*bei(x) - 0.57721566 - 
      59.05819744*r2 + 171.36272133*r3 - 60.60977451*r4 + 
      5.65539121*r5 - 0.19636347*r6 + 0.00309699*r7 - 
      0.00002458*r7*r2;
  } else {
    Complex fx,d1;

    c1 = sqrt(Pi/(2.0*x));
    d1 = (1.0+Ic)/s_sq_2;
    fx = c1*exp(-d1*x+seta(-x));
    ker = real(fx);
  }

  return(ker);
}

double kei( double x ) {

  double kei,c1;

  Complex fx,d1;
  

  if(x <= 0.0) {
    cerr << "Kei function error" << endl;
  } else if (x <= 8.0) { 
    double x8;  // ,p1,p2,p3,p4,p5,p6,p7;
    double tp, r1, r2, r3, r4, r5, r6;

    /********
    p1 = 6.76454936;
    p2 = 142.91827687;
    p3 = 124.23569650;
    p4 = 21.30060904;
    p5 = 1.17509064;
    p6 = 0.02695875;
    p7 = 0.00029532;
    *****/

    x8 = x/8.0; 
    r1 = x8 * x8;
    tp = x8 * x8 * x8 * x8;
    r2 = r1 * tp;
    r3 = r2 * tp;
    r4 = r3 * tp;
    r5 = r4 * tp;
    r6 = r5 * tp;
    c1 = log(x/2.0);
    kei = -c1*bei(x) - s_pi_4*ber(x) + 6.76454936*r1 - 
      142.91827687*r2 + 124.23569650*r3 - 21.30060904*r4 +
      1.17509064*r5 - 0.02695875*r6 + 0.00029532*r6*tp;
  } else {	  
    c1 = sqrt(Pi/(2.0*x));
    d1 = (1.0+Ic)/s_sq_2;
    fx = c1*exp(-d1*x+seta(-x));
    kei = imag(fx);
  }

  return(kei);
}


double berp(double x) {
  double berp;

  if(fabs(x) <= 8.0) {
    double x8;  // ,p1,p2,p3,p4,p5,p6,p7;
    double r1, tp, r2, r3, r4, r5, r6;

    /*******
    p1 = 4.0;
    p2 = 14.22222222;
    p3 = 6.06814810;
    p4 = 0.66047849;
    p5 = 0.02609253;
    p6 = 0.00045957;
    p7 = 0.00000394;
    *****/
    x8 = x/8.0;
    r1 = x8 * x8;
    tp = x8 * x8 * x8 * x8;
    r2 = r1 * tp;
    r3 = r2 * tp;
    r4 = r3 * tp;
    r5 = r4 * tp;
    r6 = r5 * tp;
    berp = x*(r1*(-4.0) + 14.22222222*r2 - 6.06814810*r3 + 
	      0.66047849*r4 - 0.02609253*r5 + 
	      0.00045957*r6 - 0.00000394*r6*tp);
  } else {
    cerr << "Berp function error." << endl;
  }

  return(berp);
}

double beip(double x) {

  double beip;

  if(fabs(x) <= 8.0) {
    double x8;  // ,p1,p2,p3,p4,p5,p6,p7;
    double r2, r3, r4, r5, r6;

    /******
    p1 = 0.5;
    p2 = 10.66666666;
    p3 = 11.37777772;
    p4 = 2.31167514;
    p5 = 0.14677204;
    p6 = 0.00379386;
    p7 = 0.00004609;
    ******/

    x8 = x/8.0;
    r2 = x8 * x8 * x8 * x8;
    r3 = r2 * r2;
    r4 = r3 * r2;
    r5 = r4 * r2;
    r6 = r5 * r2;
    beip = x*(0.5 - 10.66666666*r2 + 11.37777772*r3 - 
	      2.31167514*r4 + 0.14677204*r5 - 0.00379386*r6 + 
	      0.00004609*r6*r2);
  } else {
    cerr << "Beip function error" << endl;
  }

  return(beip);
}

double kerp(double x) {

  double kerp,c1;

  if(x <= 0.0) {
    cerr << "Kerp function error" << endl;
  } else if(x <= 8.0) {
    double x8,c1,c2;  // ,p1,p2,p3,p4,p5,p6,p7;
    double r1, tp, r2, r3, r4, r5, r6;

    /******
    p1 = 3.69113734;
    p2 = 21.42034017;
    p3 = 11.36433272;
    p4 = 1.41384780;
    p5 = 0.06136358;
    p6 = 0.00116137;
    p7 = 0.00001075;
    *****/

    x8 = x/8.0;
    r1 = x8 * x8;
    tp = r1 * r1;
    r2 = r1 * tp;
    r3 = r2 * tp;
    r4 = r3 * tp;
    r5 = r4 * tp;
    r6 = r5 * tp;
    c1 = log(x/2.0);
    c2 = 1.0/x;
    kerp = -c1*berp(x) - c2*ber(x) + s_pi_4*beip(x) + x*(r1*(-3.69113734) + 
	   21.42034017*r2 - 11.36433272*r3 + 1.41384780*r4 - 
	   0.06136358*r5 + 0.00116137*r6 - 0.00001075*r6*tp);
  } else {
    Complex fx,d1;

    c1 = sqrt(Pi/(2.0*x));
    d1 = (1.0+Ic)/s_sq_2;
    fx = -c1*exp(-d1*x+seta(-x))*dphi(-x);
    kerp = real(fx);
  }

  return(kerp);
}

double keip(double x) {

  double keip,c1;


  if(x <= 0.0) {
    cerr << "Keip function error" << endl;
  } else if(x <= 8.0) {
    double x8,c2;  // ,p1,p2,p3,p4,p5,p6,p7;
    double r2, r3, r4, r5, r6;

    /*******
    p1 = 0.21139217;
    p2 = 13.39858846;
    p3 = 19.41182758;
    p4 = 4.65950823;
    p5 = 0.33049424;
    p6 = 0.00926707;
    p7 = 0.00011997;
    ******/

    x8 = x/8.0;
    c1 = log(x/2.0);
    c2 = 1.0/x;
    r2 = x8 * x8 * x8 * x8;
    r3 = r2 * r2;
    r4 = r3 * r2;
    r5 = r4 * r2;
    r6 = r5 * r2;
    keip = -c1*beip(x) - c2*bei(x) - s_pi_4*berp(x) + x*(0.21139217 - 
           13.39858846*r2 + 19.41182758*r3 -
 	   4.65950823*r4 + 0.33049424*r5 - 0.00926707*r6 + 
           0.00011997*r6*r2);
  } else {
    Complex fx,d1;

    c1 = sqrt(Pi/(2.0*x));
    d1 = (1.0+Ic)/s_sq_2;
    fx = -c1*exp(-d1*x+seta(-x))*dphi(-x);
    keip = imag(fx);
  }
  return(keip);
}

static Complex  seta_p1 = cmplx(0.0,-0.3926991);
static Complex  seta_p2 = cmplx(0.0110486,0.0);
static Complex  seta_p3 = cmplx(0.0,-0.0009765);
static Complex  seta_p4 = cmplx(-0.0000906,-0.0000901);
static Complex  seta_p5 = cmplx(-0.0000252,0.0);
static Complex  seta_p6 = cmplx(-0.0000034,0.0000051);
static Complex  seta_p7 = cmplx(0.0000006,.0000019);
static Complex  dphi_p1 = cmplx(0.7071068,0.7071068);
static Complex  dphi_p2 = cmplx(-0.0625001,-0.0000001);
static Complex  dphi_p3 = cmplx(-0.0013813,0.0013811);
static Complex  dphi_p4 = cmplx(0.0000005,0.0002452);
static Complex  dphi_p5 = cmplx(0.0000346,0.0000338);
static Complex  dphi_p6 = cmplx(0.0000117,-0.0000024);
static Complex  dphi_p7 = cmplx(0.0000016,-0.0000032);



Complex seta(double x) {

  Complex seta;

  if(x == 0.0) {
    cerr << "Seta function error" << endl;
  } else {
    double x8, r3, r4, r5, r6;

    x8 = 8.0/x;
    r3 = x8 * x8;
    r4 = r3 * x8;
    r5 = r4 * x8;
    r6 = r5 * x8;
    seta = seta_p1 + seta_p2*x8 + seta_p3*r3 + seta_p4*r4 + seta_p5*r5 
      + seta_p6*r6 + seta_p7*r6*x8;
  }
  return(seta);

}


Complex dphi(double x) {

  Complex dphi;
  
  if(x == 0.0) {
    cerr << "dphi function error" << endl;
  } else {
    double x8, r3, r4, r5, r6;

    x8 = 8.0/x;
    r3 = x8 * x8;
    r4 = r3 * x8;
    r5 = r4 * x8;
    r6 = r5 * x8;
    dphi = dphi_p1 + dphi_p2*x8 + dphi_p3*r3 + dphi_p4*r4 + dphi_p5*r5 + 
      dphi_p6*r6 + dphi_p7*r6*x8;
  }
  return(dphi);

}

