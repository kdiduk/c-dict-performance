#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <iostream>
#include <string>

extern "C" {
    #include "dict.h"
}

#define LINE_BUFFER_SIZE (256)

char line_buffer[LINE_BUFFER_SIZE + 1];


void print_dict_entry(struct dict_entry* entry);


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
