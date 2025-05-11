#ifndef ARITY_H
#define ARITY_H

#include <string>
#include "iostats.h"
#include "constants.h"

void generateData(const std::string& filename, int64_t size = M);
bool verifySort(const std::string& filename, IOStats& stats);
size_t findOptimalArity(const std::string& inputFilename, const std::string& outputFilename,
                       size_t minArity, size_t maxArity, size_t memoryLimit);

#endif