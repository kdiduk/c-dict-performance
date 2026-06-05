#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <chrono>
#include <iostream>
#include <random>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

extern "C" {
    #include "dict.h"
}


constexpr int RNG_SEED = 12345;


class benchmark final {
private:
    const size_t dict_size;
    const size_t key_len;
    const size_t query_count;

    struct ::dict* cdict = nullptr;
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
        for (int i = 0; i < dict_size; ++i) {
            dict_put(cdict, keys[i].c_str(), i);
            hashmap[keys[i]] = i;
        }
    }

    ~benchmark()
    {
        if (cdict) {
            dict_destroy(cdict);
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


int main(int argc, char** argv)
{
    std::cout << "Benchmarking of naive dictionary implementation in C"
        #ifdef NDEBUG
            << " [Release build]\n"
        #else
            << " [Debug build]\n"
        #endif
        << "Compare brute force array-based dictionary vs std::unordered_map\n\n"
        << "Parameters: \n"
        << "N = size of the dictionary (number of items in the dictionary)\n"
        << "L = lengt of the keys (we use keys of the equal length)\n"
        << "Q = number of find queries to the dictionary (we search only existing keys)\n"
        << std::endl;

    const std::initializer_list<size_t> dict_sizes = { 10, 20, 50, 100, 200, 500, 1000 };
    const size_t key_len = 5;
    const size_t query_count = 100000;

    for (size_t dict_size: dict_sizes) {
        std::cout << "=====================================================\n";
        std::cout << "Benchmark with N = " << dict_size
            << ", L = " << key_len
            << ", Q = " << query_count
            << std::endl;
        benchmark bm(dict_size, key_len, query_count);

        // warm up
        bm.benchmark_dict(query_count);

        auto time_cdict = bm.benchmark_dict(query_count);
        std::cout << "C naive dictionary time: " << time_cdict << " us" << std::endl;

        // warm up
        bm.benchmark_unordered_map(query_count);

        auto time_hsmap = bm.benchmark_unordered_map(query_count);
        std::cout << "C++ unordererd map time: " << time_hsmap << " us" << std::endl;
        std::cout << "=====================================================\n";
    }

    return EXIT_SUCCESS;
}


/* EOF */
