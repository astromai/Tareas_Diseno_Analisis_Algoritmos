#ifndef IOSTATS_H
#define IOSTATS_H

#include <cstddef>
#include <vector>
#include <fstream>
#include "constants.h"

struct IOStats {
    size_t reads = 0;
    size_t writes = 0;
    
    size_t total() const;
    void reset();
};

template<typename T>
size_t readBlock(std::ifstream& file, std::vector<T>& buffer, size_t count, IOStats& stats);

template<typename T>
void writeBlock(std::ofstream& file, const std::vector<T>& buffer, IOStats& stats);

#endif