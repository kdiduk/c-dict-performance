#include "hmap.h"

#include <stdlib.h>
#include <string.h>


struct hmap {
    struct hmap_entry** table; /* A table of linked lists. */
    size_t hash_size;
};


static size_t get_hash(const char* key)
{
    size_t hash = 0;

    while (*key) {
        hash *= 31;
        hash += *key;
        key++;
    }

    return hash;
}


struct hmap* hmap_create(size_t reserved_size)
{
    struct hmap* hmap = malloc(sizeof(struct hmap));

    if (!hmap) {
        return NULL;
    }

    hmap->table = calloc(reserved_size, sizeof(struct hmap_entry*));
    if (!hmap->table) {
        free(hmap);
        return NULL;
    }
    hmap->hash_size = reserved_size;

    return hmap;
}


void hmap_destroy(struct hmap* hmap)
{
    if (!hmap) {
      return;
    }

    for (size_t i = 0; i < hmap->hash_size; ++i) {
        struct hmap_entry* entry = hmap->table[i];
        while (entry != NULL) {
            struct hmap_entry* next = entry->next;
            /* TODO: free entry->key if it was allocated. */
            free(entry);
            entry = next;
        }
    }

    free(hmap->table);
    free(hmap);
}


struct hmap_entry* hmap_find(struct hmap* hmap, const char* key)
{
    if (!hmap) {
        return NULL;
    }
    struct hmap_entry* entry = hmap->table[get_hash(key) % hmap->hash_size];
    while (entry) {
        if (strcmp(key, entry->key) == 0) {
            break;
        }

        entry = entry->next;
    }

    return entry;
}

struct hmap_entry* hmap_put(struct hmap* hmap, const char* key, int value);

void hmap_iterate(struct hmap* hmap, void (*visit)(const char* key, int value));
