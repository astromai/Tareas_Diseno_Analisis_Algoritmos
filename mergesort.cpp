#include "mergesort.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <queue>
#include <utility>
#include <cmath>
#include <filesystem>

namespace fs = std::filesystem;

/**
 * @brief Nodo para la cola de prioridad usada en la mezcla
 * 
 * Almacena un valor y el índice del archivo del cual proviene,
 * permitiendo comparar valores de diferentes archivos durante la mezcla.
 */
struct HeapNode {
    int64_t value;
    size_t fileIndex;
    
    /**
     * @brief Operador de comparación para ordenar el min-heap
     * @param other Otro nodo a comparar
     * @return true si este nodo es mayor que el otro
     */
    bool operator>(const HeapNode& other) const {
        return value > other.value;
    }
};

/**
 * @brief Implementa el algoritmo de MergeSort externo
 * 
 * @param inputFilename Archivo de entrada a ordenar
 * @param outputFilename Archivo de salida ordenado
 * @param arity Número de archivos a mezclar simultáneamente
 * @param memoryLimit Límite de memoria en bytes
 * @param stats Objeto para registrar estadísticas de I/O
 * 
 * @note Opera en dos fases principales:
 *   1. División: Divide el archivo en chunks ordenados
 *   2. Mezcla: Mezcla recursiva los chunks usando una cola de prioridad
 * 
 * @warning Crea y elimina archivos temporales en el directorio ./temp_[arity]
 */
void externalMergeSort(const std::string& inputFilename, const std::string& outputFilename, 
                      size_t arity, size_t memoryLimit, IOStats& stats) {
    auto startTime = std::chrono::high_resolution_clock::now();
    stats.reset();
    
    std::string tempDir = "./temp_" + std::to_string(arity);
    if (!fs::exists(tempDir)) {
        fs::create_directories(tempDir);
    }
    
    size_t numbersInMemory = memoryLimit / sizeof(int64_t);
    
    // Fase de división
    std::vector<std::string> chunkFiles;
    int64_t fileSize = fs::file_size(inputFilename);
    int64_t totalChunks = std::ceil(static_cast<double>(fileSize) / (numbersInMemory * sizeof(int64_t)));
    
    std::ifstream inputFile(inputFilename, std::ios::binary);
    if (!inputFile) {
        std::cerr << "Error al abrir el archivo de entrada: " << inputFilename << std::endl;
        return;
    }
    
    for (int64_t chunk = 0; chunk < totalChunks; ++chunk) {
        std::vector<int64_t> buffer;
        readBlock(inputFile, buffer, numbersInMemory, stats);
        std::sort(buffer.begin(), buffer.end());
        
        std::string chunkFilename = tempDir + "/chunk_" + std::to_string(chunk) + ".bin";
        std::ofstream chunkFile(chunkFilename, std::ios::binary);
        writeBlock(chunkFile, buffer, stats);
        chunkFile.close();
        
        chunkFiles.push_back(chunkFilename);
    }
    inputFile.close();
    
    // Fase de mezcla
    size_t bufferSize = (memoryLimit / (arity + 1)) / sizeof(int64_t);
    if (bufferSize < 1) bufferSize = 1;
    
    while (chunkFiles.size() > 1) {
        std::vector<std::string> newChunkFiles;
        
        for (size_t i = 0; i < chunkFiles.size(); i += arity) {
            size_t filesCount = std::min(arity, chunkFiles.size() - i);
            std::string outputChunk = tempDir + "/merged_" + std::to_string(newChunkFiles.size()) + ".bin";
            
            std::vector<std::ifstream> inputStreams(filesCount);
            std::vector<std::vector<int64_t>> buffers(filesCount);
            std::vector<size_t> positions(filesCount, 0);
            std::priority_queue<HeapNode, std::vector<HeapNode>, std::greater<HeapNode>> minHeap;
            
            for (size_t j = 0; j < filesCount; ++j) {
                inputStreams[j].open(chunkFiles[i + j], std::ios::binary);
                readBlock(inputStreams[j], buffers[j], bufferSize, stats);
                
                if (!buffers[j].empty()) {
                    minHeap.push({buffers[j][0], j});
                    positions[j] = 0;
                }
            }
            
            std::ofstream outputStream(outputChunk, std::ios::binary);
            std::vector<int64_t> outputBuffer;
            outputBuffer.reserve(bufferSize);
            
            while (!minHeap.empty()) {
                HeapNode top = minHeap.top();
                minHeap.pop();
                
                outputBuffer.push_back(top.value);
                
                if (outputBuffer.size() >= bufferSize) {
                    writeBlock(outputStream, outputBuffer, stats);
                    outputBuffer.clear();
                }
                
                positions[top.fileIndex]++;
                
                if (positions[top.fileIndex] >= buffers[top.fileIndex].size()) {
                    buffers[top.fileIndex].clear();
                    readBlock(inputStreams[top.fileIndex], buffers[top.fileIndex], bufferSize, stats);
                    positions[top.fileIndex] = 0;
                }
                
                if (positions[top.fileIndex] < buffers[top.fileIndex].size()) {
                    minHeap.push({buffers[top.fileIndex][positions[top.fileIndex]], top.fileIndex});
                }
            }
            
            if (!outputBuffer.empty()) {
                writeBlock(outputStream, outputBuffer, stats);
            }
            
            for (auto& stream : inputStreams) {
                stream.close();
            }
            outputStream.close();
            
            newChunkFiles.push_back(outputChunk);
        }
        
        chunkFiles = newChunkFiles;
    }
    
    if (!chunkFiles.empty()) {
        std::ifstream finalFile(chunkFiles[0], std::ios::binary);
        std::ofstream outputFile(outputFilename, std::ios::binary);
        
        std::vector<int64_t> copyBuffer;
        while (finalFile) {
            size_t read = readBlock(finalFile, copyBuffer, numbersInMemory, stats);
            if (read > 0) {
                writeBlock(outputFile, copyBuffer, stats);
            } else {
                break;
            }
        }
        
        finalFile.close();
        outputFile.close();
    }
    
    for (const auto& entry : fs::directory_iterator(tempDir)) {
        fs::remove(entry.path());
    }
    fs::remove(tempDir);
    
    auto endTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = endTime - startTime;
    
    std::cout << "Aridad " << arity << " completada en " << duration.count() << " segundos" << std::endl;
}