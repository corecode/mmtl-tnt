      SUBROUTINE DCMSLV (MO,N,M,AR,AI,KA,BR,BI,KB,IERR,IPVT,WK)
C-----------------------------------------------------------------------
C     PARTIAL PIVOT GAUSS PROCEDURE FOR INVERTING DOUBLE PRECISION
C     COMPLEX MATRICES AND SOLVING DOUBLE PRECISION COMPLEX EQUATIONS
C-----------------------------------------------------------------------
      DOUBLE PRECISION AR(KA,N), AI(KA,N), BR(*), BI(*), WK(*)
      INTEGER IPVT(N)
C
      IF (N .LT. 1 .OR. KA .LT. N) GO TO 20
C
C                      MATRIX FACTORIZATION
C
      CALL DCFACT (AR, AI, KA, N, IPVT, IERR)
      IF (IERR .NE. 0) RETURN
C
C                 SOLUTION OF THE EQUATION AX = B
C
      IF (M .LE. 0) GO TO 10
      IF (KB .LT. N) GO TO 20
      CALL DCSOL (N, M, AR, AI, KA, BR, BI, KB, IPVT)
C
C                 CALCULATION OF THE INVERSE OF A
C
   10 IF (MO .EQ. 0) CALL DCMINV (AR, AI, KA, N, IPVT, WK)
      RETURN
C
C                          ERROR RETURN
C
   20 IERR = -1
      RETURN
      END
