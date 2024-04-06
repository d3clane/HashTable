#ifndef TEST_PERFORMANCE_H
#define TEST_PERFORMANCE_H

#include <stddef.h>

#include "Hashes/Hashes.h"

void MeasureHashTableListsSizes(size_t hashTableCapacity, HashFuncType hashFunc, 
                                const char* inStreamFileName, const char* outStreamFileName);

void HashTableBenchmark(size_t hashTableCapacity, HashFuncType hashFunc,
                                const char* inStreamFileName);
                                
#endif