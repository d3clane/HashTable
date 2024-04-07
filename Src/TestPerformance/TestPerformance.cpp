#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <string.h>

#include "TestPerformance.h"
#include "HashTable/HashTable.h"
#include "HashTable/HashTableElem.h"
#include "FastIO/InputOutput.h"
#include "FastIO/StringFuncs.h"

extern "C" uint64_t GetTimeStampCounter();

void MeasureHashTableListsSizes(size_t hashTableCapacity, HashFuncType hashFunc, 
                                const char* inStreamFileName, const char* outStreamFileName)
{
    HashTableType* hashTable = HashTableCtor(hashTableCapacity, hashFunc);

    FILE* inStream =  fopen(inStreamFileName, "r");
    assert(inStream);

    FILE* outStream = fopen(outStreamFileName, "w");
    assert(outStream);

    char word[256];
    size_t numberOfWords = 0;
    while (true)
    {
        int scanfErr = fscanf(inStream, "%s", word);

        if (scanfErr == EOF)
            break;

        HashTableInsert(hashTable, word, true);
        
        ++numberOfWords;
    }
    
    double meanLength = (double)numberOfWords / (double)hashTable->numberOfBuckets;
    double standardDeviation   = 0;

    for (size_t i = 0; i < hashTable->numberOfBuckets; ++i)
    {
        const size_t bucketSize = hashTable->buckets[i].size;
        const double meanAndCurrSub = meanLength - (double)bucketSize;

        standardDeviation += meanAndCurrSub * meanAndCurrSub;
        fprintf(outStream, "%zu %zu\n", i, bucketSize);
    }

    standardDeviation = sqrt(standardDeviation / (double)hashTable->numberOfBuckets);
    printf("In file %s, outfile - %s, \nsample variance - %lf\n", 
            inStreamFileName, outStreamFileName, standardDeviation);

    fclose(inStream);
    fclose(outStream);
    
    HashTableDtor(hashTable);
}

uint64_t HashTableBenchmark(size_t hashTableCapacity, HashFuncType hashFunc,
                        const char* inStreamFileName)
{
    assert(inStreamFileName);

    FILE* inStream = fopen(inStreamFileName, "r");
    assert(inStream);

    HashTableType* hashTable = HashTableCtor(hashTableCapacity, hashFunc);

    char word[256];
    size_t numberOfWords = 0;
    while (true)
    {
        int scanfErr = fscanf(inStream, "%s", word);

        if (scanfErr == EOF)
            break;

        HashTableInsert(hashTable, word, true);
        
        ++numberOfWords;
    }

    fclose(inStream);

    //TextType text = {};
    //TextTypeCtor(&text, inStreamFileName);
    //Replace(text.text, '\n', '\0');

    uint64_t timeSpent = GetTimeStampCounter();

    static const size_t numberOfTestsRun = 10;
    for (size_t testId = 0; testId < numberOfTestsRun; ++testId)
    {
        rewind(inStream);
        while (true)
        {
            int scanfErr = fscanf(inStream, "%s", word);

            if (scanfErr == EOF)
                break;

            HashTableGetValue(hashTable, word);
        }

    }

    timeSpent = GetTimeStampCounter() - timeSpent;

    //TextTypeDtor(&text);
    HashTableDtor(hashTable);

    fclose(inStream);
    
    return timeSpent;
}