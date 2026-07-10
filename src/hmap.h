#ifndef HMAP_H
#define HMAP_H

#include <stddef.h>

/* Hashmap table entry. */
struct hmap_entry {
    const char* key;
    int value;
    struct hmap_entry* next; /* Next entry in the chain or NULL. */
};

struct hmap;


struct hmap* hmap_create(size_t reserved_size);

void hmap_destroy(struct hmap* hmap);

struct hmap_entry* hmap_find(struct hmap* hmap, const char* key);

struct hmap_entry* hmap_put(struct hmap* hmap, const char* key, int value);

void hmap_iterate(struct hmap* hmap, void (*visit)(const char* key, int value));

#endif /* HMAP_H */
