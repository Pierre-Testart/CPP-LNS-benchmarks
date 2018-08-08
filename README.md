## Logarithmic Number System library

This project was carried out by Pierre Testart, student at Politecnico di Milano.  
Its aim was to create a header-only library to support the Logarithmic Number System in C++, and then to integrate benchmarks (from [Axbench](http://axbench.org/)) to measure accuracy and performance.  
This repository contains the integrated benchmarks. The header-only library, required to compile, is available [here](https://github.com/Pierre-Testart/CPP-LNS-library).  
C++14 is the required standard to build the project.

### How to run benchmarks

A Makefile is supplied to build the project, it produces an executable file called `lns_benchmarks`. There are 2 targets available:
* `gcc`: to build with gcc
* `clang`: to build with clang

By default, calling `make` without a target will build with gcc.

Some variables can be set in the call to `make`:
* `benchmarks` contains the names of the benchmarks to run. By default, it contains all the implemented benchmarks: `"FFT BLACKSCHOLES INVERSEK2J JMEINT SOBEL KMEANS"`. A different value of this variable can be specified to run fewer benchmarks.
* `reference_type` is the name of the type used to get the theoretical result of a benchmark. Its default value is "float".
* `benchmarked_type` is the name of the type whose error and speed must compared to those of the reference type. Its default value is "lns32_t".

Types name can be "float", "double", "long double", "lns16_t", "lns32_t", "lns64_t" or "lns_t<I, F, A>" (with legal values for all template parameters).

#### Examples of make commands

Build with gcc, run benchmarks FFT and Black-Scholes, compare lns32_t to double:
```
make benchmarks="FFT BLACKSCHOLES" reference_type="double" benchmarked_type="lns32_t"
```

Build with clang, run benchmark Sobel, compare lns_t<10, 54, -1> to long double:
```
make clang benchmarks="SOBEL" reference_type="long double" benchmarked_type="lns_t<10, 54, -1>"
```
