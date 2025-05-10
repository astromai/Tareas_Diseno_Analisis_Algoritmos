#include "external_sort.h"

ExternalSort::ExternalSort(size_t block_size, size_t memory_limit) 
    : block_size(block_size), 
      memory_limit(memory_limit), 
      disk_accesses(0) {
    
    // Calculate how many int64_t numbers fit in a block
    numbers_per_block = block_size / INT64_SIZE;
    
    // Initially set arity to 2 (minimum value)
    arity = 2;
    
    // Initialize buffer to hold one block
    buffer.resize(numbers_per_block);
}

void ExternalSort::set_arity(size_t new_arity) {
    arity = new_arity;
}

size_t ExternalSort::get_arity() const {
    return arity;
}

size_t ExternalSort::get_numbers_per_block() const {
    return numbers_per_block;
}

void ExternalSort::read_block(FILE* file, int64_t* buffer, size_t block_index) {
    // Seek to the correct position
    fseek(file, block_index * block_size, SEEK_SET);
    
    // Read one block
    size_t read_items = fread(buffer, INT64_SIZE, numbers_per_block, file);
    
    // Increment disk access counter
    disk_accesses++;
}

void ExternalSort::write_block(FILE* file, const int64_t* buffer, size_t block_index) {
    // Seek to the correct position
    fseek(file, block_index * block_size, SEEK_SET);
    
    // Write one block
    fwrite(buffer, INT64_SIZE, numbers_per_block, file);
    
    // Increment disk access counter
    disk_accesses++;
}

void ExternalSort::sort_in_memory(const std::string& input_filename, const std::string& output_filename, size_t n) {
    // Read the entire file into memory
    std::vector<int64_t> data(n);
    FILE* input_file = fopen(input_filename.c_str(), "rb");
    
    size_t blocks_needed = (n + numbers_per_block - 1) / numbers_per_block;
    for (size_t i = 0; i < blocks_needed; i++) {
        size_t offset = i * numbers_per_block;
        size_t elements_to_read = std::min(numbers_per_block, n - offset);
        read_block(input_file, buffer.data(), i);
        std::copy(buffer.begin(), buffer.begin() + elements_to_read, data.begin() + offset);
    }
    
    fclose(input_file);
    
    // Sort the data in memory
    std::sort(data.begin(), data.end());
    
    // Write the sorted data back to disk
    FILE* output_file = fopen(output_filename.c_str(), "wb");
    
    for (size_t i = 0; i < blocks_needed; i++) {
        size_t offset = i * numbers_per_block;
        size_t elements_to_write = std::min(numbers_per_block, n - offset);
        std::copy(data.begin() + offset, data.begin() + offset + elements_to_write, buffer.begin());
        write_block(output_file, buffer.data(), i);
    }
    
    fclose(output_file);
}

// External Mergesort Implementation

SortResult ExternalSort::merge_sort_external(const std::string& input_filename, const std::string& output_filename, size_t n) {
    // Reset disk access counter
    disk_accesses = 0;
    
    // Start timing
    auto start_time = std::chrono::high_resolution_clock::now();
    
    // Call the recursive function
    merge_sort_external_recursive(input_filename, output_filename, n);
    
    // End timing
    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end_time - start_time;
    
    // Return the results
    SortResult result;
    result.execution_time = duration.count();
    result.disk_accesses = disk_accesses;
    
    return result;
}

void ExternalSort::merge_sort_external_recursive(const std::string& input_filename, const std::string& output_filename, size_t n) {
    // If the file fits in memory, sort it directly
    size_t max_elements_in_memory = memory_limit / INT64_SIZE;
    if (n <= max_elements_in_memory) {
        sort_in_memory(input_filename, output_filename, n);
        return;
    }
    
    // Calculate run size
    size_t run_size = n / arity;
    size_t remaining = n % arity;
    
    // Create temporary files for runs
    std::vector<std::string> run_files(arity);
    for (size_t i = 0; i < arity; i++) {
        run_files[i] = "run_" + std::to_string(i) + ".bin";
    }
    
    // Split input file into runs
    FILE* input_file = fopen(input_filename.c_str(), "rb");
    if (!input_file) {
        std::cerr << "Error opening input file: " << input_filename << std::endl;
        return;
    }
    
    // Process each run
    for (size_t i = 0; i < arity; i++) {
        size_t current_run_size = run_size + (i < remaining ? 1 : 0);
        size_t start_pos = i * run_size + std::min(i, remaining);
        
        // Create a temporary file for this run
        FILE* run_file = fopen(run_files[i].c_str(), "wb");
        if (!run_file) {
            std::cerr << "Error creating run file: " << run_files[i] << std::endl;
            continue;
        }
        
        // Copy data from input file to run file
        size_t blocks_needed = (current_run_size + numbers_per_block - 1) / numbers_per_block;
        for (size_t j = 0; j < blocks_needed; j++) {
            size_t block_index = (start_pos * INT64_SIZE + j * block_size) / block_size;
            read_block(input_file, buffer.data(), block_index);
            
            size_t offset_in_block = (start_pos * INT64_SIZE) % block_size / INT64_SIZE;
            size_t elements_to_copy = std::min(numbers_per_block - offset_in_block, 
                                              current_run_size - j * numbers_per_block);
            
            // Write to run file
            write_block(run_file, buffer.data() + offset_in_block, j);
        }
        
        fclose(run_file);
        
        // Recursively sort this run
        std::string sorted_run = "sorted_run_" + std::to_string(i) + ".bin";
        merge_sort_external_recursive(run_files[i], sorted_run, current_run_size);
        
        // Replace the run file with its sorted version
        remove(run_files[i].c_str());
        rename(sorted_run.c_str(), run_files[i].c_str());
    }
    
    fclose(input_file);
    
    // Merge all sorted runs
    merge_runs(run_files, output_filename, arity);
    
    // Clean up temporary files
    for (const auto& file : run_files) {
        remove(file.c_str());
    }
}

void ExternalSort::merge_runs(const std::vector<std::string>& input_files, const std::string& output_file, size_t runs_count) {
    // Open all input files
    std::vector<FILE*> files(runs_count);
    for (size_t i = 0; i < runs_count; i++) {
        files[i] = fopen(input_files[i].c_str(), "rb");
        if (!files[i]) {
            std::cerr << "Error opening file: " << input_files[i] << std::endl;
            
            // Close already opened files
            for (size_t j = 0; j < i; j++) {
                fclose(files[j]);
            }
            return;
        }
    }
    
    // Create output file
    FILE* out_file = fopen(output_file.c_str(), "wb");
    if (!out_file) {
        std::cerr << "Error creating output file: " << output_file << std::endl;
        
        // Close all input files
        for (auto& file : files) {
            fclose(file);
        }
        return;
    }
    
    // Allocate memory for buffers
    size_t buffer_size = memory_limit / (runs_count + 1) / INT64_SIZE;
    std::vector<std::vector<int64_t>> input_buffers(runs_count, std::vector<int64_t>(buffer_size));
    std::vector<size_t> buffer_pos(runs_count, 0);
    std::vector<size_t> buffer_size_actual(runs_count, 0);
    std::vector<bool> file_exhausted(runs_count, false);
    
    // Output buffer
    std::vector<int64_t> output_buffer(buffer_size);
    size_t output_pos = 0;
    
    // Fill all input buffers initially
    for (size_t i = 0; i < runs_count; i++) {
        size_t read_items = fread(input_buffers[i].data(), INT64_SIZE, buffer_size, files[i]);
        buffer_size_actual[i] = read_items;
        disk_accesses++;
        
        if (read_items < buffer_size) {
            if (feof(files[i])) {
                if (read_items == 0) {
                    file_exhausted[i] = true;
                }
            } else {
                std::cerr << "Error reading from file: " << input_files[i] << std::endl;
            }
        }
    }
    
    // Merge until all files are exhausted
    while (true) {
        // Find the smallest element
        int64_t min_val = INT64_MAX;
        size_t min_idx = -1;
        
        for (size_t i = 0; i < runs_count; i++) {
            if (!file_exhausted[i] && buffer_pos[i] < buffer_size_actual[i]) {
                if (input_buffers[i][buffer_pos[i]] < min_val) {
                    min_val = input_buffers[i][buffer_pos[i]];
                    min_idx = i;
                }
            }
        }
        
        // If no more elements, we're done
        if (min_idx == -1) break;
        
        // Add to output buffer
        output_buffer[output_pos++] = min_val;
        buffer_pos[min_idx]++;
        
        // If output buffer is full, write it to disk
        if (output_pos == buffer_size) {
            fwrite(output_buffer.data(), INT64_SIZE, buffer_size, out_file);
            disk_accesses++;
            output_pos = 0;
        }
        
        // If the buffer for the file is exhausted, refill it
        if (buffer_pos[min_idx] == buffer_size_actual[min_idx]) {
            size_t read_items = fread(input_buffers[min_idx].data(), INT64_SIZE, buffer_size, files[min_idx]);
            buffer_size_actual[min_idx] = read_items;
            disk_accesses++;
            buffer_pos[min_idx] = 0;
            
            if (read_items < buffer_size) {
                if (feof(files[min_idx])) {
                    if (read_items == 0) {
                        file_exhausted[min_idx] = true;
                    }
                } else {
                    std::cerr << "Error reading from file: " << input_files[min_idx] << std::endl;
                }
            }
        }
    }
    
    // Write any remaining data in the output buffer
    if (output_pos > 0) {
        fwrite(output_buffer.data(), INT64_SIZE, output_pos, out_file);
        disk_accesses++;
    }
    
    // Close all files
    for (auto& file : files) {
        fclose(file);
    }
    fclose(out_file);
}

// External Quicksort Implementation

SortResult ExternalSort::quick_sort_external(const std::string& input_filename, const std::string& output_filename, size_t n) {
    // Reset disk access counter
    disk_accesses = 0;
    
    // Start timing
    auto start_time = std::chrono::high_resolution_clock::now();
    
    // Call the recursive function
    quick_sort_external_recursive(input_filename, output_filename, n);
    
    // End timing
    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end_time - start_time;
    
    // Return the results
    SortResult result;
    result.execution_time = duration.count();
    result.disk_accesses = disk_accesses;
    
    return result;
}

void ExternalSort::quick_sort_external_recursive(const std::string& input_filename, const std::string& output_filename, size_t n) {
    // If the file fits in memory, sort it directly
    size_t max_elements_in_memory = memory_limit / INT64_SIZE;
    if (n <= max_elements_in_memory) {
        sort_in_memory(input_filename, output_filename, n);
        return;
    }
    
    // Select pivots
    std::vector<int64_t> pivots = select_pivots(input_filename, n);
    
    // Create temporary files for partitions
    std::vector<std::string> partition_files(arity);
    for (size_t i = 0; i < arity; i++) {
        partition_files[i] = "partition_" + std::to_string(i) + ".bin";
    }
    
    // Partition the input file
    partition_file(input_filename, partition_files, pivots, n);
    
    // Get the sizes of each partition
    std::vector<size_t> partition_sizes(arity);
    for (size_t i = 0; i < arity; i++) {
        FILE* file = fopen(partition_files[i].c_str(), "rb");
        if (file) {
            fseek(file, 0, SEEK_END);
            partition_sizes[i] = ftell(file) / INT64_SIZE;
            fclose(file);
        }
    }
    
    // Sort each partition recursively
    for (size_t i = 0; i < arity; i++) {
        if (partition_sizes[i] > 0) {
            std::string sorted_partition = "sorted_partition_" + std::to_string(i) + ".bin";
            quick_sort_external_recursive(partition_files[i], sorted_partition, partition_sizes[i]);
            
            // Replace the partition file with its sorted version
            remove(partition_files[i].c_str());
            rename(sorted_partition.c_str(), partition_files[i].c_str());
        }
    }
    
    // Concatenate all sorted partitions
    concatenate_files(partition_files, output_filename);
    
    // Clean up temporary files
    for (const auto& file : partition_files) {
        remove(file.c_str());
    }
}

std::vector<int64_t> ExternalSort::select_pivots(const std::string& filename, size_t n) {
    std::vector<int64_t> pivots(arity - 1);
    
    // Open the input file
    FILE* file = fopen(filename.c_str(), "rb");
    if (!file) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return pivots;
    }
    
    // Choose a random block index
    std::random_device rd;
    std::mt19937 gen(rd());
    size_t total_blocks = (n + numbers_per_block - 1) / numbers_per_block;
    std::uniform_int_distribution<size_t> dist(0, total_blocks - 1);
    size_t random_block = dist(gen);
    
    // Read the random block
    std::vector<int64_t> block_data(numbers_per_block);
    read_block(file, block_data.data(), random_block);
    fclose(file);
    
    // Adjust the size of the block data to not exceed the number of elements
    size_t actual_block_size = std::min(numbers_per_block, n - random_block * numbers_per_block);
    block_data.resize(actual_block_size);
    
    // If the block doesn't have enough elements, we'll repeat them
    if (actual_block_size < arity - 1) {
        size_t repeat_count = (arity - 2) / actual_block_size + 1;
        for (size_t i = 0; i < repeat_count; i++) {
            block_data.insert(block_data.end(), block_data.begin(), block_data.begin() + std::min(actual_block_size, arity - 1 - block_data.size()));
        }
    }
    
    // Randomly select arity-1 elements from the block
    std::shuffle(block_data.begin(), block_data.end(), gen);
    
    // Take the first arity-1 elements
    for (size_t i = 0; i < arity - 1; i++) {
        pivots[i] = block_data[i];
    }
    
    // Sort the pivots
    std::sort(pivots.begin(), pivots.end());
    
    return pivots;
}

void ExternalSort::partition_file(const std::string& input_filename, 
                                 const std::vector<std::string>& partition_files, 
                                 const std::vector<int64_t>& pivots, 
                                 size_t n) {
    // Open input file
    FILE* input_file = fopen(input_filename.c_str(), "rb");
    if (!input_file) {
        std::cerr << "Error opening input file: " << input_filename << std::endl;
        return;
    }
    
    // Open all partition files
    std::vector<FILE*> output_files(arity);
    for (size_t i = 0; i < arity; i++) {
        output_files[i] = fopen(partition_files[i].c_str(), "wb");
        if (!output_files[i]) {
            std::cerr << "Error creating partition file: " << partition_files[i] << std::endl;
            
            // Close already opened files
            for (size_t j = 0; j < i; j++) {
                fclose(output_files[j]);
            }
            fclose(input_file);
            return;
        }
    }
    
    // Allocate buffers for each partition
    size_t buffer_size = memory_limit / (arity + 1) / INT64_SIZE;
    std::vector<std::vector<int64_t>> output_buffers(arity, std::vector<int64_t>(buffer_size));
    std::vector<size_t> buffer_pos(arity, 0);
    
    // Input buffer
    std::vector<int64_t> input_buffer(buffer_size);
    
    // Process the input file in blocks
    size_t blocks_count = (n + buffer_size - 1) / buffer_size;
    for (size_t block = 0; block < blocks_count; block++) {
        // Read a block from input file
        size_t items_to_read = std::min(buffer_size, n - block * buffer_size);
        size_t read_items = fread(input_buffer.data(), INT64_SIZE, items_to_read, input_file);
        disk_accesses++;
        
        if (read_items < items_to_read) {
            std::cerr << "Error reading from input file" << std::endl;
            break;
        }
        
        // Process each element in the block
        for (size_t i = 0; i < read_items; i++) {
            // Find the partition for this element
            size_t partition = 0;
            while (partition < arity - 1 && input_buffer[i] > pivots[partition]) {
                partition++;
            }
            
            // Add to the corresponding buffer
            output_buffers[partition][buffer_pos[partition]++] = input_buffer[i];
            
            // If buffer is full, write it to disk
            if (buffer_pos[partition] == buffer_size) {
                fwrite(output_buffers[partition].data(), INT64_SIZE, buffer_size, output_files[partition]);
                disk_accesses++;
                buffer_pos[partition] = 0;
            }
        }
    }
    
    // Write any remaining data in the buffers
    for (size_t i = 0; i < arity; i++) {
        if (buffer_pos[i] > 0) {
            fwrite(output_buffers[i].data(), INT64_SIZE, buffer_pos[i], output_files[i]);
            disk_accesses++;
        }
    }
    
    // Close all files
    fclose(input_file);
    for (auto& file : output_files) {
        fclose(file);
    }
}

void ExternalSort::concatenate_files(const std::vector<std::string>& input_files, const std::string& output_file) {
    // Open output file
    FILE* out_file = fopen(output_file.c_str(), "wb");
    if (!out_file) {
        std::cerr << "Error creating output file: " << output_file << std::endl;
        return;
    }
    
    // Process each input file
    for (const auto& file_name : input_files) {
        FILE* in_file = fopen(file_name.c_str(), "rb");
        if (!in_file) {
            std::cerr << "Error opening file: " << file_name << std::endl;
            continue;
        }
        
        // Get file size
        fseek(in_file, 0, SEEK_END);
        size_t file_size = ftell(in_file);
        fseek(in_file, 0, SEEK_SET);
        
        // If file is empty, skip
        if (file_size == 0) {
            fclose(in_file);
            continue;
        }
        
        // Copy data in blocks
        size_t blocks_count = (file_size + block_size - 1) / block_size;
        for (size_t i = 0; i < blocks_count; i++) {
            read_block(in_file, buffer.data(), i);
            write_block(out_file, buffer.data(), ftell(out_file) / block_size);
        }
        
        fclose(in_file);
    }
    
    fclose(out_file);
}

size_t ExternalSort::find_optimal_arity(const std::string& input_filename, size_t n, size_t min_arity) {
    // Maximum arity: number of int64_t values that fit in a block
    size_t max_arity = std::min(numbers_per_block, n);
    
    // Binary search parameters
    size_t left = min_arity;
    size_t right = max_arity;
    size_t best_arity = min_arity;
    double best_time = std::numeric_limits<double>::max();
    
    std::cout << "Searching for optimal arity between " << left << " and " << right << std::endl;
    
    // Perform binary search
    while (left <= right) {
        size_t mid = left + (right - left) / 2;
        
        // Set the current arity
        set_arity(mid);
        
        // Measure the performance
        std::string output_file = "optimal_arity_test_" + std::to_string(mid) + ".bin";
        SortResult result = merge_sort_external(input_filename, output_file, n);
        
        std::cout << "Tested arity " << mid << ": time = " << result.execution_time 
                  << "s, disk accesses = " << result.disk_accesses << std::endl;
        
        // Clean up temporary file
        remove(output_file.c_str());
        
        // Update best arity if this is better
        if (result.execution_time < best_time) {
            best_time = result.execution_time;
            best_arity = mid;
            
            // If we're at the midpoint, try both directions
            if (mid > left && mid < right) {
                // Try increasing arity
                left = mid + 1;
            } else if (mid == left) {
                left = mid + 1;
            } else if (mid == right) {
                right = mid - 1;
            }
        } else {
            // If performance is worse, search in the other direction
            if (mid > best_arity) {
                right = mid - 1;
            } else {
                left = mid + 1;
            }
        }
    }
    
    // Reset to the best arity
    set_arity(best_arity);
    std::cout << "Optimal arity found: " << best_arity << std::endl;
    
    return best_arity;
}

void ExternalSort::generate_random_array(const std::string& filename, size_t n, size_t block_size) {
    // Open file for writing
    FILE* file = fopen(filename.c_str(), "wb");
    if (!file) {
        std::cerr << "Error creating file: " << filename << std::endl;
        return;
    }
    
    // Initialize random number generator
    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<int64_t> dist(INT64_MIN, INT64_MAX);
    
    // Calculate numbers per block
    size_t numbers_per_block = block_size / sizeof(int64_t);
    
    // Generate and write random numbers in blocks
    std::vector<int64_t> buffer(numbers_per_block);
    
    size_t blocks_count = (n + numbers_per_block - 1) / numbers_per_block;
    for (size_t i = 0; i < blocks_count; i++) {
        size_t elements_in_block = std::min(numbers_per_block, n - i * numbers_per_block);
        
        // Generate random numbers for this block
        for (size_t j = 0; j < elements_in_block; j++) {
            buffer[j] = dist(gen);
        }
        
        // Write block to file
        fwrite(buffer.data(), sizeof(int64_t), elements_in_block, file);
    }
    
    fclose(file);
}