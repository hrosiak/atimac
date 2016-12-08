#include <stdio.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <math.h>
#include "atimac.h"

static void test_u(void **state){
    struct atima_results results;
    atimac_calculate(238,92,1000, 12, 6, 2.253,1000,0,&results);
    assert_true( fabs(results.range - 10) < 0.5);
}

static void test_ca(void **state){
    struct atima_results results;
    
    // 1GeV/u
    atimac_calculate(47.9416,20,1000, 12.011, 6, 2.253,1000,0,&results);
    assert_true(fabs(results.range - 43468.599989) < 1);
    assert_true(fabs(results.Eout  - 983.485) < 0.01);
    assert_true(fabs(results.sigma_E  - 0.203587) < 0.001);
    assert_true(fabs(results.sigma_a  - 0.611274) < 0.001);
    assert_true(fabs(results.sigma_r  - 59.009583) < 0.1);
    assert_true(fabs(results.dedxi - 0.790277) < 0.001);
    
    // 100 MeV/u
    atimac_calculate(47.9416,20,100, 12.011, 6, 2.253,1000,0,&results);
    assert_true(fabs(results.range - 1067.602240) < 1);
    assert_true(fabs(results.Eout  - 19.426725) < 0.01);
    assert_true(fabs(results.sigma_E  - 0.318969) < 0.001);
    assert_true(fabs(results.sigma_a  - 8.956124) < 0.01);
    assert_true(fabs(results.sigma_r  - 1.766805) < 0.1);
    assert_true(fabs(results.dedxi - 2.597550) < 0.001);
}

static void test_stopping(void **state){
    struct atima_results results;
    
    // 100 MeV/u
    atimac_calculate(47.9416,20,100, 12.011, 6, 2.253,100000,0,&results);
    assert_true(fabs(results.range-1067.6) < 1);
    assert_true(fabs(results.dedxi - 2.597550) < 0.001);
    assert_true(fabs(results.dedxo) < 0.001);
    assert_true(fabs(results.Eout) < 0.0001);
    assert_true(fabs(results.sigma_E) < 0.0001);
    assert_true(fabs(results.sigma_a) < 0.0001);
    assert_true(fabs(results.sigma_r  - 1.766805) < 0.1);
    
    // 0.1 MeV/u
    atimac_calculate(47.9416,20,0.1, 12.011, 6, 2.253,1000,0,&results);
    assert_true(fabs(results.Eout) < 0.0001);
    
    // 0.002 MeV/u
    atimac_calculate(1,1,0.02, 12.011, 6, 2.253,1,0,&results);
    assert_true(fabs(results.Eout) < 0.0001);
}

static void test_gastarget(void **state){
    struct atima_results results;
    
    // 1GeV/u
    atimac_calculate(238.0011,92,1000, 39.948002, 18, 0.0017,17,1,&results);
    assert_true(fabs(results.range - 11939.2) < 1);
    assert_true(fabs(results.Eout  - 998.903) < 0.01);
    assert_true(fabs(results.sigma_E  - 0.037535) < 0.001);
    assert_true(fabs(results.sigma_a  - 0.10846) < 0.001);
    assert_true(fabs(results.sigma_r - 10.9) < 0.1);
    assert_true(fabs(results.dedxi - 15.362729) < 0.001);
    }

static void test_splinescache(void **state){
    struct atima_results results;
    
    assert_true(_splinescache.num == 0);
    assert_true(_splinescache.max == 0);
    assert_true(_splinescache.cur == 0);
    assert_true(_splinescache.last == 0);
    assert_true(_splinescache.s == NULL);
    
    atimac_calculate(238,92,1000, 12, 6, 2.253,1000,0,&results);
    assert_true(_splinescache.num == 1);
    assert_true(_splinescache.cur == 1);
    assert_true(_splinescache.last == 0);
    assert_true(_splinescache.max > 1);
    assert_true(_splinescache.s != NULL);
    
    atimac_calculate(238,92,1000, 12, 6, 2.253,1000,0,&results);
    assert_true(_splinescache.num == 1);
    assert_true(_splinescache.cur == 1);
    assert_true(_splinescache.last == 0);
    
    atimac_calculate(238,92,1000, 0, 8, 2.253,1000,0,&results);
    assert_true(_splinescache.num == 2);
    assert_true(_splinescache.cur == 2);
    assert_true(_splinescache.last == 1);
    
    atimac_calculate(48,20,1000, 0, 8, 2.253,1000,0,&results);
    assert_true(_splinescache.num == 3);
    assert_true(_splinescache.cur == 3);
    assert_true(_splinescache.last == 2);
    
    
    atimac_calculate(238,92,1000, 12, 6, 2.253,1000,0,&results);
    assert_true(_splinescache.num == 3);
    assert_true(_splinescache.cur == 3);
    assert_true(_splinescache.last == 0);
    
    atimac_calculate(238,92,1000, 12, 6, 1.8, 1000,0,&results);
    assert_true(_splinescache.num == 4);
    assert_true(_splinescache.cur == 4);
    assert_true(_splinescache.last == 3);
    
    atimac_calculate(238,92,1000, 12, 6, 2.253, 90,0,&results);
    assert_true(_splinescache.num == 4);
    assert_true(_splinescache.cur == 4);
    assert_true(_splinescache.last == 0);
    
    atimac_calculate(238,92,1000, 0, 216, 0.0017, 60,0,&results);
    assert_true(_splinescache.num == 5);
    assert_true(_splinescache.cur == 5);
    assert_true(_splinescache.last == 4);
    
    atimac_calculate(238,92,1000, 0, 216, 0.0017, 60,0,&results);
    assert_true(_splinescache.num == 5);
    assert_true(_splinescache.cur == 5);
    assert_true(_splinescache.last == 4);
    
    atimac_calculate(238,92,1000, 0, 26, 7.8, 60,0,&results);
    assert_true(_splinescache.num == 6);
    assert_true(_splinescache.cur == 6);
    assert_true(_splinescache.last == 5);
    
    atimac_calculate(238,92,1000, 0, 26, 7.8, 60,0,&results);
    assert_true(_splinescache.num == 6);
    assert_true(_splinescache.cur == 6);
    assert_true(_splinescache.last == 5);
    
    }



int main(){
    const struct CMUnitTest tests[] = {
	cmocka_unit_test(test_splinescache),
    cmocka_unit_test(test_stopping),
    cmocka_unit_test(test_u),
	cmocka_unit_test(test_ca),
	cmocka_unit_test(test_gastarget)
	};
    
    return cmocka_run_group_tests(tests,NULL,NULL);
}



