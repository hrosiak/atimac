#include <cppcms/application.h>
#include <cppcms/applications_pool.h>
#include <cppcms/service.h>
#include <cppcms/http_response.h>
#include <cppcms/http_request.h>
#include <cppcms/url_dispatcher.h>
#include <cppcms/url_mapper.h>
#include <cppcms/json.h>
#include <iostream>
#include <sstream>
#include "atimapp.h"


class atimac_server : public cppcms::application {
public:
    atimac_server(cppcms::service &srv) :
        cppcms::application(srv) 
    {
        dispatcher().assign("/version",&atimac_server::version,this);
        mapper().assign("version","/version");
        
        dispatcher().assign("/calculate",&atimac_server::calculate,this);
        mapper().assign("calculate","/calculate");
        
        dispatcher().assign("",&atimac_server::version,this);
        mapper().assign("");
        
        mapper().root("/acppcms"); 
    }
    
    void version();
    void calculate();
    //virtual void main(std::string url);

    void headers(){
    response().set_header("Access-Control-Allow-Headers", "accept,Content-Type");
    response().set_header("Access-Control-Allow-Methods","GET,POST,PATCH,DELETE");
    response().set_header("Access-Control-Allow-Origin","*");
    };
};


void atimac_server::version()
{
    cppcms::json::value rep;
    rep["version"] = atimac_version;
    std::cout<<"returnning version: "<<rep<<std::endl;
    headers();
    response().content_type("application/json");
    response().out()<<rep;
}

void atimac_server::calculate()
{
    double ap, zp, ein;
    atima_matter mat;
    response().content_type("application/json");
    headers();
    std::cout<<"method: "<<request().request_method()<<std::endl;
    
    cppcms::json::value rep;
    auto rawpost = request().raw_post_data();
    std::istringstream post(std::string (reinterpret_cast<char *>(rawpost.first),rawpost.second));
    std::cout<<"received: "<<request().content_length()<<" "<<post.str()<<std::endl;
    cppcms::json::value req;
    if(!req.load(post,true)){
        std::cout<<"Could not parse request json"<<std::endl;
        rep["error"] = "Could not parse JSON request";
        response().out()<<rep;
        return;
    }
    //td::cout<<req<<std::endl;
    try{
        auto projectiledata = req.get<cppcms::json::array>("projectile");
        if(projectiledata.size() != 3)throw cppcms::json::bad_value_cast();
        ap = projectiledata[0].number();
        zp = projectiledata[1].number();
        ein = projectiledata[2].number();
    }
    catch(cppcms::json::bad_value_cast){
        rep["error"] = "projectile";
        response().out()<<rep;
        return;
    }
    
    try{
        auto matterdata = req.get<cppcms::json::array>("matter");
        if(matterdata.size() < 1){
            throw cppcms::json::bad_value_cast();
            rep["hint"] = "matter field is wrong";
            }
        for(auto &entry:matterdata){
            auto layer = entry.array();
            if(layer.size()!=5){
                rep["hint"] = "layer array size is not 4";
                throw cppcms::json::bad_value_cast();
                }
            mat.add(layer[0].number(), (int)layer[1].number(), layer[2].number(),layer[3].number(),(int)layer[4].number());
        }
        
    }
    catch(cppcms::json::bad_value_cast){
        rep["error"] = "matter";
        response().out()<<rep;
        return;
    }
    
    std::cout<<"projectile: A = "<<ap<<", Z = "<<(int)zp<<std::endl;
    for(auto &e:mat.matter){
        std::cout<<"matter: A = "<<e.at<<", Z = "<<e.zt<<", rho = "<<e.rho<<", th = "<<e.th<<std::endl;
        }
    
    
    mat.calculate(ap,(int)zp,ein);
    mat.print();
    std::cout<<mat.getjson();
    response().out()<<mat.getjson();
}

int main(int argc,char ** argv)
{
    try {
        cppcms::service srv(argc,argv);
        srv.applications_pool().mount(cppcms::applications_factory<atimac_server>());
        srv.run();
    }
    catch(std::exception const &e) {
        std::cerr<<e.what()<<std::endl;
    }
}
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
