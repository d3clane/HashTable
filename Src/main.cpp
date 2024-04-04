#include <assert.h>
#include <stdio.h>

#include "Testing/TestCorrectness.h"
#include "HashTable/HashTable.h"
#include "Common/Log.h"

int main(const int argc, const char* argv[])
{
    LogOpen(argv[0]);
    
    TestCorrectness(ConstantHash);
    TestCorrectness(FirstCharASCIIHash);
    TestCorrectness(SumCharsASCIIHash);
    TestCorrectness(StringLengthHash);
    TestCorrectness(RolHash);
    TestCorrectness(RorHash);
    TestCorrectness(MurMurHash);
    TestCorrectness(CRC32Hash);
}