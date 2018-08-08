#ifndef KMEANS_HPP
#define KMEANS_HPP

#include <cstdio>
#include <string>
#include <iostream>
#include <ctime>
#include <fstream>
#include <sstream>
#include <vector>
#include "segmentation.hpp"

template<typename T>
std::vector<int> kmeans(const std::string& inputFileName)
{
    srand(time(NULL));

    RgbImage<T> srcImage;
    Clusters<T> clusters;

    initRgbImage(&srcImage);

    loadRgbImage(inputFileName.c_str(), &srcImage, T(256));

    initClusters(&clusters, 6, T(1));
    segmentImage(&srcImage, &clusters, 1);

    auto output = exportRgbImage(&srcImage, T(256));


    freeRgbImage(&srcImage);
    freeClusters(&clusters);
    return output;
}

#endif