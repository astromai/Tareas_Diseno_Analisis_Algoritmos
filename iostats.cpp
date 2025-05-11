#include "iostats.h"

// Implementación de métodos de IOStats
size_t IOStats::total() const {
    return reads + writes;
}

void IOStats::reset() {
    reads = 0;
    writes = 0;
}

// Implementación de plantillas
template<typename T>
size_t readBlock(std::ifstream& file, std::vector<T>& buffer, size_t count, IOStats& stats) {
    std::streampos posBefore = file.tellg();
    buffer.resize(count);
    file.read(reinterpret_cast<char*>(buffer.data()), count * sizeof(T));
    size_t itemsRead = file.gcount() / sizeof(T);
    buffer.resize(itemsRead);
    
    if (itemsRead > 0) {
        size_t bytesRead = itemsRead * sizeof(T);
        size_t blocksRead = (bytesRead + B - 1) / B;  // Usa la constante B
        stats.reads += blocksRead;
    }
    
    return itemsRead;
}

template<typename T>
void writeBlock(std::ofstream& file, const std::vector<T>& buffer, IOStats& stats) {
    if (buffer.empty()) return;
    
    std::streampos posBefore = file.tellp();
    file.write(reinterpret_cast<const char*>(buffer.data()), buffer.size() * sizeof(T));
    
    size_t bytesWritten = buffer.size() * sizeof(T);
    size_t blocksWritten = (bytesWritten + B - 1) / B;  // Usa la constante B
    stats.writes += blocksWritten;
}

// Instanciaciones explícitas
template size_t readBlock<int64_t>(std::ifstream&, std::vector<int64_t>&, size_t, IOStats&);
template void writeBlock<int64_t>(std::ofstream&, const std::vector<int64_t>&, IOStats&);