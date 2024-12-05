#include "structs.h"
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <cctype>
#include <algorithm>
#include "functions.h"

using namespace std;


map<int, vector<char>> mapReducerToLetters(
    int totalThreads,
    int mapThreads) {

    // letter frequencies in the average English text
    map<char, double> letterFrequencies = {
        {'S', 15.8}, {'A', 11.7}, {'T', 9.1}, {'P', 7.7}, {'C', 8.0},
        {'O', 7.4}, {'B', 7.5}, {'H', 7.0}, {'M', 6.1}, {'I', 6.2},
        {'D', 6.6}, {'R', 4.7}, {'F', 4.2}, {'W', 4.3}, {'L', 3.8},
        {'U', 2.8}, {'G', 2.4}, {'N', 2.3}, {'E', 2.0}, {'V', 1.0},
        {'K', 0.8}, {'J', 0.6}, {'Y', 0.4}, {'Q', 0.1}, {'X', 0.03}, {'Z', 0.03}
    };

    // sorted letters by frequency
    vector<char> sortedLetters = {
        'S', 'A', 'T', 'P', 'C', 'O', 'B', 'H', 'M', 'I',
        'D', 'R', 'F', 'W', 'L', 'U', 'G', 'N', 'E', 'V',
        'K', 'J', 'Y', 'Q', 'X', 'Z'
    };

    int reducerThreads = totalThreads - mapThreads;

    // initialize reducer threads with empty letter ranges
    map<int, vector<char>> reducerMap;
    vector<double> threadLoads(reducerThreads, 0.0); // tracks total load for each thread

    // greedy allocation of letters to reducers
    for (char letter : sortedLetters) {
        // find the reducer thread with the smallest load
        int minThread = min_element(threadLoads.begin(), threadLoads.end()) - threadLoads.begin();
        reducerMap[minThread + mapThreads].push_back(tolower(letter)); // adjust thread ID to start from mapThreads
        threadLoads[minThread] += letterFrequencies.at(letter);
    }

    return reducerMap;
}

void* threadFunction(void* arg) {
    ThreadArgs* args = static_cast<ThreadArgs*>(arg);

    // determine the thread type
    if (args->type == "mapper") {
        mapperFunction(args);
    } else if (args->type == "reducer") {
        reducerFunction(args);
    } else {
        cerr << "Unknown thread type for Thread " << args->id << "\n";
    }

    // exit the thread
    pthread_exit(NULL);
}

vector<pair<string, size_t>> getFileSizes(const vector<string>& files) {
    vector<pair<string, size_t>> fileSizes;
    for (const auto& file : files) {
        size_t size = filesystem::file_size(file);
        fileSizes.emplace_back(file, size);
    }
    return fileSizes;
}

void distributeFilesDynamic(const vector<pair<string, size_t>>& fileSizes,
                            int numMappers,
                            vector<vector<pair<string, int>>>& mapperFiles, unordered_map<string, int>& fileMap) {
    //buckets for each mapper
    mapperFiles.resize(numMappers);
    vector<size_t> bucketSizes(numMappers, 0);

    // sort files by size in descending order
    auto sortedFileSizes = fileSizes;
    sort(sortedFileSizes.begin(), sortedFileSizes.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });

    // distributefiles using a greedy algorithm
    for (const auto& [file, size] : sortedFileSizes) {
        // find the smallest bucket
        auto minBucketIt = min_element(bucketSizes.begin(), bucketSizes.end());
        int minBucketIndex = distance(bucketSizes.begin(), minBucketIt);

        // assgn the file to this bucket
        mapperFiles[minBucketIndex].push_back({file, fileMap[file]});
        bucketSizes[minBucketIndex] += size;
    }
}

vector<string> parseInputFile(const string& filename, unordered_map<string, int>& fileMap) {
    vector<string> fileNames;
    ifstream inputFile(filename);

    if (!inputFile.is_open()) {
        throw runtime_error("Failed to open input file: " + filename);
    }

    int numFiles;
    inputFile >> numFiles; // number of files

    // read each file name
    for (int i = 0; i < numFiles; ++i) {
        string fileName;
        inputFile >> fileName;
        fileNames.push_back(fileName);
        fileMap[fileName] = i + 1;
    }

    inputFile.close();
    return fileNames;
}
