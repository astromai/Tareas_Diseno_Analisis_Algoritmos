#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include "io_utils.h"

void generate_input(const char* filename, size_t n) {
    FILE* f = fopen(filename, "wb");
    srand(time(NULL));
    for (size_t i = 0; i < n; i++) {
        int64_t val = ((int64_t)rand() << 32) | rand();
        fwrite(&val, sizeof(int64_t), 1, f);
    }
    fclose(f);
}

bool is_sorted(const char* filename) {
    FILE* f = fopen(filename, "rb");
    int64_t prev, curr;
    if (fread(&prev, sizeof(int64_t), 1, f) != 1) {
        fclose(f);
        return true;
    }

    while (fread(&curr, sizeof(int64_t), 1, f) == 1) {
        if (curr < prev) {
            fclose(f);
            return false;
        }
        prev = curr;
    }

    fclose(f);
    return true;
}

int compare_int64(const void* a, const void* b) {
    int64_t val_a = *(const int64_t*)a;
    int64_t val_b = *(const int64_t*)b;
    return (val_a > val_b) - (val_a < val_b);
}

void k_way_merge(char** files, int k, const char* output_file, size_t B, size_t M) {
    FILE** inputs = malloc(k * sizeof(FILE*));
    for (int i = 0; i < k; i++) {
        inputs[i] = fopen(files[i], "rb");
    }

    int64_t* buffers = malloc(k * sizeof(int64_t));
    bool* has_data = malloc(k * sizeof(bool));
    for (int i = 0; i < k; i++) {
        has_data[i] = fread(&buffers[i], sizeof(int64_t), 1, inputs[i]) == 1;
    }

    FILE* out = fopen(output_file, "wb");

    while (1) {
        int min_index = -1;
        for (int i = 0; i < k; i++) {
            if (has_data[i]) {
                if (min_index == -1 || buffers[i] < buffers[min_index]) {
                    min_index = i;
                }
            }
        }
        if (min_index == -1) break;

        fwrite(&buffers[min_index], sizeof(int64_t), 1, out);
        has_data[min_index] = fread(&buffers[min_index], sizeof(int64_t), 1, inputs[min_index]) == 1;
    }

    for (int i = 0; i < k; i++) {
        fclose(inputs[i]);
    }
    free(inputs);
    free(buffers);
    free(has_data);
}
