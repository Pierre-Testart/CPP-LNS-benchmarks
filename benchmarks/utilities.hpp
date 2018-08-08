#ifndef BENCHMARK_UTILITIES_HPP
#define BENCHMARK_UTILITIES_HPP

#include <cmath>
#include "../lns.hpp"

template<typename T>
T squareRoot(T value)
{
    return std::sqrt(value);
}

template<int I, int F, int A>
lns::lns_t<I, F, A> squareRoot(lns::lns_t<I, F, A> value)
{
    return value.sqrt();
}


template<typename T>
T naturalLog(T value)
{
    return std::log(value);
}

template<int I, int F, int A>
lns::lns_t<I, F, A> naturalLog(lns::lns_t<I, F, A> value)
{
    static lns::lns_t<I, F, A> log2E(log2(M_E));
    return lns::lns_t<I, F, A>(value.template log2<double>()) / log2E;
}

template<typename T>
T square(T value)
{
    return value * value;
}

template<int I, int F, int A>
lns::lns_t<I, F, A> square(lns::lns_t<I, F, A> value)
{
    return value.square();
}

template<typename T>
bool isZero(T value)
{
    return value == 0.0;
}

template<int I, int F, int A>
bool isZero(lns::lns_t<I, F, A> value)
{
    return value.isZero();
}

template<typename T>
bool isPositive(T value)
{
    return value > 0.0;
}

template<int I, int F, int A>
bool isPositive(lns::lns_t<I, F, A> value)
{
    return value.isPositive();
}

template<typename T>
bool isNegative(T value)
{
    return value < 0.0;
}

template<int I, int F, int A>
bool isNegative(lns::lns_t<I, F, A> value)
{
    return value.isNegative();
}

template<typename T>
T cosValue(T value)
{
    return std::cos(value);
}

template<int I, int F, int A>
lns::lns_t<I, F, A> cosValue(lns::lns_t<I, F, A> value)
{
    return lns::lns_t<I, F, A>(std::cos((double)value));
}

template<typename T>
T sinValue(T value)
{
    return std::sin(value);
}

template<int I, int F, int A>
lns::lns_t<I, F, A> sinValue(lns::lns_t<I, F, A> value)
{
    return lns::lns_t<I, F, A>(std::sin((double)value));
}

template<typename T>
T absValue(T value)
{
    return std::fabs(value);
}

template<int I, int F, int A>
lns::lns_t<I, F, A> absValue(lns::lns_t<I, F, A> value)
{
    return value.abs();
}

template<typename T>
T acosValue(T value)
{
    return std::acos(std::min<T>(1.0, std::max<T>(-1.0, value)));
}

template<int I, int F, int A>
lns::lns_t<I, F, A> acosValue(lns::lns_t<I, F, A> value)
{
    return lns::lns_t<I, F, A>(std::acos(std::min(1.0, std::max(-1.0, (double)value))));
}

template<typename T>
T asinValue(T value)
{
    return std::asin(std::min<T>(1.0, std::max<T>(-1.0, value)));
}

template<int I, int F, int A>
lns::lns_t<I, F, A> asinValue(lns::lns_t<I, F, A> value)
{
    return lns::lns_t<I, F, A>(std::asin(std::min(1.0, std::max(-1.0, (double)value))));
}

template<typename T>
void readFromStream(std::istream& stream, T& value)
{
    stream >> value;
}

template<int I, int F, int A>
void readFromStream(std::istream& stream, lns::lns_t<I, F, A>& value)
{
    double dbl;
    stream >> dbl;
    value = lns::lns_t<I, F, A>(dbl);
}

#endif