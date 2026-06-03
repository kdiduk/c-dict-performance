#include "dict.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


struct dict* dict_create(int reserved_size)
{
    struct dict* dict = malloc(sizeof(struct dict));
    assert(dict);

    dict->entries = malloc(reserved_size * sizeof(struct dict_entry));
    assert(dict->entries);

    dict->size = 0;
    dict->reserved_size = reserved_size;
    
    return dict;
}


void dict_destroy(struct dict* dict)
{
    if (!dict) {
        return;
    }

    for (int i = 0; i < dict->size; i++) {
        free((void*)dict->entries[i].key);
    }
    free(dict->entries);
    free(dict);
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
    struct dict_entry* entry = dict_find(dict, key);
    if (entry) {
        /* Entry with a given key already exists, update it's value. */
        entry->value = value;
        return entry;
    }

    if (dict->size >= dict->reserved_size) {
        dict->reserved_size *= 2;
        dict->entries = realloc(
            dict->entries,
            dict->reserved_size * sizeof(struct dict_entry));
        assert(dict->entries);        
    }

    entry = &dict->entries[dict->size];
    entry->key = strdup(key);
    assert(entry->key);

    entry->value = value;

    dict->size++;

    return entry;
}


void dict_iterate(struct dict* dict, void (*visit)(struct dict_entry* entry))
{
    if (!dict) {
        return;
    }

    for (int i = 0; i < dict->size; i++) {
        visit(&dict->entries[i]);
    }
}

/* EOF */
