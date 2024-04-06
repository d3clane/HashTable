#include <assert.h>
#include <stdio.h>

#include "UnitTests/UnitTestHashMap.h"
#include "TestPerformance/TestPerformance.h"
#include "HashTable/HashTable.h"
#include "Common/Log.h"

int main(const int argc, const char* argv[])
{
    LogOpen(argv[0]);
    
    static const char* testsInputFileName = "testsInputFileName";
    
    UnitTestHashMap(ConstantHash,       testsInputFileName);
    UnitTestHashMap(FirstCharASCIIHash, testsInputFileName);
    UnitTestHashMap(SumCharsASCIIHash,  testsInputFileName);
    UnitTestHashMap(StringLengthHash,   testsInputFileName);
    UnitTestHashMap(RolHash,            testsInputFileName);
    UnitTestHashMap(RorHash,            testsInputFileName);
    UnitTestHashMap(MurMurHash,         testsInputFileName);
    UnitTestHashMap(CRC32Hash,          testsInputFileName);
    
   static const size_t hashTableCapacity = 49157;

    /*
    MeasureHashTableListsSizes(hashTableCapacity, ConstantHash, 
                                "testsInputFileName",
                                "../TestResults/ListSizesTest/ConstantHash.txt");

    MeasureHashTableListsSizes(hashTableCapacity, FirstCharASCIIHash, 
                                "testsInputFileName",
                                "../TestResults/ListSizesTest/FirstCharASCIIHash.txt");


    MeasureHashTableListsSizes(hashTableCapacity, SumCharsASCIIHash, 
                                "testsInputFileName",
                                "../TestResults/ListSizesTest/SumCharsASCIIHash.txt");

    MeasureHashTableListsSizes(389, SumCharsASCIIHash, 
                                "testsInputFileName",
                                "../TestResults/ListSizesTest/SumCharsASCIIHashSmallCap.txt");

    MeasureHashTableListsSizes(hashTableCapacity, StringLengthHash, 
                                "testsInputFileName",
                                "../TestResults/ListSizesTest/StringLengthHash.txt");

    MeasureHashTableListsSizes(hashTableCapacity, RolHash, 
                                "testsInputFileName",
                                "../TestResults/ListSizesTest/RolHash.txt");

    MeasureHashTableListsSizes(hashTableCapacity, RorHash, 
                                "testsInputFileName",
                                "../TestResults/ListSizesTest/RorHash.txt");

    MeasureHashTableListsSizes(hashTableCapacity, MurMurHash, 
                                "testsInputFileName",
                                "../TestResults/ListSizesTest/MurMurHash.txt");

    MeasureHashTableListsSizes(hashTableCapacity, CRC32Hash, 
                                "testsInputFileName",
                                "../TestResults/ListSizesTest/CRC32Hash.txt");

    */
}