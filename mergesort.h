#ifndef MERGESORT_H
#define MERGESORT_H

#include "iostats.h"

void externalMergeSort(const std::string& inputFilename, const std::string& outputFilename, 
                     size_t arity, size_t memoryLimit, IOStats& stats);

#endif