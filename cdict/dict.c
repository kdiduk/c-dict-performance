#include "dict.h"

#include <stdlib.h>
#include <string.h>


struct dict* dict_create(int reserved_size)
{
    struct dict* dict = malloc(sizeof(struct dict));
    if (!dict) {
        return NULL;
    }

    dict->entries = malloc(reserved_size * sizeof(struct dict_entry));
    if (!dict->entries) {
        free(dict);
        return NULL;
    }

    dict->size = 0;
    dict->reserved_size = reserved_size;
    
    return dict;
}


void dict_destroy(struct dict* dict)
{
    if (dict) {
        free(dict->entries);
        free(dict);
    }
}


struct dict_entry* dict_find(struct dict* dict, const char* key)
{
    for (int i = 0; i < dict->size; i++) {
        if (!strcmp(key, dict->entries[i].key)) {
            return &dict->entries[i];
        }
    }

    return NULL;
}


struct dict_entry* dict_put(struct dict* dict, const char* key, int value)
{
}
