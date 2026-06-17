#ifndef KEY_GENERATORS_HPP
#define KEY_GENERATORS_HPP

#include <vector>
#include <string>

// generate unique random keys of equal length key_len: 
// generate numbers with leading zeroes from 0 to data_count-1
std::vector<std::string> generate_num_keys(size_t dict_size, size_t key_len);


// generate random alphanumeric string keys
std::vector<std::string> generate_alnum_keys(size_t dict_size, size_t key_len);


#endif // KEY_GENERATORS_HPP