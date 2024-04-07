#ifndef HASH_TABLE_ELEM_H
#define HASH_TABLE_ELEM_H

#include <string.h>

static const size_t HashTableElemKeyLen       = 32; 
static const size_t HashTableElemKeyAlignment = 32;

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

static inline int AsmStrcmp(const char* str1, const char* str2)
{
    int res = 0;

    __asm__ inline (
        ".intel_syntax noprefix\n\t"
        "vmovdqu ymm0, YMMWORD PTR [%1]\n\t" //read str1
        "vptest ymm0, YMMWORD PTR[%2]\n\t"   // cmp str1 and str2
        "setc %b0\n\t"                       // set carry flag to res
        "vzeroupper\n\t"                     // explained in readme
        ".att_syntax prefix\n\t"
        : "+&r" (res)
        : "r" (str1), "r" (str2)
        : "ymm0", "cc"
    );

    return res;
}

#endif