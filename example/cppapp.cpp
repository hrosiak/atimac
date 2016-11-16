#include <stdio.h>

extern "C"{
#include "atimac.h"
}

#include "atimapp.h"

int main(){
	double mp,ein;
	int zp;

	atima_matter matter("test matter");
    atima_matter matter1("matter1");
    atima_matter matter2("matter2");
    
	mp = 238;
	zp = 92;
	ein = 1000;

	struct atima_results results;
    
	struct atimac_target p10, graphite;
	atimac_target_create(&p10,0,216,0.0017,17,1);
	atimac_target_create(&graphite,12,6,3,3,0);
	atimac_target_th_mm(&p10,10); // set thickness to 10mm
	
	/*
	matter1.add(p10);
	matter1.calculate(mp,zp,1000);
	matter1.print();
	matter1(mp,zp,1000);
	matter1.print();
	*/
	
	matter2.add(12,6,2.25,100,0);
	matter2.add(graphite);

	matter2.calculate(mp,zp,1000);
	matter2.print();
	matter2(mp,zp,1000);
	matter2.print();
        //matter = matter1+matter2;
//	matter.calculate(mp,zp,1000);
//	matter.print();

	//FILE *fw = fopen("out.txt","w");
	//for(int i=850;i<900;i++){
//		ein = (double)i;
		//matter1.calculate(mp,zp,ein);
		//fprintf(fw,"%g %g\n",ein, matter1.res[1].de);
	//}	
	//fclose(fw);
	return 1;
	};

