#include <assert.h>
#include <stdio.h>

#include "UnitTests/UnitTestHashMap.h"
#include "HashTable/HashTable.h"
#include "Common/Log.h"

int main(const int argc, const char* argv[])
{
    LogOpen(argv[0]);
    
    UnitTestHashMap(ConstantHash);
    UnitTestHashMap(FirstCharASCIIHash);
    UnitTestHashMap(SumCharsASCIIHash);
    UnitTestHashMap(StringLengthHash);
    UnitTestHashMap(RolHash);
    UnitTestHashMap(RorHash);
    UnitTestHashMap(MurMurHash);
    UnitTestHashMap(CRC32Hash);
}