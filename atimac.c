#include <stdio.h> 
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include "atimac.h"
#include "nucdata.h"

const char *atimac_version = "1.0";
const char *atima_version = "1.3";
const int atimac_usecache = 1;

#define ATIMAC_ZMAX 120
#define ATIMAC_FILENAME_SIZE 36  // comes from atima fortran functions
#define ATIMAC_MAXNT 5          // naximum unmber of components in compunds material

/// external functions
extern void atima_(char *filename, int *nnuca, int *igasa, double *fntpa,
                    double *rhoa, int *zpa, double *mpa, 
                    int *za, double *ma, double *Ivalue, int *stnr);


/// helper functions, intended to use only localy
void atimac_getresults(struct splines *s, double ein, double th, atima_results *r);
void atimac_preparetarget(double *a, int *z);
void atimac_compoundtarget(int *z, int *nt, double *at, int *zt, double *ipot, int *stn);
int splines_storage_get_pointer(struct splines **ss);
int atimac_get_cache_splines(double ap, int zp, double ein, double at, int zt, double rho, double th, int isgas,struct splines **ss);


/// structure to hold target data, mainly for compund targets
struct atimac_calculation_target{
    int nt;
    double at[ATIMAC_MAXNT];
    int zt[ATIMAC_MAXNT];
    double ipot[ATIMAC_MAXNT];
    int stn[ATIMAC_MAXNT];
} atimac_target0;


/// Ionisation potentials
/// taken from ATIMA
/// Ionization potentials, from NIST page (ICRU Report#49, 1994) + extrapol.
double ionisation_potentials_z[ATIMAC_ZMAX+1] = {
    9999999,  // 0   
    19.2, // H
    41.8, // He
    40.0,
    63.7,
    76.0,
    78.0, // C
    82.0,
    95.0,
    115.0,
    137.0,
    149.0,
    156.0,
    166.0,
    173.0,
    173.0,
    180.0,
    174.0,
    188.0,
    190.0,
    191.0, //Ca (20)
    216.0,
    233.0,
    245.0,
    257.0,
    272.0,
    286.0, //Fe 26
    297.0,
    311.0,
    322.0,
    330.0,
    334.0,
    350.0,
    347.0,
    348.0,
    343.0,
    352.0,
    363.0,
    366.0,
    379.0,
    393.0,
    417.0,
    424.0,
    428.0,
    441.0,
    449.0,
    470.0,
    470.0,
    469.0,
    488.0,
    488.0, //Sn 50
    487.0,
    485.0,
    491.0,
    482.0, // Xe 54
    488.0,
    491.0,
    501.0,
    523.0,
    535.0,
    546.0,
    560.0,
    574.0,
    580.0,
    591.0,
    614.0,
    628.0,
    650.0,
    658.0,
    674.0,
    684.0,
    694.0,
    705.0,
    718.0,
    727.0,
    736.0,
    746.0,
    757.0,
    790.0,
    790.0, // Au 79
    800.0,
    810.0,
    823.0, //Pb 82
    823.0,
    830.0,
    825.0,
    794.0,
    827.0,
    826.0,
    841.0,
    847.0,
    878.0,
    890.0, // U 92
      900.0,
      910.0,
      920.0,
      930.0,
      940.0,
      950.0,
      960.0,
     970.0,
     980.0,
     990.0,
    1000.0,
    1010.0,
    1020.0,
    1030.0,
    1040.0,
    1050.0,
    1060.0,
    1070.0,
    1080.0,
    1090.0,
    1100.0,
    1110.0,
    1120.0,
    1130.0,
    1140.0,
    1150.0,
    1160.0,
    1170.0,
    };

///////////// atimac_ipot() //////////////////
double atimac_ipot(int z){
    if(z<1 || z>ATIMAC_ZMAX){
        return 0.0;
    }
    return ionisation_potentials_z[z];
}

/// mocadi compound target to atima target ///
void atimac_compoundtarget(int *z, int *nt, double *at, int *zt, double *ipot, int *stn){
    // clear before filling
    for(int i=0;i<ATIMAC_MAXNT;i++){
        at[i]=0;
        zt[i]=0;
        stn[i]=0;
        ipot[i]=0;
    }
    
    if(*z == 201){ // plastic H and C
        *nt = 2;
        zt[0] = 1;
        stn[0] = 10;
        zt[1] = 6;
        stn[1] = 9;
    }
    else if(*z == 202){ //air
        *nt = 2;
        zt[0] = 7;
        zt[1] = 8;
        stn[0] = 4;
        stn[1] = 1;
        }
    else if(*z == 203){ //PE
        *nt = 2;
        zt[0] = 1;
        zt[1] = 6;
        stn[0] = 2;
        stn[1] = 1;
    }
    else if(*z == 204){ //lH
        *nt = 1;
        zt[0] = 1;
        stn[0] = 1;
    }
    else if(*z == 205){ //lD
        *nt = 1;
        zt[0] = 1;
        at[0] = 2.014;
        stn[0] = 1;
    }
    else if(*z == 206){ //water
        *nt = 2;
        zt[0] = 1;
        zt[1] = 8;
        stn[0] = 2;
        stn[1] = 1;
    }
    else if(*z == 207){ //diamond
        *nt = 1;
        zt[0] = 6;
        stn[0] = 1;
    }
    else if(*z == 208){ //glass
        *nt = 2;
        zt[0] = 14;
        stn[0] = 1;
        zt[1] = 8;
        stn[1] = 2;
    }
    else if(*z == 209){ //AlMg3
        *nt = 1;
        zt[0] = 13;
        stn[0] = 1;
    }
    else if(*z == 210){ //ArCO2 (30%)
        *nt = 3;
        zt[0] = 6;
        stn[0] = 3;
        zt[1] = 8;
        stn[1] = 6;
        zt[2] = 18;
        stn[2] = 7;
    }
    else if(*z == 211){ //CF4
        *nt = 2;
        zt[0] = 6;
        stn[0] = 1;
        zt[1] = 9;
        stn[1] = 4;
    }
    else if(*z == 212){ //Isobutene
        *nt = 2;
        zt[0] = 6;
        stn[0] = 4;
        zt[1] = 1;
        stn[1] = 10;
    }
    else if(*z == 213){ //Kapton
        *nt = 4;
        zt[0] = 1;
        stn[0] = 10;
        zt[1] = 6;
        stn[1] = 22;
        zt[2] = 7;
        stn[2] = 2;
        zt[3] = 8;
        stn[3] = 5;
    }
    else if(*z == 214){ //Mylar
        *nt = 3;
        zt[0] = 6;
        stn[0] = 5;
        zt[1] = 1;
        stn[1] = 4;
        zt[2] = 8;
        stn[2] = 2;
    }
    else if(*z == 215){ //NaF
        *nt = 2;
        zt[0] = 11;
        stn[0] = 1;
        zt[1] = 9;
        stn[1] = 1;
    }
    else if(*z == 216){ //P10
        *nt = 3;
        zt[0] = 18;
        stn[0] = 9;
        zt[1] = 6;
        stn[1] = 1;
        zt[2] = 1;
        stn[2] = 4;
    }
    else if(*z == 217){ //polyolefin
        *nt = 2;
        zt[0] = 1;
        stn[0] = 16;
        zt[1] = 6;
        stn[1] = 10;
    }
    else if(*z == 218){ //CmO2
        *nt = 2;
        zt[0] = 96;
        stn[0] = 1;
        zt[1] = 8;
        stn[1] = 2;
    }
    else if(*z == 219){ //glass suprasil
        *nt = 2;
        zt[0] = 14;
        stn[0] = 1;
        zt[1] = 8;
        stn[1] = 2;
    }
    else if(*z == 220){ //havar
        *nt = 5;
        zt[0] = 27;
        stn[0] = 42;
        zt[1] = 24;
        stn[1] = 40;
        zt[2] = 28;
        stn[2] = 13;
        zt[3] = 26;
        stn[3] = 19;
        zt[4] = 74;
        stn[4] = 1;
    }
    else{
        *nt = 0;
        }
    
    // fill masses and potentials    
    for(int i=0;i<(*nt);i++){
        if(at[i]==0)at[i] = element_atomic_weight(zt[i]);
        ipot[i] = atimac_ipot(zt[i]);
    }
}


///////////// calculate() //////////////
int atimac_calculate(double ap, int zp, double ein, double at, int zt, double rho, double th, int isgas, atima_results *res){
    struct splines *ss=NULL;
    
    if(zt<1){
        atimac_results_clear(res);
        return 0;
        }
    if(rho<=0){
        atimac_results_clear(res);
        return 0;
        }
    if(th<=0){
        atimac_results_clear(res);
        return 0;
        }
        
    if(at == 0 && zt<200){
        at = element_atomic_weight(zt);
    }
    
    #ifdef ATIMAC_USE_SPLINE_CACHE
        atimac_get_cache_splines(ap,zp,ein,at,zt,rho,th,isgas, &ss);
        atimac_getresults(ss,ein,th,res);
    #endif
    // this is without splines caching
    #ifndef ATIMAC_USE_SPLINE_CACHE
        ss = (struct splines *)malloc(sizeof(struct splines));
        atimac_calculate_splines(ap, zp, at, zt, rho, isgas,ss);
        atimac_getresults(ss,ein,th,res);
        free_atima_spline(ss);
    #endif
    return 1;
}

int atimac_calculate_target(double ap, int zp, double ein, struct atimac_target *t, atima_results *res){
    return atimac_calculate(ap,zp,ein, t->at, t->zt, t->rho, t->th, t->isgas,res);
}

void atimac_calculate_splines(double ap, int zp, double at, int zt, double rho, int isgas,struct splines *ss){
    char filename[ATIMAC_FILENAME_SIZE+1]; //35 is because of atima fortran function
    double one = 1.0;
    sprintf(filename,"/dev/shm/atimatemp");
    atimac_preparetarget(&at,&zt);
    atima_(filename,&atimac_target0.nt, &isgas, &one, &rho, &zp, &ap, atimac_target0.zt, atimac_target0.at, atimac_target0.ipot,atimac_target0.stn);
    get_atima_spline(filename,ss);
    if(zt>200){ // fix mocadi Z for later 
        ss->z_target[0] = zt;
        ss->m_target[0] = at;
    }
    unlink(filename);
}

/// void atimac_preparetarget ///
void atimac_preparetarget(double *a, int *z){
    if(*z<1){
        atimac_target0.nt = 0;
        atimac_target0.zt[0] = 0;
        atimac_target0.at[0] = 0;
    }
    if(*z<200){
        atimac_target0.nt = 1;
        atimac_target0.zt[0] = *z;
        atimac_target0.at[0] = *a;
        atimac_target0.ipot[0] = atimac_ipot(*z);
        atimac_target0.stn[0] = 1;
    }
    if(*z>200){ // take mocadi extra material
        atimac_compoundtarget(z,&atimac_target0.nt,atimac_target0.at,atimac_target0.zt,atimac_target0.ipot,atimac_target0.stn);
    }
}

/// atimac_getresults ///
void atimac_getresults(struct splines *ss, double ein, double th, atima_results *res){
    res->Ein = ein;
    res->range = atima_range(ss,res->Ein);
//    res->Eout = atima_enver(ss,res->Ein,th/res->range);
    res->Eout = atima_enver(ss,res->Ein,th);
    res->dedxi = atima_dedx(ss,res->Ein);
    res->dedxo = atima_dedx(ss,res->Eout);
    res->sigma_r = atima_sigra(ss,res->Ein);
    res->sigma_E = atima_sigre(ss,res->Ein,res->Eout)*res->dedxo/ss->m_projectile; // in MeV/u
    res->sigma_a = atima_astragg(ss,res->Ein,res->Eout)*1000; // in mrad
    res->dE =(res->Ein - res->Eout)*(ss->m_projectile); // in MeV
    #ifdef TOFSPLINE
    res->tof = atima_tof(ss,res->Ein,res->Eout);
    #endif
}

void atimac_target_create(atimac_target *t, double a, int z, double rho, double th, int isgas){
    t->at = a;
    t->zt = z;
    t->rho = rho;
    t->th = th;
    t->isgas = isgas;
}

void atimac_target_th_mg(atimac_target *t, double th){
    if(th>=0){
        t->th = th;
    }
}

void atimac_target_th_mm(atimac_target *t, double th){
    if(th>=0 && t->rho>0){
        t->th = th*t->rho*100; // rho in g/cm3 and thickness in mm, results in mg/cm2
    }
}

void atimac_results_clear(atima_results *r){
    r->range=0; 
    r->Ein=0;
    r->Eout=0;
    r->sigma_a=0;
    r->sigma_E=0;
    r->sigma_r=0;
    r->dedxo=0;
    r->dedxi=0;
    r->dE=0;
    #ifdef TOFSPLINE
    r->tof=0;
    #endif
}

/// atimac_print ///
void atimac_print(atima_results *res){
    printf("Ein  = %f MeV/u\n",res->Ein);
    printf("Range  = %f mg/cm^2\n",res->range);
    printf("Eout  = %f MeV/u\n",res->Eout);
    printf("Estr  = %f MeV/u\n",res->sigma_E);
    printf("Range straggling = %f mg/cm^2\n",res->sigma_r);
    printf("dedxi = %f MeV/mg/cm2\n",res->dedxi);
    printf("dedxo = %f MeV/mg/cm2\n",res->dedxo);
    printf("Angular straggling = %f mrad\n",res->sigma_a);
    printf("dE = %f MeV\n",res->dE);
    #ifdef TOFSPLINE
    printf("TOF = %f ns\n",res->tof);
    #endif
}


///////////////////////// Spline Cache ///////////////////////////

splines_storage _splinescache = {0,0,0,0,NULL};

int atimac_get_cache_splines(double ap, int zp, double ein, double at, int zt, double rho, double th, int isgas,struct splines **ss){
    int i;
    if(_splinescache.num){ // try if the requested spline is the same as last time
        i = _splinescache.last;
        if( (_splinescache.s[i].z_projectile == zp) && (_splinescache.s[i].m_projectile == ap)  // check projectile
            && (_splinescache.s[i].z_target[0] == zt) && (_splinescache.s[i].m_target[0] == at) //check target 
            && (_splinescache.s[i].rho == rho) && (_splinescache.s[i].igas == isgas)){ //check target rho,  gas
                *ss = &_splinescache.s[i];
                _splinescache.last = i;
                return 1;
            }
    }
    
    // search through splines
    for(i=0;i<_splinescache.num;i++){
        if( (_splinescache.s[i].z_projectile == zp) && (_splinescache.s[i].m_projectile == ap)  // check projectile
            && (_splinescache.s[i].z_target[0] == zt) && (_splinescache.s[i].m_target[0] == at) //check target 
            && (_splinescache.s[i].rho == rho) && (_splinescache.s[i].igas == isgas)){ //check target rho,  gas
                *ss = &_splinescache.s[i];
                _splinescache.last = i;
                return 1;
            }          
    }
    
    // spline not in cache so calculate new one
    struct splines *s;
    splines_storage_get_pointer(&s);
    atimac_calculate_splines(ap, zp, at, zt, rho,isgas,s);
    *ss = s;
    return 1;
}

int splines_storage_get_pointer(struct splines **ss){
    
    if(_splinescache.num == 0){ //inittialization
        _splinescache.max=ATIMAC_MAXCACHE;
        _splinescache.s = (struct splines *)malloc(_splinescache.max*sizeof(struct splines));
        }
    
    
    if(_splinescache.num == _splinescache.max){ // current max reached
        if(_splinescache.cur == _splinescache.num){
             _splinescache.cur=0;
             }
             free_atima_spline(&(_splinescache.s[_splinescache.cur]));
             *ss = &(_splinescache.s[_splinescache.cur]);
             _splinescache.last = _splinescache.cur;
            _splinescache.cur++;
    }
    
    if(_splinescache.num<_splinescache.max){
         *ss = &(_splinescache.s[_splinescache.num]);
         _splinescache.last = _splinescache.num;
         _splinescache.num++;
        _splinescache.cur = _splinescache.num;
        
         return 1;
    }
    
    return 0;
}

void free_splines_storage(){
    for(int i=0;i<_splinescache.num;i++){
        free_atima_spline(&(_splinescache.s[i]));
    }
    free(_splinescache.s);
}


//////////////////////////////////////////////////////////////////
//////// Fololwing is copy pasted from MOCADI ////////////////////
//////////////////////////////////////////////////////////////////
void read_atima_spline(FILE *fp, struct bspline *pspline){
   int i;   
   double temp;
   int r;

   fscanf(fp, "%d%d", &pspline->n, &pspline->k);
   pspline->t = (double*) malloc( sizeof(double) * (pspline->n + pspline->k) );
   pspline->b = (double*) malloc( sizeof(double) * pspline->n );
   for ( i= 0; i < pspline->n + pspline->k;i++) { 
     r = fscanf(fp, "%le",&temp);
     pspline->t[i] = temp;
   }
   for ( i= 0; i < pspline->n; i++){
     fscanf(fp, "%le",&temp);
     pspline->b[i] = temp;
   }
}

int get_atima_spline(const char * file, struct splines *s) {
   int i;
   FILE *fp;

   if ((fp=fopen(file,"r")) == NULL) {
     printf("cannot open file %s \n", file);
     return(-1);
   }
   fscanf(fp, "%d\t%le", &s->z_projectile, &s->m_projectile);
   fscanf(fp, " %d", &s->n_nuc);
   s->z_target = (int *) malloc(sizeof(int) * (s->n_nuc));
   s->count = (double *) malloc(sizeof(double) * (s->n_nuc));
   s->pot = (double *) malloc(sizeof(double) * (s->n_nuc));
   s->m_target = (double *) malloc(sizeof(double) * (s->n_nuc));   
   for( i=0; i< (s->n_nuc); i++ ){
      fscanf(fp, "%d%le%le%le", &s->z_target[i], &s->count[i], &s->pot[i], &s->m_target[i]);
   } 
   fscanf(fp, "%le%d%le", &s->rho, &s->igas, &s->fntp);
   s->range_spline = (struct bspline *) malloc(sizeof(struct bspline));
   s->rstragg_spline = (struct bspline *) malloc(sizeof(struct bspline));  
   s->astragg_spline = (struct bspline *) malloc(sizeof(struct bspline));  
   #ifdef TOFSPLINE
   s->tof_spline = (struct bspline *) malloc(sizeof(struct bspline));  
   #endif
   read_atima_spline(fp, s->range_spline);
   read_atima_spline(fp, s->rstragg_spline);
   read_atima_spline(fp, s->astragg_spline);
   
   #ifdef TOFSPLINE
   read_atima_spline(fp, s->tof_spline);
   #endif

   fclose(fp);
   return(0);
}


/**************************************
 *                                    *
 * free memory for ATIMA splines      *
 *                                    *
 **************************************/

void free_atima_spline(struct splines *s) { 
   free(s->z_target);
   free(s->count);
   free(s->pot);
   free(s->m_target);
   free(s->range_spline->t);
   free(s->range_spline->b);
   free(s->range_spline);
   free(s->rstragg_spline->t);
   free(s->rstragg_spline->b);
   free(s->rstragg_spline);
   free(s->astragg_spline->t);
   free(s->astragg_spline->b);
   free(s->astragg_spline);
   
   #ifdef TOFSPLINE
   free(s->tof_spline->t);
   free(s->tof_spline->b);
   free(s->tof_spline);
   #endif
}

double atima_range(struct splines *s, double energy) {
   int i = 0;
   return bvalue_(s->range_spline->t,s->range_spline->b,&(s->range_spline->n),&(s->range_spline->k),&energy,&i);
}

double atima_enver(struct splines *s, double energy, double thickness) {
   double range;
   double r;
   double e;
   double lo, hi;
   double dedx;
   double step;
  // double r1,r2;
   int i=1;
   const double epsilon = 0.000001;
   int counter=0;
   
   range = atima_range(s, energy);
   dedx = 1/bvalue_(s->range_spline->t,s->range_spline->b,&(s->range_spline->n),&(s->range_spline->k),&energy,&i); // 

   if ( range <= thickness ) return 0.0;
   //if( thickness >= 1) return 0.0;
   //if(thickness ==0) return energy;

   lo = 0;
   hi = energy;
   
//   r2 = range - atima_range(s,hi) - thickness;
//   r1 = range - thickness;
     
   e = energy;
   dedx = 1/bvalue_(s->range_spline->t,s->range_spline->b,&(s->range_spline->n),&(s->range_spline->k),&e,&i);
   e = energy - (thickness*dedx);
   while(1){
        //r2 = range - atima_range(s,hi) - thickness;
        //r1 = range - atima_range(s,lo) - thickness;
        r = range - atima_range(s,e) - thickness;
        //printf("Eout loop, e=%lf,range = %lf,th=%lf, dif  =%lf, dedxi = %lf\n",e,range,thickness,r,dedx);
        if(fabs(r)<epsilon)return e;
        step = -r*dedx;
        e = e-step;
        if(e<0){return 0;} 
        dedx = 1/bvalue_(s->range_spline->t,s->range_spline->b,&(s->range_spline->n),&(s->range_spline->k),&e,&i); //
        counter++;
        if(counter>100){printf("too many iterations");return -1;}
   }    
   
return -1;
//   return e_out_(s->range_spline->t,s->range_spline->b,&(s->range_spline->n),&(s->range_spline->k),&range,&energy,&thickness);


}

double atima_sigra(struct splines *s, double energy) {
    int i = 0;
    double r1;

    r1 = bvalue_(s->rstragg_spline->t,s->rstragg_spline->b,&(s->rstragg_spline->n),&(s->rstragg_spline->k),&energy,&i);
    return (double) sqrt((double)r1);
}

double atima_sigre(struct splines *s, double ein, double eout) {
    int i = 0;
    double r1,r2;

    r1 = bvalue_(s->rstragg_spline->t,s->rstragg_spline->b,&(s->rstragg_spline->n),&(s->rstragg_spline->k),&ein,&i);
    r2 = bvalue_(s->rstragg_spline->t,s->rstragg_spline->b,&(s->rstragg_spline->n),&(s->rstragg_spline->k),&eout,&i);
    return sqrt(r1-r2);
}


double atima_dedx(struct splines *s, double energy) { 
    int i = 1;
    if(energy<=0){
        return 0;
    }
    return s->m_projectile / bvalue_(s->range_spline->t,s->range_spline->b,&(s->range_spline->n),&(s->range_spline->k),&energy,&i);
}

double atima_astragg(struct splines *s, double energy_in, double energy_out) {
    int i = 0;
    double a1;
    double a2;
               
    a1 = bvalue_(s->astragg_spline->t,s->astragg_spline->b,&(s->astragg_spline->n),&(s->astragg_spline->k),&energy_in,&i);
    a2 = bvalue_(s->astragg_spline->t,s->astragg_spline->b,&(s->astragg_spline->n),&(s->astragg_spline->k),&energy_out,&i);
    if(a1<a2){
      a2=a1; 
      printf("*** warning variance of ang.scatt<0, corrected to 0.\n");
    }
    
    return (double) sqrt( (double) (a1 - a2) );                                   
}

#ifdef TOFSPLINE
double atima_tof(struct splines *s, double energy_in, double energy_out){
   int i=0;
   double t1,t2;
   t1 = bvalue_(s->tof_spline->t,s->tof_spline->b,&(s->tof_spline->n),&(s->tof_spline->k),&energy_in,&i);
   t2 = bvalue_(s->tof_spline->t,s->tof_spline->b,&(s->tof_spline->n),&(s->tof_spline->k),&energy_out,&i);
   return t1-t2;
}
#endif

