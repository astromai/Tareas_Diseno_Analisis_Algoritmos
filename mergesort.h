#ifndef MERGESORT_H
#define MERGESORT_H

#include "iostats.h"

/**
 * @brief Ordena un archivo grande usando el algoritmo de MergeSort externo
 * 
 * @param inputFilename Archivo de entrada a ordenar
 * @param outputFilename Archivo de salida ordenado
 * @param arity Número de archivos a mezclar simultáneamente
 * @param memoryLimit Límite de memoria en bytes
 * @param stats Objeto para registrar estadísticas de operaciones de I/O
 * 
 * @note Opera en dos fases principales:
 *   1. División: Divide el archivo en chunks ordenados
 *   2. Mezcla: Mezcla recursiva los chunks usando una cola de prioridad
 * 
 * @warning Crea archivos temporales en el directorio ./temp_[arity]
 */

void externalMergeSort(const std::string& inputFilename, const std::string& outputFilename, 
                     size_t arity, size_t memoryLimit, IOStats& stats);

#endif