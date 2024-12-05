#include "concurrent_map.h"

// hashes the word inserts it into the assigned bucket
void ConcurrentMap::insertOrUpdate(const std::string& key, int value) {
    size_t bucketIndex = hash(key);
    Bucket& bucket = buckets[bucketIndex];

    pthread_mutex_lock(&bucket.mutex);
    bucket.data[key].insert(value);
    pthread_mutex_unlock(&bucket.mutex);
}

// returns the set of indexed for a given word
std::set<int> ConcurrentMap::find(const std::string& key) {
    size_t bucketIndex = hash(key);
    Bucket& bucket = buckets[bucketIndex];

    pthread_mutex_lock(&bucket.mutex);
    auto it = bucket.data.find(key);
    std::set<int> result = (it != bucket.data.end()) ? it->second : std::set<int>();
    pthread_mutex_unlock(&bucket.mutex);

    return result;
}

// checks if the word is in the map
bool ConcurrentMap::contains(const std::string& key) {
    size_t bucketIndex = hash(key);
    Bucket& bucket = buckets[bucketIndex];

    pthread_mutex_lock(&bucket.mutex);
    bool found = bucket.data.find(key) != bucket.data.end();
    pthread_mutex_unlock(&bucket.mutex);

    return found;
}

// returns all the keys in the map
std::vector<std::string> ConcurrentMap::getKeys() {
    std::vector<std::string> keys;

    for (auto& bucket : buckets) {
        pthread_mutex_lock(&bucket.mutex);
        for (const auto& [key, _] : bucket.data) {
            keys.push_back(key);
        }
        pthread_mutex_unlock(&bucket.mutex);
    }

    return keys;
}
