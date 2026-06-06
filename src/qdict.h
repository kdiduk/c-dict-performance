#ifndef QDICT_H
#define QDICT_H


struct qdict_entry {
    const char* key;
    int value;
};

struct qdict;


struct qdict* qdict_create(int reserved_size);

void qdict_destroy(struct qdict* dict);

struct qdict_entry* qdict_find(struct qdict* dict, const char* key);

struct qdict_entry* qdict_put(struct qdict* dict, const char* key, int value);

void qdict_sort(struct qdict* dict);

void qdict_iterate(struct qdict* dict, void (*visit)(struct qdict_entry* entry));

#endif /* DICT_H */1
