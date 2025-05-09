// external_quicksort.c
#include "external_quicksort.h"
#include "io_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>

static int cmp_elem(const void* a, const void* b) {
    elem_t ea = *(elem_t*)a;
    elem_t eb = *(elem_t*)b;
    return (ea > eb) - (ea < eb);
}

static void partition_file(const char* input_file, char** partition_files, elem_t* pivots, int arity, size_t B, size_t N);
static void concat_files(char** files, int count, const char* output_file);

void external_quicksort(const char* input_file, const char* output_file, size_t N, size_t B, size_t M, int arity) {
    if (N * sizeof(elem_t) <= M) {
        FILE* f = fopen(input_file, "rb");
        elem_t* arr = malloc(N * sizeof(elem_t));
        fread(arr, sizeof(elem_t), N, f);
        fclose(f);

        qsort(arr, N, sizeof(elem_t), cmp_elem);

        FILE* out = fopen(output_file, "wb");
        fwrite(arr, sizeof(elem_t), N, out);
        fclose(out);

        free(arr);
        return;
    }

    // Elegir a-1 pivotes desde un bloque aleatorio
    FILE* f = fopen(input_file, "rb");
    fseek(f, rand() % (N / B) * B * sizeof(elem_t), SEEK_SET);
    elem_t* sample = malloc(B * sizeof(elem_t));
    fread(sample, sizeof(elem_t), B, f);
    fclose(f);

    qsort(sample, B, sizeof(elem_t), cmp_elem);

    elem_t* pivots = malloc((arity - 1) * sizeof(elem_t));
    for (int i = 1; i < arity; ++i)
        pivots[i - 1] = sample[i * B / arity];

    free(sample);

    // Particionar el archivo
    char** part_files = malloc(arity * sizeof(char*));
    for (int i = 0; i < arity; ++i) {
        part_files[i] = malloc(64);
        sprintf(part_files[i], "part_%d.bin", i);
    }

    partition_file(input_file, part_files, pivots, arity, B, N);

    // Ordenar recursivamente cada partición
    for (int i = 0; i < arity; ++i) {
        char* sorted_file = malloc(64);
        sprintf(sorted_file, "sorted_%d.bin", i);
        FILE* in = fopen(part_files[i], "rb");
        fseek(in, 0L, SEEK_END);
        size_t size_bytes = ftell(in);
        fclose(in);

        size_t num_elements = size_bytes / sizeof(elem_t);
        external_quicksort(part_files[i], sorted_file, num_elements, B, M, arity);

        free(part_files[i]);
        part_files[i] = sorted_file;
    }

    // Concatenar particiones ordenadas
    concat_files(part_files, arity, output_file);

    for (int i = 0; i < arity; ++i)
        free(part_files[i]);
    free(part_files);
    free(pivots);
}

static void partition_file(const char* input_file, char** part_files, elem_t* pivots, int arity, size_t B, size_t N) {
    FILE* in = fopen(input_file, "rb");
    FILE** outs = malloc(arity * sizeof(FILE*));
    for (int i = 0; i < arity; ++i)
        outs[i] = fopen(part_files[i], "wb");

    elem_t* buffer = malloc(B * sizeof(elem_t));
    size_t read;

    while ((read = fread(buffer, sizeof(elem_t), B, in)) > 0) {
        for (size_t i = 0; i < read; ++i) {
            int j = 0;
            while (j < arity - 1 && buffer[i] > pivots[j]) ++j;
            fwrite(&buffer[i], sizeof(elem_t), 1, outs[j]);
        }
    }

    for (int i = 0; i < arity; ++i)
        fclose(outs[i]);

    fclose(in);
    free(outs);
    free(buffer);
}

static void concat_files(char** files, int count, const char* output_file) {
    FILE* out = fopen(output_file, "wb");
    elem_t buffer[1024];

    for (int i = 0; i < count; ++i) {
        FILE* in = fopen(files[i], "rb");
        size_t r;
        while ((r = fread(buffer, sizeof(elem_t), 1024, in)) > 0) {
            fwrite(buffer, sizeof(elem_t), r, out);
        }
        fclose(in);
    }

    fclose(out);
}
