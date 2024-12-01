#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "structs.h"
#include <vector>
#include <string>
#include <map>

using namespace std;

// Mapper thread function
void mapperFunction(ThreadArgs* args);

// Reducer thread function
void reducerFunction(ThreadArgs* args);

// Writes output to files
void writeOutputFiles(const std::unordered_map<std::string, std::vector<int>>& results);

//demo
void writeConcurrentMapToFile(
    const tbb::concurrent_hash_map<std::string, std::set<int>>& concurrentMap,
    const std::string& outputFilename);

map<int, vector<char>> mapReducerToLetters(
    int totalThreads,
    int mapThreads);

#endif // FUNCTIONS_H
