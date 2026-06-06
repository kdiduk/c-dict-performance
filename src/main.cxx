#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <chrono>
#include <iomanip>
#include <iostream>
#include <map>
#include <random>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

extern "C" {
    #include "dict.h"
    #include "qdict.h"
}


constexpr int RNG_SEED = 12345;


class benchmark final {
private:
    const size_t dict_size;
    const size_t key_len;
    const size_t query_count;

    struct ::dict* cdict = nullptr;
    struct ::qdict* qdict = nullptr;
    std::map<std::string, int> rbtree;
    std::unordered_map<std::string, int> hashmap;
    
    // all keys in a string form, all with the same length
    const std::vector<std::string> keys; 

    // random indices of keys to be queried during a benchmark
    std::vector<std::size_t> queries;

    // generate unique random keys of equal length key_len: 
    // generate numbers with leading zeroes from 0 to data_count-1
    static std::vector<std::string> generate_keys(size_t dict_size, size_t key_len = 8)
    {
        if (dict_size == 0) {
            throw std::runtime_error("Wrong input data: dict size must be greater than 0");
        }
        
        if (std::to_string(dict_size-1).length() > key_len) {
            throw std::runtime_error("Wrong input data: key length too short");
        }

        std::vector<std::string> keys;
        keys.reserve(dict_size);
        for (size_t i = 0; i < dict_size; ++i) {
            std::string key = std::to_string(i);

            if (key.length() < key_len) {
                key.insert(0, key_len - key.length(), '0');
            }
            
            keys.push_back(std::move(key));
        }

        return keys;
    }

    // Pre-generate random indices of the keys
    static std::vector<size_t> generate_queries(size_t query_count, size_t dict_size)
    {
        std::vector<size_t> queries;
        queries.reserve(query_count);

        std::mt19937 rng(RNG_SEED);

        std::uniform_int_distribution<size_t> dist(0, dict_size - 1);

        for (size_t i = 0; i < query_count; ++i) {
            queries.push_back(dist(rng));
        }

        return queries;
    }

public:
    explicit benchmark(size_t dict_size, size_t key_len, size_t query_count)
        :   dict_size(dict_size),
            key_len(key_len),
            query_count(query_count), 
            keys(generate_keys(dict_size, key_len)),
            queries(generate_queries(query_count, dict_size))
    {
        cdict = dict_create((int)dict_size + 1);
        qdict = qdict_create((int)dict_size + 1);
        for (int i = 0; i < dict_size; ++i) {
            dict_put(cdict, keys[i].c_str(), i);
            qdict_put(qdict, keys[i].c_str(), i);
            hashmap[keys[i]] = i;
            rbtree[keys[i]] = i;
        }
        qdict_sort(qdict);

        if (hashmap.size() != dict_size) {
            throw std::runtime_error("std::unordered map not initialized correctly");
        }
    
        if (rbtree.size() != dict_size) {
            throw std::runtime_error("std::map map not initialized correctly");
        }
    }

    ~benchmark()
    {
        if (cdict) {
            dict_destroy(cdict);
            qdict_destroy(qdict);
            qdict = nullptr;
            cdict = nullptr;
        }
    }
    
    long long benchmark_dict(size_t runs_count) const
    {
        using timer = std::chrono::steady_clock;

        volatile long long sink = 0;
        timer::time_point begin = timer::now();
        for (size_t i = 0; i < runs_count; ++i) {
            const char* key = keys[queries[i]].c_str();
            struct dict_entry* entry = dict_find(cdict, key);
            sink += entry->value;
        }
        timer::time_point end = timer::now();
        
        return std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();
    }

    long long benchmark_qdict(size_t runs_count) const
    {
        using timer = std::chrono::steady_clock;

        volatile long long sink = 0;
        timer::time_point begin = timer::now();
        for (size_t i = 0; i < runs_count; ++i) {
            const char* key = keys[queries[i]].c_str();
            struct qdict_entry* entry = qdict_find(qdict, key);
            sink += entry->value;
        }
        timer::time_point end = timer::now();
        
        return std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();
    }

    long long benchmark_map(size_t runs_count) const
    {
        using timer = std::chrono::steady_clock;

        volatile long long sink = 0;

        timer::time_point begin = timer::now();
        for (size_t i = 0; i < runs_count; ++i) {
            const std::string& key = keys[queries[i]];
            auto it = rbtree.find(key);
            sink += it->second;
        }
        timer::time_point end = timer::now();
        
        return std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();
    }

    long long benchmark_unordered_map(size_t runs_count) const
    {
        using timer = std::chrono::steady_clock;

        volatile long long sink = 0;

        timer::time_point begin = timer::now();
        for (size_t i = 0; i < runs_count; ++i) {
            const std::string& key = keys[queries[i]];
            auto it = hashmap.find(key);
            sink += it->second;
        }
        timer::time_point end = timer::now();
        
        return std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();
    }
};


int main()
{
    std::cout << "Benchmarking of naive dictionary implementation in C"
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
        << "Parameters: \n"
        << "N = size of the dictionary (number of items in the dictionary)\n"
        << "L = lengt of the keys (we use keys of the equal length)\n"
        << "Q = number of find queries to the dictionary (we search only existing keys)\n"
        << "R = number of repeated runs (we use the median of them)\n"
        << "=====================================================\n"
        << std::endl;

    const std::initializer_list<size_t> dict_sizes = 
        { 10, 20, 50, 100, 200, 500, 1000, 10000 };
    const size_t key_len = 8;
    const size_t query_count = 100000;
    const size_t repeat_count = 21;

    for (size_t dict_size: dict_sizes) {
        //std::cout << "=====================================================\n";
        std::cout << "Benchmark with N = " << dict_size
            << ", L = " << key_len
            << ", Q = " << query_count
            << ", R = " << repeat_count
            << std::endl;
        benchmark bm(dict_size, key_len, query_count);

        std::vector<long long> series;
        series.reserve(repeat_count);

        // Benchmark C naive dictionary
        for (size_t i = 0; i < repeat_count; ++i) {
            series.push_back(bm.benchmark_dict(query_count));
        }
        std::sort(series.begin(), series.end());
        auto time_cdict = series[repeat_count / 2];
        std::cout << "C naive dictionary time:  " 
            << time_cdict << " us" 
            << std::endl;
        series.clear();


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

        std::cout << "=====================================================\n";
    }

    return EXIT_SUCCESS;
}


/* EOF */
