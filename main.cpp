#include "external_sort.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <string>
#include <chrono>
#include <cstring>
#include <unistd.h>

// Function to get the block size of the filesystem
size_t get_block_size() {
    // Default block size (4KB) if we can't determine it
    size_t block_size = 4096;
    
    #ifdef _SC_PAGESIZE
    // Try to get the page size from the system
    long page_size = sysconf(_SC_PAGESIZE);
    if (page_size > 0) {
        block_size = page_size;
    }
    #endif
    
    return block_size;
}

// Function to format a number with commas
std::string format_with_commas(uint64_t value) {
    std::stringstream ss;
    ss.imbue(std::locale(""));
    ss << std::fixed << value;
    return ss.str();
}

// Function to run a single experiment
void run_experiment(ExternalSort& sorter, size_t n, size_t repeat_count, std::ofstream& results_file) {
    std::cout << "\nRunning experiment with N = " << format_with_commas(n) << " (x" << repeat_count << " times)..." << std::endl;
    results_file << n;
    
    // Variables to track total time and disk accesses
    double total_merge_time = 0;
    double total_quick_time = 0;
    uint64_t total_merge_accesses = 0;
    uint64_t total_quick_accesses = 0;
    
    // Run the experiment multiple times
    for (size_t i = 0; i < repeat_count; i++) {
        std::cout << "  Run " << (i+1) << "/" << repeat_count << ": ";
        
        // Generate a random array
        std::string input_file = "input_" + std::to_string(n) + "_" + std::to_string(i) + ".bin";
        ExternalSort::generate_random_array(input_file, n, sorter.get_numbers_per_block() * sizeof(int64_t));
        
        // Run Mergesort
        std::string merge_output = "merge_" + std::to_string(n) + "_" + std::to_string(i) + ".bin";
        std::cout << "Mergesort... ";
        SortResult merge_result = sorter.merge_sort_external(input_file, merge_output, n);
        total_merge_time += merge_result.execution_time;
        total_merge_accesses += merge_result.disk_accesses;
        
        // Run Quicksort
        std::string quick_output = "quick_" + std::to_string(n) + "_" + std::to_string(i) + ".bin";
        std::cout << "Quicksort... ";
        SortResult quick_result = sorter.quick_sort_external(input_file, quick_output, n);
        total_quick_time += quick_result.execution_time;
        total_quick_accesses += quick_result.disk_accesses;
        
        std::cout << "Done!" << std::endl;
        
        // Clean up temporary files
        remove(input_file.c_str());
        remove(merge_output.c_str());
        remove(quick_output.c_str());
    }
    
    // Calculate averages
    double avg_merge_time = total_merge_time / repeat_count;
    double avg_quick_time = total_quick_time / repeat_count;
    uint64_t avg_merge_accesses = total_merge_accesses / repeat_count;
    uint64_t avg_quick_accesses = total_quick_accesses / repeat_count;
    
    // Print and save results
    std::cout << "  Average results:" << std::endl;
    std::cout << "    Mergesort: " << std::fixed << std::setprecision(3) << avg_merge_time << " seconds, " 
              << format_with_commas(avg_merge_accesses) << " disk accesses" << std::endl;
    std::cout << "    Quicksort: " << std::fixed << std::setprecision(3) << avg_quick_time << " seconds, " 
              << format_with_commas(avg_quick_accesses) << " disk accesses" << std::endl;
    
    // Save to CSV file
    results_file << "," << avg_merge_time << "," << avg_merge_accesses 
                << "," << avg_quick_time << "," << avg_quick_accesses << std::endl;
}

// Function to find optimal arity
size_t find_optimal_arity(size_t block_size) {
    std::cout << "\nFinding optimal arity..." << std::endl;
    
    // Create a sorter instance
    ExternalSort sorter(block_size);
    
    // Generate a test array of size 60M
    const size_t N = 60 * 1024 * 1024; // 60M elements
    std::string test_file = "arity_test.bin";
    ExternalSort::generate_random_array(test_file, N, sorter.get_numbers_per_block() * sizeof(int64_t));
    
    // Find the optimal arity
    size_t optimal_arity = sorter.find_optimal_arity(test_file, N);
    
    // Clean up
    remove(test_file.c_str());
    
    return optimal_arity;
}

void print_usage() {
    std::cout << "Usage:" << std::endl;
    std::cout << "  ./external_sort find_arity       - Find optimal arity only" << std::endl;
    std::cout << "  ./external_sort run_tests <a>    - Run experiments with specified arity" << std::endl;
    std::cout << "  ./external_sort full             - Find optimal arity and run experiments" << std::endl;
}

int main(int argc, char** argv) {
    if (argc < 2) {
        print_usage();
        return 1;
    }
    
    // Get block size
    size_t block_size = get_block_size();
    std::cout << "Block size: " << block_size << " bytes" << std::endl;
    
    // Parse command
    std::string command = argv[1];
    
    // Variables
    size_t arity = 0;
    
    if (command == "find_arity") {
        // Find optimal arity only
        arity = find_optimal_arity(block_size);
        std::cout << "\nOptimal arity: " << arity << std::endl;
        
    } else if (command == "run_tests") {
        // Run experiments with specified arity
        if (argc < 3) {
            std::cerr << "Error: Arity value required for run_tests" << std::endl;
            print_usage();
            return 1;
        }
        
        arity = std::stoul(argv[2]);
        std::cout << "Using arity: " << arity << std::endl;
        
        // Create results file
        std::ofstream results_file("results.csv");
        results_file << "N,MergeSort_Time,MergeSort_DiskAccesses,QuickSort_Time,QuickSort_DiskAccesses" << std::endl;
        
        // Create sorter
        ExternalSort sorter(block_size);
        sorter.set_arity(arity);
        
        // Run experiments with different array sizes
        const size_t M = 1024 * 1024; // 1M elements
        const size_t repeat_count = 5; // Number of times to repeat each experiment
        
        // Run for each size from 4M to 60M in increments of 4M
        for (size_t n = 4 * M; n <= 60 * M; n += 4 * M) {
            run_experiment(sorter, n, repeat_count, results_file);
        }
        
        results_file.close();
        std::cout << "\nResults saved to results.csv" << std::endl;
        
    } else if (command == "full") {
        // Find optimal arity and run experiments
        arity = find_optimal_arity(block_size);
        std::cout << "\nOptimal arity: " << arity << std::endl;
        
        // Create results file
        std::ofstream results_file("results.csv");
        results_file << "N,MergeSort_Time,MergeSort_DiskAccesses,QuickSort_Time,QuickSort_DiskAccesses" << std::endl;
        
        // Create sorter
        ExternalSort sorter(block_size);
        sorter.set_arity(arity);
        
        // Run experiments with different array sizes
        const size_t M = 1024 * 1024; // 1M elements
        const size_t repeat_count = 5; // Number of times to repeat each experiment
        
        // Run for each size from 4M to 60M in increments of 4M
        for (size_t n = 4 * M; n <= 60 * M; n += 4 * M) {
            run_experiment(sorter, n, repeat_count, results_file);
        }
        
        results_file.close();
        std::cout << "\nResults saved to results.csv" << std::endl;
        
    } else {
        std::cerr << "Error: Unknown command: " << command << std::endl;
        print_usage();
        return 1;
    }
    
    return 0;
}