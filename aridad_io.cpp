#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <random>
#include <string>
#include <cstdint>
#include <queue>
#include <utility>
#include <filesystem>
#include <cmath>
#include <limits>

namespace fs = std::filesystem;

// Constantes
const int64_t M = 60'000'000;    // Tamaño de la secuencia: 60M números
const size_t NUM_SIZE = 8;       // Tamaño de cada número en bytes (64 bits)
const size_t B = 4096;           // Tamaño de bloque en bytes (ajustar según sistema)
const size_t b = B / NUM_SIZE;   // Cantidad de números de 64 bits por bloque

// Estructura para almacenar estadísticas de I/O
struct IOStats {
    size_t reads = 0;            // Contador de operaciones de lectura
    size_t writes = 0;           // Contador de operaciones de escritura
    
    // Total de operaciones I/O
    size_t total() const { 
        return reads + writes; 
    }
    
    // Reiniciar contadores
    void reset() {
        reads = 0;
        writes = 0;
    }
};

// Clase para manejar elementos en la cola de prioridad para la fase de mezcla
struct HeapNode {
    int64_t value;       // Valor del elemento
    size_t fileIndex;    // Índice del archivo de origen
    
    // Operador de comparación para la cola de prioridad (min-heap)
    bool operator>(const HeapNode& other) const {
        return value > other.value;
    }
};

// Función para generar datos aleatorios
void generateData(const std::string& filename, int64_t size = M) {
    std::cout << "Generando " << size << " números aleatorios..." << std::endl;
    
    // Crear directorio si no existe
    fs::path filePath(filename);
    if (!fs::exists(filePath.parent_path())) {
        fs::create_directories(filePath.parent_path());
    }
    
    // Abrir archivo para escritura binaria
    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Error al abrir el archivo para escritura: " << filename << std::endl;
        return;
    }
    
    // Inicializar generador de números aleatorios
    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<int64_t> dist(0, std::numeric_limits<int64_t>::max());
    
    // Generar y escribir los números
    size_t writesPerformed = 0;
    for (int64_t i = 0; i < size; ++i) {
        int64_t num = dist(gen);
        file.write(reinterpret_cast<const char*>(&num), sizeof(num));
        writesPerformed++;
        
        // Mostrar progreso cada 10%
        if (i % (size/10) == 0 && i > 0) {
            std::cout << (i * 100 / size) << "% completado..." << std::endl;
        }
    }
    
    file.close();
    std::cout << "Archivo generado: " << filename << std::endl;
    std::cout << "Operaciones de escritura realizadas: " << writesPerformed << std::endl;
}

// Función para leer un bloque de datos incrementando contador de I/O
template<typename T>
size_t readBlock(std::ifstream& file, std::vector<T>& buffer, size_t count, IOStats& stats) {
    // Posición antes de leer
    std::streampos posBefore = file.tellg();
    
    buffer.resize(count);
    file.read(reinterpret_cast<char*>(buffer.data()), count * sizeof(T));
    
    // Calcular cuántos elementos se leyeron realmente
    size_t itemsRead = file.gcount() / sizeof(T);
    buffer.resize(itemsRead);  // Ajustar el tamaño real
    
    // Calcular cuántos bloques de disco se leyeron (redondeando hacia arriba)
    if (itemsRead > 0) {
        size_t bytesRead = itemsRead * sizeof(T);
        size_t blocksRead = (bytesRead + B - 1) / B;  // Redondeo hacia arriba
        stats.reads += blocksRead;
    }
    
    return itemsRead;
}

// Función para escribir un bloque de datos incrementando contador de I/O
template<typename T>
void writeBlock(std::ofstream& file, const std::vector<T>& buffer, IOStats& stats) {
    if (buffer.empty()) return;
    
    // Posición antes de escribir
    std::streampos posBefore = file.tellp();
    
    file.write(reinterpret_cast<const char*>(buffer.data()), buffer.size() * sizeof(T));
    
    // Calcular cuántos bloques de disco se escribieron (redondeando hacia arriba)
    size_t bytesWritten = buffer.size() * sizeof(T);
    size_t blocksWritten = (bytesWritten + B - 1) / B;  // Redondeo hacia arriba
    stats.writes += blocksWritten;
}

// Función para realizar External Mergesort con una aridad específica
void externalMergeSort(const std::string& inputFilename, const std::string& outputFilename, 
                      size_t arity, size_t memoryLimit, IOStats& stats) {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Reiniciar estadísticas
    stats.reset();
    
    // Crear directorio temporal
    std::string tempDir = "./temp_" + std::to_string(arity);
    if (!fs::exists(tempDir)) {
        fs::create_directories(tempDir);
    }
    
    // Calcular cuántos números pueden caber en memoria a la vez
    size_t numbersInMemory = memoryLimit / sizeof(int64_t);
    
    // 1. Fase de división
    std::vector<std::string> chunkFiles;
    int64_t fileSize = fs::file_size(inputFilename);
    int64_t totalChunks = std::ceil(static_cast<double>(fileSize) / (numbersInMemory * sizeof(int64_t)));
    
    std::cout << "Iniciando fase de división con aridad " << arity << "..." << std::endl;
    
    std::ifstream inputFile(inputFilename, std::ios::binary);
    if (!inputFile) {
        std::cerr << "Error al abrir el archivo de entrada: " << inputFilename << std::endl;
        return;
    }
    
    for (int64_t chunk = 0; chunk < totalChunks; ++chunk) {
        // Leer chunk a memoria
        std::vector<int64_t> buffer;
        readBlock(inputFile, buffer, numbersInMemory, stats);
        
        // Ordenar en memoria
        std::sort(buffer.begin(), buffer.end());
        
        // Escribir a archivo temporal
        std::string chunkFilename = tempDir + "/chunk_" + std::to_string(chunk) + ".bin";
        std::ofstream chunkFile(chunkFilename, std::ios::binary);
        
        if (!chunkFile) {
            std::cerr << "Error al crear archivo temporal: " << chunkFilename << std::endl;
            return;
        }
        
        writeBlock(chunkFile, buffer, stats);
        chunkFile.close();
        
        chunkFiles.push_back(chunkFilename);
    }
    
    inputFile.close();
    
    // 2. Fase de mezcla
    std::cout << "Iniciando fase de mezcla..." << std::endl;
    
    // Tamaño del buffer para cada archivo de entrada durante la mezcla
    size_t bufferSize = (memoryLimit / (arity + 1)) / sizeof(int64_t);
    if (bufferSize < 1) bufferSize = 1;
    
    while (chunkFiles.size() > 1) {
        std::vector<std::string> newChunkFiles;
        
        for (size_t i = 0; i < chunkFiles.size(); i += arity) {
            // Determinar cuántos archivos mezclar en esta iteración
            size_t filesCount = std::min(arity, chunkFiles.size() - i);
            
            std::string outputChunk = tempDir + "/merged_" + std::to_string(newChunkFiles.size()) + ".bin";
            
            // Preparar para mezclar 'filesCount' archivos
            std::vector<std::ifstream> inputStreams(filesCount);
            std::vector<std::vector<int64_t>> buffers(filesCount);
            std::vector<size_t> positions(filesCount, 0);
            std::priority_queue<HeapNode, std::vector<HeapNode>, std::greater<HeapNode>> minHeap;
            
            // Abrir cada archivo y leer su primer bloque
            for (size_t j = 0; j < filesCount; ++j) {
                inputStreams[j].open(chunkFiles[i + j], std::ios::binary);
                
                if (!inputStreams[j]) {
                    std::cerr << "Error al abrir archivo: " << chunkFiles[i + j] << std::endl;
                    continue;
                }
                
                // Leer primer bloque del archivo
                readBlock(inputStreams[j], buffers[j], bufferSize, stats);
                
                // Si hay elementos, agregar el primero al heap
                if (!buffers[j].empty()) {
                    minHeap.push({buffers[j][0], j});
                    positions[j] = 0;
                }
            }
            
            // Abrir archivo de salida
            std::ofstream outputStream(outputChunk, std::ios::binary);
            if (!outputStream) {
                std::cerr << "Error al crear archivo de salida: " << outputChunk << std::endl;
                return;
            }
            
            // Buffer para acumular resultados antes de escribir
            std::vector<int64_t> outputBuffer;
            outputBuffer.reserve(bufferSize);
            
            // Combinar los archivos
            while (!minHeap.empty()) {
                HeapNode top = minHeap.top();
                minHeap.pop();
                
                // Agregar el elemento mínimo al buffer de salida
                outputBuffer.push_back(top.value);
                
                // Si el buffer de salida está lleno, escribirlo
                if (outputBuffer.size() >= bufferSize) {
                    writeBlock(outputStream, outputBuffer, stats);
                    outputBuffer.clear();
                }
                
                // Mover al siguiente elemento en el buffer correspondiente
                positions[top.fileIndex]++;
                
                // Si llegamos al final del buffer actual, cargar más datos
                if (positions[top.fileIndex] >= buffers[top.fileIndex].size()) {
                    buffers[top.fileIndex].clear();
                    readBlock(inputStreams[top.fileIndex], buffers[top.fileIndex], bufferSize, stats);
                    positions[top.fileIndex] = 0;
                }
                
                // Si hay más elementos en este buffer, agregar el siguiente al heap
                if (positions[top.fileIndex] < buffers[top.fileIndex].size()) {
                    minHeap.push({buffers[top.fileIndex][positions[top.fileIndex]], top.fileIndex});
                }
            }
            
            // Escribir cualquier elemento restante en el buffer de salida
            if (!outputBuffer.empty()) {
                writeBlock(outputStream, outputBuffer, stats);
            }
            
            // Cerrar los archivos
            for (auto& stream : inputStreams) {
                stream.close();
            }
            outputStream.close();
            
            newChunkFiles.push_back(outputChunk);
        }
        
        // Reemplazar la lista de archivos con los nuevos
        chunkFiles = newChunkFiles;
    }
    
    // Copiar el archivo final al de salida
    if (!chunkFiles.empty()) {
        std::ifstream finalFile(chunkFiles[0], std::ios::binary);
        std::ofstream outputFile(outputFilename, std::ios::binary);
        
        // Copiar por bloques para contabilizar I/O
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
    
    // Limpiar archivos temporales
    for (const auto& entry : fs::directory_iterator(tempDir)) {
        fs::remove(entry.path());
    }
    fs::remove(tempDir);
    
    auto endTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = endTime - startTime;
    
    std::cout << "Aridad " << arity << " completada en " << duration.count() << " segundos" << std::endl;
    std::cout << "Operaciones de lectura: " << stats.reads << std::endl;
    std::cout << "Operaciones de escritura: " << stats.writes << std::endl;
    std::cout << "Total operaciones I/O: " << stats.total() << std::endl;
}

// Función para verificar si un archivo está ordenado correctamente
bool verifySort(const std::string& filename, IOStats& stats) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Error al abrir archivo para verificación: " << filename << std::endl;
        return false;
    }
    
    int64_t prev = std::numeric_limits<int64_t>::min();
    int64_t current;
    size_t position = 0;
    
    // Verificar por bloques para contabilizar I/O
    std::vector<int64_t> buffer;
    const size_t VERIFY_BLOCK_SIZE = 1000000;  // Verificar en bloques de 1M elementos
    
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

// Función para realizar búsqueda binaria de la aridad óptima
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