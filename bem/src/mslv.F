      SUBROUTINE MSLV(MO,N,M,A,KA,B,KB,DET,RCOND,IERR,IPVT,WK)
C     ------------------
      REAL A(KA,N),B(*)
      REAL DET(2),RCOND,T,WK(N)
      INTEGER IPVT(N),ONEJ
C     ------------------
C
C          MATRIX FACTORIZATION AND COMPUTATION OF RCOND
C
      IERR = 0
      CALL SGECO(A,KA,N,IPVT,RCOND,WK)
      T = 1.0 + RCOND
      IF (T .EQ. 1.0) GO TO 30
C
C                  SOLUTION OF THE EQUATION AX=B
C
      IF (M .LT. 1) GO TO 20
      ONEJ = 1
      DO 10 J=1,M
      CALL SGESL(A,KA,N,IPVT,B(ONEJ),0)
   10 ONEJ = ONEJ + KB
C
C             CALCULATION OF DET AND THE INVERSE OF A
C
   20 JOB = 10
      IF (MO .EQ. 0) JOB = 11
      CALL SGEDI(A,KA,N,IPVT,DET,WK,JOB)
      RETURN
C
C                  THE PROBLEM CANNOT BE SOLVED
C
   30 IERR = 1
      RETURN
      END
