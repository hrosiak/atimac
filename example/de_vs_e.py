import atimacpy
import numpy as np

m = atimacpy.atima_matter()
#m.add(0,26,7.874,39.37,0)
m.add(0,214,1.38,2.76,0)
m.add(0,216,0.0017,68,1)

zp = 92
ap = 238.0032

fr = open("out.txt","w")

for e in np.arange(600,900,1):
    m.calculate(ap,zp,e)
    de = m.get(1)['dE']
    estr = m.get(0)['Estr']
    fr.write("%g %g %g\n"%(e,estr,de))
    
fr.close()

