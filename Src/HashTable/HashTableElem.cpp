#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdalign.h>
#include <stdio.h>

#include "HashTableElem.h"

HashTableElemType HashTableElemCpy(const HashTableElemType elem)
{
    HashTableElemType elemCopy = {};

    if (elem.key == nullptr)
    {   
        elemCopy.key     = nullptr;
        elemCopy.val = elem.val;

        return elemCopy;
    }

    elemCopy.key = (char*)aligned_alloc(HashTableElemKeyAlignment, 
                                        HashTableElemKeyLen * sizeof(*elemCopy.key));
    memcpy(elemCopy.key, elem.key, HashTableElemKeyLen * sizeof(*elemCopy.key));
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

    if (key == nullptr)
    {
        elem.val = val;
        return elem;
    }

    elem.key = (char*)aligned_alloc(HashTableElemKeyAlignment, 
                                        HashTableElemKeyLen * sizeof(*elem.key));
    memcpy(elem.key, key, HashTableElemKeyLen * sizeof(*elem.key));
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
    
    return AsmStrcmp(elem1.key, elem2.key) && elem1.val == elem2.val;
}   