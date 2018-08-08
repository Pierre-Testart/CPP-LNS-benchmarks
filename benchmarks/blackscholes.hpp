#ifndef BLACKSCHOLES_HPP
#define BLACKSCHOLES_HPP

// Copyright (c) 2007 Intel Corp.

// Black-Scholes
// Analytical method for calculating European Options
//
//
// Reference Source: Options, Futures, and Other Derivatives, 3rd Edition, Prentice
// Hall, John C. Hull,

#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <vector>
#include <iomanip>
#include "../lns.hpp"
#include "utilities.hpp"

#define DIVIDE 120.0

#define NUM_RUNS 1

template<typename T>
struct OptionData {
    T s;          // spot price
    T strike;     // strike price
    T r;          // risk-free interest rate
    T divq;       // dividend rate
    T v;          // volatility
    T t;          // time to maturity or option expiration in years
    //     (1yr = 1.0, 6mos = 0.5, 3mos = 0.25, ..., etc)
    char OptionType;   // Option type.  "P"=PUT, "C"=CALL
    T divs;       // dividend vals (not used in this test)
    T DGrefval;   // DerivaGem Reference Value
};

template <typename T>
struct GlobalData
{
    OptionData<T> *data;
    T *prices;
    int numOptions;

    int    * otype;
    T * sptprice;
    T * strike;
    T * rate;
    T * volatility;
    T * otime;
    int numError = 0;
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Cumulative Normal Distribution Function
// See Hull, Section 11.8, P.243-244
#define inv_sqrt_2xPI 0.39894228040143270286

template<typename T>
T CNDF ( T InputX )
{
    int sign;

    T OutputX;
    T xInput;
    T xNPrimeofX;
    T expValues;
    T xK2;
    T xK2_2, xK2_3;
    T xK2_4, xK2_5;
    T xLocal, xLocal_1;
    T xLocal_2, xLocal_3;

    // Check for negative value of InputX
    if (isNegative(InputX)) {
        InputX = -InputX;
        sign = 1;
    } else
        sign = 0;

    xInput = InputX;

    // Compute NPrimeX term common to both four & six decimal accuracy calcs
    expValues = (T) exp((double)(T(-0.5) * square(InputX)));
    xNPrimeofX = expValues;
    xNPrimeofX = xNPrimeofX * T(inv_sqrt_2xPI);

    xK2 = T(0.2316419) * xInput;
    xK2 = T(1.0) + xK2;
    xK2 = T(1.0) / xK2;
    xK2_2 = square(xK2);
    xK2_3 = xK2_2 * xK2;
    xK2_4 = xK2_3 * xK2;
    xK2_5 = xK2_4 * xK2;

    xLocal_1 = xK2 * T(0.319381530);
    xLocal_2 = xK2_2 * T(-0.356563782);
    xLocal_3 = xK2_3 * T(1.781477937);
    xLocal_2 += xLocal_3;
    xLocal_3 = xK2_4 * T(-1.821255978);
    xLocal_2 += xLocal_3;
    xLocal_3 = xK2_5 * T(1.330274429);
    xLocal_2 += xLocal_3;

    xLocal_1 += xLocal_2;
    xLocal   = xLocal_1 * xNPrimeofX;

    //printf("# xLocal: %10.10f\n", xLocal);



    xLocal   = T(1.0) - xLocal;

    OutputX  = xLocal;

    //printf("# Output: %10.10f\n", OutputX);

    if (sign) {
        OutputX = T(1.0) - OutputX;
    }

    return OutputX;
}

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

template<typename T>
T BlkSchlsEqEuroNoDiv( T sptprice,
                            T strike, T rate, T volatility,
                            T time, int otype)
{
    T OptionPrice;

    // local private working variables for the calculation
    //fptype xStockPrice;
    //fptype xStrikePrice;
    T xRiskFreeRate;
    T xVolatility;
    T xTime;
    T xSqrtTime;

    T logValues;
    T xLogTerm;
    T xD1;
    T xD2;
    T xPowerTerm;
    T xDen;
    T d1;
    T d2;
    T FutureValueX;
    T NofXd1;
    T NofXd2;
    T NegNofXd1;
    T NegNofXd2;

    //xStockPrice = sptprice;
    //xStrikePrice = strike;
    xRiskFreeRate = rate;
    xVolatility = volatility;
    xTime = time;


    xSqrtTime = squareRoot(xTime);

    logValues = naturalLog( sptprice / strike );

    xLogTerm = logValues;


    xPowerTerm = square(xVolatility);
    xPowerTerm = xPowerTerm * T(0.5);

    xD1 = xRiskFreeRate + xPowerTerm;
    xD1 = xD1 * xTime;
    xD1 = xD1 + xLogTerm;

    xDen = xVolatility * xSqrtTime;
    xD1 = xD1 / xDen;
    xD2 = xD1 -  xDen;

    d1 = xD1;
    d2 = xD2;

    NofXd1 = CNDF( d1 );
    NofXd2 = CNDF( d2 );

    FutureValueX = strike * (T) ( exp( (double) (-(rate)*(time)) ) );
    if (otype == 0) {
        OptionPrice = (sptprice * NofXd1) - (FutureValueX * NofXd2);

    } else {
        NegNofXd1 = (T(1.0) - NofXd1);
        NegNofXd2 = (T(1.0) - NofXd2);
        OptionPrice = (FutureValueX * NegNofXd2) - (sptprice * NegNofXd1);
    }

    return OptionPrice;
}

template<typename T>
int bs_thread(void *tid_ptr, GlobalData<T>& globalData) {
    int i, j;

    int tid = *(int *)tid_ptr;
    int start = tid * (globalData.numOptions);
    int end = start + (globalData.numOptions);
    T price_orig;

    for (j=0; j<NUM_RUNS; j++) {
        for (i=start; i<end; i++) {
            /* Calling main function to calculate option value based on
             * Black & Scholes's equation.
             */
            price_orig = BlkSchlsEqEuroNoDiv( globalData.sptprice[i], globalData.strike[i],
                                              globalData.rate[i], globalData.volatility[i], globalData.otime[i],
                                              globalData.otype[i]);
            globalData.prices[i] = price_orig;
        }
    }
    return 0;
}

template<typename T>
std::vector<float> blackscholes(const std::string& inputFileName)
{
    GlobalData<T> globalData;

    int i;
    int loopnum;
    T * buffer;
    int * buffer2;

    std::vector<float> output;

    //Read input data from file
    std::ifstream file(inputFileName);
    if(!file) {
        std::cout << "ERROR: Unable to open file " << inputFileName << std::endl;
        exit(1);
    }

    file >> globalData.numOptions;

    // alloc spaces for the option data
    globalData.data = (OptionData<T>*)malloc(globalData.numOptions*sizeof(OptionData<T>));
    globalData.prices = (T*)malloc(globalData.numOptions*sizeof(T));
    for ( loopnum = 0; loopnum < globalData.numOptions; ++ loopnum )
    {
        readFromStream(file, globalData.data[loopnum].s);
        readFromStream(file, globalData.data[loopnum].strike);
        readFromStream(file, globalData.data[loopnum].r);
        readFromStream(file, globalData.data[loopnum].divq);
        readFromStream(file, globalData.data[loopnum].v);
        readFromStream(file, globalData.data[loopnum].t);
        file >> globalData.data[loopnum].OptionType;
        readFromStream(file, globalData.data[loopnum].divs);
        readFromStream(file, globalData.data[loopnum].DGrefval);
    }

#define PAD 256
#define LINESIZE 64

    buffer = (T *) malloc(5 * globalData.numOptions * sizeof(T) + PAD);
    globalData.sptprice = (T *) (((unsigned long long)buffer + PAD) & ~(LINESIZE - 1));
    globalData.strike = globalData.sptprice + globalData.numOptions;
    globalData.rate = globalData.strike + globalData.numOptions;
    globalData.volatility = globalData.rate + globalData.numOptions;
    globalData.otime = globalData.volatility + globalData.numOptions;

    buffer2 = (int *) malloc(globalData.numOptions * sizeof(T) + PAD);
    globalData.otype = (int *) (((unsigned long long)buffer2 + PAD) & ~(LINESIZE - 1));

    T divide(DIVIDE);
    for (i=0; i<globalData.numOptions; i++) {
        globalData.otype[i]      = (globalData.data[i].OptionType == 'P') ? 1 : 0;
        globalData.sptprice[i]   = globalData.data[i].s / divide;
        globalData.strike[i]     = globalData.data[i].strike / divide;
        globalData.rate[i]       = globalData.data[i].r;
        globalData.volatility[i] = globalData.data[i].v;
        globalData.otime[i]      = globalData.data[i].t;
    }

    //serial version
    int tid=0;
    bs_thread<T>(&tid, globalData);


    for(i=0; i<globalData.numOptions; i++) {
        output.push_back((float)globalData.prices[i]);
    }

    free(globalData.data);
    free(globalData.prices);
    free(buffer);
    free(buffer2);

    return output;
}

#endif