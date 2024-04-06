#include <assert.h>
#include <stdio.h>

#include "UnitTests/UnitTestHashMap.h"
#include "TestPerformance/TestPerformance.h"
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
    
   static const size_t hashTableCapacity = 49157;

    /*
    MeasureHashTableListsSizes(hashTableCapacity, ConstantHash, 
                                "../TestsInput/ListSizesTest/wordsAll.txt",
                                "../TestResults/ListSizesTest/ConstantHash.txt");

    MeasureHashTableListsSizes(hashTableCapacity, FirstCharASCIIHash, 
                                "../TestsInput/ListSizesTest/wordsAll.txt",
                                "../TestResults/ListSizesTest/FirstCharASCIIHash.txt");


    MeasureHashTableListsSizes(hashTableCapacity, SumCharsASCIIHash, 
                                "../TestsInput/ListSizesTest/wordsAll.txt",
                                "../TestResults/ListSizesTest/SumCharsASCIIHash.txt");

    MeasureHashTableListsSizes(389, SumCharsASCIIHash, 
                                "../TestsInput/ListSizesTest/wordsAll.txt",
                                "../TestResults/ListSizesTest/SumCharsASCIIHashSmallCap.txt");

    MeasureHashTableListsSizes(hashTableCapacity, StringLengthHash, 
                                "../TestsInput/ListSizesTest/wordsAll.txt",
                                "../TestResults/ListSizesTest/StringLengthHash.txt");

    MeasureHashTableListsSizes(hashTableCapacity, RolHash, 
                                "../TestsInput/ListSizesTest/wordsAll.txt",
                                "../TestResults/ListSizesTest/RolHash.txt");

    MeasureHashTableListsSizes(hashTableCapacity, RorHash, 
                                "../TestsInput/ListSizesTest/wordsAll.txt",
                                "../TestResults/ListSizesTest/RorHash.txt");

    MeasureHashTableListsSizes(hashTableCapacity, MurMurHash, 
                                "../TestsInput/ListSizesTest/wordsAll.txt",
                                "../TestResults/ListSizesTest/MurMurHash.txt");

    MeasureHashTableListsSizes(hashTableCapacity, CRC32Hash, 
                                "../TestsInput/ListSizesTest/wordsAll.txt",
                                "../TestResults/ListSizesTest/CRC32Hash.txt");

    */
}