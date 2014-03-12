      SUBROUTINE DCMINV (AR, AI, KA, N, IPVT, TEMP)
C-----------------------------------------------------------------------
C     COMPUTATION OF THE INVERSE OF A MATRIX A USING THE LU
C     DECOMPOSITION OBTAINED BY DCFACT. THIS ROUTINE CANNOT
C     BE USED WHEN DCFACT TERMINATES WITH NONZERO IERR.
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
C        IPVT = PIVOT VECTOR OBTAINED FROM DCFACT
C
C        TEMP = TEMPORARY STORAGE AREA FOR THE SUBROUTINE
C
C     OUTPUT ...
C
C        AR AND AI CONTAIN THE INVERSE OF THE MATRIX.
C
C-----------------------------------------------------------------------
      DOUBLE PRECISION AR(KA,N), AI(KA,N), TEMP(2,N)
      INTEGER IPVT(N)
      DOUBLE PRECISION SR, SI, TR, TI
C
      CALL CDIVID (1.D0, 0.D0, AR(N,N), AI(N,N), AR(N,N), AI(N,N))
      IF (N .EQ. 1) RETURN
      NP1 = N + 1
      NM1 = N - 1
C
C                REPLACE U WITH THE INVERSE OF U
C
      DO 20 NMI = 1,NM1
         I = N - NMI
         IP1 = I + 1
         CALL CDIVID (1.D0, 0.D0, AR(I,I), AI(I,I), TR, TI)
         DO 11 JB = 1,NMI
            J = NP1 - JB
            SR = 0.D0
            SI = 0.D0
            DO 10 L = IP1,J
               SR = SR + AR(I,L)*AR(L,J) - AI(I,L)*AI(L,J)
               SI = SI + AR(I,L)*AI(L,J) + AI(I,L)*AR(L,J)
   10       CONTINUE
            AR(I,J) = -SR*TR + SI*TI
            AI(I,J) = -SR*TI - SI*TR
   11    CONTINUE
         AR(I,I) = TR
         AI(I,I) = TI
   20 CONTINUE
C
C                COMPUTE  INVERSE(U)*INVERSE(L)
C
      DO 60 NMK = 1,NM1
         K = N - NMK
         KP1 = K + 1
         DO 30 I = KP1,N
            TEMP(1,I) = AR(I,K)
            TEMP(2,I) = AI(I,K)
            AR(I,K) = 0.D0
            AI(I,K) = 0.D0
   30    CONTINUE
C
         DO 41 J = KP1,N
            TR = TEMP(1,J)
            TI = TEMP(2,J)
            DO 40 I = 1,N
               AR(I,K) = AR(I,K) - AR(I,J)*TR + AI(I,J)*TI
               AI(I,K) = AI(I,K) - AR(I,J)*TI - AI(I,J)*TR
   40       CONTINUE
   41    CONTINUE
C
         L = IPVT(K)
         IF (K .EQ. L) GO TO 60
         DO 50 I = 1,N
            TR = AR(I,K)
            AR(I,K) = AR(I,L)
            AR(I,L) = TR
            TI = AI(I,K)
            AI(I,K) = AI(I,L)
   50       AI(I,L) = TI
   60 CONTINUE
      RETURN
      END
