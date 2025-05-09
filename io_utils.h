// io_utils.h
#ifndef IO_UTILS_H
#define IO_UTILS_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

typedef uint64_t elem_t;

void read_block(FILE* file, elem_t* buffer, size_t block_size, size_t block_index);
void write_block(FILE* file, elem_t* buffer, size_t block_size, size_t block_index);
void copy_file(const char* src_path, const char* dest_path);

#endif
