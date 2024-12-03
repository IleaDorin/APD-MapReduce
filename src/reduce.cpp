#include "structs.h"
#include "functions.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <tbb/concurrent_hash_map.h>
#include <set>

using namespace std;

// custom comparator for sorting words 1. count of files 2. alphabetically
struct WordComparator {
    bool operator()(const pair<string, int>& a, const pair<string, int>& b) const {
        if (a.second != b.second) {
            return a.second > b.second;
        }
        return a.first < b.first;
    }
};


set<int> getPartialResult(
    const tbb::concurrent_hash_map<string, set<int>>& partialResults, 
    const string& word);

void reducerFunction(ThreadArgs* args) {
    // wait for all mappers threads to finish
    pthread_barrier_wait(args->barrier);

    for (char letter : *(args->letters)) {
        // create a filename for each letter
        string filename = string(1, letter) + ".txt";
        ofstream outputFile(filename);
        if (!outputFile.is_open()) {
            cerr << "Failed to open output file: " << filename << "\n";
            return;
        }

        // collect words starting with the current letter
        set<pair<string, int>, WordComparator> wordList;
        
        for (const auto& [word, fileIDs] : *(args->partialResults)) {
            if (tolower(word[0]) == letter) {
                // the set will automatically add them in sorted order
                wordList.emplace(word, fileIDs.size()); // store word and its file count
            }
        }

        // write sorted words to the file
        for (const auto& [word, fileIDs] : wordList) {
            set<int> files = getPartialResult(*(args->partialResults), word);
            outputFile << word << ":[";
            for (auto it = files.begin(); it != files.end(); ++it) {
                outputFile << *it;
                if (next(it) != files.end()) { // check if this is not the last element
                    outputFile << " ";
                }
            }
            outputFile << "]\n";
        }

        outputFile.close();
    }
}

set<int> getPartialResult(
    const tbb::concurrent_hash_map<string, set<int>>& partialResults, 
    const string& word) {
    tbb::concurrent_hash_map<string, set<int>>::const_accessor accessor;

    if (partialResults.find(accessor, word)) {
        return accessor->second; // return a copy of the set
    } else {
        cerr << "Word not found: " << word << "\n";
        return {};
    }
}
