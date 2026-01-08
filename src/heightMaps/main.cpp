#include "LazySequence.hpp"
#include "HeightMapGenerator.hpp"
#include <iostream>
#include <fstream>

int main() {
    float startValue;
    size_t size;
    float amplitude;
    size_t numChunks;
    
    std::cout << "Enter initial height value: ";
    std::cin >> startValue;
    
    std::cout << "Enter map size (NxN): ";
    std::cin >> size;
    
    std::cout << "Enter height variation amplitude: ";
    std::cin >> amplitude;
    
    std::cout << "Enter number of chunks to generate: ";
    std::cin >> numChunks;
    
    auto gen = HeightMapGenerator(startValue, size, amplitude);
    
    ArraySequence<chunk> chunks;
    for (size_t chunkIdx = 0; chunkIdx < numChunks; ++chunkIdx) {
        chunks.append(gen.getNext());
    }
    
    std::ofstream csvFile("../src/heightMaps/heightmap.csv");
    
    for (size_t row = 0; row < size; ++row) {
        for (size_t chunkIdx = 0; chunkIdx < numChunks; ++chunkIdx) {
            for (size_t col = 0; col < size; ++col) {
                csvFile << chunks[chunkIdx][col][row];
                if (!(chunkIdx == numChunks - 1 && col == size - 1)) csvFile << ",";
            }
        }
        csvFile << "\n";
    }
    
    csvFile.close();
    std::cout << "Height map saved to ../src/heightMaps/heightmap.csv\n";
    
    std::cout << "Generating PNG visualization...\n";
    int result = system("python3 ../src/heightMaps/plot_heightmap.py ../src/heightMaps/heightmap.csv");
    if (result == 0) {
        std::cout << "PNG visualization generated successfully\n";
    } else {
        std::cout << "Warning: Could not generate PNG (python3/matplotlib required)\n";
    }
    
    return 0;
}