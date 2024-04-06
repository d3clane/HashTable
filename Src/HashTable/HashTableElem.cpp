#include <assert.h>
#include <string.h>
#include <stdlib.h>

#include "HashTableElem.h"

HashTableElemType HashTableElemCpy(const HashTableElemType elem)
{
    HashTableElemType elemCopy = {};

    elemCopy.key = elem.key ? strdup(elem.key) : nullptr;
    elemCopy.val = elem.val;

    return elemCopy;
}

HashTableElemType HashTableElemCtor()
{
    HashTableElemType elem = {};

    elem.key = nullptr;
    elem.val = 0;

    return elem;
}

void HashTableElemDtor(HashTableElemType* elem)
{
    assert(elem);

    if (elem->key)
        free(elem->key);

    elem->val = -1;
}

HashTableElemType HashTableElemInit(const char* key, bool val)
{
    HashTableElemType elem = HashTableElemCtor();

    elem.key = key ? strdup(key) : nullptr;
    elem.val = val;

    return elem;
}

bool              HashTableElemsCmp(const HashTableElemType elem1,
                                    const HashTableElemType elem2)
{
    if (elem1.key == nullptr && elem2.key == nullptr && elem1.val == elem2.val)
        return true;

    if (elem1.key == nullptr || elem2.key == nullptr)
        return false;
        
    return strcmp(elem1.key, elem2.key) == 0 && elem1.val == elem2.val;
}   
