#include "benchmark.hpp"


std::vector<std::string> benchmark::generate_keys(size_t dict_size, size_t key_len)
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


std::vector<size_t> benchmark::generate_queries(size_t query_count, size_t dict_size)
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

benchmark::benchmark(size_t dict_size, size_t key_len, size_t query_count)
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


benchmark::~benchmark()
{
    if (cdict) {
        dict_destroy(cdict);
        qdict_destroy(qdict);
        qdict = nullptr;
        cdict = nullptr;
    }
}


long long benchmark::benchmark_dict(size_t runs_count) const
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


long long benchmark::benchmark_qdict(size_t runs_count) const
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


long long benchmark::benchmark_map(size_t runs_count) const
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


long long benchmark::benchmark_unordered_map(size_t runs_count) const
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

// EOF