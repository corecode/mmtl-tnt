      SUBROUTINE DCSOL (N, M, AR, AI, KA, BR, BI, KB, IPVT)
C-----------------------------------------------------------------------
C     SOLUTION OF THE SYSTEM OF M EQUATIONS A*X = B USING THE
C     DECOMPOSITION OBTAINED BY DCFACT. THIS ROUTINE CANNOT BE
C     USED WHEN DCFACT TERMINATES WITH NONZERO IERR.
C-----------------------------------------------------------------------
C
C     INPUT ...
C
C        AR AND AI CONTAIN THE LU DECOMPOSITION OF THE MATRIX
C        OBTAINED BY DCFACT.
C
C        KA = DECLARED ROW DIMENSION OF THE ARRAYS AR AND AI
C
C        N  = ORDER OF THE MATRIX
C
C        BR AND BI ARE THE REAL AND IMAGINARY PARTS OF THE
C        RIGHT HAND SIDE MATRIX.
C
C        KB = DECLARED ROW DIMENSION OF THE ARRAYS BR AND BI
C
C        M  = NUMBER OF COLUMNS OF B
C
C        IPVT = PIVOT VECTOR OBTAINED FROM DCFACT
C
C     OUTPUT ...
C
C        BR AND BI CONTAIN THE REAL AND IMAGINARY PARTS OF THE
C        SOLUTION X.
C
C-----------------------------------------------------------------------
      DOUBLE PRECISION AR(KA,N), AI(KA,N), BR(KB,M), BI(KB,M)
      INTEGER IPVT(N)
      DOUBLE PRECISION PR, PI, TR, TI
C
C                    FORWARD ELIMINATION
C
      IF (N .EQ. 1) GO TO 50
      NM1 = N - 1
      DO 20 K = 1, NM1
         KP1 = K + 1
         L = IPVT(K)
         DO 11 J = 1,M
            TR = BR(L,J)
            BR(L,J) = BR(K,J)
            BR(K,J) = TR
            TI = BI(L,J)
            BI(L,J) = BI(K,J)
            BI(K,J) = TI
            IF (DABS(TR) + DABS(TI) .EQ. 0.D0) GO TO 11
            DO 10 I = KP1, N
               BR(I,J) = BR(I,J) - AR(I,K)*TR + AI(I,K)*TI
               BI(I,J) = BI(I,J) - AR(I,K)*TI - AI(I,K)*TR
   10       CONTINUE
   11    CONTINUE
   20 CONTINUE
C
C                   BACKWARD ELIMINATION
C               FOR THE LAST N - 1 VARIABLES
C
      DO 40 L = 1,NM1
         KM1 = N - L
         K = KM1 + 1
         CALL CDIVID (1.D0, 0.D0, AR(K,K), AI(K,K), PR, PI)
         DO 31 J = 1,M
            TR = BR(K,J)
            TI = BI(K,J)
            BR(K,J) = TR*PR - TI*PI
            BI(K,J) = TR*PI + TI*PR
            TR = BR(K,J)
            TI = BI(K,J)
            DO 30 I = 1, KM1
               BR(I,J) = BR(I,J) - AR(I,K)*TR + AI(I,K)*TI
               BI(I,J) = BI(I,J) - AR(I,K)*TI - AI(I,K)*TR
   30       CONTINUE
   31    CONTINUE
   40 CONTINUE
C
   50 CALL CDIVID (1.D0, 0.D0, AR(1,1), AI(1,1), PR, PI)
      DO 60 J = 1,M
         TR = BR(1,J)
         TI = BI(1,J)
         BR(1,J) = TR*PR - TI*PI
         BI(1,J) = TR*PI + TI*PR
   60 CONTINUE
      RETURN
      END
