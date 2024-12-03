#ifndef STRUCTS_H
#define STRUCTS_H

#include <string>
#include <vector>
#include <set>
#include <unordered_map>
#include <tbb/concurrent_hash_map.h>

// arguments passed to each thread
struct ThreadArgs {
    int id;                      // thread ID
    std::string type;            // "mapper" or "reducer"
    std::vector<std::pair<std::string, int>> files; // list of files (for mappers)
    
    // mapperst create it and reducers read from it
    tbb::concurrent_hash_map<std::string, std::set<int>>* partialResults;
    std::vector<char>* letters;  // letter ranges (for reducers)
    pthread_barrier_t* barrier;
};

#endif // STRUCTS_H
