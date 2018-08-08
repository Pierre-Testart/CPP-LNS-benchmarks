/*
* Created by Pierre Testart
* Student at Politecnico di Milano
*/

#include <cassert>
#include <iostream>
#include <string>
#include "lns.hpp"
#include "benchmarks/benchmarks.hpp"
#include "benchmarks/fft.hpp"
#include "benchmarks/blackscholes.hpp"
#include "benchmarks/inversek2j.hpp"
#include "benchmarks/jmeint.hpp"
#include "benchmarks/sobel.hpp"
#include "benchmarks/kmeans.hpp"

using namespace std;
using namespace lns;

template<typename F>
bool equal(F val1, F val2)
{
    if(isnan(val1) || isnan(val2))
        return isnan(val1) && isnan(val2);

    if(isinf(val1) || isinf(val2))
        return val1 == val2;

    return abs(val1 - val2) < 0.00001 || (val2 != 0 && abs(1 - val1/val2) < 0.0001);
}

template<typename F, typename L>
bool equal(F val1, const L& lns)
{
    return equal(val1, (F)lns);
}

template<typename F, typename L>
void tests()
{
    vector<F> floatValues = {0.0, 1.5, 0.0003, -3.15, -1286.12, numeric_limits<F>::infinity(),
       -numeric_limits<F>::infinity(), numeric_limits<F>::quiet_NaN(), numeric_limits<F>::signaling_NaN()};
    vector<L> lnsValues;
    for(F value : floatValues)
        lnsValues.emplace_back(value);

    for(size_t i = 0; i < floatValues.size(); ++i)
    {
        // initialization tests
        assert(equal(floatValues[i], lnsValues[i]));

        // integer cast test
        assert((int)floatValues[i] == (int)lnsValues[i]);

        // unary minus tests
        assert(equal(-floatValues[i], -lnsValues[i]));

        // square test
        assert(equal(floatValues[i] * floatValues[i], lnsValues[i].square()));

        // sqrt test
        assert(equal(sqrt(floatValues[i]), lnsValues[i].sqrt()));

        // inverse test
        assert(equal(1 / floatValues[i], lnsValues[i].inverse()));

        // log2 test
        assert(equal(log2(floatValues[i]), lnsValues[i].template log2<F>()));

        // abs test
        assert(equal(abs(floatValues[i]), lnsValues[i].abs()));

        // special value tests
        assert((floatValues[i] == 0) == lnsValues[i].isZero());
        assert((floatValues[i] > 0) == lnsValues[i].isPositive());
        assert((floatValues[i] < 0) == lnsValues[i].isNegative());
        assert(isinf(floatValues[i]) == lnsValues[i].isInf());
        assert((isinf(floatValues[i]) && (floatValues[i] > 0)) == lnsValues[i].isPosInf());
        assert((isinf(floatValues[i]) && (floatValues[i] < 0)) == lnsValues[i].isNegInf());
        assert(isnan(floatValues[i]) == lnsValues[i].isNan());

        for(size_t j = 0; j < floatValues.size(); ++j)
        {
            // multiplication tests
            assert(equal(floatValues[i] * floatValues[j], lnsValues[i] * lnsValues[j]));

            // division tests
            assert(equal(floatValues[i] / floatValues[j], lnsValues[i] / lnsValues[j]));

            // addition tests
            assert(equal(floatValues[i] + floatValues[j], lnsValues[i] + lnsValues[j]));

            // subtraction tests
            assert(equal(floatValues[i] - floatValues[j], lnsValues[i] - lnsValues[j]));

            // comparison tests
            assert((floatValues[i] == floatValues[j]) == (lnsValues[i] == lnsValues[j]));
            assert((floatValues[i] != floatValues[j]) == (lnsValues[i] != lnsValues[j]));
            assert((floatValues[i] < floatValues[j]) == (lnsValues[i] < lnsValues[j]));
            assert((floatValues[i] > floatValues[j]) == (lnsValues[i] > lnsValues[j]));
            assert((floatValues[i] <= floatValues[j]) == (lnsValues[i] <= lnsValues[j]));
            assert((floatValues[i] >= floatValues[j]) == (lnsValues[i] >= lnsValues[j]));
        }
    }
}

// function to get the name of a LNS type
template<typename T>
string getTypeName()
{
    return "lns_t<" + to_string(T::integerBits) + ", " + to_string(T::fractionalBits) + ", " + to_string(T::approximationLevel) + ">";
}
// template specializations to get the name of floating-point type
template<> string getTypeName<float>() { return "float"; }
template<> string getTypeName<double>() { return "double"; }
template<> string getTypeName<long double>() { return "long double"; }

int main()
{
    using T1 = BENCHMARK_TYPE1;
    using T2 = BENCHMARK_TYPE2;
    string typeName1 = getTypeName<T1>();
    string typeName2 = getTypeName<T2>();

    #ifdef BENCHMARK_FFT
        cout << "-------------------------------------------------------------" << endl;
        cout << "Running benchmark FFT, comparing " << typeName2 << " to " << typeName1 << endl;
        cout << "-------------------------------------------------------------" << endl;
        runBenchmark(fft<T1>, fft<T2>, 32768);
    #endif
    #ifdef BENCHMARK_BLACKSCHOLES
        cout << "-------------------------------------------------------------" << endl;
        cout << "Running benchmark Black-Scholes, comparing " << typeName2 << " to " << typeName1 << endl;
        cout << "-------------------------------------------------------------" << endl;
        runBenchmark(blackscholes<T1>, blackscholes<T2>, "benchmarks/blackscholesTrain_100K.data");
    #endif
    #ifdef BENCHMARK_INVERSEK2J
        cout << "-------------------------------------------------------------" << endl;
        cout << "Running benchmark Inversek2j, comparing " << typeName2 << " to " << typeName1 << endl;
        cout << "-------------------------------------------------------------" << endl;
        runBenchmark(inversek2j<T1>, inversek2j<T2>, "benchmarks/theta_100K.data");
    #endif
    #ifdef BENCHMARK_JMEINT
        cout << "-------------------------------------------------------------" << endl;
        cout << "Running benchmark Jmeint, comparing " << typeName2 << " to " << typeName1 << endl;
        cout << "-------------------------------------------------------------" << endl;
        runBenchmark(jmeint<T1>, jmeint<T2>, "benchmarks/jmeint_50K.data");
    #endif
    #ifdef BENCHMARK_SOBEL
        cout << "-------------------------------------------------------------" << endl;
        cout << "Running benchmark Sobel, comparing " << typeName2 << " to " << typeName1 << endl;
        cout << "-------------------------------------------------------------" << endl;
        runBenchmark(sobel<T1>, sobel<T2>, "benchmarks/sobel.rgb");
    #endif
    #ifdef BENCHMARK_KMEANS
        cout << "-------------------------------------------------------------" << endl;
        cout << "Running benchmark Kmeans, comparing " << typeName2 << " to " << typeName1 << endl;
        cout << "-------------------------------------------------------------" << endl;
        runBenchmark(kmeans<T1>, kmeans<T2>, "benchmarks/kmeans.rgb");
    #endif

    return 0;
}
