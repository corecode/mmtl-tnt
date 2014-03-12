      SUBROUTINE CMSLV1 (MO, N, M, A, KA, B, KB, IERR, IPVT, WK)
C-----------------------------------------------------------------------
C     PARTIAL PIVOT GAUSS PROCEDURE FOR INVERTING COMPLEX MATRICES
C     AND SOLVING COMPLEX EQUATIONS
C-----------------------------------------------------------------------
      COMPLEX A(KA,N), B(*), WK(*)
      INTEGER IPVT(N)
      COMPLEX D(2)
      INTEGER ONEJ
C
      IF (N .LT. 1 .OR. KA .LT. N) GO TO 30
C
C                      MATRIX FACTORIZATION
C
      CALL CGEFA (A, KA, N, IPVT, IERR)
      IF (IERR .NE. 0) RETURN
C
C                 SOLUTION OF THE EQUATION AX = B
C
      IF (M .LE. 0) GO TO 20
      IF (KB .LT. N) GO TO 30
      ONEJ = 1
      DO 10 J = 1,M
         CALL CGESL (A, KA, N, IPVT, B(ONEJ), 0)
   10    ONEJ = ONEJ + KB
C
C                 CALCULATION OF THE INVERSE OF A
C
   20 IF (MO .EQ. 0) CALL CGEDI (A, KA, N, IPVT, D, WK, 1)
      RETURN
C
C                          ERROR RETURN
C
   30 IERR = -1
      RETURN
      END
