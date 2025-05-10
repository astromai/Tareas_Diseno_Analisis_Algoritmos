#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "io_utils.h"
#include "mergesort.h"
#include "quicksort.h"

int main(int argc, char* argv[]) {
    // Parámetros ajustables (modificar solo N para probar distintos tamaños)
    size_t N = 16000000; // 4M elementos (8 bytes c/u)
    size_t B = 4096 / sizeof(int64_t);           // Bloque de 4096 bytes
    size_t M = 50 * 1024 * 1024;                 // Memoria principal: 50MB
    int a = 16;                                  // Aridad para Mergesort

    for (int i = 0; i < 5; i++) {
        char input_file[32], merge_output[32], quick_output[32];
        snprintf(input_file, sizeof(input_file), "input_%d.bin", i);
        snprintf(merge_output, sizeof(merge_output), "merge_sorted_%d.bin", i);
        snprintf(quick_output, sizeof(quick_output), "quick_sorted_%d.bin", i);

        printf("Generando arreglo aleatorio %d de %zu elementos...\n", i + 1, N);
        generate_input(input_file, N);

        printf("Ejecutando Mergesort externo...\n");
        clock_t start = clock();
        external_mergesort(input_file, merge_output, N, B, M, a);
        clock_t end = clock();
        printf("Mergesort OK. Tiempo: %.2fs\n", (double)(end - start) / CLOCKS_PER_SEC);

        printf("Ejecutando Quicksort externo...\n");
        start = clock();
        external_quicksort(input_file, quick_output, N, B, M, a);
        end = clock();
        printf("Quicksort OK. Tiempo: %.2fs\n", (double)(end - start) / CLOCKS_PER_SEC);

        printf("Verificando resultados...\n");
        printf("Mergesort ordenado: %s\n", is_sorted(merge_output) ? "Sí" : "NO");
        printf("Quicksort ordenado: %s\n", is_sorted(quick_output) ? "Sí" : "NO");
    }

    return 0;
}
