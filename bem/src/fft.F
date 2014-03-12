      SUBROUTINE FFT (C,N,ISN,IERR)
      REAL C(*)
C-----------------------------------------------------------------------
C     THE COMPLEX ARRAY C OF DIMENSION N IS INTERPRETED BY THE CODE
C     AS A REAL ARRAY OF DIMENSION 2*N. IF THIS ASSOCIATION IS NOT
C     PERMITTED BY THE FORTRAN BEING USED, THEN THE USER MAY USE THE
C     SUBROUTINE FFT1.
C-----------------------------------------------------------------------
      IF (IABS(ISN) .NE. 1) GO TO 10
      CALL SFFT (C(1),C(2),N,N,N,ISN+ISN,IERR)
      RETURN
   10 IERR = 4
      RETURN
      END
