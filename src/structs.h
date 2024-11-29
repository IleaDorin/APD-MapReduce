#ifndef STRUCTS_H
#define STRUCTS_H

#include <string>
#include <vector>
#include <set>
#include <unordered_map>
#include <tbb/concurrent_hash_map.h>

// Arguments passed to each thread
struct ThreadArgs {
    int id;                      // Thread ID
    std::string type;            // "mapper" or "reducer"
    std::vector<std::pair<std::string, int>> files; // List of files (for mappers)
    tbb::concurrent_hash_map<std::string, std::set<int>>* partialResults;
    std::unordered_map<std::string, std::vector<int>>* finalResults; // Reducer results
};

#endif // STRUCTS_H
