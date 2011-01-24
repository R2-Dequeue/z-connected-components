/*!
 * \file
 * \author Chris de Pujo
 */

#include "polynomialqq.hpp"

#include "polynomialbase.hpp"

#include <stdexcept>
#include <numeric>
#include <functional>

#include <boost/foreach.hpp>

const GiNaC::symbol & PolynomialQQ::var1 = PolynomialBase::var1;
const GiNaC::symbol & PolynomialQQ::var2 = PolynomialBase::var2;

PolynomialQQ::PolynomialQQ(const std::string & s)
{
    using namespace GiNaC;

    symtab table;

    table[var1.get_name()] = var1;
    table[var2.get_name()] = var2;

    parser reader(table);

    // parser.strict = true; // must be a typo int the tutorial
    reader.strict = true;

    // reader(s).expand(); ?
    polynomial = reader(s); // throws an exception if parsing fails

    if (!polynomial.is_polynomial(lst(var1, var2)))
        throw std::invalid_argument(
        "GiNaC parsing suceeded, but result is not a polynomial in "
        + var1.get_name() + ", " + var2.get_name() + ".");

    if (!polynomial.info(info_flags::rational_polynomial))
        throw std::invalid_argument(
        "GiNaC parsing suceeded, but the result is not a rational polynomial.");

    //if (polynomial == ex()) // should always be at least 'ex(0)'.
    //    return false;

    std::string outString("GiNaC parsing suceeded, but there is a problem with the coefficients.");

    int deg1 = polynomial.degree(var1);

    for (int i = 0; i <= deg1; i++)
    {
        ex p2 = polynomial.coeff(var1, i);

        if (!p2.is_polynomial(var2))
            throw std::invalid_argument(outString);

        if (!p2.info(info_flags::rational_polynomial))
            throw std::invalid_argument(outString);

        int deg2 = p2.degree(var2);

        for (int j = 0; j <= deg2; j++)
        {
            ex co = p2.coeff(var2, j);

            if (is_a<numeric>(co))
            {
                numeric c = ex_to<numeric>(co);

                if (!c.is_rational())
                    throw std::invalid_argument(outString);
            }
            else
                throw std::invalid_argument(outString);
        }
    }
}

PolynomialQQ::PolynomialQQ(const char * const s)
{
    GiNaC::symtab table;

    table[var1.get_name()] = var1;
    table[var2.get_name()] = var2;

    GiNaC::parser reader(table);

    // parser.strict = true; // must be a typo int the tutorial
    reader.strict = true;

    // reader(s).expand(); ?
    polynomial = reader(s); // throws an exception if parsing fails

    if (!Invariant())
        throw std::invalid_argument("Parsing of polynomial succeded, but the"
                                    "result is not canonical.");
}

PolynomialQQ::PolynomialQQ(const GiNaC::ex & e)
    : polynomial(e)
{
    if (!Invariant())
        throw std::invalid_argument("Parsing of polynomial succeded, but the"
                                    "result is not canonical.");
}

PolynomialQQ::PolynomialQQ(const GiNaC::numeric & n)
    : polynomial(n)
{
    if (!Invariant())
        throw std::invalid_argument("Parsing of polynomial succeded, but the"
                                    "result is not canonical.");
}

/*!
 * \detail Should work in cases such as 'p += p;'
 */
PolynomialQQ & PolynomialQQ::operator+=(const PolynomialQQ & rhs)
{
	assert(Invariant());
	assert(rhs.Invariant());

	polynomial += rhs.polynomial;

	assert(Invariant());

	return *this;
}

/*!
 * \detail Should work in cases such as 'p -= p;'
 */
PolynomialQQ & PolynomialQQ::operator-=(const PolynomialQQ & rhs)
{
	assert(Invariant());
	assert(rhs.Invariant());

	polynomial -= rhs.polynomial;

	assert(Invariant());

	return *this;
}

/*!
 * \detail Should work in cases such as 'p *= p;'
 */
PolynomialQQ & PolynomialQQ::operator*=(const PolynomialQQ & rhs)
{
	assert(Invariant());
	assert(rhs.Invariant());

	polynomial = expand(polynomial * rhs.polynomial); // need to figure out 'expand'

	assert(Invariant());

	return *this;
}

PolynomialQQ PolynomialQQ::getDerivative(unsigned int variable) const
{
    assert(Invariant());
    assert(variable == 1 || variable == 2);

    if (variable != 1 && variable != 2)
        throw std::invalid_argument("Tried to take derivative wrt an invalid variable.");

    PolynomialQQ temp(*this);

    temp.differentiate(variable);

    return temp;
}

PolynomialQQ & PolynomialQQ::differentiate(unsigned int variable)
{
    assert(Invariant());
    assert(variable == 1 || variable == 2);

    if (variable != 1 && variable != 2)
        throw std::invalid_argument("Tried to take derivative wrt an invalid variable.");

    polynomial = polynomial.diff((variable == 1) ? var1 : var2);

    return *this;
}

std::vector<PolynomialQQ> PolynomialQQ::getIrreducibleFactors() const
{
    assert(Invariant());

    GiNaC::ex p = factor(polynomial);

    std::vector<PolynomialQQ> factors;

    for (GiNaC::const_iterator i = p.begin(); i != p.end(); i++)
        factors.push_back(PolynomialQQ(GiNaC::is_a<GiNaC::power>(*i) ?
                                      (*i).op(0) :
                                      *i));
        // The PolyQ constructor will throw if the ex is not a valid poly.

    return factors;
}

int PolynomialQQ::signAt(const Algebraic & a, const Algebraic & b) const
{
    assert(Invariant());

    return 0;
}

GiNaC::ex PolynomialQQ::getEx() const
{
    GiNaC::ex temp(polynomial);

    return temp;
}

std::vector<PolynomialQQ>
    PolynomialQQ::IrreducibleFactors(const std::vector<PolynomialQQ> & F)
{
    // remove '0' polynomials?
    PolynomialQQ fp = std::accumulate(F.begin(),
                                 F.end(),
                                 PolynomialQQ(GiNaC::numeric(1)),
                                 std::multiplies<PolynomialQQ>());

    assert(fp.Invariant()); // isn't this redundant?

    GiNaC::ex p = factor(fp.polynomial);

    std::vector<PolynomialQQ> factors;

    for (GiNaC::const_iterator i = p.begin(); i != p.end(); i++)
        factors.push_back(PolynomialQQ(GiNaC::is_a<GiNaC::power>(*i) ?
                                      (*i).op(0) :
                                      *i));
        // The PolyQ constructor will throw if the ex is not a valid poly.

    return factors;
}

PolynomialQ PolynomialQQ::Resultant(const PolynomialQQ & f,
                                    const PolynomialQQ & g,
                                    unsigned int var)
{
    assert(f.Invariant());
    assert(g.Invariant());
    assert(var == 1 || var == 2);

    if (var != 1 && var != 2)
        throw std::invalid_argument("Tried to calculate resultant wrt an invalid variable.");

    GiNaC::ex res = resultant(f.polynomial,
                              g.polynomial,
                              (var == 1) ? PolynomialQQ::var1 : PolynomialQQ::var2);

    if (var == 1)
        res = res.subs(var2 == var1);

    return PolynomialQ(res); // Hoping constructor will be called and check
                             // Invariant().
}

bool PolynomialQQ::Invariant() const
{
    using namespace GiNaC;

    if (this == NULL)
        return false;

    if (!polynomial.is_polynomial(lst(var1, var2)))
        return false;

    if (!polynomial.info(info_flags::rational_polynomial))
        return false;

    if (polynomial == ex()) // should always be at least 'ex(0)'.
        return false;

    int deg1 = polynomial.degree(var1);

    for (int i = 0; i <= deg1; i++)
    {
        ex p2 = polynomial.coeff(var1, i);

        if (!p2.is_polynomial(var2))
            return false;

        if (!p2.info(info_flags::rational_polynomial))
            return false;

        int deg2 = p2.degree(var2);

        for (int j = 0; j <= deg2; j++)
        {
            ex co = p2.coeff(var2, j);

            if (is_a<numeric>(co))
            {
                numeric c = ex_to<numeric>(co);

                if (!c.is_rational())
                    return false;
            }
            else
                return false;
        }
    }

    return true;
}

PolynomialQQ PolynomialQQ::operator+(const PolynomialQQ & rhs) const {
	assert(Invariant() && rhs.Invariant());
	return PolynomialQQ(polynomial + rhs.polynomial);
}
PolynomialQQ PolynomialQQ::operator-(const PolynomialQQ & rhs) const {
	assert(Invariant() && rhs.Invariant());
	return PolynomialQQ(polynomial - rhs.polynomial);
}
PolynomialQQ PolynomialQQ::operator*(const PolynomialQQ & rhs) const {
	assert(Invariant() && rhs.Invariant());
	return PolynomialQQ(expand(polynomial * rhs.polynomial));
}

Algebraic
    PolynomialQQ::ANComb(const Algebraic & alpha, const Algebraic & beta, int t)
{
    Algebraic a(alpha), b(beta);
    GiNaC::ex polya = a.getEx();
    GiNaC::symbol tmp;

    GiNaC::ex res = resultant(polya.subs(a.getVar() == tmp - b.getVar()*t),
                              b.getEx(), a.getVar());
/*
 A := alpha[2]; // A = [ [l, u], poly]
 B := beta[2];
 u := Var(A);
 v := Var(B);
 r := resultant(eval(A,u=_z-t*v),B,v);
 r := eval(r,_z=w);
 gammas := FindRoots(IrreducibleFactors([r]));
 alphap := alpha;
 betap  := beta;
 while true do
   IJ := alphap[1] + t * betap[1];
   for gamma in gammas do
     K := gamma[1];
     if K[1] <= IJ[1] and IJ[2] <= K[2] then
       return gamma;
     fi;
   od;
   alphap := TightenInterval(alphap);
   betap  := TightenInterval(betap);
 od; */
}
