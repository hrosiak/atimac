C   based on function ZBRENT copied from Numerical Recipes, 
C   modified for atima
C
      real*8 FUNCTION e_out(t,bcoef,n,k,range,ei,fraction)
      implicit real*8 (a-h, o-z)
      real*8 t(*), bcoef(*)
      PARAMETER (ITMAX=100,EPS=3.E-8,TOL=1.e-6)


      INTEGER ii,n

C---------
      FUNC(X) = range - bvalue(t,bcoef,n,k,x,0) - range * fraction      

C  Geaendert am 8.11.99: Kann jetzt Eout und Ein berechnen
      IF( fraction .GT. 0)THEN
        A=ei
        B=0.d0
        FA=FUNC(A)
        FB=FUNC(B)
        IF(FB*FA.GT.0.) PRINT*, 'Range(Ein) < range*fraction'
      ELSE
        A=ei
        B=ei
        FA=FUNC(A)
        DO 17 ii=1,13
          B=2*B
          FB=FUNC(B)
C  Aenderung am 20.04.2001: EXIT entfernt, durch GOTO ersetzt:
          IF( (FB*FA .LT. 0) .OR. (B .GT. 10000) ) GOTO 177
 17     CONTINUE
 177    CONTINUE
        IF(FB*FA.GT.0.) PRINT*, 'Range(Emax) < range*fraction'
      ENDIF
      FC=FB
      DO 11 ITER=1,ITMAX
        IF(FB*FC.GT.0.) THEN
          C=A
          FC=FA
          D=B-A
          E=D
        ENDIF
        IF(ABS(FC).LT.ABS(FB)) THEN
          A=B
          B=C
          C=A
          FA=FB
          FB=FC
          FC=FA
        ENDIF
        TOL1=2.*EPS*ABS(B)+0.5*TOL
        XM=.5*(C-B)
        IF(ABS(XM).LE.TOL1 .OR. FB.EQ.0.)THEN
C         ZBRENT=B
          e_out = b
          RETURN
        ENDIF
        IF(ABS(E).GE.TOL1 .AND. ABS(FA).GT.ABS(FB)) THEN
          S=FB/FA
          IF(A.EQ.C) THEN
            P=2.*XM*S
            Q=1.-S
          ELSE
            Q=FA/FC
            R=FB/FC
            P=S*(2.*XM*Q*(Q-R)-(B-A)*(R-1.))
            Q=(Q-1.)*(R-1.)*(S-1.)
          ENDIF
          IF(P.GT.0.) Q=-Q
          P=ABS(P)
          IF(2.*P .LT. MIN(3.*XM*Q-ABS(TOL1*Q),ABS(E*Q))) THEN
            E=D
            D=P/Q
          ELSE
            D=XM
            E=D
          ENDIF
        ELSE
          D=XM
          E=D
        ENDIF
        A=B
        FA=FB
        IF(ABS(D) .GT. TOL1) THEN
          B=B+D
        ELSE
          B=B+SIGN(TOL1,XM)
        ENDIF
        FB=FUNC(B)
11    CONTINUE
      PRINT*, 'ZBRENT exceeding maximum iterations.'
      ZBRENT=B
      RETURN
      END
