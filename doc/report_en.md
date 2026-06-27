# Performance Evaluation of Simple Dictionary Implementations in C

## Introduction

***What motivated this work?***

The C programming language does not provide a standard implementation of associative arrays (hereafter referred to as *dictionaries*).

Several third-party implementations based on hash tables are available (for example, `ut_hash` and `khash`), but none of them has become a widely accepted standard.

This led to the following question: Is it possible to implement a simple and reliable dictionary using only the standard C library while achieving performance sufficient for practical use?

## Objective

***What was the objective of this work?***

The objective of this work was to investigate whether a simple dictionary implementation can be developed in the C programming language using only the standard library while remaining suitable for practical use and providing acceptable performance.

## Problem Statement

***How was the objective addressed?***

### Evaluation Criteria

In this work, a *simple dictionary implementation* is defined as an implementation that an ordinary C programmer can develop independently within a short period of time, without requiring in-depth knowledge of specialized algorithms or data structures.

A *short implementation time* is considered to be comparable to the time required to learn and integrate an existing third-party dictionary library such as `ut_hash` or `khash`.

*Specialized algorithms and data structures* refer to techniques commonly used in library implementations of dictionaries, such as balanced search trees (e.g., Red–Black or AVL trees), hash tables, hash functions, and collision resolution algorithms.

*Acceptable performance* is defined as performance of the same order of magnitude as that of existing library implementations.

### Evaluated Implementations

Two dictionary implementations based on dynamic arrays were selected for evaluation:

* Linear search.
* Binary search on a sorted array.

Both implementations were written in C using only the standard library.

### Evaluated Operation

The performance of key lookup was measured.

### Reference Implementations

For comparison, the standard C++ containers `std::map` and `std::unordered_map` were selected.

These implementations were chosen for the following reasons:

* They are considerably more widely used in practice than any individual third-party C dictionary library.
* C and C++ generally provide comparable runtime performance, making these containers suitable reference implementations for the purposes of this study.

## Implementation

### Common Data Structure

As described above, two dictionary implementations were evaluated: linear search and binary search.

Neither implementation requires specialized algorithms or data structures. Both can be implemented using only the standard C library.

Both implementations are based on the same data structure: a dynamic array of key-value pairs.

```c
struct dict_entry {
    const char* key;
    int value;
};

struct dict {
    struct dict_entry* entries;
    int size;
    int reserved_size;
};
```

The two implementations differ only in the algorithms used for insertion and lookup.

### Linear Search Implementation

#### Lookup Operation

The lookup operation performs a sequential scan of the array, comparing the requested key with each stored key until either a match is found or all entries have been examined.

```c
struct dict_entry* dict_find(struct dict* dict, const char* key)
{
    for (int i = 0; i < dict->size; i++) {
        if (!strcmp(key, dict->entries[i].key)) {
            return &dict->entries[i];
        }
    }

    return NULL;
}
```

The lookup time grows linearly with the number of dictionary entries, resulting in a time complexity of **O(N)**.

The primary advantage of this implementation is its simplicity. No ordering of the stored elements is required.

#### Insertion Operation

Insertion is straightforward. The only additional requirement is to ensure that sufficient memory has been allocated for new entries.

```c
struct dict_entry* dict_put(struct dict* dict, const char* key, int value)
{
    struct dict_entry* entry = dict_find(dict, key);
    if (entry) {
        /* Entry with a given key already exists, update it's value. */
        entry->value = value;
        return entry;
    }

    /* Check if there is enough reserved size and if not -- allocate memory for new items. */
    if (dict->size >= dict->reserved_size) {
        dict->reserved_size *= 2;
        dict->entries = realloc(
            dict->entries,
            dict->reserved_size * sizeof(struct dict_entry));
    }

    entry = &dict->entries[dict->size];
    entry->key = strdup(key);

    entry->value = value;

    dict->size++;

    return entry;
}
```

### Binary Search Implementation

#### Insertion Operation

A binary-search-based dictionary requires the array to remain sorted by key.

For immutable dictionaries, sorting can be performed once after all entries have been inserted. For mutable dictionaries, each insertion must preserve the ordering of the array.

The simplest implementation is to reuse the insertion procedure from the linear-search implementation and sort the array after each insertion.

Sorting is performed using the standard C library function `qsort()` declared in `<stdlib.h>`.

```c
struct dict_entry* dict_put(struct dict* dict, const char* key, int value)
{
    struct dict_entry* entry = dict_find(dict, key);
    if (entry) {
        /* Entry with a given key already exists, update it's value. */
        entry->value = value;
        return entry;
    }

    /* Check if there is enough reserved size and if not -- allocate memory for new items. */
    if (dict->size >= dict->reserved_size) {
        dict->reserved_size *= 2;
        dict->entries = realloc(
            dict->entries,
            dict->reserved_size * sizeof(struct dict_entry));
    }

    entry = &dict->entries[dict->size];
    entry->key = strdup(key);

    entry->value = value;

    dict->size++;
    
    
    qsort(
        dict->entries, 
        dict->size, 
        sizeof(struct dict_entry), 
        dict_compare_entries);

    return dict_find(dict, key);
}
```

The comparison function simply compares the string keys using the standard library function `strcmp()`.

```c
static int dict_compare_entries(
    const struct dict_entry* left, 
    const struct dict_entry* right)
{
    return strcmp(left->key, right->key);
}
```

#### Lookup Operation

Lookup is performed using the standard C library function `bsearch()`, also declared in `<stdlib.h>`.

```c
struct dict_entry* dict_find(struct dict* dict, const char* key)
{
    if (!dict) {
        return NULL;
    }

    struct qdict_entry key_entry = { .key = key };
    return bsearch(
        &key_entry, 
        dict->entries, 
        dict->size, 
        sizeof(struct qdict_entry), 
        dict_compare_entries);
}
```

The same comparison function is used by both `qsort()` and `bsearch()`.

This implementation provides a lookup time complexity of **O(log N)**.

The main drawback of this approach is the requirement to keep the array sorted.

In the simplest implementation, the array is re-sorted after each insertion.

Since the primary objective of this work was to evaluate lookup performance, insertion performance was not investigated separately.

#### Notes on the Insertion Operation

Although this work focuses exclusively on lookup performance, several approaches to implementing insertion into a sorted-array dictionary are possible.

The simplest approach, used in the experimental implementation, is to re-sort the array after each insertion. This solution requires only a small amount of additional code and relies exclusively on facilities provided by the standard C library.

For immutable dictionaries, sorting can be performed once after the dictionary has been fully populated. In this case, insertion is linear, while subsequent lookups benefit from binary search.

Another approach is to keep the array sorted after every insertion. This can be achieved using a modified binary search that returns the insertion position when the requested key is not found. After determining the insertion position, the remaining elements are shifted by one position. This approach provides logarithmic lookup time and linear insertion time without requiring the entire array to be re-sorted.

A third alternative is deferred sorting. In this approach, the dictionary is marked as requiring sorting after each insertion, and sorting is postponed until the next lookup or iteration over the dictionary.

A comparison of these approaches, as well as a performance evaluation of insertion operations, is outside the scope of this work.
