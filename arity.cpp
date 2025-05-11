#include "arity.h"
#include "mergesort.h"
#include "iostats.h"
#include "constants.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <random>
#include <queue>
#include <filesystem>
#include <cmath>
#include <limits>

namespace fs = std::filesystem;

/**
 * @brief Genera un archivo con datos aleatorios para pruebas de ordenamiento
 * 
 * @param filename Ruta del archivo a generar
 * @param size Cantidad de números aleatorios a generar 
 * 
 * @note Usa una distribución uniforme en todo el rango de int64_t
 * @note Muestra progreso cada 10% de la generación
 * @note Crea el directorio padre si no existe
 */
void generateData(const std::string& filename, int64_t size) {
    std::cout << "Generando " << size << " números aleatorios..." << std::endl;
    
    fs::path filePath(filename);
    if (!fs::exists(filePath.parent_path())) {
        fs::create_directories(filePath.parent_path());
    }
    
    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Error al abrir el archivo para escritura: " << filename << std::endl;
        return;
    }
    
    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<int64_t> dist(0, std::numeric_limits<int64_t>::max());
    
    size_t writesPerformed = 0;
    for (int64_t i = 0; i < size; ++i) {
        int64_t num = dist(gen);
        file.write(reinterpret_cast<const char*>(&num), sizeof(num));
        writesPerformed++;
    }
    
    file.close();
    std::cout << "Archivo generado: " << filename << std::endl;
    std::cout << "Operaciones de escritura realizadas: " << writesPerformed << std::endl;
}

/**
 * @brief Verifica si un archivo está ordenado ascendentemente
 * 
 * @param filename Ruta del archivo a verificar
 * @param stats Objeto para registrar estadísticas de I/O
 * @return true si el archivo está correctamente ordenado
 * @return false si encuentra elementos fuera de orden
 * 
 * @note Reporta la posición del primer error encontrado
 * @note Usa bloques de 1,000,000 elementos para la verificación
 */
bool verifySort(const std::string& filename, IOStats& stats) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Error al abrir archivo para verificación: " << filename << std::endl;
        return false;
    }
    
    int64_t prev = std::numeric_limits<int64_t>::min();
    size_t position = 0;
    
    std::vector<int64_t> buffer;
    const size_t VERIFY_BLOCK_SIZE = 1000000;
    
    while (true) {
        size_t read = readBlock(file, buffer, VERIFY_BLOCK_SIZE, stats);
        if (read == 0) break;
        
        for (size_t i = 0; i < buffer.size(); i++) {
            if (buffer[i] < prev) {
                std::cerr << "Error: archivo no está ordenado en la posición " << position + i << std::endl;
                return false;
            }
            prev = buffer[i];
        }
        
        position += buffer.size();
    }
    
    return true;
}

/**
 * @brief Busca la aridad óptima para algoritmos de ordenamiento externo
 * 
 * @param inputFilename Archivo de entrada para pruebas
 * @param outputFilename Base del nombre para archivos de salida
 * @param minArity Valor mínimo de aridad a evaluar
 * @param maxArity Valor máximo de aridad a evaluar
 * @param memoryLimit Memoria disponible en bytes
 * @return size_t Aridad óptima encontrada
 * 
 * @note Usa búsqueda ternaria para optimizar el proceso
 * @note Evalúa basándose en el número total de operaciones I/O
 * @note Genera archivos temporales con formato outputFilename_arity
 */
size_t findOptimalArity(const std::string& inputFilename, const std::string& outputFilename, 
                        size_t minArity, size_t maxArity, size_t memoryLimit) {
    std::cout << "Buscando aridad óptima entre " << minArity << " y " << maxArity << "..." << std::endl;
    
    size_t bestArity = minArity;
    size_t bestIOCount = std::numeric_limits<size_t>::max();
    
    // Configuración para búsqueda ternaria
    size_t left = minArity;
    size_t right = maxArity;
    
    while (left <= right) {
        size_t mid1 = left + (right - left) / 3;
        size_t mid2 = right - (right - left) / 3;
        
        // Probar primera aridad
        IOStats stats1;
        std::cout << "Probando aridad: " << mid1 << std::endl;
        externalMergeSort(inputFilename, outputFilename + "_" + std::to_string(mid1), mid1, memoryLimit, stats1);
        
        // Probar segunda aridad
        IOStats stats2;
        std::cout << "Probando aridad: " << mid2 << std::endl;
        externalMergeSort(inputFilename, outputFilename + "_" + std::to_string(mid2), mid2, memoryLimit, stats2);
        
        std::cout << "Aridad " << mid1 << ": " << stats1.total() << " operaciones I/O" << std::endl;
        std::cout << "Aridad " << mid2 << ": " << stats2.total() << " operaciones I/O" << std::endl;
        
        // Actualizar la mejor aridad basada en el total de operaciones I/O
        if (stats1.total() < bestIOCount) {
            bestIOCount = stats1.total();
            bestArity = mid1;
        }
        
        if (stats2.total() < bestIOCount) {
            bestIOCount = stats2.total();
            bestArity = mid2;
        }
        
        // Actualizar los límites de búsqueda
        if (stats1.total() < stats2.total()) {
            right = mid2 - 1;
        } else {
            left = mid1 + 1;
        }
    }
    
    std::cout << "Aridad óptima encontrada: " << bestArity << " con " << bestIOCount << " operaciones I/O" << std::endl;
    return bestArity;
}

/**
 * @brief Función principal para determinar la aridad óptima
 * 
 * @return int Código de salida (0 = éxito)
 * 
 * @note Configuración por defecto:
 *   - 60 millones de números (≈457MB)
 *   - Memoria limitada a 128MB
 *   - Busca aridad entre 2 y B (4096 bytes)
 */
int main() {
    // Nombre de archivos
    std::string inputFilename = "./data/input_array.bin";
    std::string outputFilename = "./data/sorted_array";
    
    // Memoria disponible (ejemplo: 128MB)
    const size_t AVAILABLE_MEMORY = 128 * 1024 * 1024;
    
    std::cout << "Iniciando programa de optimización de aridad para Mergesort externo" << std::endl;
    std::cout << "Tamaño de bloque (B): " << B << " bytes" << std::endl;
    std::cout << "Números por bloque (b): " << b << std::endl;
    
    // Crear directorios necesarios
    fs::path dataPath("./data");
    if (!fs::exists(dataPath)) {
        fs::create_directories(dataPath);
    }
    
    // Generar datos
    generateData(inputFilename);
    
    // Buscar aridad óptima basada en operaciones I/O
    size_t optimalArity = findOptimalArity(inputFilename, outputFilename, 2, b, AVAILABLE_MEMORY);
    
    // Verificar que el archivo resultante esté ordenado
    std::string finalSortedFile = outputFilename + "_" + std::to_string(optimalArity);
    IOStats verifyStats;
    bool isSorted = verifySort(finalSortedFile, verifyStats);
    
    std::cout << "\n=== Resultados ===" << std::endl;
    std::cout << "La aridad óptima para Mergesort externo es: " << optimalArity << std::endl;
    std::cout << "Verificación de ordenamiento: " << (isSorted ? "CORRECTO" : "INCORRECTO") << std::endl;
    std::cout << "Esta aridad también define la cantidad de subarreglos para Quicksort externo" << std::endl;
    
    return 0;
}