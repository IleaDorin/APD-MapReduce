#include "structs.h"
#include "functions.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <set>
#include "concurrent_map.h"

using namespace std;

// custom comparator for sorting words
struct WordComparator {
    bool operator()(const pair<string, int>& a, const pair<string, int>& b) const {
        if (a.second != b.second) {
            return a.second > b.second;
        }
        return a.first < b.first;
    }
};

set<int> getPartialResult(ConcurrentMap& partialResults, const string& word);

void reducerFunction(ThreadArgs* args) {
    // wait for all mapper threads to finish
    pthread_barrier_wait(args->barrier);

    for (char letter : *(args->letters)) {
        // create a filename for each letter
        string filename = string(1, letter) + ".txt";
        ofstream outputFile(filename);
        if (!outputFile.is_open()) {
            cerr << "Failed to open output file: " << filename << "\n";
            return;
        }

        // collect words starting with the curr letter
        set<pair<string, int>, WordComparator> wordList;

        auto keys = args->partialResults->getKeys();
        for (const string& word : keys) {
            if (tolower(word[0]) == letter) {
                auto files = getPartialResult(*(args->partialResults), word);
                wordList.emplace(word, files.size()); // store word and file count
            }
        }

        // write sorted words to the file
        for (const auto& [word, fileIDs] : wordList) {
            set<int> files = getPartialResult(*(args->partialResults), word);
            outputFile << word << ":[";
            for (auto it = files.begin(); it != files.end(); ++it) {
                outputFile << *it;
                if (next(it) != files.end()) {
                    outputFile << " ";
                }
            }
            outputFile << "]\n";
        }

        outputFile.close();
    }
}

set<int> getPartialResult(ConcurrentMap& partialResults, const string& word) {
    return partialResults.find(word);
}
