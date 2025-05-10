#ifndef QUICKSORT_H
#define QUICKSORT_H

#include <stddef.h>
#include <stdint.h>

void external_quicksort(const char* input_file, const char* output_file, size_t n, size_t B, size_t M, int a);
void quicksort(int64_t* arr, size_t low, size_t high);
size_t partition(int64_t* arr, size_t low, size_t high);

#endif
