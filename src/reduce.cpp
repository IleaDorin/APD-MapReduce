#include "structs.h"
#include "functions.h"
#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

void reducerFunction(ThreadArgs* args) {
    // waite for all the mappers and reducers to be here
    pthread_barrier_wait(args->barrier);

    //demo
    cout << "reducer is running\n";

    // Implementation...
}