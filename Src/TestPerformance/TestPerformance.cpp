#include <assert.h>
#include <stdio.h>

#include "TestPerformance.h"
#include "HashTable/HashTable.h"

void MeasureHashTableListsSizes(size_t hashTableCapacity, HashFuncType hashFunc, 
                                const char* inStreamFileName, const char* outStreamFileName)
{
    HashTableType* hashTable = HashTableCtor(hashTableCapacity, hashFunc);

    FILE* inStream =  fopen(inStreamFileName, "r");
    assert(inStream);

    FILE* outStream = fopen(outStreamFileName, "w");
    assert(outStream);

    char word[256];
    while (true)
    {
        int scanfErr = fscanf(inStream, "%s", word);

        if (scanfErr == EOF)
            break;

        HashTableInsert(hashTable, word, true);
    }
    
    for (size_t i = 0; i < hashTable->numberOfBuckets; ++i)
        fprintf(outStream, "%zu %zu\n", i, hashTable->buckets[i].size);
    
    fclose(inStream);
    fclose(outStream);
    
    HashTableDtor(hashTable);
}