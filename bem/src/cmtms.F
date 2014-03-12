      SUBROUTINE CMTMS (M, N, L, A, KA, B, KB, C, KC)
C-----------------------------------------------------------------------
C                   PRODUCT OF COMPLEX MATRICES
C-----------------------------------------------------------------------
      COMPLEX A(KA,N), B(KB,L), C(KC,L), W
C
      DO 30 J = 1,L
         DO 20 I = 1,M
            W = (0.0,0.0)
            DO 10 K = 1,N
               W = W + A(I,K)*B(K,J)
   10       CONTINUE
            C(I,J) = W
   20    CONTINUE
   30 CONTINUE
      RETURN
      END
