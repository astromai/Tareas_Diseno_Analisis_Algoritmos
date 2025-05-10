#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "quicksort.h"
#include "io_utils.h"

void external_quicksort(const char* input_file, const char* output_file, size_t n, size_t B, size_t M, int a) {
    int64_t* buffer = malloc(n * sizeof(int64_t));
    FILE* f = fopen(input_file, "rb");
    fread(buffer, sizeof(int64_t), n, f);
    fclose(f);

    quicksort(buffer, 0, n - 1);

    f = fopen(output_file, "wb");
    fwrite(buffer, sizeof(int64_t), n, f);
    fclose(f);

    free(buffer);
}

void quicksort(int64_t* arr, size_t low, size_t high) {
    if (low < high) {
        size_t pi = partition(arr, low, high);
        if (pi > 0) quicksort(arr, low, pi - 1);
        quicksort(arr, pi + 1, high);
    }
}

size_t partition(int64_t* arr, size_t low, size_t high) {
    int64_t pivot = arr[high];
    size_t i = low;
    for (size_t j = low; j < high; j++) {
        if (arr[j] < pivot) {
            int64_t tmp = arr[i];
            arr[i] = arr[j];
            arr[j] = tmp;
            i++;
        }
    }
    int64_t tmp = arr[i];
    arr[i] = arr[high];
    arr[high] = tmp;
    return i;
}
