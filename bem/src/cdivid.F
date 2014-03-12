      SUBROUTINE CDIVID(AR,AI,BR,BI,CR,CI)
C-----------------------------------------------------------------------
C     DOUBLE PRECISION COMPLEX DIVISION C = A/B AVOIDING OVERFLOW
C-----------------------------------------------------------------------
      DOUBLE PRECISION AR, AI, BR, BI, CR, CI
      DOUBLE PRECISION D, T, U, V
      DOUBLE PRECISION DPMPAR
C
      IF (DABS(BR) .LE. DABS(BI)) GO TO 10
      T = BI/BR
      D = BR + T*BI
      U = (AR + AI*T)/D
      V = (AI - AR*T)/D
      CR = U
      CI = V
      RETURN
C
   10 IF (BI .EQ. 0.D0) GO TO 20
      T = BR/BI
      D = BI + T*BR
      U = (AR*T + AI)/D
      V = (AI*T - AR)/D
      CR = U
      CI = V
      RETURN
C
C     DIVISION BY ZERO. C = INFINITY
C
   20 CR = DPMPAR(3)
      CI = CR
      RETURN
      END
