#include "structs.h"
#include "functions.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <mutex>
#include <unordered_map>
#include <cctype>

using namespace std;

std::string normalizeWord(const std::string& inputWord);

void mapperFunction(ThreadArgs* args) {
     for (const auto& [file, fileID] : args->files) {
        std::ifstream inputFile(file);
        if (!inputFile.is_open()) {
            std::cerr << "Failed to open file: " << file << "\n";
            continue;
        }

        std::string word;
        while (inputFile >> word) {
            // Normalize the word
            std::string normalizedWord = normalizeWord(word);

            // Skip empty words after normalization
            if (!normalizedWord.empty()) {
                // Insert or update the word in partialResults
                tbb::concurrent_hash_map<std::string, std::set<int>>::accessor acc;
                args->partialResults->insert(acc, normalizedWord); // Thread-safe insertion
                acc->second.insert(fileID);                     // Add the file ID to the vector
            }
        }
    }

    //demo
    cout << "mapper is finished\n";
    // Wait for all threads to finish
    pthread_barrier_wait(args->barrier);
}

std::string normalizeWord(const std::string& inputWord) {
    std::string normalizedWord;

    // Copy only alphabetic characters to the normalized word
    std::copy_if(inputWord.begin(), inputWord.end(), std::back_inserter(normalizedWord),
                 [](char c) { return std::isalpha(c); });

    // Convert to lowercase
    std::transform(normalizedWord.begin(), normalizedWord.end(), normalizedWord.begin(),
                   [](unsigned char c) { return std::tolower(c); });

    return normalizedWord;
}