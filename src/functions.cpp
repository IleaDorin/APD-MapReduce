#include "structs.h"
//#include <fstream>

// void writeOutputFiles(const std::unordered_map<std::string, std::vector<int>>& results) {
//     // Write results to a.txt, b.txt, ...
// }

#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <tbb/concurrent_hash_map.h>

// Function to write the content of the concurrent map to a file
void writeConcurrentMapToFile(
    const tbb::concurrent_hash_map<std::string, std::set<int>>& concurrentMap,
    const std::string& outputFilename) {
    std::ofstream outputFile(outputFilename);

    // Check if the file opened successfully
    if (!outputFile.is_open()) {
        std::cerr << "Failed to open output file: " << outputFilename << "\n";
        return;
    }

    // Iterate over the concurrent map and write to the file
    for (const auto& [word, fileIDs] : concurrentMap) {
        outputFile << word << ": ";
        for (const int fileID : fileIDs) {
            outputFile << fileID << " ";
        }
        outputFile << "\n"; // Newline after each word entry
    }

    outputFile.close();
    std::cout << "Results successfully written to " << outputFilename << "\n";
}

