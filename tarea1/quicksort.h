#ifndef QUICKSORT_H
#define QUICKSORT_H

#include "iostats.h"
#include "constants.h"
#include <string>
#include <vector>
#include <filesystem>

namespace fs = std::filesystem;

/**
 * @brief Divide un archivo en particiones usando pivotes
 * 
 * @param inputFilename Nombre del archivo de entrada a particionar
 * @param outputFilenames Vector con nombres de archivos para las particiones
 * @param pivotes Vector con los valores pivote para la partición
 * @param memoryLimit Límite de memoria en bytes para procesamiento
 * @param stats Objeto para registrar estadísticas de I/O
 * 
 * @note Crea múltiples archivos de salida, uno por cada partición
 * @note Los elementos menores al primer pivote van a la primera partición, etc.
 */
void partition(const std::string& inputFilename, 
               const std::vector<std::string>& outputFilenames,
               const std::vector<int64_t>& pivots, 
               size_t memoryLimit,
               IOStats& stats);

/**
 * @brief Genera datos aleatorios en un archivo binario
 * 
 * @param filename Nombre del archivo a generar
 * @param size Cantidad de números aleatorios a generar
 * 
 * @note Crea el directorio padre si no existe
 * @note Usa una distribución uniforme para generar los números
 */
void generateData(const std::string& filename, int64_t size);

/**
 * @brief Verifica si un archivo está ordenado
 * 
 * @param filename Archivo a verificar
 * @param stats Objeto para registrar estadísticas de I/O
 * @return true si el archivo está ordenado, false en caso contrario
 * 
 * @note Verifica que cada elemento sea mayor o igual que el anterior
 * @note Reporta la posición donde se encuentra el primer error
 */
bool verifySort(const std::string& filename, IOStats& stats);

/**
 * @brief Genera un archivo con números enteros aleatorios
 * 
 * @param filename Ruta del archivo a generar
 * @param size Cantidad de números aleatorios a generar
 * 
 * @note Crea el directorio padre si no existe
 * @note Usa una distribución uniforme en todo el rango de int64_t
 * @note El archivo resultante es binario con números de 64 bits
 */
std::vector<int64_t> selectPivots(const std::string& filename, 
                                 size_t numPivots, 
                                 IOStats& stats);

/**
 * @brief Implementación recursiva del Quicksort externo
 * 
 * @param inputFilename Archivo de entrada a ordenar
 * @param outputFilename Archivo de salida ordenado
 * @param arity Número de particiones a crear en cada paso
 * @param memoryLimit Límite de memoria para ordenar en RAM
 * @param tempDir Directorio para archivos temporales
 * @param stats Objeto para registrar estadísticas de I/O
 * 
 * @note Si el archivo cabe en memoria, lo ordena directamente
 * @note Para archivos grandes, usa particionamiento recursivo
 */
void quicksortRecursive(const std::string& inputFilename, 
                       const std::string& outputFilename, 
                       size_t arity,
                       size_t memoryLimit, 
                       const std::string& tempDir,
                       IOStats& stats);

/**
 * @brief Ordena un archivo usando Quicksort externo
 * 
 * @param inputFilename Archivo de entrada a ordenar
 * @param outputFilename Archivo de salida ordenado
 * @param arity Número de particiones a crear en cada paso
 * @param memoryLimit Límite de memoria en bytes
 * @param stats Objeto para registrar estadísticas de I/O
 * 
 * @note Maneja toda la lógica de directorios temporales
 * @note Mide y reporta tiempo de ejecución y operaciones I/O
 */
void externalQuickSort(const std::string& inputFilename, 
                     const std::string& outputFilename, 
                     size_t arity,
                     size_t memoryLimit,
                     IOStats& stats);             
#endif