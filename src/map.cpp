#include "structs.h"
#include "functions.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <mutex>
#include <unordered_map>
#include <cctype>
#include <algorithm>
#include "concurrent_map.h"

using namespace std;

string normalizeWord(const string& inputWord);

void mapperFunction(ThreadArgs* args) {
     for (const auto& [file, fileID] : args->files) {
        ifstream inputFile(file);
        if (!inputFile.is_open()) {
            cerr << "Failed to open file: " << file << "\n";
            continue;
        }

        string word;
        while (inputFile >> word) {
            // normalize the word
            string normalizedWord = normalizeWord(word);

            // skip empty words after normalization
            if (!normalizedWord.empty()) {
                // insert or update the word in partialResults
                args->partialResults->insertOrUpdate(normalizedWord, fileID);
            }
        }
    }
    // wait for all threads to finish
    pthread_barrier_wait(args->barrier);

    
}

string normalizeWord(const string& inputWord) {
    string normalizedWord;

    // copy only alphabetic characters to the normalized word
    copy_if(inputWord.begin(), inputWord.end(), back_inserter(normalizedWord),
                 [](char c) { return isalpha(c); });

    // convert to lowercase
    transform(normalizedWord.begin(), normalizedWord.end(), normalizedWord.begin(),
                   [](unsigned char c) { return tolower(c); });

    return normalizedWord;
}