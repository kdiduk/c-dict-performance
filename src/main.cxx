#include <algorithm>
#include <initializer_list>
#include <iostream>
#include <vector>

#include "benchmark.hpp"


int main()
{
    std::cout << "Benchmarking of simple dictionary implementations in C"
        #ifdef NDEBUG
            << " [Release build]\n"
        #else
            << " [Debug build]\n"
        #endif
        << "Compare performance of the following variants:\n"
        << " - naive array-based dictionary in plain C with full search\n"
        << " - sorted array-based dictionary in plain C with binary search\n"
        << " - std::map from C++\n"
        << " - std::unordered_map from C++\n\n"
        << "Using random alphanumeric keys\n"
        << "Parameters: \n"
        << "N = size of the dictionary (number of items in the dictionary)\n"
        << "L = lengt of the keys (we use keys of the equal length)\n"
        << "Q = number of find queries to the dictionary (we search only existing keys)\n"
        << "R = number of repeated runs (we use the median of them)\n"
        << "===========================================================================\n"
        << std::endl;

    const std::initializer_list<size_t> dict_sizes = 
        { 10, 20, 50, 100, 200, 500, 1000, 10000, 50000 };
    const size_t key_len = 12;
    const size_t query_count = 100000;
    const size_t repeat_count = 21;

    for (size_t dict_size: dict_sizes) {
        std::cout << "Benchmark with N = " << dict_size
            << ", L = " << key_len
            << ", Q = " << query_count
            << ", R = " << repeat_count
            << std::endl;
        benchmark bm(dict_size, key_len, query_count);

        std::vector<long long> series;
        series.reserve(repeat_count);

        // Benchmark C naive dictionary
        // (only for dict sizes <= 1000)
        //if (dict_size <= 1000) {
            for (size_t i = 0; i < repeat_count; ++i) {
                series.push_back(bm.benchmark_dict(query_count));
            }
            std::sort(series.begin(), series.end());
            auto time_cdict = series[repeat_count / 2];
            std::cout << "C naive dictionary time:  " 
                << time_cdict << " us" 
                << std::endl;
            series.clear();
        //}

        // Benchmark C sorted dictionary with binary search
        for (size_t i = 0; i < repeat_count; ++i) {
            series.push_back(bm.benchmark_qdict(query_count));
        }
        std::sort(series.begin(), series.end());
        auto time_qdict = series[repeat_count / 2];
        std::cout << "C sorted dictionary time: "
            << time_qdict << " us"
            << " (speed ratio vs above: "
            << std::fixed
            << std::setprecision(2)
            << static_cast<double>(time_cdict)/time_qdict 
            << "x)" 
            << std::endl;
        series.clear();

        
        // Benchmark std::map
        for (size_t i = 0; i < repeat_count; i++) {
            series.push_back(bm.benchmark_map(query_count));
        }
        std::sort(series.begin(), series.end());
        auto time_rbtree = series[repeat_count / 2];
        std::cout << "C++ std::map time:        " 
            << time_rbtree << " us"
            << " (speed ratio vs above: "
            << std::fixed
            << std::setprecision(2)
            << static_cast<double>(time_qdict)/time_rbtree 
            << "x)" 
            << std::endl;

        
        // Benchmark std::unordered_map
        series.clear();
        for (size_t i = 0; i < repeat_count; i++) {
            series.push_back(bm.benchmark_unordered_map(query_count));
        }
        std::sort(series.begin(), series.end());
        auto time_hsmap = series[repeat_count / 2];
        std::cout << "C++ unordererd map time:  " 
            << time_hsmap << " us" 
            << " (speed ratio vs above: "
            << std::fixed
            << std::setprecision(2)
            << static_cast<double>(time_rbtree)/time_hsmap 
            << "x)" 
            << std::endl;

        std::cout << "===========================================================================\n";
    }

    return EXIT_SUCCESS;
}

/* EOF */
