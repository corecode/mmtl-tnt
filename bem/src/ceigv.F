      SUBROUTINE CEIGV(IBAL,AR,AI,KA,N,WR,WI,ZR,ZI,IERR,TEMP)
C-----------------------------------------------------------------------
C          EIGENVALUES AND EIGENVECTORS OF COMPLEX MATRICES
C-----------------------------------------------------------------------
      REAL AR(KA,N),AI(KA,N),WR(N),WI(N),ZR(KA,N),ZI(KA,N),TEMP(*)
C----------------------
C     TEMP IS A TEMPORARY STORAGE AREA
C        DIMENSION(TEMP) .GE. 2*N    IF IBAL .EQ. 0
C        DIMENSION(TEMP) .GE. 3*N    IF IBAL .NE. 0
C----------------------
      I2 = 1
      I3 = N + 1
      I1 = N + I3
      LOW = 1
      IGH = N
      IF (IBAL .NE. 0) CALL CBAL(KA,N,AR,AI,LOW,IGH,TEMP(I1))
      CALL CORTH(KA,N,LOW,IGH,AR,AI,TEMP(I2),TEMP(I3))
      CALL COMQR2(KA,N,LOW,IGH,TEMP(I2),TEMP(I3),AR,AI,WR,WI,ZR,ZI,IERR)
      IF (IERR .NE. 0) RETURN
      IF (IBAL .NE. 0) CALL CBABK2(KA,N,LOW,IGH,TEMP(I1),N,ZR,ZI)
      RETURN
      END
