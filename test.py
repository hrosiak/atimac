import unittest
import atimacpy as atima
import json
import math

total_result_keys = ["Ein","Eout","dE","tof","Zp","Ap","T","sigma_a","sigma_E"]
partial_result_keys = ["Ein","Eout","dE","tof","sigma_a","sigma_E","sigma_r","dedxi","dedxo"]

class Test_atima_matter(unittest.TestCase):
    def test_results_has_all_keys(self):  # check if results contains all keys
        mat = atima.atima_matter()
        mat.add(12,6,2,2000,0)
        mat.calculate(238,92,1000)
        res = mat.result()
        
        for key in total_result_keys:
            self.assertTrue(key in res)
        
        for r in mat.results:
            for key in partial_result_keys:
                self.assertTrue(key in r)
    
    def test_calculation_calls(self):  # test if .calculate and () calls are identical
        mat1 = atima.atima_matter()
        mat1.add(12,6,2,2000,0)
        mat2 = atima.atima_matter()
        mat2.add(12,6,2,2000,0)
        
        res1 = mat1.calculate(238,92,1000)
        res2 = mat2(238,92,1000)
        
        for key in total_result_keys:
            self.assertTrue(res1[key] == res2[key])
    
    def test_getting_results(self):
        mat = atima.atima_matter()
        mat.add(12,6,2,500,0)
        mat.add(12,6,2,500,0)
        mat.add(40,18,0.0017,50,0)
        
        # following 2 calls should returns the same 
        res1 = mat.calculate(238,92,1000)
        res2 = mat.result()
        self.assertTrue(res1 == res2)
        
        # returns directly key from total results dictionary
        var = mat.result("Eout")
        self.assertTrue(res1["Eout"] == var)
        var = mat.result("tof")
        self.assertTrue(res1["tof"] == var)
        
        # return total result key via array operator
        var = mat["Eout"]
        self.assertTrue(res1["Eout"] == var)
        
        var = mat["Ex"]
        self.assertTrue(var is None)
        
        # if wrong key is requested return None
        var = mat.result("Ex")
        self.assertTrue(var is None)
        
        # getting partial result for layer, numbered from 0
        # getting via getter and directly
        resa = mat.result(0)
        resb = mat.results[0]
        resc = mat[0]
        self.assertTrue(resa == resb)
        self.assertTrue(resa == resc)
        resa = mat.result(1)
        resb = mat.results[1]
        resc = mat[1]
        self.assertTrue(resa == resb)
        self.assertTrue(resa == resc)
        
        # requesting out of array layer shoud return nothing
        res = mat.result(3)
        self.assertTrue(res is None)
        res = mat[3]
        self.assertTrue(res is None)
        
    def test_adding_matter(self):
        mat1 = atima.atima_matter()
        mat1.add(12,6,2,500,0)
        mat1.add(12,6,2,500,0)
            
        mat2 = atima.atima_matter()
        mat2.add(40,18,0.0017,50,0)
        
        # we can add matter
        mat3 = mat1+mat2
        self.assertTrue(isinstance(mat3,atima.atima_matter))
        self.assertTrue(mat3.nlayers()==3)
        res1 = mat3(238,92,1000)

        # matter should stay even if we removed previous
        mat1.matter = []
        mat2.matter = []
        self.assertTrue(mat3.nlayers()==3)    
        res2 = mat3(238,92,1000)
        self.assertTrue(res1==res2)
    
    def test_total_result_one_layer(self): # test if total results are like partial for one layer
        mat1 = atima.atima_matter()
        mat1.add(12,6,2,500,0)
        r1 = mat1(238,92,1000)
        r2 = mat1[0]
        self.assertEqual(r1["Eout"],r2["Eout"]);
        self.assertEqual(r1["dE"],r2["dE"]);
        self.assertEqual(r1["sigma_a"],r2["sigma_a"]);
        self.assertEqual(r1["sigma_E"],r2["sigma_E"]);
        self.assertEqual(r1["tof"],r2["tof"]);
        
    def test_total_result_multi_layer(self): # test some total results
        mat1 = atima.atima_matter()
        mat1.add(12,6,2,500,0)
        mat1.add(12,6,2,500,0)
        mat1.add(40,18,0.0017,70,0)
        
        r = mat1(238,92,1000)
        r0 = mat1[0]
        r1 = mat1[1]
        r2 = mat1[2]
        
        self.assertEqual(r["Eout"],r2["Eout"]);
        
        key = "dE"
        self.assertEqual(r[key],r0[key]+r1[key]+r2[key]);
        key = "tof"
        self.assertEqual(r[key],r0[key]+r1[key]+r2[key]);
        
        key = "sigma_a"
        self.assertEqual(r[key],math.sqrt(math.pow(r0[key],2)+math.pow(r1[key],2)+math.pow(r2[key],2)));
        
        # check if this is really correct
        #key = "sigma_E"
        #self.assertEqual(r[key],math.sqrt(math.pow(r0[key],2)+math.pow(r1[key],2)+math.pow(r2[key],2)));
        
    def test_total_result_multi_layer2(self): # 3 targets with same total thickness, differently divided
        th = 1000
        mat1 = atima.atima_matter()
        mat1.add(12,6,2,th,0)
        mat1.add(12,6,2,th,0)
        mat1.add(12,6,2,th,0)
        mat1.add(12,6,2,th,0)
        mat1.add(12,6,2,th,0)
        mat1.add(12,6,2,th,0)
        mat1.add(12,6,2,th,0)
        mat1.add(12,6,2,th,0)
        
        mat2 = atima.atima_matter()
        mat2.add(12,6,2,th,0)
        mat2.add(12,6,2,th*7,0)
        
        mat3 = atima.atima_matter()
        mat3.add(12,6,2,th*8,0)
        
        r1 = mat1(238,92,1000)
        r2 = mat2(238,92,1000)
        r3 = mat3(238,92,1000)
        
        key="dE"
        self.assertAlmostEqual(r1[key],r2[key],places=1)
        self.assertAlmostEqual(r1[key],r3[key],places=1)
        
        key="Eout"
        self.assertAlmostEqual(r1[key],r2[key],places=3)
        self.assertAlmostEqual(r1[key],r3[key],places=3)
        
        key="tof"
        self.assertAlmostEqual(r1[key],r2[key],places=7)
        self.assertAlmostEqual(r1[key],r3[key],places=7)
        
        key="sigma_a"
        self.assertAlmostEqual(r1[key],r2[key],places=5)
        self.assertAlmostEqual(r1[key],r3[key],places=5)
        
        key="sigma_E"
        self.assertAlmostEqual(r1[key],r2[key],places=5)
        self.assertAlmostEqual(r1[key],r3[key],places=5)

    def test_total_result_multi_layer3(self): # 3 targets with same total thickness, differently divided
        mat1 = atima.atima_matter()
        mat1.add(12,6,2,1000,0)
        mat1.add(40,18,0.0017,60,1)
        mat1.add(0,14,2.3212,116.06,0)
        
        
        r1 = mat1(123.9059,54,700)
        
        key="Eout"
        self.assertAlmostEqual(r1[key],637.58,places=1)
        key="sigma_E"
        self.assertAlmostEqual(r1[key],0.24,places=1)
        key="sigma_a"
        self.assertAlmostEqual(r1[key],1.04,places=1)
    
if __name__ == '__main__':
    unittest.main()

