#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "UnitTestHashMap.h"
#include "HashTable/HashTable.h"

void UnitTestHashMap(HashFuncType hashFunc, const char* testsFileName)
{   
    assert(testsFileName);
    
    FILE* inStream = fopen(testsFileName, "r");
    assert(inStream);

    HashTableType* hashTable = HashTableCtor(1e5, hashFunc);

    char word[256];
    while (true)
    {
        memset(word, 0, 256);
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

        if (!HashTableGetValue(hashTable, word))
        {
            fprintf(stderr, "word - %s\n", word);
        }
        assert(HashTableGetValue(hashTable, word));
        HashTableErase(hashTable, word);
        assert(!HashTableGetValue(hashTable, word));
    }

    fclose(inStream);
    HashTableDtor(hashTable);
}