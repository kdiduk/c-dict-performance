#ifndef DICT_H
#define DICT_H


struct dict_entry {
    const char* key;
    int value;
};

struct dict {
    struct dict_entry* entries;
    int size;
    int reserved_size;
};


struct dict* dict_create(int reserved_size);

void dict_destroy(struct dict* dict);

struct dict_entry* dict_find(struct dict* dict, const char* key);

struct dict_entry* dict_put(struct dict* dict, const char* key, int value);

void dict_iterate(struct dict* dict, void (*visit)(struct dict_entry* entry));

#endif /* DICT_H */
