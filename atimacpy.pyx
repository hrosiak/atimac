cimport atimacpy 
import math

class atima_matter:
    def __init__(self):
        self.total = {}
        self.results = []
        self.matter = []
        self.A = 0
        self.Z = 0
        self.T = 0
        self.clear()
    
    def clear(self):
        self.results = []
        self.total = {"Ein":0, "Eout":0, "Eloss":0, "sigma_a":0, "sigma_E":0, "Ap":0, "Zp":0,"T":0,"tof":0}
    
    def add(self,a,z,rho,th,isgas):
        self.matter.append([a,z,rho,th,isgas])
    
    def nlayers(self):
        return len(self.matter)
    
    def __add__(self,other):
        r = atima_matter()
        r.matter = self.matter + other.matter
        return r
    
    def result(self,n=None):
        if(n is None):
            return self.total
        if(isinstance(n,int) and n<len(self.results)):
            return self.results[n]
        if(isinstance(n,str) and (n in self.total)):
            return self.total[n]
        return None
    
    # array like getter
    def __getitem__(self,key):
        if(isinstance(key,str) and (key in self.total)):
            return self.total[key]
        if(isinstance(key,int) and (key < len(self.results) )):
            return self.results[key]
        
        return None
    
    def __call__(self,a,z,e):
        return self.calculate(a,z,e)
        
    def calculate(self,a,z,e): 
        cdef atima_results ares;
        self.clear();
        self.total["Ein"] = e;
        self.total["Eout"] = e;
        ein = e
        dedxprev = 99999
        for m in self.matter:
            if(atimacpy.atimac_calculate(a,z,ein,m[0],m[1],m[2],m[3],m[4],&ares)):
                ein = ares.Eout
                #self.total["sigma_E"] = self.total["sigma_E"] + (math.pow(ares.sigma_E,2)*math.pow(ares.dedxo/ares.dedxi,2));
                #self.total["sigma_E"] = (math.pow(ares.sigma_E,2)) + (math.pow(ares.dedxo/ares.dedxi,2)*self.total["sigma_E"])
                if(ares.Eout):
                        self.total["sigma_E"] = + self.total["sigma_E"] + (math.pow(ares.sigma_E*a/ares.dedxo,2))
                        self.total["sigma_a"] = self.total["sigma_a"] + math.pow(ares.sigma_a,2);
                        self.total["tof"] = self.total["tof"] + ares.tof;
                self.total["Eloss"] = self.total["Eloss"] + ares.dE;
            else:
                ares.Ein = ein
                ares.Eout = ein
            self.results.append({"Ein":ares.Ein,"Eout":ares.Eout,"sigma_E":ares.sigma_E,"Eloss":ares.dE,"sigma_a":ares.sigma_a,"sigma_r":ares.sigma_r,"range":ares.range,"tof":ares.tof,"dEdxi":ares.dedxi,"dEdxo":ares.dedxo})
        
        # now check if projectile was stopped and set total results
        self.total["Eout"] = ares.Eout
        if(ares.Eout>0):
                #self.total["sigma_E"] = math.sqrt(self.total["sigma_E"])
                self.total["sigma_E"] = math.sqrt(self.total["sigma_E"])*ares.dedxo/a
                self.total["sigma_a"] = math.sqrt(self.total["sigma_a"])
        else: 
                self.total["sigma_E"] = 0
                self.total["sigma_a"] = 0
        self.A = a;
        self.Z = z;
        self.T = e;
        return self.total
    
    def getJSON(self):
        res = {}
        res["projectile"] = {"A":self.A, "Z":self.Z, "T":self.T}
        res["result"] = self.total
        res["partial"] = self.results
        
        res["result"]["Ein"] = round(res["result"]["Ein"],4)
        res["result"]["Eout"] = round(res["result"]["Eout"],4)
        res["result"]["sigma_E"] = round(res["result"]["sigma_E"],4)
        res["result"]["sigma_a"] = round(res["result"]["sigma_a"],4)
        res["result"]["Eloss"] = round(res["result"]["Eloss"],3)
        res["result"]["tof"] = round(res["result"]["tof"],3)
        
        for e in res["partial"]:
            for k in ["Eout","Ein","sigma_E","sigma_a","Eloss","sigma_r","range","tof","dEdxi","dEdxo"]:
                e[k] = round(e[k],5)
        return res
        
    def print_results(self):
        res = self.total
        print("Summary\n-------")
        print("Ein = %g MeV/u, Eout = %g MeV/u, dE = %g MeV"%(res["Ein"],res["Eout"],res["Eloss"]))
        for i, r in enumerate(self.results):
            print("------- %d -------"%i)
            print("Ein = %g MeV/u, Eout = %g MeV/u, dE = %g MeV\nsigma_E = %g MeV/u, sigma_a = %g mrad"%(r["Ein"],r["Eout"],r["Eloss"],r["sigma_E"],r["sigma_a"]))
    
