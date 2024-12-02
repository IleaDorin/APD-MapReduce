#include "structs.h"
#include "functions.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <tbb/concurrent_hash_map.h>
#include <set>

using namespace std;

// Custom comparator for sorting words by file count (descending) and alphabetically
struct WordComparator {
    bool operator()(const std::pair<std::string, int>& a, const std::pair<std::string, int>& b) const {
        if (a.second != b.second) {
            return a.second > b.second; // Sort by file count (descending)
        }
        return a.first < b.first; // Sort alphabetically in case of a tie
    }
};


std::set<int> getPartialResult(
    const tbb::concurrent_hash_map<std::string, std::set<int>>& partialResults, 
    const std::string& word);

void reducerFunction(ThreadArgs* args) {
    // Wait for all the mappers and reducers to reach the barrier
    pthread_barrier_wait(args->barrier);

    for (char letter : *(args->letters)) {
        // Create a filename for each letter
        //std::string filename = "output/" + std::string(1, letter) + ".txt";  //for manual testing
        std::string filename = std::string(1, letter) + ".txt"; // for automated testing
        std::ofstream outputFile(filename);
        if (!outputFile.is_open()) {
            std::cerr << "Failed to open output file: " << filename << "\n";
            return;
        }

        // Collect words starting with the current letter
        std::set<std::pair<std::string, int>, WordComparator> wordList;
        
        for (const auto& [word, fileIDs] : *(args->partialResults)) {
            if (std::tolower(word[0]) == letter) {
                wordList.emplace(word, fileIDs.size()); // Store word and its file count
            }
        }

        // Sort the words: first by file count (descending), then alphabetically
        // std::sort(wordList.begin(), wordList.end(), [](const auto& a, const auto& b) {
        //     if (a.second != b.second) {
        //         return a.second > b.second; // Sort by file count (descending)
        //     }
        //     return a.first < b.first; // Sort alphabetically
        // });


        // Write sorted words to the file
        for (const auto& [word, fileIDs] : wordList) {
            set<int> files = getPartialResult(*(args->partialResults), word);
            outputFile << word << ":[";
            for (auto it = files.begin(); it != files.end(); ++it) {
                outputFile << *it;
                if (std::next(it) != files.end()) { // Check if this is not the last element
                    outputFile << " ";
                }
            }
            outputFile << "]\n"; // Close the list and start a new line
        }

        outputFile.close(); // Close the file after writing
    }
}

std::set<int> getPartialResult(
    const tbb::concurrent_hash_map<std::string, std::set<int>>& partialResults, 
    const std::string& word) {
    tbb::concurrent_hash_map<std::string, std::set<int>>::const_accessor accessor;

    // Attempt to find the word in the concurrent hash map
    if (partialResults.find(accessor, word)) {
        return accessor->second; // Return a copy of the set
    } else {
        std::cerr << "Word not found: " << word << "\n";
        return {}; // Return an empty set if the word is not found
    }
}
