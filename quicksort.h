#ifndef QUICKSORT_H
#define QUICKSORT_H

#include "iostats.h"
#include "constants.h"
#include <string>
#include <vector>
#include <filesystem>

namespace fs = std::filesystem;

/**
 * @brief Particiona un archivo en múltiples archivos según pivotes dados
 * @param inputFilename Archivo de entrada a particionar
 * @param outputFilenames Vector con nombres de archivos de salida para cada partición
 * @param pivotes Vector con los valores pivote para la partición
 * @param memoryLimit Límite de memoria disponible en bytes
 * @param stats Estructura para registrar estadísticas de I/O
 */
void partition(const std::string& inputFilename, 
               const std::vector<std::string>& outputFilenames,
               const std::vector<int64_t>& pivots, 
               size_t memoryLimit,
               IOStats& stats);

/**
 * @brief Genera un archivo con números aleatorios
 * @param filename Nombre del archivo a generar
 * @param size Cantidad de números a generar
 */
void generateData(const std::string& filename, int64_t size);

/**
 * @brief Verifica si un archivo está ordenado
 * @param filename Archivo a verificar
 * @param stats Estructura para estadísticas de I/O
 * @return true si el archivo está ordenado, false en caso contrario
 */
bool verifySort(const std::string& filename, IOStats& stats);

/**
 * @brief Selecciona pivotes para el Quicksort externo
 * @param filename Archivo del cual seleccionar los pivotes
 * @param numPivots Número de pivotes a seleccionar
 * @param stats Estructura para registrar estadísticas de I/O
 * @return Vector con los pivotes seleccionados
 */
std::vector<int64_t> selectPivots(const std::string& filename, 
                                 size_t numPivots, 
                                 IOStats& stats);

/**
 * @brief Implementación recursiva del Quicksort externo
 * @param inputFilename Archivo de entrada a ordenar
 * @param outputFilename Archivo de salida ordenado
 * @param arity Número de particiones a crear en cada paso
 * @param memoryLimit Límite de memoria disponible en bytes
 * @param tempDir Directorio temporal para archivos intermedios
 * @param stats Estructura para registrar estadísticas de I/O
 */
void quicksortRecursive(const std::string& inputFilename, 
                       const std::string& outputFilename, 
                       size_t arity,
                       size_t memoryLimit, 
                       const std::string& tempDir,
                       IOStats& stats);

/**
 * @brief Función principal para el Quicksort externo
 * @param inputFilename Archivo de entrada a ordenar
 * @param outputFilename Archivo de salida ordenado
 * @param arity Número de particiones a crear en cada paso
 * @param memoryLimit Límite de memoria disponible en bytes
 * @param stats Estructura para registrar estadísticas de I/O
 */
void externalQuickSort(const std::string& inputFilename, 
                     const std::string& outputFilename, 
                     size_t arity,
                     size_t memoryLimit,
                     IOStats& stats);             
#endif // QUICKSORT_H