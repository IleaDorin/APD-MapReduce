# MapReduce Framework in Theory

MapReduce is a programming model designed to process large-scale datasets by distributing 
computation across multiple parallel workers. It breaks down into two key phases:

### Map Phase:
- Each worker (Mapper) processes a subset of the input data.
- Transforms the data into intermediate key-value pairs (e.g., `word → file_ids`).
- The output of the Map phase is partitioned and grouped by keys.

### Reduce Phase:
- Reducers aggregate the intermediate key-value pairs from Mappers.
- Apply operations such as summing, grouping, or sorting to produce final results.

---

## My implementation of this model

The program uses this model to create a reverse index for a large pool of words from a large 
pool of files. The executable takes the following parameters:
1. Number of Mapper threads.
2. Number of Reducer threads.
3. A text document containing all the file names.

All the threads are created simultaneously. Mapper threads parse the files and insert the words 
into a `concurrent_map` along with the IDs of the files containing each word. The 
`concurrent_map` ensures safe insertion and prevents race conditions. Reducer threads create 
separate files for each letter and sort the words based on:
1. The number of files containing the word (descending order).
2. Alphabetical order as a tie-breaker.

### Task Distribution
The tasks for both types of threads are distributed using a greedy approach:

- **Mappers**:
  - The main thread distributes the load by sorting the file sizes and assigning the next file 
    in line to the thread with the smallest load.

- **Reducers**:
  - A statistic ("https://en.wikipedia.org/wiki/Letter_frequency") on the percentage of words starting with 
  each letter (e.g., `s → 15.8%`, `a → 11.7%`) is used.
  Letters are distributed to reducers similarly to the mappers, based on load balancing.

### **Synchronization**

- All threads use a barrier passed as part of their arguments.
- **Mappers:** Wait at the barrier after completing their tasks.  
- **Reducers:** Wait at the barrier before starting their tasks.  
- The barrier is initialized with the total number of threads, ensuring that reducers cannot start working until all mappers have reached the barrier.


---

  -"Can we use the concurrent_hash_map from tbb library?"
  -"No, we have concurrent_map at home!"
  -(concurrent map at home:)
## Concurrent_map implemetation
  - The concurrent_map structure is just a list of buckets, each consisting in a unordered_map and a mutex
  (similar to a normal map but for each bucket access, it locks and unlocks it)
  - Each time a maper tries to insert an element into the partialRsult, the word is allocated to a bucket,
  then the bucket is locked, the element gets inserted and the bucket reopens. I tried running the code with
  a variety of buckets and for scalability I initialized the concurrent_map with no_of_threads * 16 based on
  a trial and error approach.

## File Summary

### `main.cpp`
- **Role**: Entry point of the program.
- **Responsibilities**: Creates threads and assigns workloads.

### `structs.h`
- **Role**: Defines the structure for thread arguments.
- **Fields**:
  - **Common**: Thread ID, type of thread, barrier.
  - **Mapper-specific**: Files to parse and the hash map to populate.
  - **Reducer-specific**: Letters assigned and access to the shared hash map.

### `map.cpp`
- **Role**: Parses files and populates the `partialResults` map.

### `reduce.cpp`
- **Role**: Processes the `partialResults` map for assigned letters.
- **Process**:
  1. Reads only words of interest from the map.
  2. Stores words in a set (avoiding duplicates) with file counts.
  3. Uses a custom `WordComparator` for sorting:
     - Number of files first (descending).
     - Alphabetical order second.
  4. Writes results to the corresponding output files.

### `functions.cpp` and `functions.h`
- **Role**: Contains helper functions to declutter the code structure.
- **Details**: See source code for specifics.

---
