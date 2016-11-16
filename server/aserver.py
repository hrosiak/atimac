# -*- coding: utf-8 -*-
from flask import Flask, abort, jsonify, request, make_response
import atimacpy

app = Flask(__name__)


@app.route('/calculate', methods=['GET', 'POST'])
def de():
    try:
        data = request.get_json(force=True)
    except:
        return make_response(jsonify({"error": "JSON parsing"}))

    print(data);
    
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
