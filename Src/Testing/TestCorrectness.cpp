#include <assert.h>
#include <stdio.h>

#include "TestCorrectness.h"
#include "HashTable/HashTable.h"

void TestCorrectness(HashFuncType hashFunc)
{   
    FILE* inStream = fopen("wordsAll.txt", "r");
    assert(inStream);

    HashTableType* hashTable = HashTableCtor(1e5, hashFunc);

    char word[256];
    while (true)
    {
        int scanfErr = fscanf(inStream, "%s", word);

        if (scanfErr == EOF)
            break;

        HashTableInsert(hashTable, word, true);
    }

    rewind(inStream);

    while (true)
    {
        int scanfErr = fscanf(inStream, "%s", word);

        if (scanfErr == EOF)
            break;

        assert(HashTableGetValue(hashTable, word));
        HashTableErase(hashTable, word);
        assert(!HashTableGetValue(hashTable, word));
    }

    HashTableDtor(hashTable);
}