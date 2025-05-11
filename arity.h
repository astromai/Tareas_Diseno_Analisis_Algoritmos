#ifndef ARITY_H
#define ARITY_H

#include <string>
#include <cstdint>
#include "iostats.h"

// Constantes globales
extern const int64_t M;
extern const size_t NUM_SIZE;
extern const size_t B;
extern const size_t b;

// Estructuras
struct HeapNode {
    int64_t value;
    size_t fileIndex;
    bool operator>(const HeapNode& other) const;
};

// Declaraciones de funciones
void generateData(const std::string& filename, int64_t size = M);
bool verifySort(const std::string& filename, IOStats& stats);
size_t findOptimalArity(const std::string& inputFilename, const std::string& outputFilename,
                       size_t minArity, size_t maxArity, size_t memoryLimit);

// Plantillas (declaraciones)
template<typename T>
size_t readBlock(std::ifstream& file, std::vector<T>& buffer, size_t count, IOStats& stats);

template<typename T>
void writeBlock(std::ofstream& file, const std::vector<T>& buffer, IOStats& stats);

#endif