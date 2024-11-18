/*  Comp 361 A3 Q1+Q2+Q5
    Kat Matheson
    40296043
*/
#include "apf.h"
#include <iostream>
#include <iomanip>
#include <functional>
#include <fstream>
#include <deque>
#include <cmath>
using namespace std;

// compilation
// g++ -Wall -g -std=c++20 -o CubeRoot CubeRoot.cpp apf.cpp -lgmpxx -lgmp

// for solving val^{1/exp} = \sqrt[exp]{val}
pair<function<apf(apf)>,function<apf(apf)>> rootG(int exp, double val)
{
    function<apf(apf)> g = [=](apf x) 
        -> apf
        {
            return apf::exp(x, exp) - val;
        };
    function<apf(apf)> gp = [=](apf x) 
        -> apf
        {
            return apf::mult(apf::exp(x, exp-1), exp);
        };
    return {g,gp};
}

/* Newton's Method: returns x_{k+1} = x_k + g(x_k)/g'(x_k)
   Chord Method:    returns x_{k+1} = x_k + g(x_k)/g'(x_0)
    ggp is g and g' (g and g 'prime')
    try rootG for an example
    */
apf applyIteration(apf xk, pair<function<apf(apf)>,function<apf(apf)>> ggp, string type, apf x0)
{
    // apply x[k+1] = x[k] - g(x[k])/g'(x[k])
    // set xk to x_{k+1}
    apf gxk, gpxk, result;

    gxk = ggp.first(xk);
    if(type == "chord") 
    { gpxk = ggp.second(x0); }
    else if(type == "newton")
    { gpxk = ggp.second(xk); }
    
    result = xk - apf::div(gxk, gpxk);
    return result;
}

/*
Newton's Method for \sqrt[n]{x}
    - int n > 1
    - float x > 0
    - algorithm finds solution y in g(y) = 0
        g(x) = x**n - x
        g'(x) = nx**(n-1)
    - iteration
        x[k+1] = x[k] - g(x)/g'(x)
    */
apf generateIterationOutput(pair<function<apf(apf)>,function<apf(apf)>> ggp,
                            apf x0, int minimumIterations, int maxIterations, int epsilonExp,
                            string type = "newton", bool consoleout = false, string filename = "")
{
    deque<apf> xarr; // x_0 = 1
    apf xk, rel_error, epsilon, max_x;
    max_x = "1e100";
    
    // initial value and error threshold
    xk = x0;
    if(consoleout)
    {
        cout << "x0 = " << x0 << endl;
    }
    epsilon = "1@" + to_string(epsilonExp);

    xarr.push_front(xk); 

    int count = 0;

    while(true)
    {        
        count++;
        xk = applyIteration(xarr.front(), ggp, type, x0);

        if(consoleout)
        {
            cout << std::setprecision(std::min(50,apf::precision())) << "x" << count << "= " << xk << endl;
        }

        rel_error = apf::reldiff(xk, xarr.front());

        xarr.push_front(xk);

        if(count >= maxIterations && maxIterations != 0)
        {
            cout << to_string(maxIterations) << " iterations reached. Did not converge with threshold 10e"
                 << to_string(epsilonExp) << "." << endl;
            break;
        }
        if((rel_error < epsilon && (count >= minimumIterations || minimumIterations == 0))) 
        { 
            if(consoleout)
            {
                cout << "Converged to: " <<  xarr.front() << " after " << count << " iterations." << endl;
            }
            break; 
        }
        if(xk > max_x)
        {
            if(consoleout)
            {
                cout << "Maximum value of x (" << max_x << ") exceeded." << endl;
                break;
            }
        }
    }

    apf xstar;
    xstar = xarr.front();

    if(filename != "") 
    {
        ofstream ofout;
        try
        {
            ofout.open(filename, ios::out | ios::app);
                    
            count = 0;
            while(!xarr.empty())
            {
                count++;
                ofout << std::setprecision(std::min(50,apf::precision())) << "x" << to_string(count)
                      << " = " << xarr.back() << endl << flush;
                xarr.pop_back();
            }

        }
        catch(const std::exception& e)
        {
            cerr << e.what() << endl;
        }

    }

    return xstar;

}


int main()
{
    string filename = "pyq1chord.txt";
    string type = "chord";
    long precision = 10000;
    apf::precision(precision); // questions ask for 10^-9 ish
    int errorTolerance = -20;
    int minimumIterations = 10;
    int maxIterations = 20;

    bool calculatingFPrime = false;
    bool testing1to3 = false;
    bool testing500 = false;
    bool testing10pows = false;
    bool pythonout = true;

    generateIterationOutput(rootG(2, 2), apf(1.0), minimumIterations, maxIterations, errorTolerance, type, true);

    if(calculatingFPrime)
    {
            // finding values needed for calculations
        apf x,y;
        // \sqrt[3]{5}
        x = generateIterationOutput(rootG(3, 5), apf(1.0), minimumIterations, maxIterations, errorTolerance, type, true);
        cout << setprecision(50) << "\\sqrt[3]{5}: " << x << endl;
        // \sqrt[3]{5^4}
        //y = generateIterationOutput(rootG(3, pow(5,4)), apf(1.0), minimumIterations, maxIterations, errorTolerance);
        //cout << setprecision(50) << "\\sqrt[3]{5^4} " << y << endl;

        apf result;
        //result = apf::div(apf::mult(y,2) - apf::mult(x,10), apf::mult(y, 3));
        //cout << "f'(\\sqrt[3]{5}) = " << setprecision(50) << result << endl;
    }

    if(testing1to3)
    {
        // check x_0 = {0.01n | n = 1,...,300}
        for(int i = 1; i <= 300; i++)
        {
            cout << "x_0 = " << to_string(0.01*i) << ":  ";
            generateIterationOutput(rootG(3,5), apf(0.01*i), minimumIterations, maxIterations, errorTolerance, type, true, "");
        }
    }

    if(testing500)
    {
        // check int x_0 = [-500,500] \ {0}
        for(long j = -500; j <= 500; j++)
        {
            if(j == 0) { continue; }
            cout << "x_0 = " << to_string(j) << ": ";
            generateIterationOutput(rootG(3,5), apf(j), minimumIterations, maxIterations, errorTolerance, type, true, "");
        }
    }

    if(testing10pows)
    {
        // check x_0 = 10^n for n = 1,...19
        int k = 100;
        apf::precision(1000);
        for(long n = 1; n <= k; n++)
        {
            long longx0 = pow(10, n);
            cout << "x_0 = 10^" << to_string(n) << ": ";
            generateIterationOutput(rootG(3,5), apf(longx0), minimumIterations, 1000000, errorTolerance, type, true, "");
        }
    }

    if(pythonout)
    {
        generateIterationOutput(rootG(3,5), apf(1.0), 0, maxIterations, errorTolerance, type, false, filename);
        generateIterationOutput(rootG(3,5), apf(0.1), 0, maxIterations, errorTolerance, type, false, filename);
    }

}