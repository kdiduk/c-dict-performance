#include "key_generators.hpp"

#include <stdexcept>
#include <random>
#include <unordered_set>


std::vector<std::string> generate_num_keys(size_t dict_size, size_t key_len)
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


std::vector<std::string> generate_alnum_keys(size_t dict_size, size_t key_len)
{
    if (dict_size == 0) {
        throw std::runtime_error(
            "Wrong input data: dict size must be greater than 0");
    }

    static constexpr char alphabet[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";

    constexpr size_t alphabet_size =
        sizeof(alphabet) - 1;

    std::mt19937 rng(42);

    std::uniform_int_distribution<size_t> dist(
        0,
        alphabet_size - 1);

    std::vector<std::string> keys;
    keys.reserve(dict_size);

    std::unordered_set<std::string> unique_keys;
    unique_keys.reserve(dict_size);

    while (keys.size() < dict_size)
    {
        std::string key;
        key.reserve(key_len);

        for (size_t i = 0; i < key_len; ++i) {
            key.push_back(alphabet[dist(rng)]);
        }

        if (unique_keys.insert(key).second) {
            keys.push_back(std::move(key));
        }
    }

    return keys;
}
