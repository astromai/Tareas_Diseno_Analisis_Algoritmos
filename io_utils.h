#ifndef IO_UTILS_H
#define IO_UTILS_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

void generate_input(const char* filename, size_t n);
bool is_sorted(const char* filename);
int compare_int64(const void* a, const void* b);
void k_way_merge(char** files, int k, const char* output_file, size_t B, size_t M);

#endif
