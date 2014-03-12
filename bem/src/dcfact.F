      SUBROUTINE DCFACT (AR, AI, KA, N, IPVT, IERR)
C-----------------------------------------------------------------------
C     DECOMPOSES A COMPLEX MATRIX BY PARTIAL PIVOT GAUSS ELIMINATION
C-----------------------------------------------------------------------
C
C     INPUT ...
C
C        AR AND AI ARE THE REAL AND IMAGINARY PARTS OF THE MATRIX A
C        TO BE DECOMPOSED.
C
C        KA = DECLARED ROW DIMENSION OF THE ARRAYS AR AND AI
C
C        N  = ORDER OF THE MATRIX A
C
C     OUTPUT ...
C
C        AR AND AI CONTAIN AN UPPER TRIANGULAR MATRIX U AND THE
C        MULTIPLIERS NEEDED TO CONSTRUCT L SO THAT A = L*U .
C
C        IPVT = THE PIVOT VECTOR.
C            IPVT(I) = THE INDEX OF THE K-TH PIVOT ROW (I .LT. N)
C            IPVT(N) = (-1)**(NUMBER OF INTERCHANGES)
C
C        IERR IS A VARIABLE THAT REPORTS THE STATUS OF THE RESULTS.
C        IERR HAS ONE OF THE FOLLOWING VALUES ...
C            IERR = 0   THE DECOMPOSITION OF A WAS OBTAINED.
C            IERR = K   THE K-TH PIVOT ELEMENT IS 0.
C
C     IF IERR = 0 THEN THE DETERMINANT OF A HAS THE VALUE ...
C        DET(A) = IPVT(N) * A(1,1) * A(2,2) * ... * A(N,N)
C-----------------------------------------------------------------------
      DOUBLE PRECISION AR(KA,N), AI(KA,N)
      INTEGER IPVT(N)
      DOUBLE PRECISION P, PR, PI, T, TR, TI
C
      IERR = 0
      IPVT(N) = 1
      IF (N .EQ. 1) GO TO 50
      NM1 = N - 1
C
      DO 40 K = 1,NM1
         KP1 = K + 1
C
C               SEARCH FOR THE K-TH PIVOT ELEMENT
C
         P = DABS(AR(K,K)) + DABS(AI(K,K))
         L = K
         DO 10 I = KP1,N
            T = DABS(AR(I,K)) + DABS(AI(I,K))
            IF (P .GE. T) GO TO 10
            P = T
            L = I
   10    CONTINUE
         IF (P .EQ. 0.D0) GO TO 100
C
         PR = AR(L,K)
         PI = AI(L,K)
         IPVT(K) = L
         IF (L .EQ. K) GO TO 20
         IPVT(N) = -IPVT(N)
         AR(L,K) = AR(K,K)
         AR(K,K) = PR
         AI(L,K) = AI(K,K)
         AI(K,K) = PI
C
C                    COMPUTE THE MULTIPLIERS
C
   20    CALL CDIVID(1.D0, 0.D0, PR, PI, PR, PI)
         DO 21 I = KP1,N
            TR = AR(I,K)
            TI = AI(I,K)
            AR(I,K) = TR*PR - TI*PI
            AI(I,K) = TR*PI + TI*PR
   21    CONTINUE
C
C              INTERCHANGE AND ELIMINATE BY COLUMNS
C
         DO 31 J = KP1,N
            TR = AR(L,J)
            AR(L,J) = AR(K,J)
            AR(K,J) = TR
            TI = AI(L,J)
            AI(L,J) = AI(K,J)
            AI(K,J) = TI
            IF (DABS(TR) + DABS(TI) .EQ. 0.D0) GO TO 31
            DO 30 I = KP1,N
               AR(I,J) = AR(I,J) - AR(I,K)*TR + AI(I,K)*TI
               AI(I,J) = AI(I,J) - AR(I,K)*TI - AI(I,K)*TR
   30       CONTINUE
   31    CONTINUE
   40 CONTINUE
C
C                  CHECK THE N-TH PIVOT ELEMENT
C
   50 IF (DABS(AR(N,N)) + DABS(AI(N,N)) .EQ. 0.D0) IERR = N
      RETURN
C
C                    K-TH PIVOT ELEMENT IS 0
C
  100 IERR = K
      RETURN
      END
