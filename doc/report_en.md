# Performance Evaluation of Simple Dictionary Implementations in C

## Introduction

***What motivated this work?***

The C programming language does not provide a standard implementation of associative arrays (hereafter referred to as *dictionaries*).

Several third-party implementations based on hash tables are available (for example, `ut_hash` and `khash`), but none of them has become a widely accepted standard.

This led to the following question: Is it possible to implement a simple and reliable dictionary using only the standard C library while achieving performance sufficient for practical use?

