/*!
 * \file
 * \author Chris de Pujo
 */

#include "polynomialq.hpp"

#include <string>
#include <cassert>
#include <numeric>
#include <stdexcept>

#include "polynomialbase.hpp"

#include <boost/foreach.hpp>

// Field to use
// Number of variables
//
// Complex      C,i
// Reals        R
// Algebraic    A
// Rationals    Q,F,R
// Integers     Z,I

const GiNaC::symbol & PolynomialQ::variable = PolynomialBase::var1;

PolynomialQ::PolynomialQ(const std::string & s)
{
    using namespace GiNaC;

    symtab table;

    table[variable.get_name()] = variable;

    parser reader(table);

    // parser.strict = true; // must be a typo int the tutorial
    reader.strict = true;

    // reader(s).expand(); ?
    polynomial = reader(s); // throws an exception if parsing fails

    assert(Invariant());

    if (!Invariant())
        throw std::invalid_argument("PolynomialQ Constructor: Parsing"
                                    "univariate polynomial over the rationals"
                                    "failed.");
}

PolynomialQ::PolynomialQ(const char * const s)
{
    using namespace GiNaC;

    symtab table;

    table[variable.get_name()] = variable;

    parser reader(table);

    // parser.strict = true; // must be a typo int the tutorial
    reader.strict = true;

    std::string a(s);

    // reader(s).expand(); ?
    polynomial = reader(a); // throws an exception if parsing fails

    assert(Invariant());

    if (!Invariant())
        throw std::invalid_argument("PolynomialQ Constructor: Parsing"
                                    "univariate polynomial over the rationals"
                                    "failed.");
}

PolynomialQ::PolynomialQ(const GiNaC::ex & e)
    : polynomial(e)
{
    if (!Invariant())
        throw std::invalid_argument("Parsing of polynomial succeded, but the result is not canonical.");
}

PolynomialQ::PolynomialQ(const GiNaC::numeric & n)
    : polynomial(n)
{
    if (!Invariant())
        throw std::invalid_argument("Parsing of polynomial succeded, but the result is not canonical.");
}

/*!
 * \detail This method checks that the internal state is free of errors. It is
 *         intended to be used only with 'assert' at the begining of its
 *         methods ( assert(Invariant()); ).
 * \todo Add check for irreducibility.
 */
bool PolynomialQ::Invariant() const
{
    using namespace GiNaC;

	if (this == NULL)
		return false;

    // is this false if there are additional variables?
    if (!polynomial.is_polynomial(variable))
        return false;

    if (!polynomial.info(info_flags::rational_polynomial))
        return false;

    //if (polynomial == ex()) // should always be at least 'ex(0)'.
    //    return false;

    int deg = polynomial.degree(variable);

    for (int i = 0; i <= deg; i++)
    {
        if (is_a<numeric>(polynomial.coeff(variable, i)))
        {
            numeric c = ex_to<numeric>(polynomial.coeff(variable, i));

            if (!c.is_rational())
                return false;
        }
        else
            return false;
    }

    return true;
}

/*!
 * \detail Just returns the degree from the underlying 'ex' class. This implies
 *         that the degree of the polynomial '0' is 0 and not -1 as it is
 *         sometimes defined.
 */
inline int PolynomialQ::degree() const
{
	assert(Invariant());

	return polynomial.degree(variable);
}

inline bool PolynomialQ::isMonic() const
{
	assert(Invariant());

	return (polynomial.lcoeff(variable) == 1);
}

inline bool PolynomialQ::isZero() const
{
	assert(Invariant());

	return (polynomial == 0);
}

PolynomialQ PolynomialQ::getMonic()
{
    assert(Invariant());

    PolynomialQ temp;

    temp.polynomial = (polynomial / polynomial.lcoeff(variable)).expand();

    assert(temp.Invariant());

    return temp;
}

PolynomialQ & PolynomialQ::makeMonic()
{
    assert(Invariant());

    polynomial = (polynomial / polynomial.lcoeff(variable)).expand();

    assert(Invariant());

    return *this;
}

PolynomialQ PolynomialQ::getDerivative() const
{
    assert(Invariant());

    PolynomialQ temp(*this);

    temp.differentiate();

    return temp;
}

/*!
 * \detail Note that this modifies the polynomial object; this doesn't just
 *         return a new polynomial that is the derivative.
 */
PolynomialQ & PolynomialQ::differentiate()
{
	assert(Invariant());

    polynomial = polynomial.diff(variable);

	return *this;
}

GiNaC::ex PolynomialQ::getEx() const
{
    GiNaC::ex temp(polynomial);

    return temp;
}
/*
int PolynomialQ::signAt(const Algebraic & a) const
{
    assert(Invariant());

    PolynomialQ rem(*this % a.polynomial);
    Algebraic alpha(a);

    if (r == 0)
        return 0;

    while (true)
    {
        IntervalQ Y = BoundRange(alpha.getInterval());

        if (Y.upper() < 0) return -1;
        if (Y.lower() > 0) return  1;

        alpha.TightenInterval();
    }

    assert(false);

    return -41;
}*/

/*
IntervalQ BoundRange(const IntervalQ & interval) const
{
    assert(Invariant());
}*/

void PolynomialQ::TestClass()
{
    unsigned int errCount = 0;

    PolynomialQ x1("x-1"), x2("x-2"),
                x3("x-3"), pzero;

    std::cout << "Testing class PolynomialQ." << std::endl;

    TestCompare(x1, variable - 1, errCount);
    TestCompare(x2, variable - 2, errCount);
    TestCompare(x3, variable - 3, errCount);

    TestCompare(x1 + x2, variable*2 - 3, errCount);
    TestCompare(x2 + x3, variable*2 - 5, errCount);
    TestCompare(x1 + x3, variable*2 - 4, errCount);

    TestCompare(x1 + x1, variable*2 - 2, errCount);
    TestCompare(x2 + x2, variable*2 - 4, errCount);
    TestCompare(x3 + x3, variable*2 - 6, errCount);

    TestCompare(x1 * x1, variable*variable + variable*(-2) + 1, errCount);
    TestCompare(x2 * x2, variable*variable + variable*(-4) + 4, errCount);
    TestCompare(x3 * x3, variable*variable + variable*(-6) + 9, errCount);

    TestCompare(x1 * (GiNaC::numeric)0, 0, errCount);
    TestCompare(x2 * (GiNaC::numeric)0, 0, errCount);
    TestCompare(x3 * (GiNaC::numeric)0, 0, errCount);

    TestCompare(x1 * (GiNaC::numeric)1, variable - 1, errCount);
    TestCompare(x2 * (GiNaC::numeric)1, variable - 2, errCount);
    TestCompare(x3 * (GiNaC::numeric)1, variable - 3, errCount);

    TestCompare(x1 - x2, 1, errCount);
    TestCompare(x2 - x3, 1, errCount);
    TestCompare(x1 - x3, 2, errCount);

    TestCompare(x1 - x1, 0, errCount);
    TestCompare(x2 - x2, 0, errCount);
    TestCompare(x3 - x3, 0, errCount);

    TestCompare(x1.getDerivative(), 1, errCount);
    TestCompare(x2.getDerivative(), 1, errCount);
    TestCompare(x3.getDerivative(), 1, errCount);

    TestCompare(pzero, 0, errCount);
    TestCompare(pzero.getDerivative(), 0, errCount);

    PolynomialQ p1("5*x^3-x^2+7");

    TestCompare(p1, pow(variable,3)*5-pow(variable,2)+7, errCount);
    TestCompare(p1.getMonic(),
                pow(variable,3)-pow(variable,2)/5+GiNaC::numeric(7,5),
                errCount);
    TestCompare(p1.differentiate(),
                15*pow(variable,2)-2*variable,
                errCount);
    TestCompare(p1.differentiate(),
                30*variable-2,
                errCount);
    TestCompare(p1.differentiate(),
                30,
                errCount);

    TestCompare(x1.getMonic(), variable - 1, errCount);
    TestCompare(x2.getMonic(), variable - 2, errCount);
    TestCompare(x3.getMonic(), variable - 3, errCount);

    TestCompare(PolynomialQ("x-5")*PolynomialQ("x-7"),
                pow(variable,2)-12*variable+35,
                errCount);

    if (errCount == 0)
        std::cout << "All tests on class PolynomialQ passed." << std::endl;
    else
        std::cout << errCount << " tests on class PolynomialQ failed."
                  << std::endl;
}

void PolynomialQ::TestCompare(const PolynomialQ p,
                              const GiNaC::ex expected,
                              unsigned int & count)
{
    if (p.polynomial != expected)
    {
        std::cout << "Test failed. Expected \"" << expected << "\" but got: \""
                  << p.polynomial << "\"." << std::endl;

        count++;
    }
}

std::ostream & operator<<(std::ostream & output, const PolynomialQ & p)
{
    output << p.getEx() << std::endl;

    return output;
}

/*!
 * \detail Returns a vector of unique irreducible factors of the polynomials
 *         in F.
 * \return Each element should only appear once.
 * \todo Add more error checking and handle zero-polynomials.
 *//*
std::vector<PolynomialQ> IrreducibleFactors(const std::vector<PolynomialQ> & F)
{
    using namespace GiNaC;

    // remove '0' polynomials?
    PolynomialQ fp = accumulate(F.begin(),
                                F.end(),
                                1,//PolynomialQ(1),
                                std::multiplies<PolynomialQ>);
    // implement PolynomialQ.mul(set)?

    assert(fp.Invariant()); // isn't this redundant?

    ex p = factor(fp.polynomial);

    std::vector<PolynomialQ> factors;

    BOOST_FOREACH(const ex & e, p)
    {
        if (is_a<power>(e))
            factors.push_back(PolynomialQ(e.op(0)));
        else
            factors.push_back(PolynomialQ(e));
    }

    return factors;
}*/

/*!
 * \detail Should work in cases such as 'p += p;'
 */
PolynomialQ & PolynomialQ::operator+=(const PolynomialQ & rhs)
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
PolynomialQ & PolynomialQ::operator-=(const PolynomialQ & rhs)
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
PolynomialQ & PolynomialQ::operator*=(const PolynomialQ & rhs)
{
	assert(Invariant());
	assert(rhs.Invariant());

	polynomial = expand(polynomial * rhs.polynomial); // need to figure out 'expand'

	assert(Invariant());

	return *this;
}

PolynomialQ & PolynomialQ::operator/=(const PolynomialQ & rhs)
{
    assert(Invariant());
    assert(rhs.Invariant());

    polynomial = quo(polynomial, rhs.polynomial, variable);

    assert(Invariant());

    return *this;
}

PolynomialQ operator+(const PolynomialQ & lhs, const PolynomialQ & rhs)
{
	assert(lhs.Invariant());
	assert(rhs.Invariant());

	return PolynomialQ(lhs.polynomial + rhs.polynomial);
}

PolynomialQ operator-(const PolynomialQ & lhs, const PolynomialQ & rhs)
{
	assert(lhs.Invariant());
	assert(rhs.Invariant());

	return PolynomialQ(lhs.polynomial - rhs.polynomial);
}

PolynomialQ operator*(const PolynomialQ & lhs, const PolynomialQ & rhs)
{
	assert(lhs.Invariant());
	assert(rhs.Invariant());

    // need to figure out 'expand'
	return PolynomialQ(expand(lhs.polynomial * rhs.polynomial));
}

PolynomialQ operator/(const PolynomialQ & lhs, const PolynomialQ & rhs)
{
    assert(lhs.Invariant());
    assert(rhs.Invariant());

    return PolynomialQ(quo(lhs.polynomial,
                           rhs.polynomial,
                           PolynomialQ::variable));
}

PolynomialQ operator%(const PolynomialQ & lhs, const PolynomialQ & rhs)
{
    assert(lhs.Invariant());
    assert(rhs.Invariant());

    return PolynomialQ(rem(lhs.polynomial,
                           rhs.polynomial,
                           PolynomialQ::variable));
}

PolynomialQ operator*(const PolynomialQ & lhs, const GiNaC::numeric & num)
{
    assert(lhs.Invariant());
    assert(num.is_rational());

    return lhs.polynomial*(GiNaC::ex)num; // will convert
}

PolynomialQ operator/(const PolynomialQ & lhs, const GiNaC::numeric & num)
{
    assert(lhs.Invariant());
    assert(num.is_rational());

    return lhs.polynomial/(GiNaC::ex)num; // rely on GiNaC throwing an exception if num==0.
}

inline bool operator==(const PolynomialQ & lhs, const PolynomialQ & rhs)
{
    assert(lhs.Invariant());
    assert(rhs.Invariant());

    return (lhs.polynomial == rhs.polynomial);
}

inline bool operator!=(const PolynomialQ & lhs, const PolynomialQ & rhs)
{
    assert(lhs.Invariant());
    assert(rhs.Invariant());

    return (lhs.polynomial != rhs.polynomial);
}