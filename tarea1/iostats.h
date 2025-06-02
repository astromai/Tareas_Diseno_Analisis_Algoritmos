#ifndef IOSTATS_H
#define IOSTATS_H

#include <cstddef>
#include <vector>
#include <fstream>
#include "constants.h"

/**
 * @brief Estructura para registrar estadísticas de operaciones de E/S en memoria externa.
 * 
 * Mantiene un conteo de las operaciones de lectura y escritura realizadas
 * durante algoritmos de memoria externa, medido en bloques (usando la constante B).
 */
struct IOStats {
    size_t reads = 0;
    size_t writes = 0;
    
    /**
     * @brief Obtiene el total de operaciones de E/S realizadas.
     * @return Suma de lecturas y escrituras.
     */
    size_t total() const;
    /**
     * @brief Reinicia los contadores de lecturas y escrituras a cero.
     */
    void reset();
};

/**
 * @brief Lee un bloque de elementos de un archivo y actualiza las estadísticas de I/O.
 * 
 * @tparam T Tipo de elementos.
 * @param file Archivo de entrada en modo binario.
 * @param buffer Vector donde se almacenarán los elementos leídos.
 * @param count Número máximo de elementos a leer.
 * @param stats Objeto IOStats para registrar las operaciones de I/O.
 * @return size_t Número de elementos leídos efectivamente.
 * 
 * @note El tamaño de bloque (B) se utiliza para calcular las operaciones de I/O en bloques completos.
 */
template<typename T>
size_t readBlock(std::ifstream& file, std::vector<T>& buffer, size_t count, IOStats& stats);

/**
 * @brief Escribe un bloque de datos en un archivo y actualiza las estadísticas de I/O.
 * 
 * @tparam T Tipo de elementos a escribir.
 * @param file Archivo de salida en modo binario.
 * @param buffer Vector con los elementos a escribir.
 * @param stats Objeto IOStats para registrar las operaciones.
 * 
 * @note Si el buffer está vacío, no se realiza ninguna operación.
 * @note El tamaño de bloque (B) se utiliza para calcular las operaciones de I/O en bloques completos.
 */
template<typename T>
void writeBlock(std::ofstream& file, const std::vector<T>& buffer, IOStats& stats);

#endif