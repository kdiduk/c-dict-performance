# Benchmark of Simple Dictionary Implementations in C

A small engineering experiment exploring whether it is possible to implement a simple dictionary in plain C using only the standard library while achieving acceptable performance for practical use.

## Motivation

The project started with a simple question:

> Can an ordinary C programmer implement a dictionary in an hour or two, without using hash tables, balanced trees or third-party libraries, and still obtain acceptable performance for practical use?

The goal of this project is not to develop the fastest dictionary implementation, but to investigate whether a very simple implementation can be practical.

## Repository contents

 * [`src/`](src/) -- source code of dictionary implementations and benchmarks.
 * [`doc/`](doc/) -- technical reports
   - [`doc/report_en.md`](doc/report_en.md) -- English version.
   - [`doc/report_ru.md`](doc/report_ru.md) -- Russian version.
 * [`results/`](results/) -- benchmark results.
