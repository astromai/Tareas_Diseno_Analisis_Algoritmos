#include "iostats.h"

/**
 * @brief Calcula el total de operaciones de I/O realizadas (lecturas + escrituras).
 * 
 * @return size_t Número total de operaciones de I/O contabilizadas.
 */
size_t IOStats::total() const {
    return reads + writes;
}

/**
 * @brief Reinicia los contadores de operaciones de I/O a cero.
 */
void IOStats::reset() {
    reads = 0;
    writes = 0;
}

/**
 * @brief Lee un bloque de elementos de un archivo y actualiza las estadísticas de I/O.
 * 
 * @tparam T Tipo de elementos.
 * @param file Archivo de entrada abierto en modo binario.
 * @param buffer Vector donde se almacenarán los datos leídos.
 * @param count Número máximo de elementos a leer.
 * @param stats Objeto IOStats para registrar las operaciones de I/O.
 * @return size_t Número de elementos leídos efectivamente.
 * 
 * @note El tamaño de bloque (B) se utiliza para calcular las operaciones de I/O en bloques completos.
 */
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

/**
 * @brief Escribe un bloque de datos en un archivo y actualiza las estadísticas de I/O.
 * 
 * @tparam T Tipo de elementos a escribir.
 * @param file Archivo de salida abierto en modo binario.
 * @param buffer Vector con los elementos a escribir.
 * @param stats Objeto IOStats para registrar las operaciones de I/O.
 * 
 * @note Si el buffer está vacío, no se realiza ninguna operación.
 * @note El tamaño de bloque (B) se utiliza para calcular las operaciones de I/O en bloques completos.
 */
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