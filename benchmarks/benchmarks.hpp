/*
* Created by Pierre Testart
* Student at Politecnico di Milano
*/

#ifndef BENCHMARKS_HPP
#define BENCHMARKS_HPP

#include <chrono>
#include <random>
#include <vector>
#include <iostream>
#include <iomanip>

template<typename T, int N>
void saxpy()
{
    std::mt19937 generator;
    std::uniform_real_distribution<double> distribution(-1000.0, 1000.0);
    T a(distribution(generator));
    std::vector<T> x, y;
    x.reserve(N);
    y.reserve(N);
    for(int i = 0; i < N; ++i)
    {
        x.emplace_back(distribution(generator));
        y.emplace_back(distribution(generator));
    }

    auto startTime = std::chrono::steady_clock::now();

    for(int i = 0; i < N; ++i)
        y[i] = a * x[i] + y[i];

    auto endTime = std::chrono::steady_clock::now();
    std::cout << "Saxpy time: " << std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count() << " ms" << std::endl;
}

double error(double value1, double value2)
{
	if(std::isnan(value1) || std::isnan(value2))
        return 1.0;

    if((value1 == std::numeric_limits<double>::infinity()) != (value2 == std::numeric_limits<double>::infinity()))
        return 1.0;

    if((value1 == -std::numeric_limits<double>::infinity()) != (value2 == -std::numeric_limits<double>::infinity()))
        return 1.0;

    if(value1 == 0.0 && value2 == 0.0)
        return 0.0;

    return fabs(value1 - value2) / std::max(fabs(value1), fabs(value2));
}

// compare generic vectors of floats
void printVectorError(const std::vector<float>& values1, const std::vector<float>& values2)
{
    if(values1.size() != values2.size())
    {
        std::cout << "Error: different vector sizes in printError" << std::endl;
        return;
    }

    double maxError = 0.0, avgError = 0.0, maxAbsError = 0.0, avgAbsError = 0.0;
    float maxErrorVal1 = 0.0f, maxErrorVal2 = 0.0f, maxAbsErrorVal1 = 0.0f, maxAbsErrorVal2 = 0.0f;
    for(size_t i = 0; i < values1.size(); ++i)
    {
        double e = error(values1[i], values2[i]);
        if(e > maxError)
        {
            maxError = e;
            maxErrorVal1 = values1[i];
            maxErrorVal2 = values2[i];
        }
        avgError += e;
        auto absError = std::isnan(values2[i]) ? fabs(values1[i]) : fabs(values1[i] - values2[i]);
        if(absError > maxAbsError)
        {
            maxAbsError = absError;
            maxAbsErrorVal1 = values1[i];
            maxAbsErrorVal2 = values2[i];
        }
        avgAbsError += absError;
    }
    avgError /= values1.size();
    avgAbsError /= values1.size();

    std::cout << std::setprecision(10);
    std::cout << "Maximum error: " << (maxError * 100) << "% from values: " << maxErrorVal1 << " and " << maxErrorVal2 << std::endl;
    std::cout << "Average error: " << (avgError * 100) << "%" << std::endl;
    std::cout << "Maximum absolute error: " << maxAbsError << " from values: " << maxAbsErrorVal1 << " and " << maxAbsErrorVal2 << std::endl;
    std::cout << "Average absolute error: " << avgAbsError << std::endl;
}

// compare results of a binary classification test
void printVectorError(const std::vector<bool>& values1, const std::vector<bool>& values2)
{
    if(values1.size() != values2.size())
    {
        std::cout << "Error: different vector sizes in printError" << std::endl;
        return;
    }

    size_t posCount = 0, negCount = 0, truePosCount = 0, falsePosCount = 0, trueNegCount = 0, falseNegCount = 0;
    for(size_t i = 0; i < values1.size(); ++i)
    {
        if(values1[i])
        {
            ++posCount;
            if(values2[i])
                ++truePosCount;
            else
                ++falseNegCount;
        }
        else
        {
            ++negCount;
            if(values2[i])
                ++falsePosCount;
            else
                ++trueNegCount;
        }
    }
    double sensitivity = truePosCount * 100.0 / posCount;
    double specificity = trueNegCount * 100.0 / negCount;
	double accuracy = (truePosCount + trueNegCount) * 100.0 / (posCount + negCount);

    std::cout << std::setprecision(10);
    std::cout << "True positive count: " << truePosCount << std::endl;
    std::cout << "True negative count: " << trueNegCount << std::endl;
    std::cout << "False positive count: " << falsePosCount << std::endl;
    std::cout << "False negative count: " << falseNegCount << std::endl;
    std::cout << "Sensitivity: " << sensitivity << "%" << std::endl;
    std::cout << "Specificity: " << specificity << "%" << std::endl;
    std::cout << "Accuracy: " << accuracy << "%" << std::endl;
}

// to compare images (3 adjacent ints correspond to 1 pixel)
void printVectorError(const std::vector<int>& values1, const std::vector<int>& values2)
{
    if(values1.size() != values2.size())
    {
        std::cout << "Error: different vector sizes in printError" << std::endl;
        return;
    }
    if(values1.size() % 3 != 0)
    {
        std::cout << "Error: pixel count in benchmark output is not an integer" << std::endl;
        return;
    }

    double meanAbsError = 0, maxAbsError = 0, meanSqError = 0;
    for(size_t i = 0; i < values1.size(); i += 3)
    {
        double sqError = 0.0;
        for(size_t j = i; j < i + 3; ++j)
            sqError += (values1[j] - values2[j]) * (values1[j] - values2[j]);

        meanSqError += sqError;
        double error = sqrt(sqError);
        meanAbsError += error;
        maxAbsError = std::max(maxAbsError, error);
    }
    meanAbsError /= values1.size();
    meanSqError /= values1.size();

    std::cout << std::setprecision(10);
    std::cout << "Maximum absolute error: " << maxAbsError << std::endl;
    std::cout << "Mean absolute error: " << meanAbsError << std::endl;
    std::cout << "Root-mean-square error: " << sqrt(meanSqError) << std::endl;
}

void printTimeDifference(long long int time1, long long int time2)
{
    if(time2 > time1)
        std::cout << (static_cast<double>(time2) / time1) << " times slower" << std::endl;
    else
        std::cout << (static_cast<double>(time1) / time2) << " times faster" << std::endl;
}

template<typename F1, typename F2, typename Param>
void runBenchmark(const F1& f1, const F2& f2, const Param& param)
{
    auto start = std::chrono::steady_clock::now();
    auto values1 = f1(param);
    auto time1 = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start).count();

    start = std::chrono::steady_clock::now();
    auto values2 = f2(param);
    auto time2 = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start).count();

    printTimeDifference(time1, time2);
    printVectorError(values1, values2);
    std::cout << std::endl;
}

#endif
