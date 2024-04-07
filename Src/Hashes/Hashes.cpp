#include <assert.h>
#include <string.h>
#include <time.h>
#include <nmmintrin.h>

#include "Hashes.h"

static inline uint32_t Rol(uint32_t dWord);
static inline uint32_t Ror(uint32_t dWord);

uint32_t ConstantHash      (const char* inString)
{
    assert(inString);

    return (uint32_t)42;
}

uint32_t FirstCharASCIIHash(const char* inString)
{
    assert(inString);

    return (uint32_t)inString[0];
}

uint32_t SumCharsASCIIHash (const char* inString)
{
    assert(inString);

    uint32_t sum = 0;
    const char* inStringPtr = inString;

    while (*inStringPtr)
    {
        sum += (uint32_t)(*inStringPtr);
        ++inStringPtr;
    }

    return sum;
}

uint32_t StringLengthHash  (const char* inString)
{
    assert(inString);

    return (uint32_t)strlen(inString);
}

uint32_t RolHash           (const char* inString)
{
    assert(inString);

    uint32_t hash = 0;
    const char* inStringPtr = inString;

    while (*inStringPtr)
    {
        hash = Rol(hash) ^ (uint32_t)(*inStringPtr);
        ++inStringPtr;
    }

    return hash;
}

uint32_t RorHash           (const char* inString)
{
    assert(inString);

    uint32_t hash = 0;
    const char* inStringPtr = inString;

    while (*inStringPtr)
    {
        hash = Ror(hash) ^ (uint32_t)(*inStringPtr);
        ++inStringPtr;
    }

    return hash;
}

uint32_t MurMurHash        (const char* inString)
{
    assert(inString);

    static const uint32_t seed = time(NULL);

    const uint32_t c1 = 0x5bd1e995; 
    const uint32_t c2 = 24;

    size_t length = strlen(inString);
    uint32_t hash = seed ^ (uint32_t)length;

    uint32_t word = 0;

    const char* inStringPtr = inString;
    while (length >= 4)
    {
        word  = (uint32_t)inStringPtr[0];
        word |= (uint32_t)inStringPtr[1] <<  8;
        word |= (uint32_t)inStringPtr[2] << 16;
        word |= (uint32_t)inStringPtr[3] << 24;

        word *= c1;
        word ^= word >> c2;
        word *= c1;

        hash *= c1;
        hash ^= word;

        inStringPtr += 4;
        length -= 4;
    }

    assert(length < 4);

    switch (length)
    { 
        case 3:
            hash ^= (uint32_t)inStringPtr[2] << 16;
            // fall through
        case 2:
            hash ^= (uint32_t)inStringPtr[1] << 8;
            // fall through
        case 1:
            hash ^= (uint32_t)inStringPtr[0];
            hash *= c1;
            break;
        default:
            break;
    };

    hash ^= hash >> 13;
    hash *= c1;
    hash ^= hash >> 15;

    return hash;
}

uint32_t CRC32Hash         (const char* inString)
{
    unsigned int crc = 0xFFFFFFFF, mask = 0;

    const char* inStringPtr = inString;
    while (*inStringPtr)
    {
        crc = crc ^ (unsigned int)(*inStringPtr);

        for (int j = 7; j >= 0; j--)
        {
            mask = -(crc & 1);
            crc = (crc >> 1) ^ (0xEDB88320 & mask);
        }

        ++inStringPtr;
    }

    return (uint32_t)~crc;
}

uint32_t CRC32HashIntrinsic(const char* inString)
{
    uint32_t crc = 0;
    crc = _mm_crc32_u64(crc, *((uint64_t*)inString + 0));
    crc = _mm_crc32_u64(crc, *((uint64_t*)inString + 1));
    crc = _mm_crc32_u64(crc, *((uint64_t*)inString + 2));
    crc = _mm_crc32_u64(crc, *((uint64_t*)inString + 3));

    return crc;
}

static inline uint32_t Rol(uint32_t dWord)
{
    return ((dWord << 1) | (dWord >> (sizeof(dWord) * 8 - 1)));
}

static inline uint32_t Ror(uint32_t dWord)
{
    return ((dWord >> 1) | (dWord << (sizeof(dWord) * 8 - 1)));
}
