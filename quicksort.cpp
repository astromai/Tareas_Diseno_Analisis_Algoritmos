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

// Función para particionar un archivo usando pivotes
void partition(const std::string& inputFilename, 
               const std::vector<std::string>& outputFilenames,
               const std::vector<int64_t>& pivots, 
               size_t memoryLimit,
               IOStats& stats) {
    
    // Abrir archivo de entrada
    std::ifstream inputFile(inputFilename, std::ios::binary);
    if (!inputFile) {
        std::cerr << "Error al abrir archivo para partición: " << inputFilename << std::endl;
        return;
    }
    
    // Crear archivos de salida para cada partición
    size_t numPartitions = pivots.size() + 1;
    std::vector<std::ofstream> outputFiles(numPartitions);
    for (size_t i = 0; i < numPartitions; ++i) {
        outputFiles[i].open(outputFilenames[i], std::ios::binary);
        if (!outputFiles[i]) {
            std::cerr << "Error al crear archivo de partición: " << outputFilenames[i] << std::endl;
            return;
        }
    }
    
    // Tamaño del buffer para procesar en memoria
    size_t bufferSize = memoryLimit / sizeof(int64_t);
    std::vector<int64_t> buffer;
    std::vector<std::vector<int64_t>> partitionBuffers(numPartitions);
    
    // Procesar el archivo por bloques
    while (true) {
        // Leer un bloque de datos
        size_t itemsRead = readBlock(inputFile, buffer, bufferSize, stats);
        if (itemsRead == 0) break;
        
        // Clasificar cada elemento en la partición correspondiente
        for (int64_t value : buffer) {
            size_t partitionIndex = 0;
            // Encontrar la partición correcta según los pivotes
            while (partitionIndex < pivots.size() && value >= pivots[partitionIndex]) {
                partitionIndex++;
            }
            partitionBuffers[partitionIndex].push_back(value);
        }
        
        // Escribir los buffers de partición a sus respectivos archivos
        for (size_t i = 0; i < numPartitions; ++i) {
            if (!partitionBuffers[i].empty()) {
                writeBlock(outputFiles[i], partitionBuffers[i], stats);
                partitionBuffers[i].clear();
            }
        }
    }
    
    // Escribir cualquier dato restante en los buffers de partición
    for (size_t i = 0; i < numPartitions; ++i) {
        if (!partitionBuffers[i].empty()) {
            writeBlock(outputFiles[i], partitionBuffers[i], stats);
        }
    }
    
    // Cerrar todos los archivos
    inputFile.close();
    for (auto& file : outputFiles) {
        file.close();
    }
}

// Función para seleccionar pivotes aleatorios del archivo
std::vector<int64_t> selectPivots(const std::string& filename, size_t numPivots, IOStats& stats) {
    if (numPivots == 0) return {};
    
    std::vector<int64_t> pivots;
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    
    if (!file) {
        std::cerr << "Error al abrir archivo para selección de pivotes: " << filename << std::endl;
        return pivots;
    }
    
    // Obtener el tamaño del archivo
    std::streamsize fileSize = file.tellg();
    file.seekg(0, std::ios::beg);
    
    if (fileSize == 0) {
        file.close();
        return pivots;
    }
    
    // Calcular cuántos números hay en el archivo
    size_t numNumbers = fileSize / sizeof(int64_t);
    
    // Inicializar generador de números aleatorios
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<size_t> dist(0, numNumbers - 1);
    
    // Leer un bloque aleatorio
    const size_t SAMPLE_SIZE = std::min(numNumbers, size_t(B / sizeof(int64_t)));
    std::vector<int64_t> sampleBuffer;
    
    // Posición aleatoria para muestreo
    size_t samplePos = dist(gen);
    if (samplePos + SAMPLE_SIZE > numNumbers) {
        samplePos = numNumbers - SAMPLE_SIZE;
    }
    
    // Posicionarse en el bloque aleatorio
    file.seekg(samplePos * sizeof(int64_t));
    
    // Leer el bloque
    readBlock(file, sampleBuffer, SAMPLE_SIZE, stats);
    file.close();
    
    // Si no tenemos suficientes números, usar lo que tengamos
    if (sampleBuffer.size() <= numPivots) {
        std::sort(sampleBuffer.begin(), sampleBuffer.end());
        return sampleBuffer;
    }
    
    // Ordenar el bloque muestreado
    std::sort(sampleBuffer.begin(), sampleBuffer.end());
    
    // Seleccionar pivotes distribuidos uniformemente
    pivots.resize(numPivots);
    for (size_t i = 0; i < numPivots; ++i) {
        size_t index = (i + 1) * sampleBuffer.size() / (numPivots + 1);
        pivots[i] = sampleBuffer[index];
    }
    
    return pivots;
}

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
        
        if (i % (size/10) == 0 && i > 0) {
            std::cout << (i * 100 / size) << "% completado..." << std::endl;
        }
    }
    
    file.close();
    std::cout << "Archivo generado: " << filename << std::endl;
    std::cout << "Operaciones de escritura realizadas: " << writesPerformed << std::endl;
}

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


// Función para ordenar recursivamente un archivo usando Quicksort externo
void quicksortRecursive(const std::string& inputFilename, 
                        const std::string& outputFilename, 
                        size_t arity,
                        size_t memoryLimit, 
                        const std::string& tempDir,
                        IOStats& stats) {
    // Obtener el tamaño del archivo
    int64_t fileSize = fs::file_size(inputFilename);
    
    // Si el archivo es pequeño, ordenar en memoria
    if (fileSize <= memoryLimit) {
        std::ifstream inputFile(inputFilename, std::ios::binary);
        std::vector<int64_t> buffer;
        
        // Leer todo el archivo
        size_t itemsRead = readBlock(inputFile, buffer, fileSize / sizeof(int64_t), stats);
        inputFile.close();
        
        // Ordenar en memoria
        std::sort(buffer.begin(), buffer.end());
        
        // Escribir resultado ordenado
        std::ofstream outputFile(outputFilename, std::ios::binary);
        writeBlock(outputFile, buffer, stats);
        outputFile.close();
        
        return;
    }
    
    // Seleccionar pivotes
    std::vector<int64_t> pivots = selectPivots(inputFilename, arity - 1, stats);
    
    // Crear archivos temporales para las particiones
    std::vector<std::string> partitionFiles;
    for (size_t i = 0; i < arity; ++i) {
        partitionFiles.push_back(tempDir + "/partition_" + std::to_string(i) + "_" + 
                               fs::path(inputFilename).filename().string());
    }
    
    // Particionar el archivo de entrada
    partition(inputFilename, partitionFiles, pivots, memoryLimit, stats);
    
    // Ordenar recursivamente cada partición
    std::vector<std::string> sortedPartitionFiles;
    for (size_t i = 0; i < arity; ++i) {
        std::string sortedFile = tempDir + "/sorted_" + std::to_string(i) + "_" + 
                                fs::path(inputFilename).filename().string();
        
        // Verificar si el archivo de partición existe y no está vacío
        if (fs::exists(partitionFiles[i]) && fs::file_size(partitionFiles[i]) > 0) {
            quicksortRecursive(partitionFiles[i], sortedFile, arity, memoryLimit, tempDir, stats);
            sortedPartitionFiles.push_back(sortedFile);
        }
        
        // Eliminar el archivo de partición
        if (fs::exists(partitionFiles[i])) {
            fs::remove(partitionFiles[i]);
        }
    }
    
    // Concatenar las particiones ordenadas
    std::ofstream outputFile(outputFilename, std::ios::binary);
    
    for (const auto& partitionFile : sortedPartitionFiles) {
        if (!fs::exists(partitionFile)) continue;
        
        std::ifstream inputFile(partitionFile, std::ios::binary);
        std::vector<int64_t> buffer;
        
        // Leer y escribir por bloques
        while (true) {
            size_t read = readBlock(inputFile, buffer, memoryLimit / sizeof(int64_t), stats);
            if (read == 0) break;
            
            writeBlock(outputFile, buffer, stats);
        }
        
        inputFile.close();
        
        // Eliminar archivo de partición ordenada
        fs::remove(partitionFile);
    }
    
    outputFile.close();
}

// Función principal para el Quicksort externo
void externalQuickSort(const std::string& inputFilename, 
                      const std::string& outputFilename, 
                      size_t arity,
                      size_t memoryLimit,
                      IOStats& stats) {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Reiniciar estadísticas
    stats.reset();
    
    std::cout << "Iniciando Quicksort externo con " << arity << " particiones..." << std::endl;
    
    // Crear directorio temporal
    std::string tempDir = "./temp_quick_" + std::to_string(arity);
    if (!fs::exists(tempDir)) {
        fs::create_directories(tempDir);
    }
    
    // Ejecutar Quicksort recursivo
    quicksortRecursive(inputFilename, outputFilename, arity, memoryLimit, tempDir, stats);
    
    // Limpiar directorio temporal
    for (const auto& entry : fs::directory_iterator(tempDir)) {
        fs::remove(entry.path());
    }
    fs::remove(tempDir);
    
    auto endTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = endTime - startTime;
    
    std::cout << "Quicksort externo completado en " << duration.count() << " segundos" << std::endl;
    std::cout << "Operaciones de lectura: " << stats.reads << std::endl;
    std::cout << "Operaciones de escritura: " << stats.writes << std::endl;
    std::cout << "Total operaciones I/O: " << stats.total() << std::endl;
}