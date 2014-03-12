      SUBROUTINE FMIN(F, A0, B0, X, W, AERR, RERR, ERROR, IND)
C     ******************************************************************
C     GOLDEN SECTION MINIMIZATION OF A FUNCTION F(T)
C     ******************************************************************
      REAL F, A0, B0, X, W, AERR, RERR, ERROR
      REAL EPS, EPS0, ATOL, FTOL, RTOL, TOL
      REAL A, B, C1, C2, E, FU, FV, U, V
      REAL SPMPAR
      EXTERNAL F
C     -------------------
C     C1 = 1 - C2
C     C2 = 0.5*(-1 + SQRT(5))
C     -------------------
      DATA EPS0/5.E-3/
      DATA C1/.3819660112501052/
      DATA C2/.6180339887498948/
C     -------------------
C     ****** EPS IS A MACHINE DEPENDENT CONSTANT. EPS IS THE
C            SMALLEST NUMBER SUCH THAT 1 + EPS .GT. 1.
C
                   EPS = SPMPAR(1)
C     -------------------
      A = A0
      B = B0
      IND = 0
      ATOL = AMAX1(AERR,1.E-20)
      FTOL = AMAX1(2.0*EPS,RERR)
      RTOL = AMAX1(7.0*EPS,RERR)
C
      E = B - A
      U = A + C1*E
      V = A + C2*E
      FU = F(U)
      FV = F(V)
C
C        LOCATION OF THE REGION OF A LOCAL MINIMUM
C
   10 IF (E .LE. EPS0*(1.0 + ABS(A))) GO TO 40
      IF (FU - FV) 20,11,30
   11 IF (FU .GT. F(B)) GO TO 30
C
   20 B = V
      E = B - A
      V = U
      U = A + C1*E
      FV = FU
      FU = F(U)
      GO TO 10
C
   30 A = U
      E = B - A
      U = V
      V = A + C2*E
      FU = FV
      FV = F(V)
      GO TO 10
C
   40 IF (A .GT. 0.0 .OR. B .LT. 0.0) GO TO 41
         W = F(0.0)
         IF (W .LE. AMIN1(FU,FV)) GO TO 100
   41 IF (A .NE. A0) GO TO 42
         IF (A .EQ. 0.0) GO TO 201
         W = F(A)
         IF (W .LE. AMIN1(FU,FV)) GO TO 130
         GO TO 201
   42 IF (B .NE. B0) GO TO 201
         IF (B .EQ. 0.0) GO TO 201
         W = F(B)
         IF (W .LE. AMIN1(FU,FV)) GO TO 150
         GO TO 201
C
C              CHECK IF 0 IS A LOCAL MINIMUM
C
  100 IF (B .LE. ATOL) GO TO 110
      X = 0.01*B
      IF (W .GT. F(X)) GO TO 180
      B = X
      GO TO 100
C
  110 IF (ABS(A) .LE. ATOL) GO TO 120
      X = 0.01*A
      IF (W .GT. F(X)) GO TO 180
      A = X
      GO TO 110
C
  120 X = 0.0
      ERROR = AMAX1(ABS(A),B)
      RETURN
C
C             CHECK IF A0 IS A LOCAL MINIMUM
C
  130 TOL = AMAX1(RTOL*ABS(A),ATOL)
  131 X = A + 0.01*E
      IF (W .GT. F(X)) GO TO 180
      B = X
      E = B - A
      IF (E .GT. TOL) GO TO 131
C
      X = A
      ERROR = E
      RETURN
C
C             CHECK IF B0 IS A LOCAL MINIMUM
C
  150 TOL = AMAX1(RTOL*ABS(B),ATOL)
  151 X = B - 0.01*E
      IF (W .GT. F(X)) GO TO 180
      A = X
      E = B - A
      IF (E .GT. TOL) GO TO 151
C
      X = B
      ERROR = E
      RETURN
C
  180 E = B - A
      U = A + C1*E
      V = A + C2*E
      FU = F(U)
      FV = F(V)
C
C             REFINEMENT OF THE LOCAL MINIMUM
C
  200 IND = 0
  201 IF (FU .GT. FV) GO TO 210
C
      B = V
      E = B - A
      V = U
      U = A + C1*E
      FV = FU
      FU = F(U)
      GO TO 220
C
  210 A = U
      E = B - A
      U = V
      V = A + C2*E
      FU = FV
      FV = F(V)
C
C        CHECKING THE ACCURACY OF THE LOCAL MINIMUM
C
  220 IF (E .LE. AMAX1(RTOL*ABS(A),ATOL)) GO TO 240
      IF (ABS(FV - FU) .GT. FTOL*AMAX1(ABS(FU),ABS(FV))) GO TO 200
      IF (IND .EQ. 1) GO TO 241
      IND = 1
      GO TO 201
C
C                    REPORT THE RESULTS
C
  240 IND = 0
  241 IF (FU - FV) 242,243,244
  242 X = U
      W = FU
      ERROR = C1*E
      RETURN
  243 X = V
      W = FV
      ERROR = E
      RETURN
  244 X = V
      W = FV
      ERROR = C1*E
      RETURN
      END
