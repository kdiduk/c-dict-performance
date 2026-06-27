#include "qdict.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


struct qdict  {
    struct qdict_entry* entries;
    int size;
    int reserved_size;
     
    /* Flag indicating that the entries has been updated and need sorting. */
    int dirty_flag;
};

static int qdict_compare_entries(
    const void* left,
    const void* right)
{
    const struct qdict_entry* item_left = left;
    const struct qdict_entry* item_right = right;
    return strcmp(item_left->key, item_right->key);
}

struct qdict* qdict_create(int reserved_size)
{
    struct qdict* dict = malloc(sizeof(struct qdict));
    assert(dict);

    dict->entries = malloc(reserved_size * sizeof(struct qdict_entry));
    assert(dict->entries);

    dict->size = 0;
    dict->reserved_size = reserved_size;
    dict->dirty_flag = 0;
    
    return dict;
}


void qdict_destroy(struct qdict* dict)
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


struct qdict_entry* qdict_find(struct qdict* dict, const char* key)
{
    if (!dict) {
        return NULL;
    }

    if (dict->dirty_flag) {
        qdict_sort(dict);
    }

    struct qdict_entry key_entry = { .key = key };

    return bsearch(
        &key_entry, 
        dict->entries, 
        dict->size, 
        sizeof(struct qdict_entry), 
        qdict_compare_entries);
}


struct qdict_entry* qdict_put(struct qdict* dict, const char* key, int value)
{
    if (!dict) {
        return NULL;
    }

    if (dict->dirty_flag) {
        qdict_sort(dict);
    }

    struct qdict_entry* entry = qdict_find(dict, key);
    if (entry) {
        /* Entry with a given key already exists, update it's value. */
        entry->value = value;
        return entry;
    }

    if (dict->size >= dict->reserved_size) {
        dict->reserved_size *= 2;
        dict->entries = realloc(
            dict->entries,
            dict->reserved_size * sizeof(struct qdict_entry));
        assert(dict->entries);        
    }

    entry = &dict->entries[dict->size];
    entry->key = strdup(key);
    assert(entry->key);

    entry->value = value;

    dict->size++;
    dict->dirty_flag = 1;

    return entry;
}


void qdict_sort(struct qdict* dict)
{
    if (!dict->dirty_flag) {
        return;
    }

    qsort(
        dict->entries, 
        dict->size, 
        sizeof(struct qdict_entry), 
        qdict_compare_entries);
    
    dict->dirty_flag = 0;
}


void qdict_iterate(struct qdict* dict, void (*visit)(struct qdict_entry* entry))
{
    if (!dict) {
        return;
    }

    if (dict->dirty_flag) {
        qdict_sort(dict);
    }

    for (int i = 0; i < dict->size; i++) {
        visit(&dict->entries[i]);
    }
}

/* EOF */
