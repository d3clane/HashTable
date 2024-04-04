#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <stddef.h>


#include "HashTable/HashTableList/HashTableList.h"
#include "Hashes/Hashes.h"

struct HashTableType
{
    HtListType* buckets; // array of lists
    size_t      numberOfBuckets;

    HashFuncType hashFunc;
};

enum HashTableErrors
{
    NO_ERR,

    HT_LIST_ERR,

    MEM_ALLOC_ERR,

    NO_ELEM_TO_ERASE_ERR,
};

HashTableType* HashTableCtor(size_t capacity, HashFuncType hashFunc, 
                             HashTableErrors* error = nullptr);
void           HashTableDtor(HashTableType* table);

void HashTableInsert(HashTableType* table, const char* key, const bool val, 
                     HashTableErrors* error = nullptr);

void HashTableErase     (HashTableType* table, const char* key, HashTableErrors* error = nullptr);
bool HashTableGetValue  (HashTableType* table, const char* key, HashTableErrors* error = nullptr);

#endif