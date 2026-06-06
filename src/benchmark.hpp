#ifndef BENCHMARK_HPP
#define BENCHMARK_HPP

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


class benchmark final {
private:

    static constexpr int RNG_SEED = 12345;

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
    static std::vector<std::string> generate_keys(size_t dict_size, size_t key_len = 8);

    // Pre-generate random indices of the keys
    static std::vector<size_t> generate_queries(size_t query_count, size_t dict_size);

public:
    explicit benchmark(size_t dict_size, size_t key_len, size_t query_count);

    ~benchmark();
    
    long long benchmark_dict(size_t runs_count) const;

    long long benchmark_qdict(size_t runs_count) const;

    long long benchmark_map(size_t runs_count) const;

    long long benchmark_unordered_map(size_t runs_count) const;
};

#endif // BENCHMARK_HPP