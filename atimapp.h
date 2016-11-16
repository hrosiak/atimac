/**
 *  Class to caclulate and manipulate matter layers for atima
 *  A. Prochazka
 *  last update: 14.10.2016
 */ 

#ifndef atimapp
#define atimapp
#include <vector>
#include <map>
#include <cmath>
#include <string>
#include <iostream>
#include <sstream>

extern "C"{
#include "atimac.h"
}

#define ATIMA_JSON

class atima_matter{
	public:
	/**
	 * Constructor can take matter name, its not used for calculation
	 */ 
	atima_matter(){clear();name="";}
	atima_matter(const char *_n){clear();setname(_n);}
	
	/**
	 * Add material layer to the class
	 * @param _t is struct atimac_target structure defined in atimac.h
	 */ 
	void add(struct atimac_target _t){matter.push_back(_t);}
	
	/**
	 * Add material layer to the class
	 * @param _at mass number of the target matter, if 0 natural element mass is used
	 * @param _zt proton number of the target matter, can be >200 for mocadi matter
	 * @param _rho density in g/cm3
	 * @param _th thickness in mg/cm3
	 * @param _isgas 1 if material is gas, otherwise 0, default is 0 if not specified
	 */ 
	void add(double _at, int _zt, double _rho, double _th, int _isgas=0);
	
	/**
	 * Clear and reset variables for result storing
	 */ 
	void clear(){res.clear();total.clear();sigma_a = 0; sigma_E = 0;dE = 0; Eout = Ein;tof=0;}
	
	/**
	 * Main function to call C atima calculation
	 * @param _ap mass number of projectile
	 * @param _zp charge of the projectile
	 * @param ein energy of the projectile in MeV/u
	 */ 
	void calculate(double _ap, int _zp, double ein);
	
	/**
	 * get total results map
	 * map cotain string->double map
	 */ 
	std::map<std::string,double> result(){return total;};
	
	/**
	 * return certain key from total results,
	 * ie results("Eout") return Eout from total results
	 * @param s string identifier for map
	 */ 
	double result(std::string s){return total.count(s)?total[s]:-1;};
	
	/**
	 * Return struct atima_results for defined material layer
	 * @param i index number of material results to be returned
	 */ 
	struct atima_results result(int i){return res[i];};
	
	/**
	 * shortut to calculate function
	 * material(A,Z,E) is the same as material.calculate(A,Z,E)
	 */ 
	void operator()(double _ap, int _zp, double ein){calculate(_ap,_zp,ein);}
	
	/**
	* set material name
	*/ 
	void setname(const char *_n){name = _n;};
	
	/**
	 * print results
	 */ 
	void print();
	
	
	#ifdef ATIMA_JSON
	/**
	 * return results in for of json string 
	 */ 
	std::string getjson();
	#endif

	/**
	 * two atima_matter classes can be added together 
	 * and another atima_matter class is returned with combined matter
	 */ 
	friend atima_matter operator+(const atima_matter &m1, const atima_matter &m2);
    
	double sigma_a;  /**< total angular straggling  */
	double sigma_E;  /**< total Energy loss straggling  */
	double dE;    /**< total Energy loss */
	double Eout;  /**< Energy in */
	double Ein;   /**< Energy out  */
	#ifdef TOFSPLINE
    double tof;   /**< TOF */
    #endif
	double Ap;    /**< projectile mass number  */
	int Zp;       /**< projectile charge */
    double T;     /**< Energy in */ 
    
	std::vector<struct atimac_target> matter;  /**< vector of material layers */ 
	std::vector<struct atima_results> res;     /**< vector of partial results of each material layer */ 
	std::map<std::string,double> total;        /**< map of total results */ 
	std::string name;						   /**< matter name */ 
};
#endif
