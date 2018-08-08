#ifndef INVERSEK2J_HPP
#define INVERSEK2J_HPP

/*
 *  Created on: Sep. 10 2013
 *			Author: Amir Yazdanbakhsh <yazdanbakhsh@wisc.edu>
 */

#include <iostream>
#include <cstdlib>
#include <fstream>
#include <ctime>
#include <cmath>
#include <iomanip>
#include <string>
#include <vector>
#include "utilities.hpp"

template<typename T>
void forward(T theta1, T theta2, T* x, T* y) {

    static const T l1(0.5);
    static const T l2(0.5);

    *x = l1 * cosValue(theta1) + l2 * cosValue(theta1 + theta2) ;
    *y = l1 * sinValue(theta1) + l2 * sinValue(theta1 + theta2) ;
}

template<typename T>
void inverse(T x, T y, T* theta1, T* theta2) {

    static const T l1(0.5);
    static const T l2(0.5);
    static const T a(T(2) * l1 * l2);
    static const T b((l1 * l1) + (l2 * l2));

    *theta2 = acosValue(((x * x) + (y * y) - b) / a);
    *theta1 = asinValue((y * (l1 + l2 * cosValue(*theta2)) - x * l2 * sinValue(*theta2))/(x * x + y * y));
}

template<typename T>
std::vector<float> inversek2j(const std::string& inputFileName)
{
    std::vector<float> output;

    std::ifstream file(inputFileName);
    if(!file) {
        std::cout << "ERROR: Unable to open file " << inputFileName << std::endl;
        exit(1);
    }

    int n;

    // first line defins the number of enteries
    file >> n;

    T* t1t2xy = (T*)malloc(n * 2 * 2 * sizeof(T));

    if(t1t2xy == nullptr)
    {
        std::cerr << "# Cannot allocate memory for the coordinates an angles!" << std::endl;
        exit(1);
    }

    for(int i = 0 ; i < n * 2 * 2 ; i += 2 * 2)
    {
        T theta1, theta2;
        readFromStream(file, theta1);
        readFromStream(file, theta2);

        t1t2xy[i] = theta1;
        t1t2xy[i + 1] = theta2;

        forward(t1t2xy[i + 0], t1t2xy[i + 1], t1t2xy + (i + 2), t1t2xy + (i + 3));
    }

    for(int i = 0 ; i < n * 2 * 2 ; i += 2 * 2)
    {
        inverse(t1t2xy[i + 2], t1t2xy[i + 3], t1t2xy + (i + 0), t1t2xy + (i + 1));
    }

    for(int i = 0 ; i < n * 2 * 2 ; i += 2 * 2)
    {
        output.push_back((float)t1t2xy[i+0]);
        output.push_back((float)t1t2xy[i+1]);
    }

    free(t1t2xy);
    t1t2xy = nullptr;

    return output;
}

#endif