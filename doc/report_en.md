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
