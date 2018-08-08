#ifndef COMPLEX_HPP
#define COMPLEX_HPP

#include <cmath>
#include "utilities.hpp"

template<typename T>
struct Complex
{
    T real;
    T imag;
};

template<typename T>
void fftSinCos(T x, T* s, T* c) {
    static const T minus2PI(-2 * M_PI);
    *s = sinValue(minus2PI * x);
    *c = cosValue(minus2PI * x);
}

#endif