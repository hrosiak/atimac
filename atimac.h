#ifndef ATIMAC_H
#define ATIMAC_H

extern const char *atimac_version;
extern const char *atima_version;

#define ATIMAC_USE_SPLINE_CACHE 1
#define ATIMAC_MAXCACHE 100 // maximum cached splines
#define TOFSPLINE 1

/**
 * structure to store results of the ATIMA calculation
 * 
 */ 
typedef struct atima_results{
    double range;
    double Ein;
    double Eout;
    double sigma_a;
    double sigma_E;
    double sigma_r;
    double dedxo;
    double dedxi;
    double dE;
    #ifdef TOFSPLINE
    double tof;
    #endif
}atima_results;

/**
 * structure to hold target data
 */ 
typedef struct atimac_target{
    int zt;
    double at;
    double rho;
    double th;
    int isgas;
}atimac_target;


/**
 * structure to store atima splines in memory
 */ 
typedef struct splines_storage{
  int num;
  int max;  
  int cur;
  int last;
  struct splines *s;
} splines_storage;

extern splines_storage _splinescache;

/**
 * Returns ionisation potential for nucleus with proton number z
 * @param z integer from 1 to 120
 * @return ionisation potential in eV
 */ 
double atimac_ipot(int z);

/**
 * calculates projectile energy loss, range, angular struggling etc and stores the results in struct atima_results
 * 
 */ 
int atimac_calculate(double ap, int zp, double ein, double at, int zt, double rho, double th, int isgas, atima_results *res);

int atimac_calculate_target(double ap, int zp, double ein, atimac_target *t, atima_results *res);

/**
 * calculates atima splines for projectile - target configuration, splines are written to *ss structure
 * 
 */ 
void atimac_calculate_splines(double ap, int zp, double at, int zt, double rho, int isgas, struct splines *ss);


/**
 * prints the results
 */ 
void atimac_print(atima_results *res);

/**
 * Assign target data to the target structure
 */ 
void atimac_target_create(atimac_target *t, double a, int z, double rho, double th, int isgas);
/**
 * Set target thickness of target structure to specified value in mg/cm2
 */ 
void atimac_target_th_mg(atimac_target *t, double th);

/**
 * Set target thickness of target structure to specified value in mm
 */ 
void atimac_target_th_mm(atimac_target *t, double th);

/**
 * cleat atima_result structure
 */ 
void atimac_results_clear(atima_results *r);

/**
 * free allocated memory for splines cache storage
 */ 
void free_splines_storage();

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////// Following is copy pasted from MOCADI, just variable types are adjusted /////////////////////////
/////////////// float is changed to double 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct bspline {
	int n;
    int k;
    double *t;
    double *b;
};

struct splines { 
    int z_projectile;
    double m_projectile;
    int n_nuc;
    int *z_target;
    double *count; 
    double *pot;
    double *m_target;
    double rho;
    int igas;
    double fntp; 
    struct bspline *range_spline;
    struct bspline *rstragg_spline;
    struct bspline *astragg_spline;
    struct bspline *tof_spline;
};

int get_atima_spline(const char * file, struct splines *s);
void free_atima_spline(struct splines *s); 
double atima_range(struct splines *s, double energy); 
double atima_enver(struct splines *s, double energy, double thickness); 
double atima_sigra(struct splines *s, double energy);
double atima_sigre(struct splines *s, double ein, double eout);
double atima_dedx(struct splines *s, double energy);
double atima_astragg(struct splines *s, double energy_in, double energy_out);
double atima_tof(struct splines *s, double energy_in, double energy_out);

// for fortran code
double bvalue_(double *, double *, int *, int *, double *, int *);
double e_out_(double *, double *, int *, int *, double *, double *, double *);

#endif
