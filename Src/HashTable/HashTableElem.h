#ifndef HASH_TABLE_ELEM_H
#define HASH_TABLE_ELEM_H

struct HashTableElemType
{
    char* key;

    bool val;
};

HashTableElemType HashTableElemCpy (const HashTableElemType elem);
HashTableElemType HashTableElemCtor();
void              HashTableElemDtor(HashTableElemType* elem);
HashTableElemType HashTableElemInit(const char* key, bool val);

bool              HashTableElemsCmp(const HashTableElemType elem1,
                                    const HashTableElemType elem2);

#endif