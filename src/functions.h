#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "structs.h"
#include <vector>
#include <string>
#include <map>
#include <filesystem>

using namespace std;

void* threadFunction(void* arg);

// Mapper thread function
void mapperFunction(ThreadArgs* args);

// Reducer thread function
void reducerFunction(ThreadArgs* args);

// Dsistributes the letters to the reducers in a greedy way
map<int, vector<char>> mapReducerToLetters(
    int totalThreads,
    int mapThreads);

vector<pair<string, size_t>> getFileSizes(const vector<string>& files);

// Distributes files to mappers in a greedy way
void distributeFilesDynamic(const vector<pair<string, size_t>>& fileSizes,
                            int numMappers,
                            vector<vector<pair<string, int>>>& mapperFiles, unordered_map<string, int>& fileMap);

std::vector<std::string> parseInputFile(const std::string& filename, unordered_map<string, int>& fileMap);

#endif // FUNCTIONS_H
