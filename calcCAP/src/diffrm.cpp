/*********************************************************/
/***                                                   ***/
/***      standard and non - standard matrix form      ***/
/***                using wavelet basis                ***/
/***                                                   ***/
/*********************************************************/


//#include "stdafx.h"
#include <math.h>
#include <iostream>
#include <iomanip>
#include <stdlib.h>

#include "complex.h"
#include "vector.h"
//#include "ivector.h"
#include "ivectorl.h"
#include "matrix.h"
#include "cmplxvec.h"
#include "cmplxmat.h"
#include "myfunc.h"

#include "diffrm.h"


void  set_wavelets( Vector& sc, Vector& wc, const int& filtsz ) {

    int i;

    sc.resize( filtsz );
    wc.resize( filtsz );

    if( filtsz == 12 ) {

       double fw[] = { 0.011587596739,-0.029320137980,-0.047639590310,
                       0.273021046535, 0.574682393857, 0.294867193696,
                      -0.054085607092,-0.042026480461, 0.016744410163,
                       0.003967883613,-0.001289203356,-0.000509505399 };

       for( i = 0; i <= 11; i++ )
           sc[i] = fw[i] * sqrt( 2.0 );

       for( i = 0; i <= 11; i++ )
           wc[i] = power( -1, i + 1 ) * sc[12-i-1];

    } else {

       if( filtsz == 4 ) {

          sc[0] = ( 1.0 + sqrt( 3.0 ) ) / 4.0;
          sc[1] = ( 3.0 + sqrt( 3.0 ) ) / 4.0;
          sc[2] = ( 3.0 - sqrt( 3.0 ) ) / 4.0;
          sc[3] = ( 1.0 - sqrt( 3.0 ) ) / 4.0;

          sc /= sqrt( 2.0 );

          for( i = 0; i < 4; i++ )
              wc[i] = power( -1, i ) * sc[4-i-1];

       } else {

 	  cerr << endl
               << " Something is wrong with the filter size in set_wavelets !!! "
               << endl;

          exit(1);
       }
    }
}


void  filtermat_all( Matrix& p, Matrix& pt, Matrix& q, Matrix& qt, int& ns, int& np,
                     Vector& sc, Vector& wc ) {

    int     ls = sc.dim();
    int     i, j, m, n;
    Matrix  res(ns,np,0.0), res0(ns,np,0.0);


    for( i = 0; i < ns; i ++ ) {

        for( j = 0; j < ls; j++ ) {

  	    m = j + 2 * i;

            if( m >= 0 && m < np ) {

  	       res(i,m)  = sc[j];
               res0(i,m) = wc[j];

	    } else {

	       n = m - np;

	       if( n < np ) {

	          res(i,n)  = sc[j];
	          res0(i,n) = wc[j];
	       }
           }
        }
    }


    p  = res;
    q  = res0;
    pt = res.trans();
    qt = res0.trans();
}


void  fastwt( CmplxMatrix& al, CmplxMatrix& bt, CmplxMatrix& gm, CmplxMatrix& tt, CmplxMatrix& tup,
              Vector& sc, Vector& wc ) {

    const   int  nh = tup.dim_i();
    const   int  nl = nh / 2;
    const   int  ls = sc.dim();
    int          i, l, k, m, p, q;


    /************* to resize outout matrices *********/

    al.resize(nl,nl);
    bt.resize(nl,nl);
    gm.resize(nl,nl);
    tt.resize(nl,nl);


    /************** fast wavelet transform ***********/

    for( i = 0; i < nl; i++ ) {

        for( l = 0; l < nl; l++ ) {

  	    for( k = 0; k < ls; k++ ) {

                for( m = 0; m < ls; m++ ) {

	 	    p = k + 2 * i;
                    q = m + 2 * l;

                    while( p >= nh )
		       p -= nh;

                    while( q >= nh )
		       q -= nh;

	            al(i,l) += wc[k] *
		               wc[m] *
	                       tup(p,q);

                    bt(i,l) += wc[k] *
		               sc[m] *
		               tup(p,q);

                    gm(i,l) += sc[k] *
		               wc[m] *
	                       tup(p,q);

                    tt(i,l) += sc[k] *
		               sc[m] *
		               tup(p,q);
                }
            }
        }
    }
}


void  nonstdform( CmplxMatrix& res, CmplxMatrix& minp,
                  int& jl, Vector& sc, Vector& wc ) {

    int          i, j, l, ns, nl, sft;
    CmplxMatrix  A, B, G, T, Tup;


    /******** matrix Tup at max resolution level **/

    Tup = minp;


    /******** res - output matrix *****************/

    ns = 0;
    j = Tup.dim_i();

    for( i = 0; i < jl; i++ ) {

        ns += j;
        j /= 2;
    }


    /******** ro resize output matrix *************/

    res.resize(ns,ns);


    /******** discrete wavelet transform **********/

    sft = 0;

    for( l = 0; l < jl; l++ ) {

        fastwt( A, B, G, T, Tup, sc, wc );

        nl = A.dim_i();

        for( i = 0; i < nl; i++ ) {

	    for( j = 0; j < nl; j++ ) {

                res(i+sft,j+sft)    = A(i,j);
                res(i+nl+sft,j+sft) = G(i,j);
                res(i+sft,j+nl+sft) = B(i,j);


                if( l == jl-1 ) {
		   res(i+sft+nl,j+sft+nl) = T(i,j);
                }
            }
        }

        if( l != jl-1 ) {

           sft += 2*nl;
           Tup = T;
        }
    }
}


void  fwt_vec_decomp( CmplxVector& al, CmplxVector& bt, CmplxVector& tup,
                      Vector& sc, Vector& wc ) {

    const  int  nh = tup.dim();
    const  int  nl = nh / 2;
    const  int  ls = sc.dim();
    int         k, m, p;


    /************* to resize outout matrices *********/

    al.resize( nl );
    bt.resize( nl );


    /************** fast wavelet transform ***********/

    for( k = 0; k < nl; k++ ) {

        for( m = 0; m < ls; m++ ) {

	    p = m + 2 * k;

            while( p >= nh )
	       p -= nh;

            al[k] += sc[m] * tup[p];
            bt[k] += wc[m] * tup[p];
        }
    }
}


void  std_vec_decomp( CmplxVector& res, CmplxVector& minp,
                      int& jl, Vector& sc, Vector& wc ) {

    const int    ns = minp.dim();
    int          i, l, nl, sft;
    CmplxVector  A, B, Tup;


    /******** vector Tup at max resolution level **/

    Tup = minp;


    /******** ro resize output vector *************/

    res.resize(ns);


    /******** discrete wavelet transform **********/

    sft = 0;

    for( l = 0; l < jl; l++ ) {

        fwt_vec_decomp( A, B, Tup, sc, wc );
        nl = A.dim();

        for( i = 0; i < nl; i++ ) {

            res[i+sft] = B[i];

            if( l == jl-1 )
	       res[i+sft+nl] = A[i];
        }

        if( l != jl-1 ) {

           sft += nl;
           Tup  = A;
        }
    }
}


void  fwt_vec_reconstr( CmplxVector& tup, CmplxVector& al, CmplxVector& bt,
                        Vector& sc, Vector& wc ) {

    const  int  nh  = al.dim();
    const  int  nl  = nh * 2;
    const  int  ls  = sc.dim();
    const  int  lsh = ls / 2;
    int         k, m, p;


    /************* to resize outout matrices *********/

    tup.resize( nl );


    /********** fast wavelet reconstruction **********/

    for( k = 0; k <= nh-1; k++ ) {

        for( m = 0; m <= lsh-1; m++ ) {

            p = k - m;

            while( p < 0 )
               p += nh;

            tup[2*k] += sc[2*m] * al[p];
            tup[2*k] += wc[2*m] * bt[p];
        }
    }


    for( k = 1; k <= nh; k++ ) {

        for( m = 1; m <= lsh; m++ ) {

            p = k - m;

            while( p < 0 )
               p += nh;

            tup[2*k-1] += sc[2*m-1] * al[p];
            tup[2*k-1] += wc[2*m-1] * bt[p];
        }
    }
}


void  std_vec_reconstr( CmplxVector& res, CmplxVector& minp,
                        int& jl, Vector& sc, Vector& wc ) {

    const int    ns = minp.dim();
    int          nl = ns / int( power( 2, jl ) );
    int          i, l, sft;
    CmplxVector  A, B, tup;


    /******** discrete wavelet transform **********/

    for( l = 0; l < jl; l++ ) {

        if( l == 0 ) {

	   A.resize(nl);
           B.resize(nl);
           sft = ns - 2 * nl;

           for( i = 0; i < nl; i++ ) {

	       A[i] = minp[ns-nl+i];
               B[i] = minp[sft+i];
           }

        } else {

           B.resize(nl);
           sft = ns - 2 * nl;

           for( i = 0; i < nl; i++ )
               B[i] = minp[sft+i];
        }

        fwt_vec_reconstr( tup, A, B, sc, wc );

        nl *= 2;
        A = tup;
    }

    res = A;
}


void  getsbs( Matrix& matin, const int& ns, const int& jl ) {

    int  i;
    int  nl  = 0;
    int  sft = ns / 2;

    matin.resize(3,jl+1);

    for( i = 0; i <= jl; i++ ) {

        if( i != jl )
           matin(0,i) = jl - i;
        else
           matin(0,i) = jl - i + 1;

        matin(1,i) = nl;
        matin(2,i) = matin(1,i) + sft - 1;

        nl = int( matin(2,i) ) + 1;

        if( i != jl - 1 )
           sft /= 2;
    }
}


void  fastwt_row( CmplxMatrix& al, CmplxMatrix& cl, CmplxMatrix& cup,
                  Vector& sc, Vector& wc ) {

    const   int  nj = cup.dim_j();
    const   int  ni = cup.dim_i();
    const   int  nl = ni / 2;
    const   int  ls = sc.dim();
    int          i, l, m, p;


    /************* to resize outout matrices *********/

    al.resize(nl,nj);
    cl.resize(nl,nj);


    /************** fast wavelet transform ***********/

    for( i = 0; i < nl; i++ ) {

        for( l = 0; l < nj; l++ ) {

            for( m = 0; m < ls; m++ ) {

	        p = m + 2 * i;

                while( p >= ni )
	           p -= ni;

	        al(i,l) += wc[m] *
	                   cup(p,l);

                cl(i,l) += sc[m] *
	                   cup(p,l);
            }
        }
    }
}


void  fastwt_col( CmplxMatrix& al, CmplxMatrix& bl, CmplxMatrix& bup,
                  Vector& sc, Vector& wc ) {

    const   int  nj = bup.dim_j();
    const   int  ni = bup.dim_i();
    const   int  nl = nj / 2;
    const   int  ls = sc.dim();
    int          i, l, m, p;


    /************* to resize outout matrices *********/

    al.resize(ni,nl);
    bl.resize(ni,nl);


    /************** fast wavelet transform ***********/

    for( i = 0; i < ni; i++ ) {

        for( l = 0; l < nl; l++ ) {

            for( m = 0; m < ls; m++ ) {

	        p = m + 2 * l;

                while( p >= nj )
	           p -= nj;

	        al(i,l) += wc[m] *
	                   bup(i,p);

                bl(i,l) += sc[m] *
	                   bup(i,p);
            }
        }
    }
}


void  stdform( CmplxMatrix& res, CmplxMatrix& minp,
               int& jl, Vector& sc, Vector& wc ) {

    const int    ns = minp.dim_i();
    int          i, j, l, nl, sft, nr, nc, nrl, ncl, ind;
    Matrix       uns;
    CmplxMatrix  A, B, G, T, Tup;
    CmplxMatrix  Al, Cl, Cinp, Alc, Bl, Binp;


    /******** get some data ***********************/

    getsbs( uns, ns, jl );


    /******** matrix Tup at max resolution level **/

    Tup = minp;


    /******** ro resize output matrix *************/

    res.resize(ns,ns);


    /******** discrete wavelet transform **********/

    sft = 0;

    for( l = 0; l < jl; l++ ) {

        /**** 4 block - A, B, G, T *************/

        fastwt( A, B, G, T, Tup, sc, wc );
        nl = A.dim_i();


        /*** diagonal blocks A and last level **/

        for( i = 0; i < nl; i++ ) {

	    for( j = 0; j < nl; j++ ) {

                res(i+sft,j+sft) = A(i,j);

                if( l == jl - 1 ) {

                   ind = sft + nl;

		   res(i+ind,j+ind) = T(i,j);
                   res(i+ind,j+sft) = G(i,j);
                   res(i+sft,j+ind) = B(i,j);
                }
            }
        }


        /**** off - diagonal blocks ************/

        if( l != jl -1 ) {

	   /**** row blocks *****************/

           Cinp = G;
           nrl  = 0;

	   for( nr = 0; nr <= uns(0,l)-2; nr++ ) {

               fastwt_row( Al, Cl, Cinp, sc, wc );
               ind = sft+nl+nrl;

               for( j = 0; j < nl; j++ )
                   for( i = 0; i < Al.dim_i(); i++ )
                       res(i+ind,j+sft) = Al(i,j);

               nrl += Al.dim_i();

               if( nr != uns(0,l)-2 )
                  Cinp = Cl;
	   }

           ind = sft+nl+nrl;

           for( j = 0; j < nl; j++ )
               for( i = 0; i < Cl.dim_i(); i++ )
                   res(i+ind,j+sft) = Cl(i,j);


           /**** column blocks **************/

           Binp = B;
           ncl  = 0;

           for( nr = 0; nr <= uns(0,l)-2; nr++ ) {

               fastwt_col( Alc, Bl, Binp, sc, wc );
               ind = sft + nl + ncl;

               for( i = 0; i < nl; i++ )
                   for( j = 0; j < Alc.dim_j(); j++ )
                       res(i+sft,j+ind) = Alc(i,j);

               ncl += Alc.dim_j();

               if( nr != uns(0,l)-2 )
                  Binp = Bl;
           }

           ind = sft + nl + ncl;

           for( i = 0; i < nl; i++ )
               for( j = 0; j < Bl.dim_j(); j++ )
                   res(i+sft,j+ind) = Bl(i,j);
        }


        if( l != jl-1 ) {

           sft += nl;
           Tup  = T;
        }
    }
}


void  stdform_new( CmplxMatrix& minp, int& jl, Vector& sc, Vector& wc ) {

    const int    ns = minp.dim_i();
    int          i, j, l, nl, sft, nr, nc, nrl, ncl, ind;
    Matrix       uns;
    CmplxMatrix  A, B, G, T, Tup;
    CmplxMatrix  Al, Cl, Cinp, Alc, Bl, Binp;


    /******** get some data ***********************/

    getsbs( uns, ns, jl );


    /******** matrix Tup at max resolution level **/

    Tup = minp;


    /******** discrete wavelet transform **********/

    sft = 0;

    for( l = 0; l < jl; l++ ) {

        /**** 4 block - A, B, G, T *************/

        fastwt( A, B, G, T, Tup, sc, wc );
        nl = A.dim_i();


        /*** diagonal blocks A and last level **/

        for( i = 0; i < nl; i++ ) {

	    for( j = 0; j < nl; j++ ) {

                minp(i+sft,j+sft) = A(i,j);

                if( l == jl - 1 ) {

                   ind = sft + nl;

		   minp(i+ind,j+ind) = T(i,j);
                   minp(i+ind,j+sft) = G(i,j);
                   minp(i+sft,j+ind) = B(i,j);
                }
            }
        }


        /**** off - diagonal blocks ************/

        if( l != jl -1 ) {

	   /**** row blocks *****************/

           Cinp = G;
           nrl  = 0;

	   for( nr = 0; nr <= uns(0,l)-2; nr++ ) {

               fastwt_row( Al, Cl, Cinp, sc, wc );
               ind = sft+nl+nrl;

               for( j = 0; j < nl; j++ )
                   for( i = 0; i < Al.dim_i(); i++ )
                       minp(i+ind,j+sft) = Al(i,j);

               nrl += Al.dim_i();

               if( nr != uns(0,l)-2 )
                  Cinp = Cl;
	   }

           ind = sft+nl+nrl;

           for( j = 0; j < nl; j++ )
               for( i = 0; i < Cl.dim_i(); i++ )
                   minp(i+ind,j+sft) = Cl(i,j);


           /**** column blocks **************/

           Binp = B;
           ncl  = 0;

           for( nr = 0; nr <= uns(0,l)-2; nr++ ) {

               fastwt_col( Alc, Bl, Binp, sc, wc );

               ind = sft + nl + ncl;

               for( i = 0; i < nl; i++ )
                   for( j = 0; j < Alc.dim_j(); j++ )
                       minp(i+sft,j+ind) = Alc(i,j);

               ncl += Alc.dim_j();

               if( nr != uns(0,l)-2 )
                  Binp = Bl;
           }

           ind = sft + nl + ncl;

           for( i = 0; i < nl; i++ )
               for( j = 0; j < Bl.dim_j(); j++ )
                   minp(i+sft,j+ind) = Bl(i,j);
        }


        if( l != jl-1 ) {

           sft += nl;
           Tup  = T;
        }
    }


    /***************************************/
}


void  std_vec_decomp_new( CmplxVector& minp, int& jl, Vector& sc, Vector& wc ) {

    const int    ns = minp.dim();
    int          i, l, nl, sft;
    CmplxVector  A, B, Tup;


    /******** vector Tup at max resolution level **/

    Tup = minp;


    /******** discrete wavelet transform **********/

    sft = 0;

    for( l = 0; l < jl; l++ ) {

        fwt_vec_decomp( A, B, Tup, sc, wc );
        nl = A.dim();

        for( i = 0; i < nl; i++ ) {

            minp[i+sft] = B[i];

            if( l == jl-1 )
	       minp[i+sft+nl] = A[i];
        }

        if( l != jl-1 ) {

           sft += nl;
           Tup  = A;
        }
    }
}


void  std_vec_reconstr_new( CmplxVector& minp, int& jl, Vector& sc, Vector& wc ) {

    const int    ns = minp.dim();
    int          nl = ns / int( power( 2, jl ) );
    int          i, l, sft;
    CmplxVector  A, B, tup;


    /******** discrete wavelet transform **********/

    for( l = 0; l < jl; l++ ) {

        if( l == 0 ) {

	   A.resize(nl);
           B.resize(nl);
           sft = ns - 2 * nl;

           for( i = 0; i < nl; i++ ) {

	       A[i] = minp[ns-nl+i];
               B[i] = minp[sft+i];
           }

        } else {

           B.resize(nl);
           sft = ns - 2 * nl;

           for( i = 0; i < nl; i++ )
               B[i] = minp[sft+i];
        }

        fwt_vec_reconstr( tup, A, B, sc, wc );

        nl *= 2;
        A = tup;
    }


    if( minp.dim() != A.dim() ) {

       cerr << endl
            << " Something is wrong in std_vec_reconstr "
            << endl;

       exit(1);
    }


    minp = A;
}


void  stdform_new_fast( CmplxMatrix& minp, int& jl, Vector& sc, Vector& wc ) {

    const int    ns = minp.dim_i();
    int          i, j;
    CmplxVector  res;


    /****** FWT of rows ********************/

    for( i = 0; i < ns; i++ ) {

        res = minp.row( i );
        std_vec_decomp_new( res, jl, sc, wc );

        for( j = 0; j < ns; j++ )
            minp(i,j) = res[j];
    }


    /****** FWT of columns *****************/

    for( i = 0; i < ns; i++ ) {

        res = minp.col( i );
        std_vec_decomp_new( res, jl, sc, wc );

        for( j = 0; j < ns; j++ )
            minp(j,i) = res[j];
    }


    /***************************************/
}
