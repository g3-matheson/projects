#ifndef ARBITRARY_PRECISION_FLOATS_H
#define ARBITRARY_PRECISION_FLOATS_H

/*

    Arbitrary Precision Floats
    Kat Matheson

*/
#include <gmp.h>
#include <gmpxx.h>
#include <ostream>

class apf
{
    public:

    static int precision(int p = 500);
    static apf zerodiv();
    mpf_t value;

    explicit apf(double d = 0.0);
    explicit apf(long i);
    // copy constructor for containers
    apf(const apf& other);
    ~apf();

    apf& operator=(const apf& other);
    apf& operator=(const mpf_t& f);    
    apf& operator=(const double& d);
    apf& operator=(const unsigned long& l);
    apf& operator=(const std::string& s);

    apf operator+(const apf& other) const;
    apf operator+(const double& other) const;
    apf operator+(const long& i) const;
    apf operator-(const apf& other) const;
    apf operator-(const double& other) const;
    apf operator-(const long& i) const;

    bool operator<(const apf& other) const;
    bool operator<(const double& d) const;
    bool operator<(const int& i) const;

    bool operator>(const apf& other) const;
    bool operator>(const double& d) const;
    bool operator>(const int& i) const;

    static bool isZero(const apf& x);

    static apf mult(const apf& a, const apf& b); // a*b;
    static apf mult(const apf&a, const double& d);
    static apf mult(const apf& a, int b); 

    // TODO
    //friend apf operator*(const apf& a, const apf& b);

    static apf div(const apf& a, const apf& b); // a/b
    static apf div(const apf& a, int b);

    static apf exp(const apf& a, int b);

    static apf abs(const apf& a);
    static apf reldiff(const apf& a, const apf& b);

    static apf intminus(const int& i, const apf& a);

    static apf sqrt(const apf& x);
    static apf norm2(const std::pair<apf,apf>& x);

    friend std::ostream& operator<<(std::ostream& os, const apf& f);

};

#endif