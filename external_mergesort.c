// external_mergesort.c
#include "external_mergesort.h"
#include "external_quicksort.h"
#include "io_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdint.h>

static int cmp_elem(const void* a, const void* b) {
    elem_t ea = *(elem_t*)a;
    elem_t eb = *(elem_t*)b;
    return (ea > eb) - (ea < eb);
}

static void merge_files(char** temp_files, int num_files, const char* output, size_t B);

void external_mergesort(const char* input_file, const char* output_file, size_t N, size_t B, size_t M, int arity) {
    if (N * sizeof(elem_t) <= M) {
        // Cargar en RAM, ordenar y guardar
        FILE* f = fopen(input_file, "rb");
        elem_t* arr = malloc(N * sizeof(elem_t));
        fread(arr, sizeof(elem_t), N, f);
        fclose(f);

        qsort(arr, N, sizeof(elem_t), cmp_elem); // uso simple

        FILE* out = fopen(output_file, "wb");
        fwrite(arr, sizeof(elem_t), N, out);
        fclose(out);

        free(arr);
        return;
    }

    // Dividir en chunks de tamaño <= M
    size_t chunk_size = M / sizeof(elem_t);
    size_t num_chunks = (N + chunk_size - 1) / chunk_size;

    char** temp_files = malloc(num_chunks * sizeof(char*));
    elem_t* buffer = malloc(chunk_size * sizeof(elem_t));
    FILE* in = fopen(input_file, "rb");

    for (size_t i = 0; i < num_chunks; ++i) {
        size_t count = fread(buffer, sizeof(elem_t), chunk_size, in);
        qsort(buffer, count, sizeof(elem_t), cmp_elem);

        char* temp_filename = malloc(64);
        sprintf(temp_filename, "temp_%zu.bin", i);
        FILE* temp = fopen(temp_filename, "wb");
        fwrite(buffer, sizeof(elem_t), count, temp);
        fclose(temp);
        temp_files[i] = temp_filename;
    }

    fclose(in);
    free(buffer);

    // Merge recursivo en aridad
    merge_files(temp_files, num_chunks, output_file, B);

    for (size_t i = 0; i < num_chunks; ++i)
        free(temp_files[i]);
    free(temp_files);
}

static void merge_files(char** files, int count, const char* output, size_t B) {
    // Solo realiza un merge a 2 vías, pero puede extenderse a "arity"
    if (count == 1) {
        copy_file(files[0], output);
        return;
    }

    int mid = count / 2;
    char* left = "merge_left.bin";
    char* right = "merge_right.bin";

    merge_files(files, mid, left, B);
    merge_files(files + mid, count - mid, right, B);

    // Abrir los dos archivos y mergearlos
    FILE* f1 = fopen(left, "rb");
    FILE* f2 = fopen(right, "rb");
    FILE* out = fopen(output, "wb");

    elem_t* buf1 = malloc(B * sizeof(elem_t));
    elem_t* buf2 = malloc(B * sizeof(elem_t));
    size_t i = 0, j = 0, n1 = fread(buf1, sizeof(elem_t), B, f1);
    size_t n2 = fread(buf2, sizeof(elem_t), B, f2);

    while (n1 > 0 || n2 > 0) {
        while (i < n1 && j < n2) {
            if (buf1[i] <= buf2[j]) {
                fwrite(&buf1[i++], sizeof(elem_t), 1, out);
            } else {
                fwrite(&buf2[j++], sizeof(elem_t), 1, out);
            }
        }
        if (i == n1 && (n1 = fread(buf1, sizeof(elem_t), B, f1)) > 0) i = 0;
        if (j == n2 && (n2 = fread(buf2, sizeof(elem_t), B, f2)) > 0) j = 0;
    }

    free(buf1); free(buf2);
    fclose(f1); fclose(f2); fclose(out);
}
