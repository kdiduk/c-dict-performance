# Simple Dictionary Implementations in C

A small engineering experiment exploring whether it is possible to implement a simple dictionary in plain C using only the standard library while achieving acceptable performance for practical use.

## Motivation

The project started with a simple question:

> Can an ordinary C programmer implement a dictionary in an hour or two, without using hash tables, balanced trees or third-party libraries, and still obtain acceptable performance for practical use?

The goal of this project is not to develop the fastest dictionary implementation, but to investigate whether a very simple implementation can be practical.

The benchmark results presented here are intended to answer this specific engineering question rather than to provide a comprehensive comparison of dictionary implementations.

## Scope

This study focused exclusively on the performance of **successful key lookup** operations.

Insertion performance was intentionally left outside the scope of this work, as the primary objective was to investigate whether a simple dictionary implementation could provide practical lookup performance.

Future work may include a comparison of different insertion strategies for sorted-array dictionaries, as well as an evaluation of their impact on overall performance.

## Implementations

The following dictionary implementations were evaluated:

 * Array of structs `(key, value)` with linear search.
 * Same array of structs as above, but sorted using `qsort()` and with binary search using `bsearch()`.
 * `std::map` from C++ (reference implementation).
 * `std::unordered_map` from C++ (reference implementation).

## Main observations

 * Linear search becomes significantly slower as the dictionary grows:
   - starting from 100 items it's more than **twice as slow** as the other implementations;
   - starting from 1000 items it's more than **10x as slow** as the other implementations.
 * A sorted array combined with `bsearch()` achieves **performance surprisingly close to `std::map`** for all inputs, and is often slightly faster in these benchmarks.
 * The sorted-array implementation with `qsort()` and `bsearch()` is **as simple to implement as linear search** (if not even simpler).

## Repository contents

 * [`src/`](src/) -- source code of dictionary implementations and benchmarks.
 * [`doc/`](doc/) -- technical reports:
   - [`doc/report_en.md`](doc/report_en.md) -- English version.
   - [`doc/report_ru.md`](doc/report_ru.md) -- Russian version.
 * [`results/`](results/) -- benchmark results.

## Building

```bash
mkdir build
cmake -B build
cmake --build build
```

## License

This project is released under the MIT License.
