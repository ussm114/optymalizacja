/*
 Copyright (C) 2010 Botao Jia

 This file is an implementation of the downhill simplex optimization algorithm using C++.
 To use BT::Simplex correctly, the followings are needed, inclusively.

 1. f: a function object or a function which takes a vector<class Type> and returns a Type, inclusively.
       Signature, e.g. for double Type,
            double f(vector<double> x);

 2. init: an inital guess of the fitted parameter values which minmizes the value of f.
          init must be a vector<D>, where D must be the exactly same type as the vector taken by f.
          init must have the exactly same dimension as the vector taken by f.
      init must order the parameters, such that the order follows the vector taken by f.
          e.g. f takes vector<double> x, where x[0] represents parameter1; x[1] represents parameter2, etc.
          And init must follow this order exactly, init[0] is the initial guess for parameter1,
          init[1] is the initial guess for parameter2, etc.

3 to 5 are all optional:

3. tol: the termination criteria. 
        It measures the difference of the simplex centroid*(N+1) of consecutive iterations.

4. x:  an initial simplex, which is calculated according to the initial trial parameter values.

5. iterations: maximum iterations.

The return value of BT::Simplex is a vector<Type>, 
which represents the optimized parameter values that minimize f.
The order of the parameter is the same as in the vector<Type> init.


 You can redistribute it and/or modify it at will.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.
*/

//simplex.h
#include <vector>
#include <limits>
#include <algorithm>
#include <functional>
#include <iostream>
#include "fparser.hh"

std::vector<double> Simplex(std::string function, int N, std::vector<std::vector<double> > simplex, double tol, int iterations)                                                         //max iteration step number
{
    FunctionParser fparser;
    std::string vars;
    if (N==1)
        vars = "x";
    else
    {
        for (int i = 0; i < N-1; i++)
            vars = vars + "x" + std::to_string(i+1) + ",";
        vars = vars + "x" + std::to_string(N);
    }

    fparser.AddConstant("pi", 3.1415926535897932);
    fparser.Parse(function, vars);

    const double a=1.0, b=1.0, g=0.5, h=0.5;   // coefficients
    //a: reflection  -> xr
    //b: expansion   -> xe
    //g: contraction -> xc
    //h: full contraction to x1

    std::vector<double> xcenter_old(N,0);   // simplex center * (N+1) N liczb z wart 0
    std::vector<double> xcenter_new(N,0);   // simplex center * (N+1)
    std::vector<double> vf(N+1,0);          // f evaluated at simplex vertices

    int x1 = 0, xn = 0, xnp1 = 0;
    //x1:   f(x1) = min { f(x1), f(x2)...f(x_{n+1} }
    //xnp1: f(xnp1) = max { f(x1), f(x2)...f(x_{n+1} }
    //xn:   f(xn)<f(xnp1) && f(xn)> all other f(x_i)
    int cnt = 0; //iteration step number
    
    // optimization begins
    for(cnt = 0; cnt < iterations; ++cnt)
    {
        for(int i = 0; i < N+1; i++)
            vf[i] = fparser.Eval(simplex[i]);

        x1 = 0; xn = 0; xnp1 = 0; // find index of max, second max, min of vf.

        for(int i = 0; i < vf.size(); i++)
        {
            if(vf[i] < vf[x1])
                x1 = i;
            if(vf[i]>vf[xnp1])
                xnp1=i;
        }
        xn = x1;

        for(int i = 0; i < vf.size(); i++)
            if(vf[i] < vf[xnp1] && vf[i] > vf[xn] )
                xn = i;

        // x1, xn, xnp1 are found

        std::vector<double> xg(N, 0);   // xg: centroid of the N best vertexes
        for(int i=0; i<simplex.size(); i++)
            if(i!=xnp1)
                std::transform(xg.begin(), xg.end(), simplex[i].begin(), xg.begin(), std::plus<double>() );

        std::transform(xg.begin(), xg.end(), simplex[xnp1].begin(), xcenter_new.begin(), std::plus<double>());
        std::transform(xg.begin(), xg.end(), xg.begin(), std::bind2nd(std::divides<double>(), N));
        // xg found, xcenter_new updated

        // termination condition
        double diff=0;          // calculate the difference of the simplex center
        for(int i=0; i<N; i++)
            diff += fabs(xcenter_old[i]-xcenter_new[i]);

        if (diff/N < tol) break;            // terminate the optimizer
        else xcenter_old.swap(xcenter_new); // update simplex center

        // reflection:
        std::vector<double> xr(N,0);
        for( int i=0; i<N; i++)
            xr[i]=xg[i]+a*(xg[i]-simplex[xnp1][i]);
        // reflection, xr found

        double fxr=fparser.Eval(xr);    // record function at xr

        if(vf[x1]<=fxr && fxr<=vf[xn])
            std::copy(xr.begin(), xr.end(), simplex[xnp1].begin());

        // expansion:
        else if(fxr<vf[x1])
        {
            std::vector<double> xe(N,0);
            for(int i = 0; i < N; i++)
                xe[i] = xr[i] + b * (xr[i] - xg[i]);
            if(fparser.Eval(xe) < fxr)
                std::copy(xe.begin(), xe.end(), simplex[xnp1].begin());
            else
                std::copy(xr.begin(), xr.end(), simplex[xnp1].begin());
        }   // expansion finished,  xe is not used outside the scope

        // contraction:
        else if(fxr > vf[xn])
        {
            std::vector<double> xc(N,0);
            for(int i = 0; i < N; i++)
                xc[i] = xg[i] + g * (simplex[xnp1][i] - xg[i]);
            if(fparser.Eval(xc) < vf[xnp1])
                std::copy(xc.begin(), xc.end(), simplex[xnp1].begin());
            else
            {
                for( int i=0; i<simplex.size(); i++ )
                    if( i!=x1 )
                        for(int j=0; j<N; j++)
                            simplex[i][j] = simplex[x1][j] + h * ( simplex[i][j]-simplex[x1][j] );
            }
        } // contraction finished, xc is not used outside the scope

    } // optimization is finished

    if(cnt==iterations)     //max number of iteration achieves before tol is satisfied
        std::cout<<"Iteration limit achieves, result may not be optimal"<<std::endl;

    return simplex[x1];
}

