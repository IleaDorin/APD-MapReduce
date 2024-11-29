#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "structs.h"

// Mapper thread function
void mapperFunction(ThreadArgs* args);

// Reducer thread function
void reducerFunction(ThreadArgs* args);

// Writes output to files
void writeOutputFiles(const std::unordered_map<std::string, std::vector<int>>& results);

#endif // FUNCTIONS_H
