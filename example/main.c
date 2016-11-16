#include <stdio.h>
#include "atimac.h"

int main(){
	double ein;
	double rho;
	int isgas;
	double thick;
	
	double mp,mt;
	int zp,zt;

	mp = 47.9416;
	zp = 20;
	ein = 1000;
	
	mt = 12.011;
	zt = 6;
	rho = 2.253;
	isgas = 0;
	thick = 1000;

	struct atima_results results;
	atimac_calculate(mp,zp,ein, mt, zt,rho,thick,isgas,&results);
	atimac_print(&results);
    
    struct atimac_target p10;
    atimac_target_create(&p10,0,216,0.017,17,1);
    atimac_target_th_mm(&p10,10); // set thickness to 10mm
    atimac_calculate_target(mp,zp,ein, &p10,&results);
    //atimac_calculate(mp,zp,ein, 0, 216,0.0016,17,1,&results);
	atimac_print(&results);
	
	return 1;
	};
