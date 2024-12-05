#pragma once

#include <vector>
#include <string>
#include <set>
#include <unordered_map>
#include <pthread.h>

class ConcurrentMap {
public:
    explicit ConcurrentMap(size_t numBuckets) : buckets(numBuckets) {}

    void insertOrUpdate(const std::string& key, int value);
    std::set<int> find(const std::string& key);
    bool contains(const std::string& key);
    std::vector<std::string> getKeys();

private:
    struct Bucket {
        std::unordered_map<std::string, std::set<int>> data;
        pthread_mutex_t mutex;

        Bucket() { pthread_mutex_init(&mutex, nullptr); }
        ~Bucket() { pthread_mutex_destroy(&mutex); }
    };

    std::vector<Bucket> buckets;

    size_t hash(const std::string& key) {
        return std::hash<std::string>{}(key) % buckets.size();
    }
};
