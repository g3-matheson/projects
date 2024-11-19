#pragma region Internals
#include "apf.h"
#include <Magick++.h>
#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <functional>
#include <format>
#include <filesystem>
using namespace std;
using namespace Magick;
// compilation
// g++ -Wall -g -std=c++20 -o Logistic Logistic.cpp apf.cpp -lgmpxx -lgmp $(Magick++-config --cxxflags --libs)
// create gif (since makeGIF below isn't working rn)
// convert -delay 1 -loop 0 $(ls ./plots/*.png | sort -V) logisticmap.gif

// functions to use for logistic function and its derivative
pair<function<apf(apf)>,function<apf(apf)>> logistic(apf c)
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
            return apf(c) - apf::mult(x, apf::mult(c, 2.0));
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

void runIteration(apf x0, apf c, int maxPrint, apf maxError, vector<apf>* xarr, bool consolePrint = false)
{
    apf xk, relError;
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
        if(xk < maxError)
        {
            if(consolePrint)
            {
                cout << "x0 = " << x0 << " converged to 0"
                     << " in " << count << " iterations." << endl;
            }
            return;
        }
        // to current value
        relError = apf::reldiff(xk, xarr->back());
        if(relError < maxError)
        {
            if(consolePrint)
            {
                cout << "x0 = " << x0 << " converged to: " << xk 
                 << " in " << count << " iterations." << endl;
            }
            return;
        }
        // stop at maxPrint iterations
        else if(count >= maxPrint && maxPrint != 0)
        {
            if(consolePrint)
            {
                cout << "x0 = " << x0 << " failed to converge within " << maxError
                 << " in " << to_string(maxPrint) << " iterations." << endl;
            }
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

/*
    Each bucket goes to its own .txt file which are all processed by the python script
*/
void processBucket(pair<int,pair<apf,apf>> bucket, string filename, apf maxError, int maxPrint)
{
    apf cstart, cend, delta, x0;
    x0 = 0.8; // HACK can make this randomly fluctuating, might look nice
                // e.g inside of runIteration, add uniform[a,b] noise for small a < 0, b > 0
                // or normal (0, sigma) for small sigma (bounded)

    int nFrames = bucket.first;
    cstart = bucket.second.first;
    cend = bucket.second.second;
    delta = apf::div(cend-cstart, nFrames);

    vector<apf>* xarr = new vector<apf>();
    
    ofstream ofout;
    try
    {
        ofout.open(filename, ios::out | ios::trunc);
    }
    catch(const std::exception& e)
    {
        cerr << e.what() << endl;
    }

    for(apf c = cstart; c < cend; c = c + delta)
    {
        runIteration(x0, c, maxPrint, maxError, xarr);

        ofout << "!" << endl;
        ofout << "c=" << c << endl;

        for(size_t i = 0; i < xarr->size(); i++)
        {
            ofout << "x" << to_string(i)
                  << "=" << xarr->at(i) << endl << flush;
        }
    
        xarr->clear();
    }

    ofout.close();
    delete xarr;
}

void writeIterationsToFile(vector<pair<int,pair<apf,apf>>> cbuckets, string filenameRoot, apf maxError, int maxPrint)
{
    int bucketCounter = 0;
    string filename;
    for(pair<int,pair<apf,apf>> bucket : cbuckets)
    {
        filename = filenameRoot + to_string(bucketCounter++) + ".txt";
        processBucket(bucket, filename, maxError, maxPrint);
    }
}

void runPythonScript(string filename, int nBuckets)
{
    for(int i = 0; i < nBuckets; i++)
    {
        cout << "Running: " << format("python {} {}", filename, i) << endl;
        string command = format("python {} {}", filename, i);
        int result = system(command.c_str());
        if(result != 0)
        {
            cerr << "Error running python script." << endl;
        }
    }

}

void makeGIF(string gifFilename)
{
    try
    {
        InitializeMagick(nullptr);
        vector<Image> images;
        vector<string> imageFilenames;

        for(const auto& imagePath : filesystem::directory_iterator("./plots/"))
        {
            Image image(imagePath.path().string());
            images.push_back(image);
        }
        
        // TODO figure out why this doesn't work!
        // can just use the following:
            // command(convert -delay 1 -loop 0 $(ls ./plots/*.png | sort -V) logisticmap.gif)
        writeImages(images.begin(), images.end(), gifFilename);
    }
    catch(const std::exception& e)
    {
        cerr << e.what() << endl;
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

    apf::precision(1000);
    apf maxError;
    maxError = "1e-30";

    apf c1,c2,c3,c4;
    c1 = apf(long(1)) + apf::sqrt(apf(long(6)));
    c2 = "3.54409035955192285361596598660480454058309984544457367545781253030584294285886301225625856642489179996";
    c3 = "3.5699456718709449018420051513864989367638369115148323781079755299213628875001367775263210342163";
    c4 = "3.67857351042832226"; // TODO use Newton's Method (Comp 361 A3) to find this as the sol'n to x^3-2x^2-4x-8=0
    
    // HACK make this read from a file to avoid re-compiling every time
    // e.g ./Logistic <filename.txt> <outname.gif>
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
        { 50, {apf(0.7), apf(2.0)} },
        { 50, {apf(2.0), apf(3.0)} },
        { 50, {apf(3.0), c1} },
        { 50, {c1, c2} },
        { 50, {c2, c3} },
        { 100, {c3, c4} },
        { 150, {c4, apf(3.9999)}}
    }; 

    writeIterationsToFile(cbuckets, filenameRoot, maxError, maxPrint);
    runPythonScript(pythonScriptFilename, static_cast<int>(cbuckets.size()));
    //makeGIF(frameCount, delayMultiplier, cbuckets, gifFilename);

    return 0;

}