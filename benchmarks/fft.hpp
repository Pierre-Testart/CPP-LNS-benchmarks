#ifndef FFT_H
#define FFT_H

#include <cstdio>
#include <iostream>
#include "fourier.hpp"
#include <vector>
#include <fstream>
#include "fft.hpp"
#include "complex.hpp"

template<typename T>
std::vector<float> fft(int n)
{
    std::vector<float> output;
    static int* indices;
    static Complex<T>* x;
    static Complex<T>* f;
    int i ;

    // create the arrays
    x 		= (Complex<T>*)malloc(n * sizeof (Complex<T>));
    f 		= (Complex<T>*)malloc(n * sizeof (Complex<T>));
    indices = (int*)malloc(n * sizeof (int));

    if(x == NULL || f == NULL || indices == NULL)
    {
        std::cout << "cannot allocate memory for the triangle coordinates!" << std::endl;
        exit(1);
    }

    int K = n;

    for(i = 0;i < K ; i++)
    {
        x[i].real = T(i);
        x[i].imag = T(0) ;
    }
    radix2DitCooleyTykeyFft<T>(K, indices, x, f) ;

    for(i = 0;i < K ; i++)
    {
        output.push_back((float) f[i].real);
        output.push_back((float) f[i].imag);
    }
    free(indices);
    free(x);
    free(f);
    return output;
}

#endif