// io_utils.c
#include "io_utils.h"

void read_block(FILE* file, elem_t* buffer, size_t block_size, size_t block_index) {
    fseek(file, block_index * block_size * sizeof(elem_t), SEEK_SET);
    fread(buffer, sizeof(elem_t), block_size, file);
}

void write_block(FILE* file, elem_t* buffer, size_t block_size, size_t block_index) {
    fseek(file, block_index * block_size * sizeof(elem_t), SEEK_SET);
    fwrite(buffer, sizeof(elem_t), block_size, file);
}

void copy_file(const char* src_path, const char* dest_path) {
    FILE* src = fopen(src_path, "rb");
    FILE* dest = fopen(dest_path, "wb");
    elem_t buffer[1024];
    size_t read;

    while ((read = fread(buffer, sizeof(elem_t), 1024, src)) > 0) {
        fwrite(buffer, sizeof(elem_t), read, dest);
    }

    fclose(src);
    fclose(dest);
}
