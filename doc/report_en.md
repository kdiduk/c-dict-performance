# Performance Evaluation of Simple Dictionary Implementations in C

## Introduction

***What motivated this work?***

The C programming language does not provide a standard implementation of associative arrays (hereafter referred to as *dictionaries*).

Several third-party implementations based on hash tables are available (for example, `ut_hash` and `khash`), but none of them has become a widely accepted standard.

This led to the following question: Is it possible to implement a simple and reliable dictionary using only the standard C library while achieving performance sufficient for practical use?

## Objective

***What was the objective of this work?***

The objective of this work was to investigate whether a simple dictionary implementation can be developed in the C programming language using only the standard library while remaining suitable for practical use and providing acceptable performance.

