#ifndef IOSTATS_H
#define IOSTATS_H

#include <cstdint>  // Para int64_t
#include <fstream>
#include <vector>

// Constante B debe estar declarada (añadir esto)
const size_t B = 4096;  // Tamaño de bloque en bytes

struct IOStats {
    size_t reads = 0;
    size_t writes = 0;
    
    size_t total() const { 
        return reads + writes; 
    }
    
    void reset() {
        reads = 0;
        writes = 0;
    }
};

// Declaraciones de las funciones template
template<typename T>
size_t readBlock(std::ifstream& file, std::vector<T>& buffer, size_t count, IOStats& stats);

template<typename T>
void writeBlock(std::ofstream& file, const std::vector<T>& buffer, IOStats& stats);

// Instanciaciones explícitas para int64_t
extern template size_t readBlock<int64_t>(std::ifstream&, std::vector<int64_t>&, size_t, IOStats&);
extern template void writeBlock<int64_t>(std::ofstream&, const std::vector<int64_t>&, IOStats&);

#endif