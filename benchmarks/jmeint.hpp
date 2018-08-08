#ifndef JMEINT_HPP
#define JMEINT_HPP

#include "tritri.hpp"

#include <fstream>
#include <iostream>
#include <map>
#include <ctime>
#include <vector>
#include "utilities.hpp"

template<typename T>
std::vector<bool> jmeint(const std::string& inputFileName)
{
    int i;
    int x;
    int n;

    std::vector<bool> output;

    std::ifstream file(inputFileName);
    if(!file) {
        std::cout << "ERROR: Unable to open file " << inputFileName << std::endl;
        exit(1);
    }

    // first line defins the number of enteries
    file >> n;

    // create the directory for storing data
    T* xyz = (T*)malloc(n * 6 * 3 * sizeof (T)) ;
    if(xyz == nullptr)
    {
        std::cout << "cannot allocate memory for the triangle coordinates!" << std::endl;
        exit(1);
    }

    i = 0;
    while (i < n)
    {
        for(int j = 0 ; j < 18; j++)
        {
            readFromStream(file, xyz[i * 18 + j]);
        }
        i++;
    }

    for(i = 0 ; i < (n * 6 * 3); i += 6 * 3)
    {
        x = tri_tri_intersect<T>(
                xyz + i + 0 * 3, xyz + i + 1 * 3, xyz + i + 2 * 3,
                xyz + i + 3 * 3, xyz + i + 4 * 3, xyz + i + 5 * 3);

        output.push_back(x != 0);
    }

    free(xyz) ;
    xyz = nullptr ;

    return output;
}

#endif