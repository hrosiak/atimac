       SUBROUTINE ATIMA( filename, 
     &                    nnuca, igasa, fntpa, rhoa, zpa, mpa, 
     &                    za, ma, IValue, stnr)


       IMPLICIT NONE 

       INCLUDE './atima.1.3/src/atima/system.inc'

       INTEGER   igasa, nnuca, zpa
       REAL*8  ma(nnuca), IValue(nnuca)
       INTEGER  za(nnuca), stnr(nnuca)
       REAL*8  rhoa, fntpa, GIValue, mpa

       INTEGER   i1,i2,j,unit,ioflag
       REAL*8    r1,r2,r3
       CHARACTER*35   filename       
       
         unit=17
         zp = zpa
         mp = mpa
         nnuc = nnuca
         GIValue = 0.0d0;

         IF( nnuca .GT. maxnuc) THEN 
           PRINT *,'nnuc = ', nnuc,' > maxnuc = ',maxnuc
         ELSE
C           j=1
           DO 177 j = 1,nnuc
             zt(j) = za(j)
             mt(j) = ma(j)
             IF( GIValue .NE. 0.0d0 )THEN
               pot(j) = GIValue
             ELSE 
               pot(j) = IValue(j)
             ENDIF
             anuc(j) = stnr(j)
 177      CONTINUE 
         ENDIF
         rho = rhoa
         fntp = fntpa
         gas = igasa 

C    here the splines are generated, more info passed in a common block 
         CALL calcint( filename )

       END 
