#pragma region Internals
#include "apf.h"
#include <Magick++.h>
#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <functional>
using namespace std;
using namespace Magick;
// compilation
// g++ -Wall -g -std=c++20 -o Logistic Logistic.cpp apf.cpp -lgmpxx -lgmp

// functions to use for logistic function and its derivative
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

// checks for n-cycles in last n entries
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
        /* to cycles
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
        */

        xarr->push_back(xk);
    }

}

void processBucket(pair<int,pair<apf,apf>> bucket, string filename, apf maxError, int maxPrint)
{
    apf cstart, cend, delta;
    int nFrames = bucket.first;
    cstart = bucket.second.first;
    cend = bucket.second.second;
    delta = apf::div(cend-cstart, nFrames);

    string filename;
    
    ofstream ofout;
    try // open ofout
    {
        ofout.open(filename, ios::out | ios::trunc);
    }
    catch(const std::exception& e)
    {
        cerr << e.what() << endl;
    }

    for(apf c = cstart; c < cend; c = c + delta)
    {
        // 
    }
}

void writeIterationsToFile(vector<pair<int,pair<apf,apf>>> cbuckets, string filenameRoot, apf maxError, int maxPrint)
{
    /*
        Write info file for python to know which files to read in
    */
    int bucketCounter = 0;
    string filename;
    for(pair<int,pair<apf,apf>> bucket : cbuckets)
    {
        filename = filenameRoot + to_string(bucketCounter++) + ".txt";
        processBucket(bucket, filename, maxError, maxPrint);
    }
}

void runPythonScript(string filename)
{
    string command = "python " + filename;
    int result = system(command.c_str());
    if(result != 0)
    {
        cerr << "Error running python script." << endl;
    }
}

void makeGIF(int frameCount, int delayMultiplier, vector<pair<int,pair<apf,apf>>> cbuckets, string gifFilename)
{
    try
    {
        InitializeMagick(nullptr);
        vector<Image> images;
        vector<string> imageFilenames;
        // TODO stopped here
        /* unpack cbuckets to find file names
            for each imageFilename use:
        */
        Image image("test.png");
        image.animationDelay(delayMultiplier);
        images.push_back(image);
        
        writeImages(images.begin(), images.end(), gifFilename);
        
    }
    catch(Magick::Exception& e)
    {
        std::cerr << "Error creating .gif: " << e.what() << endl;
    }
    
}

#pragma endregion Internals

/*  Logistic GIF Maker
    
    f(x) = cx(1-x)
        c \in (0, 4-\epsion) for small \epsilon > 0
    c values of interest (declared below)
        * (0,1) --> 0
        * (1,2) --> (c-1)/c
        * (2,3) --> (c-1)/c but slowly
    (c1)* 1+\sqrt{6} --> cycles start
    (c2)* https://oeis.org/A086181 ~ 3.54409 --> when period-doubling cascade starts
    (c3)* https://oeis.org/A098587 ~ 3.56995 --> onset of chaos
    (c4)* ~3.67857351042832226 --> Misiurewicz Point https://sprott.physics.wisc.edu/chaos/mispoint.htm
*/
int main()
{
    /*
        User Settings
    */
    string gifFilename = "1.gif";
    string filenameRoot = "Logistic_";
    string pythonScriptFilename = "LogisticPlot.py";
    
    int maxPrint = 500;         // HACK # of lines to print to file of the iterations, can be expanded to work like cvalues
    int frameCount = 5000;      // note that Magic::ImageList.animationDelay(1) is 1s/1000
    int delayMultiplier = 1;    // set to 2,3,4,5 for 10,15,20,25s version

    apf::precision(1000);
    apf maxError;
    maxError = "1e-30";

    apf c1,c2,c3,c4;
    c1 = apf(long(1)) + apf::sqrt(apf(long(6)));
    c2 = "3.54409035955192285361596598660480454058309984544457367545781253030584294285886301225625856642489179996";
    c3 = "3.5699456718709449018420051513864989367638369115148323781079755299213628875001367775263210342163";
    c4 = "3.67857351042832226"; // TODO use Newton's Method (Comp 361 A3) to find this as the sol'n to x^3-2x^2-4x-8=0
    
    vector<pair<int,pair<apf,apf>>> cbuckets
    {
        /*
        Example: 
            * will allocate 500 frames to move linearly through 0.7, 2.0
            * files will be named logistic_[batch#]_[frame#] where batch# is their place in this vector
        {
            400, {apf(0.7), apf(2.0)}
        },
        */
        { 500, {apf(0.7), apf(2.0)} },
        { 500, {apf(2.0), apf(3.0)} },
        { 500, {apf(3.0), c1} },
        { 500, {c1, c2} },
        { 1500, {c2, c3} },
        { 500, {c3, c4} }
    };

    writeIterationsToFile(cbuckets, filenameRoot, maxError, maxPrint);
    runPythonScript(pythonScriptFilename);
    makeGIF(frameCount, delayMultiplier, cbuckets, gifFilename);

}