#ifndef ARITY_H
#define ARITY_H

#include <string>
#include "iostats.h"
#include "constants.h"

/**
 * @brief Genera un archivo con datos aleatorios para pruebas
 * 
 * @param filename Nombre del archivo a generar
 * @param size Cantidad de números a generar
 * 
 * @note Usa números aleatorios de 64 bits con distribución uniforme
 * @note El archivo se crea en formato binario
 */
void generateData(const std::string& filename, int64_t size = M);

/**
 * @brief Verifica si un archivo está correctamente ordenado
 * 
 * @param filename Archivo a verificar
 * @param stats Objeto para registrar estadísticas de I/O
 * @return true si el archivo está ordenado ascendentemente
 * @return false si encuentra algún elemento fuera de orden
 * 
 * @note Reporta la primera posición donde encuentra un error
 */
bool verifySort(const std::string& filename, IOStats& stats);

/**
 * @brief Encuentra la aridad óptima para algoritmos de ordenamiento externo
 * 
 * @param inputFilename Archivo de entrada para pruebas
 * @param outputFilename Archivo de salida ordenado
 * @param minArity Valor mínimo de aridad a evaluar
 * @param maxArity Valor máximo de aridad a evaluar
 * @param memoryLimit Límite de memoria disponible en bytes
 * @return size_t Aridad óptima encontrada
 * 
 * @note Realiza una búsqueda empírica probando diferentes valores
 * @warning Requiere que el archivo de entrada exista
 */
size_t findOptimalArity(const std::string& inputFilename, const std::string& outputFilename,
                       size_t minArity, size_t maxArity, size_t memoryLimit);

#endif