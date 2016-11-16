cdef extern from "atimac.h":

    ctypedef struct atima_results:
        double range;
        double Ein;
        double Eout;
        double sigma_a;
        double sigma_E;
        double sigma_r;
        double dedxo;
        double dedxi;
        double dE;
        double tof;

    double atimac_ipot(int z)
    int atimac_calculate(double ap, int zp, double ein, double at, int zt, double rho, double th, int isgas,atima_results* res);
    void atimac_print(atima_results* res);

