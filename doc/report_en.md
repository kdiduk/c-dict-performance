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

## Methodology

***How was the evaluation performed?***

The performance of successful key lookup operations was evaluated.

Randomly generated alphanumeric string keys of equal length were used throughout the experiments.

The following parameters were used:

* `N` — dictionary size (number of entries with unique keys);
* `L` — key length (number of characters);
* `Q` — number of lookup queries;
* `R` — number of repetitions of the same measurement (sample size).

A fixed value of `Q = 100000` was used in all experiments.

For fixed key lengths of `L = 8` and `L = 12`, lookup performance was measured for the following dictionary sizes:

`N = 10, 20, 50, 100, 200, 500, 1000, 10000`.

Each measurement was repeated `R = 21` times, and the median value was used as the reported result.

All measurements were performed on a system with the following configuration:

* Processor: Intel Core i9-14900HX
* Memory: 32 GiB
* Operating system: Microsoft Windows 11

All benchmarks were performed using a Release build.

## Results

The benchmark results are presented in Tables 1 and 2.

For the linear-search implementation, lookup time increases significantly as the dictionary size grows. The binary-search implementation exhibits a much slower increase in lookup time and achieves performance close to that of `std::map`. Among all evaluated implementations, `std::unordered_map` consistently provides the lowest lookup time.

### Table 1. Lookup performance for randomly generated alphanumeric keys of length 8

| Dictionary Size (N) | Linear Search, μs | Binary Search, μs | std::map, μs | std::unordered_map, μs |
| ------------------: | ----------------: | ----------------: | -----------: | ---------------------: |
|                  10 |              2033 |              2379 |         2664 |                    771 |
|                  20 |              2878 |              2564 |         3334 |                   1074 |
|                  50 |              4911 |              3203 |         3741 |                   1374 |
|                 100 |              7877 |              3783 |         4623 |                    673 |
|                 200 |             14808 |              4437 |         6157 |                    941 |
|                 500 |             39514 |              5986 |         7567 |                   1692 |
|                1000 |             77525 |              7090 |         8484 |                   1910 |
|               10000 |            766481 |             12208 |        12506 |                   1772 |

### Table 2. Lookup performance for randomly generated alphanumeric keys of length 12

| Dictionary Size (N) | Linear Search, μs | Binary Search, μs | std::map, μs | std::unordered_map, μs |
| ------------------: | ----------------: | ----------------: | -----------: | ---------------------: |
|                  10 |              1931 |              2170 |         2973 |                   1195 |
|                  20 |              3009 |              3180 |         3938 |                   1091 |
|                  50 |              4849 |              3339 |         4686 |                   1654 |
|                 100 |              9020 |              4096 |         5277 |                    963 |
|                 200 |             16664 |              4621 |         5521 |                   1189 |
|                 500 |             32407 |              5101 |         6628 |                   1753 |
|                1000 |             64217 |              5959 |         7389 |                   1795 |
|               10000 |            643265 |             10303 |        10821 |                   1603 |
|               50000 |           3956211 |             19127 |        19809 |                   3108 |

For keys of length 12, an additional benchmark was performed using a dictionary containing 50,000 entries.

## Conclusions

The linear-search implementation demonstrated relatively poor performance. Lookup time was approximately two times slower than `std::map` for a dictionary containing 100 entries and nearly ten times slower for 1,000 entries.

These results contradict the initial expectation that a linear-search implementation would provide acceptable performance for dictionaries containing up to approximately 1,000 entries.

The binary-search implementation achieved lookup performance comparable to that of `std::map`.

Furthermore, it proved to be almost as simple to implement as the linear-search version. Both implementations require approximately the same amount of code and rely only on facilities provided by the standard C library, without requiring specialized algorithms or data structures.

These findings contradict the initial expectation that a binary-search-based implementation would be significantly more complex than a linear-search implementation.
