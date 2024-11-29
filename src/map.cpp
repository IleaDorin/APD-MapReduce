#include "structs.h"
#include "functions.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <mutex>
#include <unordered_map>

using namespace std;

void mapperFunction(ThreadArgs* args) {
     for (const auto& [file, fileID] : args->files) {
        std::cout << "Processing file: " << file << " (ID: " << fileID << ")\n";
        // Process the file here and update args->partialResults
    }
    
}