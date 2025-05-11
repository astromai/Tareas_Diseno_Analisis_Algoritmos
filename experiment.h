#ifndef EXPERIMENT_H
#define EXPERIMENT_H

#include "arity.h"
#include "mergesort.h"
#include "quicksort.h"
#include "iostats.h"
#include "constants.h"
#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

/**
 * @brief Ejecuta experimentos comparativos entre MergeSort y QuickSort externos
 * @param optimalArity Aridad óptima encontrada para los algoritmos
 */
void runExperiments(size_t optimalArity);

/**
 * @brief Función principal del programa de experimentación
 * @note Esta versión incluye tanto la búsqueda de aridad óptima como los experimentos comparativos
 */
int experimentalMain();

#endif // EXPERIMENT_H