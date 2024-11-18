/*  Comp 361 A3 Q3
    Kat Matheson
    40296043
*/
#include "apf.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <functional>
using namespace std;

// compilation
// g++ -Wall -g -std=c++20 -o Logistic Logistic.cpp apf.cpp -lgmpxx -lgmp

/* returns 
    logist(x) = c*x(1-x)
        calculated as x * (x-1) * -c for use of apf
    logistp(x) = c -2cx

*/
pair<function<apf(apf)>,function<apf(apf)>> logistic(double c)
{
    function<apf(apf)> logist = [=](apf x)
        -> apf
        {
            apf one(1.0);
            return apf::mult(apf::mult(x, one-x), c);
        };
    function<apf(apf)> logistp = [=](apf x)
        -> apf
        {
            return apf(c) - apf::mult(x, apf(2.0 * c));
        };
    
    return {logist, logistp};
}

vector<apf> checkCycle(vector<apf>* xarr, apf max_error, int n)
{
    vector<apf> result;
    int back = xarr->size() - 1;
    for(int i = 0; i < n; i++)
    {
        if(apf::reldiff(xarr->at(back-i), xarr->at(back-i-n)) < max_error)
        {
            result.push_back(xarr->at(back-i));
        }
    }
    if(static_cast<int>(result.size()) == n)
    {
        return result;
    }
    else
    {
        return vector<apf>();
    }
}

void runIteration(double x0, double c, int maxIterations, apf max_error, vector<apf>* xarr)
{
    apf xk, rel_error;
    pair<function<apf(apf)>, function<apf(apf)>> logfns = logistic(c);
    function<apf(apf)> logisticfn = logfns.first;

    xk = x0;
    xarr->push_back(xk);

    int count = 0;

    while(true)
    {
    // iterate
        count++;
        xk = logisticfn(xarr->back());
        //cout << "x" << count << ": " << xk << endl;

    // check convergence
        // to 0
        if(xk < max_error)
        {
            cout << "x0 = " << x0 << " converged to 0"
                 << " in " << count << " iterations." << endl;
            return;
        }
        // to current value
        rel_error = apf::reldiff(xk, xarr->back());
        if(rel_error < max_error)
        {
            cout << "x0 = " << to_string(x0) << " converged to: " << xk 
                 << " in " << count << " iterations." << endl;
            return;
        }
        // diverged after maxIterations
        else if(count >= maxIterations && maxIterations != 0)
        {
            cout << "x0 = " << to_string(x0) << " failed to converge within " << max_error
                 << " in " << to_string(maxIterations) << " iterations." << endl;
            return;
        }
        // to cycles
        for(int ncycles = 2; ncycles <= 8; ncycles++)
        {
            if(static_cast<int>(xarr->size()) < 2 * ncycles) { break; }

            vector<apf> cycle = checkCycle(xarr, max_error, ncycles);
            if(cycle.size() > 0)
            {
                cout << "x0 = " << x0 << " converged to a " << ncycles << "-cycle: ";
                for(apf c : cycle)
                {
                    cout << c << "->";
                }
                cout << " in " << count << " iterations." << endl;
                return;
            }
        }

        xarr->push_back(xk);
    }

}

int main()
{
    bool printFile = true;
    string filename = "pyq3.txt";

    apf::precision(1000);
    vector<apf>* xarr = new vector<apf>();

    apf max_error;
    int maxIterations = 100;
    max_error = "1e-10";

    ofstream ofout;
    try
    {
        ofout.open(filename, ios::out | ios::trunc);
    }
    catch(const std::exception& e)
    {
        cerr << e.what() << endl;
    }

    ofout << "max_error=" << max_error << endl << flush;

    vector<double> cvals = {0.7, 1.0, 1.8, 2.0, 3.3, 3.5, 3.97};
    for(double c : cvals)
    {
        cout << "c=" << c << endl << flush;
        ofout << "c=" << c << endl << flush;
        for(int i = 80; i <= 80; i++)
        {

            runIteration(0.01*i, c, maxIterations, max_error, xarr);
            
            if(printFile)
            {
                for(size_t i = 0; i < xarr->size(); i++)
                {
                    ofout << "x" << to_string(i)
                          << "=" << xarr->at(i) << endl << flush;
                }
            }
            xarr->clear();
        }

    }

    ofout.close();
    
    delete xarr;
}