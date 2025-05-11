#ifndef EXPERIMENT_H
#define EXPERIMENT_H

#include "mergesort.h"
#include "quicksort.h"
#include "iostats.h"
#include "constants.h"
#include <map>
#include <iostream>

namespace fs = std::filesystem;

/**
 * @brief Ejecuta experimentos comparativos entre MergeSort y QuickSort en memoria externa.
 * 
 * @param optimalArity La aridad óptima (número de vías para MergeSort o subarreglos para QuickSort) calculada previamente.
 * 
 * Esta función genera datos aleatorios para diferentes tamaños de entrada, ejecuta ambos algoritmos,
 * mide sus tiempos y operaciones de I/O, y guarda los resultados promediados en un archivo CSV.
 */
void runExperiments(size_t optimalArity);

/**
 * @brief Función principal del programa.
 * 
 * @return int Código de salida (0 si éxito, otro valor si error).
 * 
 * Configura los parámetros iniciales (tamaño de bloque, memoria disponible),
 * calcula la aridad óptima para los algoritmos y ejecuta los experimentos.
 */
int experimentalMain();

#endif // EXPERIMENT_H