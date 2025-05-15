#pragma once
#include <string>
#include <sstream>
#include <complex>
#include <vector>
#include <stdexcept>
#include <iomanip>
#include <cstdint>
#include <algorithm>
#include <cctype>

// базовые функции
inline int inc1(int x) { return x + 1; }
inline int inc2(int x) { return x + 2; }
inline int inc3(int x) { return x + 3; }

class Type
{
public:
    virtual ~Type() = default;

    virtual std::size_t size() const = 0;
    virtual Type *cloneType() const = 0;
    virtual void *clone(void *p) const = 0;
    virtual void *createFromString(const std::string &s) const = 0;
    virtual void destroy(void *p) const = 0;

    virtual int compare(void *a, void *b) const = 0;
    virtual void print(void *a, std::ostream &os) const = 0;
};

class IntType : public Type
{
public:
    std::size_t size() const override { return sizeof(int); }
    Type *cloneType() const override { return new IntType(*this); }
    void *clone(void *p) const override { return new int{*static_cast<int *>(p)}; }
    void *createFromString(const std::string &s) const override { return new int{std::stoi(s)}; }
    void destroy(void *p) const override { delete static_cast<int *>(p); }
    int compare(void *a, void *b) const override
    {
        int x = *static_cast<int *>(a), y = *static_cast<int *>(b);
        return x < y ? -1 : (x > y ? +1 : 0);
    }
    void print(void *a, std::ostream &os) const override { os << *static_cast<int *>(a); }
};

class DoubleType : public Type
{
public:
    std::size_t size() const override { return sizeof(double); }
    Type *cloneType() const override { return new DoubleType(*this); }
    void *clone(void *p) const override { return new double{*static_cast<double *>(p)}; }
    void *createFromString(const std::string &s) const override { return new double{std::stod(s)}; }
    void destroy(void *p) const override { delete static_cast<double *>(p); }
    int compare(void *a, void *b) const override
    {
        double x = *static_cast<double *>(a), y = *static_cast<double *>(b);
        return x < y ? -1 : (x > y ? +1 : 0);
    }
    void print(void *a, std::ostream &os) const override { os << *static_cast<double *>(a); }
};

using Complex = std::complex<double>;
class ComplexType : public Type
{
public:
    std::size_t size() const override { return sizeof(Complex); }
    Type *cloneType() const override { return new ComplexType(*this); }
    void *clone(void *p) const override { return new Complex{*static_cast<Complex *>(p)}; }
    void *createFromString(const std::string &s) const override
    {
        std::string t = s;
        t.erase(std::remove_if(t.begin(), t.end(), ::isspace), t.end());
        if (t.empty())
            throw std::invalid_argument("bad complex");
        if (t.back() != 'i')
            return new Complex{std::stod(t), 0.};
        t.pop_back();
        size_t pos = t.find_last_of("+-", t.size() - 1);
        if (pos == std::string::npos || pos == 0)
        {
            double im = std::stod(t);
            return new Complex{0., im};
        }
        double re = std::stod(t.substr(0, pos));
        double im = std::stod(t.substr(pos));
        return new Complex{re, im};
    }
    void destroy(void *p) const override { delete static_cast<Complex *>(p); }
    int compare(void *a, void *b) const override
    {
        auto &A = *static_cast<Complex *>(a), &B = *static_cast<Complex *>(b);
        double na = std::norm(A), nb = std::norm(B);
        if (na != nb)
            return na < nb ? -1 : +1;
        if (A.real() != B.real())
            return A.real() < B.real() ? -1 : +1;
        if (A.imag() != B.imag())
            return A.imag() < B.imag() ? -1 : +1;
        return 0;
    }
    void print(void *a, std::ostream &os) const override
    {
        auto &z = *static_cast<Complex *>(a);
        os << z.real() << (z.imag() >= 0 ? "+" : "") << z.imag() << "i";
    }
};

class StringType : public Type
{
public:
    std::size_t size() const override { return sizeof(std::string); }
    Type *cloneType() const override { return new StringType(*this); }
    void *clone(void *p) const override { return new std::string{*static_cast<std::string *>(p)}; }
    void *createFromString(const std::string &s) const override { return new std::string{s}; }
    void destroy(void *p) const override { delete static_cast<std::string *>(p); }
    int compare(void *a, void *b) const override
    {
        auto &A = *static_cast<std::string *>(a), &B = *static_cast<std::string *>(b);
        return A < B ? -1 : (A > B ? +1 : 0); // dictionary order comparation
    }
    void print(void *a, std::ostream &os) const override { os << *static_cast<std::string *>(a); }
};

using FunctionPtr = int (*)(int);
class FunctionType : public Type
{
public:
    std::size_t size() const override { return sizeof(FunctionPtr); }
    Type *cloneType() const override { return new FunctionType(*this); }
    void *clone(void *p) const override { return new FunctionPtr{*static_cast<FunctionPtr *>(p)}; }
    void *createFromString(const std::string &s) const override
    {
        if (s == "inc1")
            return new FunctionPtr{inc1};
        if (s == "inc2")
            return new FunctionPtr{inc2};
        if (s == "inc3")
            return new FunctionPtr{inc3};
        throw std::invalid_argument("bad func");
    }
    void destroy(void *p) const override { delete static_cast<FunctionPtr *>(p); }
    int compare(void *a, void *b) const override
    {
        auto pa = reinterpret_cast<std::uintptr_t>(*static_cast<FunctionPtr *>(a));
        auto pb = reinterpret_cast<std::uintptr_t>(*static_cast<FunctionPtr *>(b));
        return pa < pb ? -1 : (pa > pb ? +1 : 0); // memory address comparation
    }
    void print(void *a, std::ostream &os) const override
    {
        os << "Func@" << std::hex << reinterpret_cast<std::uintptr_t>(*static_cast<FunctionPtr *>(a)) << std::dec;
    }
};

class PersonType : public Type
{
public:
    std::size_t size() const override { return sizeof(std::string); }
    Type *cloneType() const override { return new PersonType(*this); }
    void *clone(void *p) const override { return new std::string{*static_cast<std::string *>(p)}; }
    void *createFromString(const std::string &s) const override { return new std::string{s}; }
    void destroy(void *p) const override { delete static_cast<std::string *>(p); }
    int compare(void *a, void *b) const override
    {
        auto &A = *static_cast<std::string *>(a), &B = *static_cast<std::string *>(b);
        return A < B ? -1 : (A > B ? +1 : 0);
    }
    void print(void *a, std::ostream &os) const override { os << *static_cast<std::string *>(a); }
};
