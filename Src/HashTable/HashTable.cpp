#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "Common/Log.h"
#include "HashTable.h"

static inline size_t HashTableGetBucketPos(HashTableType* table, const char* key);

HashTableType* HashTableCtor(size_t capacity, HashFuncType hashFunc, HashTableErrors* error)
{
    HashTableType* table = (HashTableType*)calloc(1, sizeof(*table));
    
    if (!table)
    {
        if (error) *error = MEM_ALLOC_ERR;

        return nullptr;
    }
        
    table->hashFunc = hashFunc;

    static const size_t primes[] =
    {
        53, 97, 193, 389, 769, 1543, 3079, 6151, 12289, 24593, 49157, 98317, 
        196613, 393241, 786433, 1572869, 3145739, 6291469, 12582917, 25165843,
        50331653, 100663319, 201326611, 402653189, 805306457, 1610612741
    };

    static const size_t primesArrSize = sizeof(primes) / sizeof(*primes);

    table->numberOfBuckets = 0;
    for (size_t i = 0; i < primesArrSize; ++i)
    {
        if (primes[i] >= capacity)
        {
            table->numberOfBuckets = primes[i];
            break;
        }
    }

    if (table->numberOfBuckets == 0)
    {
    #ifdef LOGGING
        LOG_BEGIN();

        Log("No prime number found for table capacity(%zu)\n. " 
            "Using capacity as the size", capacity);

        LOG_END();
    #endif
    
        table->numberOfBuckets = capacity;
    }

    HashTableErrors err    = HashTableErrors::NO_ERR;

    table->buckets         = (HtListType*)calloc(table->numberOfBuckets, sizeof(*table->buckets));
    
    if (!table)
    {
        if (error) *error = MEM_ALLOC_ERR;

        return nullptr;
    }

    HtListErrors htListErr = HtListErrors::NO_ERR;

    for (size_t i = 0; i < table->numberOfBuckets; ++i)
    {
        htListErr = HtListCtor(table->buckets + i);

        if (htListErr != HtListErrors::NO_ERR)
        {
            err = HashTableErrors::HT_LIST_ERR;
            break;
        }
    }

    if (err != HashTableErrors::NO_ERR)
    {
        if (error) *error = err;

        HashTableDtor(table);

        return nullptr;
    }

    return table;
}

void HashTableDtor(HashTableType* table)
{
    assert(table);

    if (table->buckets)
    {
        for (size_t i = 0; i < table->numberOfBuckets; ++i)
            HtListDtor(table->buckets + i);

        free(table->buckets);
    }

    table->buckets         = nullptr;
    table->numberOfBuckets = 0;
    table->hashFunc        = nullptr;

    free(table);
}

void HashTableInsert(HashTableType* table, const char* key, const bool val, 
                     HashTableErrors* error)
{
    assert(table);
    assert(key);

    size_t insertedValPos = 0;
    size_t bucketPos      = HashTableGetBucketPos(table, key);
    
    // fprintf(stderr, "bucket pos - %zu\n", bucketPos);
    HtListErrors htListErr = HtListInsert(table->buckets + bucketPos, 
                                          table->buckets->end, 
                                          HashTableElemInit(key, val), &insertedValPos);
    
#ifdef LOGGING
    Log("bucket pos - %zu\n", bucketPos);
    HT_LIST_DUMP(table->buckets + bucketPos);
#endif

    if (htListErr != HtListErrors::NO_ERR)
    {
        if (error) *error = HashTableErrors::HT_LIST_ERR;
        
        return;
    }
}

void HashTableErase (HashTableType* table, const char* key, HashTableErrors* error)
{
    assert(table);
    assert(key);

    size_t elemPos = 0;
    size_t bucketPos = HashTableGetBucketPos(table, key);
    HtListErrors htListErr = HtListFindElemByKey(table->buckets + bucketPos, 
                                                 key, &elemPos);

    if (htListErr == HtListErrors::NO_ELEMENT_FOUND)
    {
        if (error) *error = HashTableErrors::NO_ELEM_TO_ERASE_ERR;

        return;
    }

    if (htListErr != HtListErrors::NO_ERR)
    {
        if (error) *error = HashTableErrors::HT_LIST_ERR;

        return;
    }

    htListErr = HtListErase(table->buckets + bucketPos, elemPos); //TODO: check err

    if (htListErr != HtListErrors::NO_ERR)
    {
        if (error) *error = HashTableErrors::HT_LIST_ERR;

        return;
    }
}

bool HashTableGetValue(HashTableType* table, const char* key, HashTableErrors* error)
{
    assert(table);
    assert(key);

    size_t elemPos = 0;
    size_t bucketPos = HashTableGetBucketPos(table, key);
    HtListErrors htListErr = HtListFindElemByKey(table->buckets + bucketPos, 
                                                 key, &elemPos);

#ifdef LOGGING
    Log("bucket pos - %zu, found val pos - %zu\n", bucketPos, elemPos);
    HT_LIST_DUMP(table->buckets + bucketPos);
#endif

    if (htListErr == HtListErrors::NO_ELEMENT_FOUND)
    {
        if (error) *error = HashTableErrors::NO_ELEM_TO_ERASE_ERR;

        return false;
    }

    if (htListErr != HtListErrors::NO_ERR)
    {
        if (error) *error = HashTableErrors::HT_LIST_ERR;

        return false;
    }

    HashTableElemType foundValue = {};
    htListErr = HtListGetElem(table->buckets + bucketPos, elemPos, &foundValue);

    if (htListErr != HtListErrors::NO_ERR)
    {
        if (error) *error = HashTableErrors::HT_LIST_ERR;

        HashTableElemDtor(&foundValue);
        return false;
    } 

    bool retValue = foundValue.val;

    HashTableElemDtor(&foundValue);

    return retValue;
}

static inline size_t HashTableGetBucketPos(HashTableType* table, const char* key)
{
    return (size_t)table->hashFunc(key) % table->numberOfBuckets;
}
