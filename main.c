#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

#include "external_mergesort.h"
#include "external_quicksort.h"

typedef uint64_t elem_t;

// Genera un archivo binario de N elementos aleatorios
void generate_input(const char* filename, size_t N) {
    FILE* f = fopen(filename, "wb");
    if (!f) {
        perror("fopen");
        exit(1);
    }

    srand((unsigned)time(NULL));
    for (size_t i = 0; i < N; ++i) {
        elem_t x = ((elem_t)rand() << 32) | rand(); // 64-bit aleatorio
        fwrite(&x, sizeof(elem_t), 1, f);
    }

    fclose(f);
}

// Verifica si un archivo binario está ordenado
int is_sorted(const char* filename) {
    FILE* f = fopen(filename, "rb");
    if (!f) return 0;

    elem_t prev, curr;
    if (fread(&prev, sizeof(elem_t), 1, f) != 1) return 1;

    while (fread(&curr, sizeof(elem_t), 1, f) == 1) {
        if (prev > curr) {
            fclose(f);
            return 0;
        }
        prev = curr;
    }

    fclose(f);
    return 1;
}

int main(int argc, char* argv[]) {
    // Parámetros ajustables
    size_t N = 1000000*8;        // Cantidad de elementos (p.ej. 1 millón)
    size_t B = 4096 / 8;       // Tamaño de bloque en elementos (4096 bytes / 8 bytes)
    size_t M = 50 * 1024 * 1024; // Memoria principal (50 MB)
    int a = 16;                // Aridad

    printf("Generando arreglo aleatorio de %zu elementos...\n", N);
    generate_input("input.bin", N);

    printf("Ejecutando Mergesort externo...\n");
    clock_t start = clock();
    external_mergesort("input.bin", "merge_sorted.bin", N, B, M, a);
    clock_t end = clock();
    printf("Mergesort OK. Tiempo: %.2fs\n", (double)(end - start) / CLOCKS_PER_SEC);

    printf("Ejecutando Quicksort externo...\n");
    start = clock();
    external_quicksort("input.bin", "quick_sorted.bin", N, B, M, a);
    end = clock();
    printf("Quicksort OK. Tiempo: %.2fs\n", (double)(end - start) / CLOCKS_PER_SEC);

    printf("Verificando resultados...\n");
    printf("Mergesort ordenado: %s\n", is_sorted("merge_sorted.bin") ? "Sí" : "NO");
    printf("Quicksort ordenado: %s\n", is_sorted("quick_sorted.bin") ? "Sí" : "NO");

    return 0;
}
