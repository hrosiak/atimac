# -*- coding: utf-8 -*-
from flask import Flask, abort, jsonify, request, make_response
import atimacpy
import numpy as np

app = Flask(__name__)

def log_data(d):
    print("CATIMA:{\"IP\":%s, \"p\":%s,\"m\":%s}"%(request.remote_addr,d["projectile"],d["matter"]))

@app.route('/calculate', methods=['GET', 'POST'])
def de():
    try:
        data = request.get_json(force=True)
    except:
        return make_response(jsonify({"error": "JSON parsing"}))
    
    # now check input data
    if("projectile" not in data):
        return error("projectile missing")
    
    if("matter" not in data or len(data["matter"])<1):
        return error("matter missing")
    
    
    if(len(data["projectile"])<3):
        return error("projectile ")
    
    
    Ap = data["projectile"][0]
    Zp = data["projectile"][1]
    T = data["projectile"][2]
    
    mat = atimacpy.atima_matter()
    for m in data["matter"]:
        if(len(m)<5):
            error("matter error")
        try:
            a = float(m[0])
            z = int(m[1])
            rho = float(m[2])
            th = float(m[3])
            gas = int(m[4])
        except:
            return error("matter error")
        mat.add(a,z,rho,th,gas)
        
    mat.calculate(Ap,Zp,T)
    return make_response(jsonify(mat.getJSON()))

@app.route("/data",methods=['GET', 'POST'])
def data():
    try:
        data = request.get_json(force=True)
    except:
        return make_response(jsonify({"error": "JSON parsing"}))

    # now check input data
    if("projectile" not in data):
        return error("projectile missing")
    
    if("matter" not in data or len(data["matter"])<1):
        return error("matter missing")
    
    
    if(len(data["projectile"])<3):
        return error("projectile ")
    
    energy_table = np.logspace(-2.99,4.99,num=500)
    Ap = data["projectile"][0]
    Zp = data["projectile"][1]

    mat = atimacpy.atima_matter()
    energy = []
    dEdx = []
    ran = []
    res = {}
    Am = 0
    Zm = 0
    for m in data["matter"]:
        if(len(m)<5):
            error("matter error")
        try:
            Am = float(m[0])
            Zm = int(m[1])
            rho = float(m[2])
            th = float(m[3])
            gas = int(m[4])
        except:
            return error("matter error")
        mat.add(Am,Zm,rho,th,gas)
        
    for e in energy_table:
        energy.append(e)
        mat.calculate(Ap,Zp,e)
        dEdx.append(mat.results[0]["dEdxi"])
        ran.append(mat.results[0]["range"])
    res = {"energy":energy,"dEdx":dEdx,"range":ran,"Am":Am,"Zm":Zm}
    return make_response(jsonify(res))


@app.route("/version",methods=['GET'])
def version():
    return make_response(jsonify({"version":"1.0","atima":"1.3"}))

@app.route('/')
def main():
    ### TODO
    return "This will be main page with help"

@app.errorhandler(404)
def error_404(e):
    return make_response(jsonify({"error":"Error 404"}))

def error(m):
    return make_response(jsonify({"error": m}))

@app.after_request
def after_request(response):
    response.headers.add('Access-Control-Allow-Origin', '*')
    response.headers.add('Access-Control-Allow-Headers', 'Content-Type,Authorization')
    response.headers.add('Access-Control-Allow-Methods', 'GET,PUT,POST,DELETE, OPTIONS')
    return response

if __name__ == '__main__':
    app.run()
