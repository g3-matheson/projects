#include "apf.h"

int apf::precision(int p)
{
    static int precision(p);
    return precision;
}

apf apf::zerodiv()
{
    static apf zerodiv;
    std::string errorStr = "1@-" + std::to_string(apf::precision() / 10);
    mpf_set_str(zerodiv.value, errorStr.c_str(), 10);
    return zerodiv;
}

apf::apf(double d)
{
    mpf_init(value);
    mpf_set_prec(value, apf::precision());
    mpf_set_d(value, d);
}

apf::apf(long i)
{
    mpf_init(value);
    mpf_set_prec(value, apf::precision());
    mpf_set_ui(value, i);
}

apf::apf(const apf& other)
{
    mpf_init(value);
    mpf_set_prec(value, apf::precision());
    mpf_set(value, other.value);
}

apf::~apf()
{
    mpf_clear(value);
}

apf& apf::operator=(const apf& other)
{
    if(this != &other)
    {
        mpf_set(value, other.value);
    }
    return *this;
}

apf& apf::operator=(const mpf_t& f)
{
    mpf_set(value, f);
    return *this;
}

apf& apf::operator=(const double& d)
{
    mpf_set_d(value, d);
    return *this;
}

apf& apf::operator=(const unsigned long& l)
{
    mpf_set_ui(value, l);
    return *this;
}

apf& apf::operator=(const std::string& s)
{
    const char* charStr;
    if(s.find('e',0) != std::string::npos)
    {
        std::string newStr = s.substr(0, s.find('e',0));
        newStr += "e" + s.substr(s.find('e',0)+1);
        charStr = newStr.c_str();
    }
    else
    {
        charStr = s.c_str();
    }
    mpf_set_str(value, charStr, 10);
    return *this;
}

apf apf::operator+(const apf& other) const 
{
    apf result;
    mpf_add(result.value, value, other.value);
    return result;
}

apf apf::operator+(const double& d) const
{
    apf result;
    mpf_set_d(result.value, d);
    mpf_add(result.value, result.value, value);
    return result;
}

apf apf::operator+(const long& i) const
{
    apf result;
    mpf_add_ui(result.value, value, i);
    return result;
}

apf apf::operator-(const apf& other) const
{
    apf result;
    mpf_sub(result.value, value, other.value);
    return result;
}

apf apf::operator-(const double& d) const
{
    apf result;
    mpf_set_d(result.value, d);
    mpf_sub(result.value, value, result.value);
    return result;
}

apf apf::operator-(const long& i) const
{
    apf result;
    mpf_sub_ui(result.value, value, i);
    return result;
}

bool apf::operator<(const apf& other) const
{
    return mpf_cmp(value, other.value) < 0;
}
bool apf::operator<(const double& d) const
{
    return mpf_cmp_d(value, d) < 0;
}
bool apf::operator<(const int& i) const
{
    return mpf_cmp_ui(value, i) < 0;
}

bool apf::operator>(const apf& other) const
{
    return mpf_cmp(value, other.value) > 0;
}
bool apf::operator>(const double& d) const
{
    return mpf_cmp_d(value, d) > 0;
}
bool apf::operator>(const int& i) const
{
    return mpf_cmp_ui(value, i) > 0;
}

bool apf::isZero(const apf& x)
{
    return apf::abs(x) < apf::zerodiv();
}

apf apf::mult(const apf& a, const apf& b)
{
    apf result;
    mpf_mul(result.value, a.value, b.value);
    return result;
}

apf apf::mult(const apf&a, const double& d)
{
    apf result;
    apf apd(d);
    mpf_mul(result.value, a.value, apd.value);
    return result;
}

apf apf::mult(const apf& a, int b)
{
    apf result;
    mpf_mul_ui(result.value, a.value, b);
    return result;
}

apf apf::div(const apf& a, const apf& b)
{
    apf result;
    if(apf::abs(b) < apf::zerodiv())
    {
        throw std::domain_error("Division by zero error.");
    }
    else
    {
        mpf_div(result.value, a.value, b.value);
        return result;
    }

}

apf apf::div(const apf& a, int b)
{
    apf result;
    mpf_div_ui(result.value, a.value, b);
    return result;
}

apf apf::exp(const apf& a, int b)
{
    apf result;
    mpf_pow_ui(result.value, a.value, b);
    return result;
}

apf apf::abs(const apf& a)
{
    apf result = a;
    mpf_abs(result.value, a.value);
    return result;
}

apf apf::reldiff(const apf& a, const apf& b)
{
    apf result;
    result = a-b;
    result = apf::div(result, b);
    result = apf::abs(result);
    return result;
}

apf apf::intminus(const int& i, const apf& a)
{
    apf result;
    mpf_ui_sub(result.value, i, a.value);
    return result;
}

apf apf::sqrt(const apf& x)
{
    apf result;
    mpf_sqrt(result.value, x.value);
    return result;
}

apf apf::norm2(const std::pair<apf,apf>& x)
{
    apf result;
    result = apf::exp(x.first, 2) + apf::exp(x.second,2);
    result = apf::sqrt(result);
    return result;
}

std::ostream& operator<<(std::ostream& os, const apf& f)
{
    // https://gmplib.org/manual/Converting-Floats#index-mpf_005fget_005fstr
    os << f.value;
    return os;
}