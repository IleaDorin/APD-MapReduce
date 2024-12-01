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
#include <map>
#include <cctype>

using namespace std;

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
}


map<int, vector<char>> mapReducerToLetters(
    int totalThreads,
    int mapThreads) {

    // Letter frequencies (example)
    map<char, double> letterFrequencies = {
        {'S', 15.8}, {'A', 11.7}, {'T', 9.1}, {'P', 7.7}, {'C', 8.0},
        {'O', 7.4}, {'B', 7.5}, {'H', 7.0}, {'M', 6.1}, {'I', 6.2},
        {'D', 6.6}, {'R', 4.7}, {'F', 4.2}, {'W', 4.3}, {'L', 3.8},
        {'U', 2.8}, {'G', 2.4}, {'N', 2.3}, {'E', 2.0}, {'V', 1.0},
        {'K', 0.8}, {'J', 0.6}, {'Y', 0.4}, {'Q', 0.1}, {'X', 0.03}, {'Z', 0.03}
    };

    // Pre-sorted letters by frequency
    vector<char> sortedLetters = {
        'S', 'A', 'T', 'P', 'C', 'O', 'B', 'H', 'M', 'I',
        'D', 'R', 'F', 'W', 'L', 'U', 'G', 'N', 'E', 'V',
        'K', 'J', 'Y', 'Q', 'X', 'Z'
    };

    int reducerThreads = totalThreads - mapThreads;

    // Initialize reducer threads with empty letter ranges
    map<int, vector<char>> reducerMap;
    vector<double> threadLoads(reducerThreads, 0.0); // Tracks total load for each thread

    // Greedy allocation of letters to reducers
    for (char letter : sortedLetters) {
        // Find the reducer thread with the smallest load
        int minThread = min_element(threadLoads.begin(), threadLoads.end()) - threadLoads.begin();
        reducerMap[minThread + mapThreads].push_back(tolower(letter)); // Adjust thread ID to start from mapThreads
        threadLoads[minThread] += letterFrequencies.at(letter);
    }

    return reducerMap;
}
