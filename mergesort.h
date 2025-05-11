#ifndef MERGESORT_H
#define MERGESORT_H

#include <string>
#include <filesystem>
#include "iostats.h"  // Para la estructura IOStats

namespace fs = std::filesystem;

// Función principal del mergesort externo
void externalMergeSort(const std::string& inputFilename, const std::string& outputFilename, 
                      size_t arity, size_t memoryLimit, IOStats& stats);

#endif