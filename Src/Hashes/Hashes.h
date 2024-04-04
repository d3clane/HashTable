#ifndef HASHES_H
#define HASHES_H

#include <stdint.h>

typedef uint32_t (*HashFuncType)(const char* inString);

uint32_t ConstantHash      (const char* inString);
uint32_t FirstCharASCIIHash(const char* inString);
uint32_t SumCharsASCIIHash (const char* inString);
uint32_t StringLengthHash  (const char* inString);
uint32_t RolHash           (const char* inString);
uint32_t RorHash           (const char* inString);
uint32_t MurMurHash        (const char* inString);
uint32_t CRC32Hash         (const char* inString);

#endif