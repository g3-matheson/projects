/*  Comp 361 A3 Q4
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
// g++ -Wall -g -std=c++20 -o Newtons2x2 Newtons2x2.cpp apf.cpp -lgmpxx -lgmp

// -g1(x) for RHS
apf g1rhs(pair<apf,apf> x)
{
    return apf::intminus(1, apf::mult(apf::exp(x.first,2), x.second));
}
// -g2(x) for RHS
apf g2rhs(pair<apf,apf> x)
{
    return apf::exp(x.first,4) - x.second;
}

// derivatives dg[1,2]/dx[1,2]
apf dg1dx1(pair<apf,apf> x)
{
    return apf::mult(apf::mult(x.first,x.second), 2);
}
apf dg1dx2(pair<apf,apf> x)
{
    return apf::exp(x.first,2);
}
apf dg2dx1(pair<apf,apf> x)
{
    return apf::mult(apf::exp(x.first,3), -4.0);
}
apf dg2dx2(pair<apf,apf> x)
{
    return apf(1.0);
}

// solves dx1, dx2 from G'(x) dx = -G(x)
// using Gaussian elimination
    // nb: for 2x2, dx can just be found with det(G'(x)) and adjoint
pair<apf,apf> solveMatrixEq(pair<apf,apf> x, vector<vector<apf>> gprime, vector<apf> grhs)
{
    /* 2x2 matrix:
        | a b |
        | c d |
        */
    // find the matrices as 2x2 apf values

    if(!apf::isZero(gprime[1][0]))
    {
        // perform gaussian elimination to get upper-triangular matrix in G'(x)'s place
        // find factor c/a to perform row2 - (c/a)row1
        apf multFactor = apf::div(gprime[1][0], gprime[0][0]);

        // perform row2 - (c/a)row1
        gprime[1][0] = apf(0.0);
        gprime[1][1] = gprime[1][1] - apf::mult(gprime[0][1], multFactor);
        grhs[1] = grhs[1] - apf::mult(grhs[0], multFactor);
    }

    // solve with back-substitution:
    apf dx1, dx2;
    // dx2 = grhs2 / gprime[1][1]
    dx2 = apf::div(grhs[1], gprime[1][1]);
    // dx1 = (grhs1 - gprime[0][1]*dx2)/gprime[0][0]
    dx1 = apf::div(grhs[0] - apf::mult(gprime[0][1], dx2), gprime[0][0]);

    return {dx1, dx2};
}

void runIteration(apf x1_0, apf x2_0, apf max_error, int maxIterations, vector<pair<apf,apf>>* xarr, bool printout = false)
{
    apf dnorm, det;
    bool swapped = false;

    pair<apf,apf> x, dx;
    apf x1, x2, dx1, dx2;

    int count = 0;
    x1 = x1_0;
    x2 = x2_0;
    xarr->push_back({x1, x2});

    if(printout)
    {
        cout << count << "--> " << "(" << x1 << "," << x2 << ")" << endl;
    }

    while(true)
    {
        count++;
        x = xarr->back();
        vector<vector<apf>> gprime = {{dg1dx1(x), dg1dx2(x)}, {dg2dx1(x), dg2dx2(x)}};
        vector<apf> grhs = {g1rhs(x), g2rhs(x)};

        // find determinant, if it's 0 then the system is not solvable
        det = apf::mult(gprime[0][0], gprime[1][1]) - apf::mult(gprime[0][1], gprime[1][0]);
        if(apf::isZero(det))
        {
            cout << "(x1,x2) = (" << x1 << "," << x2 << "): |G'(x)| = 0, cannot solve." << endl;
            return;
        }

        // if gprime[0][0] = 0, have to swap rows
        if(apf::isZero(gprime[0][0]))
        {
            apf tmp;
            gprime[0].swap(gprime[1]);
            tmp = grhs[1];
            grhs[1] = grhs[0];
            grhs[0] = tmp;
            swapped = true;
        }

        dx = solveMatrixEq(x, gprime, grhs);

        if(swapped)
        {
            x1 = x1 + dx.second;
            x2 = x2 + dx.first;
        }
        else
        {
            x1 = x1 + dx.first;
            x2 = x2 + dx.second;
        }


        xarr->push_back({x1,x2});
        if(printout)
        {
            cout << count << "--> " << "(" << x1 << "," << x2 << ")" << endl;
        }

        dnorm = apf::norm2(dx);
        if(dnorm < max_error)
        {
            cout << "(x1,x2) = (" << x1_0 << "," << x2_0 << ") converged to (" << x1 << "," << x2 
                 << ") in " << count << " iterations." << endl;
            return;
        }
        else if(count > maxIterations)
        {
            cout << "(x1,x2) = (" << x1_0 << "," << x2_0 << ") did not converge within "
                 << max_error << " in " << maxIterations << " iterations." << endl;
            return;
        }

    }

}

int main()
{
    apf::precision(1000);

    vector<pair<apf,apf>>* xarr = new vector<pair<apf,apf>>();

    int maxIterations = 10000;
    apf max_error;
    max_error = "10e-5";

    cout << "Integer Tests" << endl;
    for(long i = 0; i <= 3; i++)
    {
        for(long j = 0; j <= 3; j++)
        {
            runIteration(apf(i), apf(j), max_error, maxIterations, xarr);
        }
    }

    cout << "****************************" << endl;
    cout << "Float Tests near (0,0)" << endl;
    runIteration(apf(0.1), apf(0.1), max_error, maxIterations, xarr);
    runIteration(apf(0.01), apf(0.01), max_error, maxIterations, xarr);
    runIteration(apf(0.001), apf(0.001), max_error, maxIterations, xarr);

    cout << "****************************" << endl;
    cout << "Float Tests near (0,3)" << endl;
    runIteration(apf(0.1), apf(long(3)), max_error, maxIterations, xarr);
    runIteration(apf(0.001), apf(long(3)), max_error, maxIterations, xarr);
    runIteration(apf(0.00001), apf(long(3)), max_error, maxIterations, xarr);
    runIteration(apf(0.0000001), apf(long(3)), max_error, maxIterations, xarr);

    cout << "****************************" << endl;
    cout << "Float Tests near (3,0)" << endl;
    runIteration(apf(long(3)), apf(0.1), max_error, maxIterations, xarr);
    runIteration(apf(long(3)), apf(0.001), max_error, maxIterations, xarr);
    runIteration(apf(long(3)), apf(0.00001), max_error, maxIterations, xarr);
    runIteration(apf(long(3)), apf(0.0000001), max_error, maxIterations, xarr);

    cout << "****************************" << endl;
    cout << "Investigating (n,0) divergence" << endl;
    runIteration(apf(long(1)), apf(long(0)), max_error, 50, xarr, true);
    runIteration(apf(long(2)), apf(long(0)), max_error, 50, xarr, true);
    runIteration(apf(long(3)), apf(long(0)), max_error, 50, xarr, true);

    delete xarr;
}