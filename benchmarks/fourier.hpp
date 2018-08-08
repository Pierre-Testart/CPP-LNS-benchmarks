#ifndef FOURIER_HPP
#define FOURIER_HPP

#include "complex.hpp"
#include <iostream>
#include <cmath>

void calcFftIndices(int K, int* indices)
{
    int i, j ;
    int N ;

    N = (int)log2f(K) ;

    indices[0] = 0 ;
    indices[1 << 0] = 1 << (N - (0 + 1)) ;
    for (i = 1; i < N; ++i)
    {
        for(j = (1 << i) ; j < (1 << (i + 1)); ++j)
        {
            indices[j] = indices[j - (1 << i)] + (1 << (N - (i + 1))) ;
        }
    }
}

template<typename T>
void radix2DitCooleyTykeyFft(int K, int* indices, Complex<T>* x, Complex<T>* f)
{

    calcFftIndices(K, indices) ;

    int step ;
    T arg ;
    int eI ;
    int oI ;

    T fftSin;
    T fftCos;

    Complex<T> t;
    int i ;
    int N ;
    int j ;
    int k ;

    for(i = 0, N = 1 << (i + 1); N <= K ; i++, N = 1 << (i + 1))
    {
        for(j = 0 ; j < K ; j += N)
        {
            step = N >> 1 ;
            for(k = 0; k < step ; k++)
            {
                arg = (T)k / (T)N ;
                eI = j + k ;
                oI = j + step + k ;

                fftSinCos(arg, &fftSin, &fftCos);

                // Non-approximate
                t =  x[indices[eI]] ;
                x[indices[eI]].real = t.real + (x[indices[oI]].real * fftCos - x[indices[oI]].imag * fftSin);
                x[indices[eI]].imag = t.imag + (x[indices[oI]].imag * fftCos + x[indices[oI]].real * fftSin);

                x[indices[oI]].real = t.real - (x[indices[oI]].real * fftCos - x[indices[oI]].imag * fftSin);
                x[indices[oI]].imag = t.imag - (x[indices[oI]].imag * fftCos + x[indices[oI]].real * fftSin);
            }
        }
    }

    for (int i = 0 ; i < K ; i++)
    {
        f[i] = x[indices[i]] ;
    }
}

#endif