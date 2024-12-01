#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "structs.h"
#include "functions.h"
#include <string>
#include <vector>
#include <filesystem>
#include <fstream>
#include <algorithm>
#include <tbb/concurrent_hash_map.h>
#include <semaphore>

using namespace std;

void* threadFunction(void* arg);
vector<pair<string, size_t>> getFileSizes(const vector<string>& files);
void distributeFilesDynamic(const vector<pair<string, size_t>>& fileSizes,
                            int numMappers,
                            vector<vector<pair<string, int>>>& mapperFiles, unordered_map<string, int>& fileMap);
std::vector<std::string> parseInputFile(const std::string& filename, unordered_map<string, int>& fileMap);

int main(int argc, char *argv[]) {
    
    const int NUM_MAPPERS = atoi(argv[1]);
    const int NUM_REDUCERS = atoi(argv[2]);
    string test_file = argv[3];
    const int TOTAL_THREADS = NUM_MAPPERS + NUM_REDUCERS;
    pthread_barrier_t barrier;
    pthread_barrier_init(&barrier, NULL, TOTAL_THREADS);
    //the conc map shared by all mappers
    tbb::concurrent_hash_map<std::string, std::set<int>> partialResults;

    //map to store the filename and id
    unordered_map<string, int> fileMap;

    //parsing the input file
    std::vector<std::string> fileNames = parseInputFile(test_file, fileMap);
    
    vector<pair<string, size_t>> fileSizes = getFileSizes(fileNames);
    vector<vector<pair<string, int>>> mapperFiles;
    distributeFilesDynamic(fileSizes, NUM_MAPPERS, mapperFiles, fileMap);

    //distributr the letters to the reducers
    map<int, vector<char>> reducerLetters = mapReducerToLetters(TOTAL_THREADS, NUM_MAPPERS);

    pthread_t threads[TOTAL_THREADS];
    ThreadArgs threadArgs[TOTAL_THREADS];
    int r;

    // Creează toate thread-urile
    for (int i = 0; i < TOTAL_THREADS; ++i) {
        
        if (i < NUM_MAPPERS) {
            threadArgs[i] = {i, "mapper", mapperFiles[i], &partialResults, {}, &barrier}; // assign arguments
            // Adaugă fișierele specifice Mapperului
        } else {
            threadArgs[i].id = i;
            threadArgs[i].type = "reducer";
            threadArgs[i].partialResults = &partialResults;
            threadArgs[i].letters = &reducerLetters[i];
            threadArgs[i].barrier = &barrier;
        }

        r = pthread_create(&threads[i], NULL, threadFunction, &threadArgs[i]);
        if (r) {
            printf("Eroare la crearea thread-ului %d\n", i);
            exit(-1);
        }
    }

    // Așteaptă toate thread-urile
    for (int i = 0; i < TOTAL_THREADS; ++i) {
        r = pthread_join(threads[i], NULL);
        if (r) {
            printf("Eroare la join-ul thread-ului %d\n", i);
            exit(-1);
        }
    }

    //demo 
    //writeConcurrentMapToFile(partialResults, "output.txt");

    return 0;
}

void* threadFunction(void* arg) {
    // Cast the argument to ThreadArgs
    ThreadArgs* args = static_cast<ThreadArgs*>(arg);

    // Determine the thread type
    if (args->type == "mapper") {
        mapperFunction(args); // Call Mapper logic
    } else if (args->type == "reducer") {
        //cout << "reducer is running\n";
        reducerFunction(args); // Call Reducer logic
    } else {
        cerr << "Unknown thread type for Thread " << args->id << "\n";
    }

    // Exit the thread
    pthread_exit(NULL);
}

// calculate file sizes
vector<pair<string, size_t>> getFileSizes(const vector<string>& files) {
    vector<pair<string, size_t>> fileSizes;
    for (const auto& file : files) {
        size_t size = std::filesystem::file_size(file);
        fileSizes.emplace_back(file, size);
    }
    return fileSizes;
}

// distribution function
void distributeFilesDynamic(const vector<pair<string, size_t>>& fileSizes,
                            int numMappers,
                            vector<vector<pair<string, int>>>& mapperFiles, unordered_map<string, int>& fileMap) {
    //buckets for each mapper
    mapperFiles.resize(numMappers);
    vector<size_t> bucketSizes(numMappers, 0);

    // sort files by size in descending order
    auto sortedFileSizes = fileSizes;
    std::sort(sortedFileSizes.begin(), sortedFileSizes.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });

    // distributefiles using a greedy algorithm
    for (const auto& [file, size] : sortedFileSizes) {
        // find the bucket with the smallest total size
        auto minBucketIt = std::min_element(bucketSizes.begin(), bucketSizes.end());
        int minBucketIndex = std::distance(bucketSizes.begin(), minBucketIt);

        // assgn the file to this bucket
        mapperFiles[minBucketIndex].push_back({file, fileMap[file]});
        bucketSizes[minBucketIndex] += size;
    }
}

vector<string> parseInputFile(const string& filename, unordered_map<string, int>& fileMap) {
    std::vector<std::string> fileNames;
    std::ifstream inputFile(filename);

    if (!inputFile.is_open()) {
        throw std::runtime_error("Failed to open input file: " + filename);
    }

    int numFiles;
    inputFile >> numFiles; // Read the number of files

    // Read each file name
    for (int i = 0; i < numFiles; ++i) {
        std::string fileName;
        inputFile >> fileName;
        fileNames.push_back(fileName);
        fileMap[fileName] = i + 1;
    }

    inputFile.close();
    return fileNames;
}
