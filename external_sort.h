#ifndef EXTERNAL_SORT_H
#define EXTERNAL_SORT_H

#include <cstdint>
#include <string>
#include <vector>
#include <ctime>
#include <random>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <chrono>
#include <cstring>

// Structure to store results
struct SortResult {
    double execution_time;  // in seconds
    uint64_t disk_accesses; // count of disk accesses
};

// Constants
static const uint64_t MEMORY_LIMIT = 50 * 1024 * 1024; // 50 MB
static const size_t INT64_SIZE = sizeof(int64_t);

class ExternalSort {
private:
    size_t block_size;          // B - Size of a block in bytes
    size_t memory_limit;        // M - Memory limit in bytes
    size_t numbers_per_block;   // Number of int64_t elements that fit in a block
    size_t arity;               // a - Number of runs to merge at once or number of quicksort partitions
    uint64_t disk_accesses;     // Counter for disk accesses

    // Buffer for reading/writing blocks
    std::vector<int64_t> buffer;

    // Helper methods for disk operations
    void read_block(FILE* file, int64_t* buffer, size_t block_index);
    void write_block(FILE* file, const int64_t* buffer, size_t block_index);
    
    // External Mergesort methods
    void merge_sort_external_recursive(const std::string& input_filename, const std::string& output_filename, size_t n);
    void merge_runs(const std::vector<std::string>& input_files, const std::string& output_file, size_t runs_count);
    
    // External Quicksort methods
    void quick_sort_external_recursive(const std::string& input_filename, const std::string& output_filename, size_t n);
    std::vector<int64_t> select_pivots(const std::string& filename, size_t n);
    void partition_file(const std::string& input_filename, 
                       const std::vector<std::string>& partition_files, 
                       const std::vector<int64_t>& pivots, 
                       size_t n);
    void concatenate_files(const std::vector<std::string>& input_files, const std::string& output_file);
    
    // In-memory sort
    void sort_in_memory(const std::string& input_filename, const std::string& output_filename, size_t n);

public:
    ExternalSort(size_t block_size, size_t memory_limit = MEMORY_LIMIT);
    ~ExternalSort() = default;

    // Set the arity for both algorithms
    void set_arity(size_t new_arity);
    
    // Get current arity
    size_t get_arity() const;
    
    // Get numbers per block
    size_t get_numbers_per_block() const;
    
    // External Mergesort entry point
    SortResult merge_sort_external(const std::string& input_filename, const std::string& output_filename, size_t n);
    
    // External Quicksort entry point
    SortResult quick_sort_external(const std::string& input_filename, const std::string& output_filename, size_t n);
    
    // Find optimal arity for Mergesort
    size_t find_optimal_arity(const std::string& input_filename, size_t n, size_t min_arity = 2);
    
    // Generate random array
    static void generate_random_array(const std::string& filename, size_t n, size_t block_size);
};

#endif // EXTERNAL_SORT_H