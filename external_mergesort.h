// external_mergesort.h
#ifndef EXTERNAL_MERGESORT_H
#define EXTERNAL_MERGESORT_H
#include <stddef.h>
#include <stdint.h>  // para uint64_t
#include <stdio.h>   // para FILE, fread, fwrite
#include <stdlib.h>  // para malloc, free, qsort
#include <string.h>  // para memcpy si usas

void external_mergesort(const char* input_file, const char* output_file, size_t N, size_t B, size_t M, int arity);

#endif
