#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <iostream>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>


extern "C" {
    #include "dict.h"
}

#define LINE_BUFFER_SIZE (256)

char line_buffer[LINE_BUFFER_SIZE + 1];


void print_dict_entry(struct dict_entry* entry);


constexpr int RNG_SEED = 12345;


class benchmark {
private:
    const size_t dict_size;
    const size_t key_len;
    const size_t query_count;
    
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
    }

    std::uint32_t benchmark_dict(size_t runs_count)
    {
        return 0;
    }

    std::uint32_t benchmark_unordered_map(size_t runs_count)
    {
        return 0;
    }
};


int main(int argc, char** argv)
{
    if (argc < 2) {
        std::cerr << "Missing program argument: number of unique keys in a dictionary" 
            << std::endl;
        std::exit(EXIT_FAILURE);
    }

    const int unique_keys_count = std::stoi(argv[1]);
    std::cout << "Running test with " << unique_keys_count << "keys" << std::endl;

    struct dict* dict = dict_create(unique_keys_count);
    
    // Fill in the dict with keys
    for (int i = 0; i < unique_keys_count; i++) {
        char key[10];
        snprintf(key, sizeof(key), "%08u", i);
        dict_put(dict, key, i);
    }

    dict_iterate(dict, print_dict_entry);

    dict_destroy(dict);

    return EXIT_SUCCESS;
}


void print_dict_entry(struct dict_entry* entry)
{
    fprintf(stdout, "%s: %d\n", entry->key, entry->value);
}

/* EOF */
