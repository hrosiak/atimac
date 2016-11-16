#include <vector>
#include <cmath>
#include "atimapp.h"


atima_matter operator+(const atima_matter &m1, const atima_matter &m2){
    atima_matter m;
    for(auto &e:m1.matter){
        m.add(e);
    }
    for(auto &e:m2.matter){
        m.add(e);
    }
    return m;
}

void atima_matter::add(double _at, int _zt, double _rho, double _th, int _isgas){
	struct atimac_target _t;
	_t.at = _at;
	_t.zt = _zt;
	_t.rho = _rho;
	_t.th = _th;
	_t.isgas = _isgas;
	add(_t);
}

void atima_matter::calculate(double _ap, int _zp, double ein){
    clear();
    double e = ein;
    Ein = e;
    if(matter.size()<1)return;
	atima_results result;
	for(auto &m:matter){
		if(atimac_calculate_target(_ap,_zp, ein, &m,&result)){
			ein = result.Eout;
			sigma_a += result.sigma_a*result.sigma_a;
			dE += result.dE;
			sigma_E +=result.sigma_E*result.sigma_E;
			tof +=result.tof;
			}
		else{
			result.Ein = ein;
			result.Eout = ein;
			}
		res.push_back(result);
	}
	Eout = result.Eout; // last layer result Eout
	sigma_a = std::sqrt(sigma_a);
	sigma_E = std::sqrt(sigma_E);
    Ap = _ap;
    Zp = _zp;
    T = ein;
    total["Ap"] = Ap;
    total["Zp"] = Zp;
    total["T"] = T;
    total["Ein"] = T;
    total["Eout"] = Eout;
    total["tof"] = tof;
    total["astr"] = sigma_a;
    total["Estr"] = sigma_E;
    total["dE"] = dE;
    
}

void atima_matter::print(){
    if(matter.size()<1){
        std::cout<<"No matter defined."<<std::endl;
        return;
    }
	std::cout<<"--- "<<name<<" ---"<<std::endl;
	std::cout<<"Ein: "<<Ein<<" MeV/u"<<std::endl;
	std::cout<<"Eout: "<<Eout<<" MeV/u"<<std::endl;
	std::cout<<"dE: "<<dE<<" MeV"<<std::endl;
	std::cout<<"angular str.  "<<sigma_a<<" mrad"<<std::endl;
	std::cout<<"Estr: "<<sigma_E<<" MeV/u"<<std::endl;
	std::cout<<"TOF: "<<tof<<" ns"<<std::endl;
	std::cout<<std::endl<<"Partial data: "<<std::endl;
	
	for(int i=0;i<res.size();i++){
		std::cout<<"----------------"<<std::endl;
		atimac_print(&res[i]);
	}
}

#ifdef ATIMA_JSON
std::string atima_matter::getjson(){
	std::ostringstream ss;
	if(res.size()<1){
		ss<<"{\"error\":\"no matter defined\"}";
		return ss.str();
	}
	ss<<"{\"results\":{";
		ss<<"\"Ein\":"<<Ein;
		ss<<",\"Eout\":"<<Eout;
		ss<<",\"dE\":"<<dE;
		ss<<",\"Estr\":"<<sigma_E;
		ss<<",\"astr\":"<<sigma_a;
		ss<<",\"tof\":"<<tof;
		ss<<",\"A\":"<<Ap;
		ss<<",\"Z\":"<<Zp;
		ss<<",\"T\":"<<T;
		ss<<"},";
	ss<<"\"partial\":[";
	for(int i=0;i<res.size();i++){
		if(i>0)ss<<",";
		ss<<"{";
		ss<<"\"Ein\":"<<res[i].Ein;
		ss<<",\"Eout\":"<<res[i].Eout;
		ss<<",\"range\":"<<res[i].range;
		ss<<",\"Estr\":"<<res[i].sigma_E;
		ss<<",\"astr\":"<<res[i].sigma_a;
		ss<<",\"rstr\":"<<res[i].sigma_r;
		ss<<",\"dE\":"<<res[i].dE;
		ss<<",\"dedxi\":"<<res[i].dedxi;
		ss<<",\"dedxo\":"<<res[i].dedxo;
		ss<<",\"tof\":"<<res[i].tof;
		ss<<"}";
	}
	ss<<"]";
	ss<<"}";
	std::cout<<"returning "<<ss.str()<<std::endl;
	return ss.str();
}
#endif
