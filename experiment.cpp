#include "experiment.h"

namespace fs = std::filesystem;

/**
 * @brief Ejecuta experimentos comparativos entre MergeSort y QuickSort en memoria externa.
 * 
 * @param optimalArity La aridad óptima (número de vías para MergeSort o subarreglos para QuickSort) calculada previamente.
 * 
 * Esta función genera datos aleatorios para diferentes tamaños de entrada, ejecuta ambos algoritmos,
 * mide sus tiempos y operaciones de I/O, y guarda los resultados promediados en un archivo CSV.
 */
void runExperiments(size_t optimalArity) {
    std::cout << "\n=== Iniciando experimentos de comparación ===" << std::endl;
    
    // Tamaños de los arreglos a evaluar (en millones)
    std::vector<int64_t> sizes = {4, 8, 12, 16};
    
    // Número de repeticiones por tamaño
    const int REPETITIONS = 5;
    
    // Memoria disponible (50 MB como indicado en el enunciado)
    const size_t MEMORY_LIMIT = 50 * 1024 * 1024;
    
    /**
     * @brief Estructura para almacenar resultados individuales (tiempo y operaciones de I/O).
     */
    struct Result {
        double time;
        size_t io;
    };
    
    // Almacenar resultados por tamaño y algoritmo
    std::map<int64_t, std::vector<Result>> mergeSortResults;
    std::map<int64_t, std::vector<Result>> quickSortResults;
    
    // Crear directorios para datos y resultados
    fs::path dataPath("./dataExp");
    fs::path resultsPath("./results");
    
    if (!fs::exists(dataPath)) {
        fs::create_directories(dataPath);
    }
    if (!fs::exists(resultsPath)) {
        fs::create_directories(resultsPath);
    }
    
    // Ejecutar experimentos para cada tamaño
    for (const auto& N : sizes) {
        std::cout << "\nEvaluando tamaño: " << N << "M elementos" << std::endl;
        
        int64_t actualSize = N * 1000000; // Convertir a millones
        
        for (int rep = 0; rep < REPETITIONS; ++rep) {
            std::cout << "Repetición " << (rep + 1) << "/" << REPETITIONS << std::endl;
            
            // Nombre de archivos para esta repetición
            std::string inputFile = "./dataExp/input_" + std::to_string(N) + "M_" + std::to_string(rep) + ".bin";
            std::string mergeSortOutput = "./results/mergesort_" + std::to_string(N) + "M_" + std::to_string(rep) + ".bin";
            std::string quickSortOutput = "./results/quicksort_" + std::to_string(N) + "M_" + std::to_string(rep) + ".bin";
            
            // Generar datos nuevos para cada repetición
            generateData(inputFile, actualSize);
            
            // Ejecutar MergeSort
            IOStats mergeStats;
            auto mergeStart = std::chrono::high_resolution_clock::now();
            externalMergeSort(inputFile, mergeSortOutput, optimalArity, MEMORY_LIMIT, mergeStats);
            auto mergeEnd = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> mergeDuration = mergeEnd - mergeStart;
            
            // Verificar que el resultado esté ordenado
            bool mergeSorted = verifySort(mergeSortOutput, mergeStats);
            if (!mergeSorted) {
                std::cerr << "¡Error! MergeSort no ordenó correctamente." << std::endl;
            }
            
            // Ejecutar QuickSort
            IOStats quickStats;
            auto quickStart = std::chrono::high_resolution_clock::now();
            externalQuickSort(inputFile, quickSortOutput, optimalArity, MEMORY_LIMIT, quickStats);
            auto quickEnd = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> quickDuration = quickEnd - quickStart;
            
            // Verificar que el resultado esté ordenado
            bool quickSorted = verifySort(quickSortOutput, quickStats);
            if (!quickSorted) {
                std::cerr << "¡Error! QuickSort no ordenó correctamente." << std::endl;
            }
            
            // Guardar resultados
            mergeSortResults[N].push_back({mergeDuration.count(), mergeStats.total()});
            quickSortResults[N].push_back({quickDuration.count(), quickStats.total()});
            
            // Mostrar resultados de esta repetición
            std::cout << "MergeSort: " << mergeDuration.count() << "s, " 
                      << mergeStats.total() << " I/Os" << std::endl;
            std::cout << "QuickSort: " << quickDuration.count() << "s, " 
                      << quickStats.total() << " I/Os" << std::endl;
            
            // Eliminar archivos grandes para ahorrar espacio
            if (fs::exists(inputFile)) fs::remove(inputFile);
            if (fs::exists(mergeSortOutput)) fs::remove(mergeSortOutput);
            if (fs::exists(quickSortOutput)) fs::remove(quickSortOutput);
        }
    }
    
    // Calcular y guardar promedios
    std::ofstream resultsFile("./results/comparison_results.csv");
    resultsFile << "Size(M),MergeSort_Time(s),MergeSort_IO,QuickSort_Time(s),QuickSort_IO\n";
    
    std::cout << "\n=== Resultados promedio ===" << std::endl;
    std::cout << "Tamaño(M),MergeSort_Tiempo(s),MergeSort_IO,QuickSort_Tiempo(s),QuickSort_IO" << std::endl;
    
    for (const auto& N : sizes) {
        // Calcular promedios para MergeSort
        double avgMergeTime = 0.0;
        double avgMergeIO = 0.0;
        for (const auto& result : mergeSortResults[N]) {
            avgMergeTime += result.time;
            avgMergeIO += result.io;
        }
        avgMergeTime /= REPETITIONS;
        avgMergeIO /= REPETITIONS;
        
        // Calcular promedios para QuickSort
        double avgQuickTime = 0.0;
        double avgQuickIO = 0.0;
        for (const auto& result : quickSortResults[N]) {
            avgQuickTime += result.time;
            avgQuickIO += result.io;
        }
        avgQuickTime /= REPETITIONS;
        avgQuickIO /= REPETITIONS;
        
        // Guardar en archivo CSV
        resultsFile << N << "," 
                   << avgMergeTime << "," 
                   << avgMergeIO << "," 
                   << avgQuickTime << "," 
                   << avgQuickIO << "\n";
        
        // Mostrar en consola
        std::cout << N << "," 
                 << avgMergeTime << "," 
                 << avgMergeIO << "," 
                 << avgQuickTime << "," 
                 << avgQuickIO << std::endl;
    }
    
    resultsFile.close();
    std::cout << "\nResultados guardados en ./results/comparison_results.csv" << std::endl;
}

/**
 * @brief Función principal del programa.
 * 
 * @return int Código de salida (0 si éxito, otro valor si error).
 * 
 * Configura los parámetros iniciales (tamaño de bloque, memoria disponible),
 * calcula la aridad óptima para los algoritmos y ejecuta los experimentos.
 */
int main() {
    // Nombre de archivos
    std::string inputFilename = "./dataExp/input_array.bin";
    std::string outputFilename = "./dataExp/sorted_array";
    
    std::cout << "Iniciando programa de comparación entre Mergesort y Quicksort externo" << std::endl;
    std::cout << "Tamaño de bloque (B): " << B << " bytes" << std::endl;
    std::cout << "Números por bloque (b): " << b << std::endl;
    std::cout << "Memoria disponible (M): " << (50 * 1024 * 1024 / (1024*1024)) << " MB" << std::endl;
    
    // Crear directorios necesarios
    fs::path dataPath("./dataExp");
    if (!fs::exists(dataPath)) {
        fs::create_directories(dataPath);
    }
    
    // Buscar aridad óptima basada en operaciones I/O
    size_t optimalArity = 57;
    std::cout << "La aridad óptima para Mergesort externo es: " << optimalArity << std::endl;
    std::cout << "Esta aridad también define la cantidad de subarreglos para Quicksort externo" << std::endl;
    
    // Ejecutar experimentos comparativos
    runExperiments(optimalArity);
    
    return 0;
}