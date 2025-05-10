#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "mergesort.h"
#include "io_utils.h"

void external_mergesort(const char* input_file, const char* output_file, size_t n, size_t B, size_t M, int a) {
    size_t block_bytes = B * sizeof(int64_t);
    size_t buffer_blocks = M / block_bytes;
    size_t run_size = buffer_blocks * B;

    int num_runs = (n + run_size - 1) / run_size;

    char** run_files = malloc(num_runs * sizeof(char*));

    FILE* input = fopen(input_file, "rb");
    for (int i = 0; i < num_runs; i++) {
        run_files[i] = malloc(32);
        snprintf(run_files[i], 32, "run_%d.bin", i);

        size_t elems_to_read = (i + 1) * run_size > n ? n - i * run_size : run_size;
        int64_t* buffer = malloc(elems_to_read * sizeof(int64_t));
        fread(buffer, sizeof(int64_t), elems_to_read, input);
        qsort(buffer, elems_to_read, sizeof(int64_t), compare_int64);
        FILE* f = fopen(run_files[i], "wb");
        fwrite(buffer, sizeof(int64_t), elems_to_read, f);
        fclose(f);
        free(buffer);
    }
    fclose(input);

    while (num_runs > 1) {
        int new_num_runs = (num_runs + a - 1) / a;
        char** new_run_files = malloc(new_num_runs * sizeof(char*));

        for (int i = 0; i < new_num_runs; i++) {
            new_run_files[i] = malloc(32);
            snprintf(new_run_files[i], 32, "merge_run_%d.bin", i);

            int start = i * a;
            int end = start + a < num_runs ? start + a : num_runs;
            k_way_merge(&run_files[start], end - start, new_run_files[i], B, M);
        }

        for (int i = 0; i < num_runs; i++) free(run_files[i]);
        free(run_files);

        run_files = new_run_files;
        num_runs = new_num_runs;
    }

    rename(run_files[0], output_file);
    free(run_files[0]);
    free(run_files);
}
