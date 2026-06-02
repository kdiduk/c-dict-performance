#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dict.h"

#define LINE_BUFFER_SIZE (256)

char line_buffer[LINE_BUFFER_SIZE + 1];


void print_dict_entry(struct dict_entry* entry);


int main(int argc, char** argv)
{
    if (argc < 2) {
        fprintf(stderr, "Missing file name argument\n");
        return EXIT_FAILURE;
    }

    FILE* fp = fopen(argv[1], "r");
    if (!fp) {
        fprintf(stderr, "Failed to open file %s\n", argv[1]);
        return EXIT_FAILURE;
    }

    struct dict* dict = dict_create(100);
    while (fgets(line_buffer, LINE_BUFFER_SIZE, fp)) {
        size_t len = strlen(line_buffer);
        if (line_buffer[len-1] == '\n') {
            line_buffer[len-1] = '\0';
        }

        struct dict_entry* entry = dict_find(dict, line_buffer);
        if (entry) {
            entry->value++;
        }
        else {
            dict_put(dict, line_buffer, 1);
        }
    }

    dict_iterate(dict, print_dict_entry);

    fclose(fp);
    dict_destroy(dict);

    return EXIT_SUCCESS;
}


void print_dict_entry(struct dict_entry* entry)
{
    fprintf(stdout, "%s: %d\n", entry->key, entry->value);
}

/* EOF */
