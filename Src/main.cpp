#include <assert.h>
#include <stdio.h>

#include "UnitTests/UnitTestHashMap.h"
#include "TestPerformance/TestPerformance.h"
#include "HashTable/HashTable.h"
#include "Common/Log.h"

int main(const int argc, const char* argv[])
{
    LogOpen(argv[0]);
    
    /*
    UnitTestHashMap(ConstantHash);
    UnitTestHashMap(FirstCharASCIIHash);
    UnitTestHashMap(SumCharsASCIIHash);
    UnitTestHashMap(StringLengthHash);
    UnitTestHashMap(RolHash);
    UnitTestHashMap(RorHash);
    UnitTestHashMap(MurMurHash);
    UnitTestHashMap(CRC32Hash);
    */

   static const size_t hashTableCapacity = 512;

    MeasureHashTableListsSizes(hashTableCapacity, ConstantHash, 
                                "../TestsInput/ListSizesTest/wordsCommon.txt",
                                "../TestResults/ListSizesTest/ConstantHash.txt");

    MeasureHashTableListsSizes(hashTableCapacity, FirstCharASCIIHash, 
                                "../TestsInput/ListSizesTest/wordsCommon.txt",
                                "../TestResults/ListSizesTest/FirstCharASCIIHash.txt");

    MeasureHashTableListsSizes(hashTableCapacity, SumCharsASCIIHash, 
                                "../TestsInput/ListSizesTest/wordsCommon.txt",
                                "../TestResults/ListSizesTest/SumCharsASCIIHash.txt");

    MeasureHashTableListsSizes(hashTableCapacity, StringLengthHash, 
                                "../TestsInput/ListSizesTest/wordsCommon.txt",
                                "../TestResults/ListSizesTest/StringLengthHash.txt");

    MeasureHashTableListsSizes(hashTableCapacity, RolHash, 
                                "../TestsInput/ListSizesTest/wordsCommon.txt",
                                "../TestResults/ListSizesTest/RolHash.txt");

    MeasureHashTableListsSizes(hashTableCapacity, RorHash, 
                                "../TestsInput/ListSizesTest/wordsCommon.txt",
                                "../TestResults/ListSizesTest/RorHash.txt");

    MeasureHashTableListsSizes(hashTableCapacity, MurMurHash, 
                                "../TestsInput/ListSizesTest/wordsCommon.txt",
                                "../TestResults/ListSizesTest/MurMurHash.txt");

    MeasureHashTableListsSizes(hashTableCapacity, CRC32Hash, 
                                "../TestsInput/ListSizesTest/wordsCommon.txt",
                                "../TestResults/ListSizesTest/CRC32Hash.txt");
}