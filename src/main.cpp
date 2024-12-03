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

using namespace std;

int main(int argc, char *argv[]) {
    
    const int NUM_MAPPERS = atoi(argv[1]);
    const int NUM_REDUCERS = atoi(argv[2]);
    string test_file = argv[3];
    const int TOTAL_THREADS = NUM_MAPPERS + NUM_REDUCERS;
    pthread_barrier_t barrier;
    pthread_barrier_init(&barrier, NULL, TOTAL_THREADS);
    // the conc map shared by all mappers
    tbb::concurrent_hash_map<string, set<int>> partialResults;

    // map to store the filename and id
    unordered_map<string, int> fileMap;

    // parsing the input file
    vector<string> fileNames = parseInputFile(test_file, fileMap);
    
    vector<pair<string, size_t>> fileSizes = getFileSizes(fileNames);
    vector<vector<pair<string, int>>> mapperFiles;
    distributeFilesDynamic(fileSizes, NUM_MAPPERS, mapperFiles, fileMap);

    // distribute the letters to the reducers
    map<int, vector<char>> reducerLetters = mapReducerToLetters(TOTAL_THREADS, NUM_MAPPERS);

    pthread_t threads[TOTAL_THREADS];
    ThreadArgs threadArgs[TOTAL_THREADS];
    int r;

    // create threads
    for (int i = 0; i < TOTAL_THREADS; ++i) {
        
        if (i < NUM_MAPPERS) {
            threadArgs[i] = {i, "mapper", mapperFiles[i], &partialResults, {}, &barrier};
        } else {
            threadArgs[i].id = i;
            threadArgs[i].type = "reducer";
            threadArgs[i].partialResults = &partialResults;
            threadArgs[i].letters = &reducerLetters[i];
            threadArgs[i].barrier = &barrier;
        }

        r = pthread_create(&threads[i], NULL, threadFunction, &threadArgs[i]);
        if (r) {
            printf("error creating thread: %d\n", i);
            exit(-1);
        }
    }

    // wait for threads to finish
    for (int i = 0; i < TOTAL_THREADS; ++i) {
        r = pthread_join(threads[i], NULL);
        if (r) {
            printf("Error joining thread: %d\n", i);
            exit(-1);
        }
    }

    return 0;
}

