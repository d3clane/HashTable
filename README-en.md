# Hash table research

Translated using DeepL translator.

## Other languages

1. [Russian](/README.md)
2. [English](/README-en.md)

## Installation and run

```
cd Src
make buildDirs
make 
./hashTable
```

## Description

In this project, a hash table is implemented using the chain method and the following parts of it are investigated:

1. Hash functions to see which one distributes the most evenly across the hash table. 
2. Performance of hash table lookup operation and possibilities to optimize it.

It is important that in this project hash table size is pretty small and therefore load-factor is huge. Actually, there could be no need in these optimizations, because hash table works pretty well with simply scaling it's size and using evenly spreading hash function. However, the goal of this project is to learn how assembler optimization works and therefore we make this assumption.

Let's describe how a hash table is organized.

## Hash table description

The hash table is written using the chain method, with a string as the key and a Boolean variable as the value. If a row exists in the hash table, its value is true, as if to confirm the fact that this row exists in the hash table. Chains are implemented using my implementation of [cache-friendly doubly-linked list](https://github.com/d3clane/list), which uses a dynamic array to store blocks of data.

Schematic view of the hash table:

![hash table](https://github.com/d3clane/HashTable/blob/master/ReadmeAssets/imgs/HashTable.png)

The picture shows that two input values with the keys "emberlizard" and "soggyturtle" get the same value at the output of the hash function, so they end up in the same cell of the hash table. Here a list is used to store these values in one cell sequentially - like a chain.

The top [370000 English words](https://github.com/dwyl/english-words?tab=readme-ov-file) set is used for the study. Load-factor for my hash table is $7.52$. Why so much? This is to better see how evenly the hash functions are distributed over the hash table. If the load-factor was optimal and selected to speed up the hash table (about 0.7), then when examining the distribution, the peaks of loading of individual cells would not be so visible and it would seem that the distribution is close to uniform, although in reality it is not so. The size of the hash table itself is 49157 - this is a simple number, because with this choice the distribution is more likely to be uniform. I will explain the choice of the size equal to a prime number in more detail:

Often the data at the output of a hash function may have a specific pattern, i.e., it may be in some step or the number of elements multiple of some $k$ is greater than others. Suppose our hash function has received some data as input and the output is ${k, 2k, 3k, 4k, ...}$ - all data are multiples of $k$. To get the data into the cells of the hash table the results are taken modulo $hashTableSize$ - that is, modulo the number of cells in the hash table. Then let's see how many different cells the hash table can fill in this case, how many cells it can theoretically put this data into. The data will start to fall into the same cells when 

$$k \equiv r \cdot k \pmod{hashTableSize}$$, where $r - 1$ is obtained equal to the number of different remainders by division that can be obtained. Let's solve this equation - it can be rewritten as:

$$k + hashTableSize \cdot \alpha = r \cdot k$$, where $\alpha$ is some integer. The smallest $k$ when this is satisfied is $r \cdot k = LCM(k, hashTableSize)$, that is, $r \cdot k = \frac{k \cdot hashTableSize}{GCD(k, hashTableSize)}$ or $r = \frac{hashTableSize}{GCD(k, hashTableSize)}$. 

It turns out that by taking hashTableSize as a prime number we are more likely to get $GCD(k, hashTableSize) = 1$, i.e. we can fill all the cells of the hash table. 

## Hash function research

All hash functions have a signature of the form `uint32_t HashFunc(const char* inString)`, and a total of 8 different hash functions have been written:

1. [ConstantHash](#Constant-hash)
2. [FirstCharASCIIHash](#First-char-ASCII-hash)
3. [StringLengthHash](#String-length-hash)
4. [SumCharsASCIIHash](#Sum-chars-ASCII-hash)
5. [RolHash](#Rol-hash)
6. [RorHash](#Ror-hash)
7. [MurmurHash](#Murmur-hash)
8. [CRC32Hash](#CRC32-hash)

It's not hard to see that some of these are knowingly bad, but still necessary for a more complete study.

The study will be both visual and formulaic - graphs are plotted, compared, and the standard deviation is calculated. The standard deviation is calculated using the formula: 

$$standardDeviation = \sqrt{\frac{\sum\limits_{i = 0}^{n} (x_i - x_{mean})^2}{n}}$$ where $x_i$ is the length of the i-th chain, $x_{mean}$ is the average length of the chain, and $n$ is the number of such chains. In my case, $n = 49157$.

### Constant hash

```
uint32_t ConstantHash(const char* inString)
{
    assert(inString);

    return (uint32_t)42;
}
```

This hash function always returns the number 42.

Obviously, with such a hash function, only one cell in the hash table will always be filled and it will be slow:

![ConstantHash](https://github.com/d3clane/HashTable/blob/master/ReadmeAssets/imgs/ConstantHash.png). 

Zoomed in:

![ConstantHashClose](https://github.com/d3clane/HashTable/blob/master/ReadmeAssets/imgs/ConstantHashClose.png)

The standard deviation is 1669.3.

Here the problem in the hashing method is obvious - only one cell is filled.

### First char ASCII hash

```
uint32_t FirstCharASCIIHash(const char* inString)
{
    assert(inString);

    return (uint32_t)inString[0];
}
```

This hash function returns the ASCII code of the first letter of the string. 

Expectedly, the distribution should be in the range $[97, 122]$, where 97 is the ASCII code of the letter 'a', 122 is the ASCII code of the letter 'z'. That is, a very small number of cells are filled. Distribution histogram:

![First char ASCII](https://github.com/d3clane/HashTable/blob/master/ReadmeAssets/imgs/FirstCharASCIIHash.png)

Zoomed in:

![First char ASCII close](https://github.com/d3clane/HashTable/blob/master/ReadmeAssets/imgs/FirstCharASCIIHashClose.png)

The standard deviation is 403.4.

According to the histogram, we can understand that the most frequent words begin with the letters 'c', 'p', 's', as they have the maximum load peaks. The most rare is the letter 'x'. 

The problem with this hashing method is the limited number of cells used, and the fact that there are letters that words almost never begin with, which means these cells will almost never be filled.

### String length hash

```
uint32_t StringLengthHash(const char* inString)
{
    assert(inString);

    return (uint32_t)strlen(inString);
}
```

This hash function returns the length of the string.

The average length of an English word is 4.7, it is considered that the longest word is pneumonoultramicroscopicsilicovolcanoconiosis, the length of which is 45. It turns out that on our dataset the values will be in the boundaries of [1, 45], and a normal distribution is expected, since in fact words of very short and very long length are strongly less than words of average length. Let's check it in practice:

![string length hash](https://github.com/d3clane/HashTable/blob/master/ReadmeAssets/imgs/StringLengthHash.png)

Zoomed in:

![string length hash close](https://github.com/d3clane/HashTable/blob/master/ReadmeAssets/imgs/StringLengthHashClose.png)

The standard deviation is 525.9.

The problem with this hashing method is the strong limitation on the number of cells used, as well as the normal distribution. 

### Sum chars ASCII hash

```
uint32_t SumCharsASCIIHash(const char* inString)
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
```

This hash function summarizes the ASCII codes of the letters of the string and returns as the hash result.

Let's estimate what result such a hash function can return to me. The longest word is 45 letters, the letter with the largest ASCII code is 'z' = 122. Then the result is $\leq 45 \cdot 122 =  5490$. This is obviously never achieved (there is no word 'zzzzzzzzzzzzz....'). At the same time, there are more English words on average in the neighborhood of 4 to 15 letters, i.e. hash function values in the neighborhood of $a \cdot 4 =  388 \leq hashFuncRes \leq z \cdot 15 =  1830$. Before, the hash table size was $49157$, now, we will choose the size $389$ on purpose. Why this was done will be clear later. 

The resulting graph:

![ASCII sum small cap](https://github.com/d3clane/HashTable/blob/master/ReadmeAssets/imgs/SumCharsASCIIHashSmallCap.png)

Also check on another [dataset](/TestsInput/ListSizesTest/wordsCommon.txt) of 3000 most popular English words, for them load-factor = 7.7:

![ASCII sum small cap common](https://github.com/d3clane/HashTable/blob/master/ReadmeAssets/imgs/SumCharsASCIIHashSmallCapCommon.png)

It would seem to be a good hash function - in the first case its problem is a giant load-factor, but in the case of the top 3000 most popular English words it looks quite good. It turns out that if you increase the size of the hash table, you will get a very good distribution, right? Let's check it out.

Let's go back to the past size of the hash table. The resulting graph for a dataset of 370k words:

![ASCII sum](https://github.com/d3clane/HashTable/blob/master/ReadmeAssets/imgs/SumCharsASCIIHash.png)

Zoomed in:

![ASCII sum close](https://github.com/d3clane/HashTable/blob/master/ReadmeAssets/imgs/SumCharsASCIIHashClose.png)

The standard deviation is 63.7.

For a dataset of 3000 words:

![ASCII sum common](https://github.com/d3clane/HashTable/blob/master/ReadmeAssets/imgs/SumCharsASCIIHashCommon.png)

Zoomed in:

![ASCII sum total close](https://github.com/d3clane/HashTable/blob/master/ReadmeAssets/imgs/SumCharsASCIIHashCommonClose.png)

Cells with numbers greater than 2500 were almost not filled, and starting from some number, not a single key got into these cells - it was not possible to extrapolate the result. It has already been explained above that the values of such a hash function lie in a rather narrow range. At the same time, when the hash table size is small, this effect is not so strong - the reason is that the result of the hash function is taken modulo, and the values are "wrapped" and distributed over the cells more or less evenly. Such a hash function is not bad for small hash tables, but not for large ones.

### Rol hash

```
uint32_t RolHash(const char* inString)
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

static inline uint32_t Rol(uint32_t dWord)
{
    return ((dWord << 1) | (dWord >> (sizeof(dWord) * 8 - 1)));
}
```

This hash function recalculates hash using the formula $hash(n) = ROL(hash(n - 1)) \oplus str[n]$, running over all letters in the string, where str is the string, str[n] is the $n$-th letter of the string, ROL is the bitwise rotate left operation. Initially hash(0) = 0, numbering from one. $\oplus$ - XOR operation. 

Besides hashing, I'm interested in the following point - there is a `rol` instruction in assembler, will the compiler be able to see it and replace my `Rol` function call with a simple instruction. Let's check it in godbolt. 

![No opt rol](https://github.com/d3clane/HashTable/blob/master/ReadmeAssets/imgs/NoOptRol.png)

Even without optimizations, the compiler was able to notice that it was a rol instruction and replace it! However, it still left my Rol function call. And now with optimization -O1:

![Opt rol](https://github.com/d3clane/HashTable/blob/master/ReadmeAssets/imgs/OptRol.png)

The compiler completely got rid of the Rol function call and replaced it with the rol instruction. With flags -O2, -O3 no optimizations with the rol instruction are performed anymore. 

Now a graph of the distribution:

![Rol hash](https://github.com/d3clane/HashTable/blob/master/ReadmeAssets/imgs/RolHash.png)

The standard deviation is 8.3.

It is noticeable that the list sizes are somehow alternating - there are a lot of strongly filled lists in a row, then a lot of sparsely filled ones. At large hash values the distribution is even.

### Ror hash

```
uint32_t RorHash(const char* inString)
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

static inline uint32_t Ror(uint32_t dWord)
{
    return ((dWord >> 1) | (dWord << (sizeof(dWord) * 8 - 1)));
}
```

This hash function is similar to RolHash, only $hash(n) = ROR(hash(n - 1)) \oplus str[n]$. ROR is a bitwise rotate right operation.

The results in godbolt are exactly the same, the compiler notices that it is a ror instruction. Occupancy graph:

![Ror hash](https://github.com/d3clane/HashTable/blob/master/ReadmeAssets/imgs/RorHash.png)

The standard deviation is 18.0

The distribution is worse than for RolHash as there are very high peaks. What can this be related to? The average word size is around 5 - 16 characters. I get about 5-16 rotate right of my hash, which means that the top 5 - 16 bits of my hash are very much filled. And then this hash is taken modulo $49157$ and all these bits become sharply unimportant, and, as a result, very many hashes match by modulo.

### Murmur hash

```
uint32_t MurMurHash(const char* inString)
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
        word = (uint32_t)inStringPtr[0];
        word |= (uint32_t)inStringPtr[1] << 8;
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
        Case 3:
            hash ^= (uint32_t)inStringPtr[2] << 16;
            // fall through
        Case 2:
            hash ^= (uint32_t)inStringPtr[1] << 8;
            // fall through
        Case 1:
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
```

It is impossible to explain the idea of this hash function briefly, so it is better to read the [wikipedia](https://en.wikipedia.org/wiki/MurmurHash) page.

Distribution on the graph:

![MurMurHash](https://github.com/d3clane/HashTable/blob/master/ReadmeAssets/imgs/MurMurHash.png)

Due to the scale peculiarities, the graph does not show that there are actually lists with not very large sizes. It is easy to guess about their existence - if we judge by the graph, it turns out that the load-factor does not correspond to the declared one (it seems to be obviously larger than 7.5 according to the graph). Just for such cases you need to calculate the standard deviation in order to estimate additionally by formulas and compare with visual results. MurMurHash in enlarged scale:

![MurMurHash close](https://github.com/d3clane/HashTable/blob/master/ReadmeAssets/imgs/MurMurHashClose.png)

The standard deviation is 2.74. 

It can be seen that the distribution turns out to be fairly uniform. 

### CRC32 hash

```
uint32_t CRC32Hash(const char* inString)
{
    unsigned int crc = 0xFFFFFFFFFF, mask = 0;

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
```

To understand the idea, it is better to consult [wikipedia](https://en.wikipedia.org/wiki/Cyclic_redundancy_check).

Distribution on the graph:

![CRC32 hash](https://github.com/d3clane/HashTable/blob/master/ReadmeAssets/imgs/CRC32Hash.png)

Here too, because of the scale features, you can't see that there are actually lists with not very large sizes. In a larger scale it will work out:

![CRC32 hash close](https://github.com/d3clane/HashTable/blob/master/ReadmeAssets/imgs/CRC32HashClose.png)

The standard deviation is 2.75.

It is also a well-distributing hash function.

### Intermediate output

Summary table of standard deviations for different hash functions:

|                       |Standard deviation     |
|---                  |---                  |
|Constant hash          | 1669.3                |
|String length hash     | 525.9                 |
|First char ASCII hash  | 403.4                 |
|Sum chars ASCII hash   | 63.7                  |
|Ror hash               | 18.0                  |
|Rol hash               | 8.3                   |
|CRC32 hash             | 2.75                  |
|Murmur hash            | 2.74                  |

Obviously, you should choose between MurMurHash and CRC32 Hash, because the distribution is the most uniform, and there is no special difference between the distributions - both visually and by standard deviation they are as similar as possible. The next selection criterion may be the speed of each hash function - they are called during insertion / deletion / search operations in the hash table, i.e. quite often. For my hash table I will choose CRC32 because this choice will give me more room for optimizations in the second part of the work - there are intrinsic-i that implement the CRC32 hashing algorithm, so there is a possibility for acceleration. 

## Hash Table Optimization

To test the running time of the hash table lookup operation, we use the following algorithm:
1. Insert all elements from the test file with English words into the hash table, save them to an array so that we don't waste time reading from the file.
2. The loop runs through all the data in the English word array 100 times, and for each element, the hash table lookup function is called.

The time is measured, which is spent directly on the 2nd part of the algorithm - hash table search. Time is measured in processor cycles with the help of a function written in assembler and which uses the `rdtsc` instruction. Representing time in this form is enough, because to measure the improvement of program performance it is enough to see how many times faster the program has become.

When measuring time, three runs of the program are made with the compiler flags `-O3 -mavx2 -D NDEBUG`, and then the average time over the three runs is averaged using the least squares method to exclude external influences from other processes on the computer.

For optimizations we need to understand what, in fact, we need to optimize first of all, i.e. we need to find bottlenecks in the program. For this purpose we will use `valgrind --tool=callgrind` and view the result with kcachegrind. This utility will help you measure the running time of separate parts of the program and determine what should be optimized first of all.

We will improve the performance of the program in three ways:
1. using intrinsics.
2. assembler inserts.
3. writing code in a separate assembly language file and linking it to my program.

It is not difficult to understand that such optimizations are not always good. Their use is strongly tied to a particular architecture, the code becomes harder to maintain and read, so you should keep a balance between the lines of codes written for optimization and the performance gain obtained. If the gain is insignificant but the code changes dramatically, you should rather refuse such optimization than use it. To estimate the balance between the written lines of code and the performance gain we will use the following coefficient:

$$balance = \frac{\frac{T}{T_0}}{NumberOfOptimizingCodeLines} \cdot 1000$$

where $T_0$ is the program execution time without optimizations, $T$ is the program execution time with optimizations, $NumberOfOptimizingCodeLines$ is the number of lines of code written using intrinsic/assembler during optimization. 

When applying each of the optimizations we will build a table 

| | Absolute acceleration | Relative acceleration|
|--- |--- |--- |
| Naive implementation | 1 | 1 |
| Optimization 1 | $K_1$ | $K_1$ |
| Optimization 2 | $K_2$ | $K_3$ |

Absolute acceleration - how many times faster the program became relative to the naive implementation after applying optimizations. 

Relative acceleration - how many times faster the program has become relative to the previous version with optimizations. For the first optimization absolute and relative speedups are the same.

## Run without optimizations

To begin with, let's measure the running time of a naive implementation of the program without any optimizations:

| | Run1 | Run2 | Run3 | Mean |
|--- |--- |--- |--- |--- |
| Naive implementation | 50843355582 | 50002732758 | 51492446142 | $(508 \pm 8) \cdot 10^8$ |

The characteristic time of one run is about 20 seconds. 

## Search for bottlenecks

Let's use `valgrind --tool=cachegrind` and see how optimized we are using the cache in the program. Result:

![Cache grind](https://github.com/d3clane/HashTable/blob/master/ReadmeAssets/imgs/CacheGrind.png)

It can be seen that we already have optimized cache usage and there are no special prerequisites for trying to improve something even more - we can't get dramatic performance improvements. 

Now let's look directly at the time spent on calling various functions. The result obtained with valgrind:

![CallGrindNoOpt](https://github.com/d3clane/HashTable/blob/master/ReadmeAssets/imgs/CallGrindCRC32Opt.png)

The functions are sorted by value in the second column. This column is responsible for the time spent directly on the code of the function itself, not including the time spent on functions called from under it. The execution time of the different parts is given as a percentage of the total execution time. Let's consider the first two lines:

| | Percentage of total working time |
|--- |--- |
| CRC32 | 55.01 |
| __strcmp_sse2_unaligned | 11.37 |

And the first on the list is CRC32Hash - hashing function, it takes much more time than any other function, so it should be optimized first. 

## CRC32 intrinsic

Let us recall that there are intrinsics that hardware implement CRC32 hash. We will use intrinsic `unsigned __int64 _mm_crc32_u64 (unsigned __int64 crc, unsigned __int64 v)`, which takes int64 as input, i.e. it can hash 8 bytes of simultaneously. Here we face a problem - what is the length of our word - is it a multiple of 8? Do I need a loop to know how many times to call the hash in a row? Let's analyze our words.

Our dataset consists of English words, not a random set of letters. We can safely assume that there are almost no words of length greater than 32, and if such words appear, we can maintain a second hash table for words of longer length. Due to the fact that there are few such words (there are none in my dataset), such a hash table will not affect performance. That is, let's assume that all words are added to 32 bytes and it is necessary to call the hashing function exactly 4 times. The question arises - who should add words up to 32 bytes? There are two scenarios here:

1. My hash table receives the word as input, adds it to 32 bytes and only then starts searching.
2. When accessing the table, it is already guaranteed that the words are augmented to 32 bytes.

The second option is preferable, because if the keys cannot be aligned in advance, and the program using the hash table has to complete the words each time, it obviously works no worse than the first option, because they are equivalent actions. But at the same time, if it is possible to align the keys in advance, which will represent queries to the hash table, then the 2nd variant will obviously work faster, because the hash table will not need to spend time copying a string into an array of 32 bytes. 

In my testing of the program, I pre-add all keys to 32 bytes, so the time for this is not counted in the final running time. 

New CRC32 implementation:

```
uint32_t CRC32HashIntrinsic(const char* inString)
{
    uint32_t crc = 0;
    crc = _mm_crc32_u64(crc, *((uint64_t*)inString + 0));
    crc = _mm_crc32_u64(crc, *((uint64_t*)inString + 1));
    crc = _mm_crc32_u64(crc, *((uint64_t*)inString + 2));
    crc = _mm_crc32_u64(crc, *((uint64_t*)inString + 3));

    return crc;
}
```

It is assumed here that at least 32 bytes of memory are allocated to the input lines, so that no memory that is not its own is accessed. 

Measurements:

| | Run1 | Run2 | Run3 | Mean |
|--- |--- |--- |--- |--- |
|Intrinsic CRC32 | 28663836850 | 27360564576 | 28704863644 | $(282 \pm 8) \cdot 10^8$ |
                        
The characteristic running time of the program is about 13 seconds.

Performance gains:

| | Absolute acceleration | Relative acceleration|
|--- |--- |--- |
| Naive implementation | 1 | 1 |
|Intrinsic CRC32 | $1.80 \pm 0.08$ | $1.80 \pm 0.08$ |

## Strcmp in embedded assembler

Let's run valgrind again:

![CallGrindCRC32Intrinsic](https://github.com/d3clane/HashTable/blob/master/ReadmeAssets/imgs/CallGrindStrCmpOpt.png)

You can see that the bottleneck is now the strcmp function, which has already been optimized by the compiler using SSE instructions. 

Let's remember again that the word size does not exceed 32 bytes. And this fits perfectly into the size of the YMM register. Let's imagine that our words are stored in two YMM registers. Each byte is an ASCII code of a letter. Then, having unloaded the strings into the YMM registers, we can compare them and return the result. Note that there is an intrinsic `int _mm256_testc_si256 (__m256i a, __m256i b)` that does `(~a & b)` and puts CF = 1 if `(~a & b) == 0`. Obviously, `(~a & b) == 0` if and only if `a == b`. That is, you can quickly compare two YMM registers for equality using this operation. Moreover, the vptest instruction has a version that takes a pointer to memory as the second operand, i.e. there is no need to unload the second operand. Unfortunately, this is not implemented on intrinsic instructions. 

Let's write our own implementation of strcmp using the built-in assembler and make it inline, since it is a rather small function and the time spent on calling it can be significant. First of all, let's write it in intrinsic ones. For unloading from memory, we will use the intrinsic that works with unaligned strings on a 32-byte boundary, then we will write with aligned strings and compare them. 

```
bool StrCmp(const char* str1, const char* str2)
{
    __m256i val1 = _mm256_loadu_si256((__m256i*)str1);
    __m256i val2 = _mm256_loadu_si256((__m256i*)str2);

    int mask = _mm256_testc_si256(val1, val2);

    return mask;
}
```

Compiled in:

```
StrCmp(char const*, char const*):
        vmovdqu ymm0, YMMWORD PTR [rdi]
        vptest ymm0, YMMWORD PTR [rsi]
        setb al
        vzeroupper
        ret
```

Here you can immediately see that even though I unload both operands from memory and use an intrinsic that works with the two ymm registers, it was able to detect that you can use the same instruction but working with the `ymm, m256` operands instead of `ymm, ymm`.

Among the unusual ones is the vzeroupper instruction. When compiling with the `-O1` optimization flag, this instruction is not present. Googling, I found references to it on the [intel](https://www.intel.com/content/dam/develop/external/us/en/documents/11mc12-avoiding-2bavx-sse-2btransition-2bpenalties-2brh-2bfinal-809104.pdf) site, which states:

> When using Intel® AVX instructions, it is important to know that mixing 256-bit Intel® AVX instructions with legacy (non VEX-encoded) Intel® SSE instructions may result in penalties that could impact performance. 

That is, mixing AVX and SSE instructions may cause program performance to degrade. Due to the fact that it is not known in advance whether SSE instructions will be used in the future, after using AVX instructions, it is suggested to use `vzeroupper` to zeroize the upper parts of YMM registers, so that no time is spent on saving the upper parts when using SSE instructions.

So, let's rewrite it as an assembly insert:

```
static inline int AsmStrcmp(const char* str1, const char* str2)
{
   int res = 0;

    __asm__ inline (
        ".intel_syntax noprefix\n\t."
        "vmovdqu ymm0, YMMWORD PTR [%1]\n\t" //read str1
        "vptest ymm0, YMMWORD PTR[%2]\n\t" // cmp str1 and str2
        "setc %b0\n\t" // set carry flag to res
        "vzeroupper\n\t" // explained in readme.
        ".att_syntax prefix\n\t."
        : "+&r" (res)
        : "r" (str1), "r" (str2).
        : "ymm0", "cc".
    );

    return res;
}
```

My implementation of `AsmStrcmp` returns 1 if the strings match and 0 otherwise. 

Before measuring, let me draw your attention to a bug I encountered while writing `AsmStrcmp`. Let's pay attention to the output operands of my asm code, namely `: "+&r" (res)`. Before I had written `: "=&r" (res)` and the code worked incorrectly. Why? In the case of '=', I tell the compiler that I need res only for writing and I don't care what is in it - that's the problem. The compiler, seeing such a requirement, threw out the `res = 0` initialization, and as a result the res stored garbage, and as a result the function could often return a non-zero value despite the string mismatch, because the `setc %b0%` instruction fills only the lower 8 bits of the register provided for `res`, and the rest remain unaffected.

So, let's get to the measurements:

| | Run1 | Run2 | Run3 | Mean |
|--- |--- |--- |--- |--- |
|strcmp unaligned | 21314818430 | 20246098392 | 19284039290 | $(203 \pm 10) \cdot 10^8$ |
                        
The characteristic operating time is about 10 seconds.

Now let's rewrite using the instruction that requires alignment. Accordingly, for alignment, we will use `aligned_alloc`. 

In this case, `vmovdqu` is changed to `vmovdqa`.
```
static inline int AsmStrcmp(const char* str1, const char* str2)
{
   int res = 0;

    __asm__ inline (
        ".intel_syntax noprefix\n\t."
        "vmovdqa ymm0, YMMWORD PTR [%1]\n\t" //read str1 aligned
        "vptest ymm0, YMMWORD PTR[%2]\n\t" // cmp str1 and str2
        "setc %b0\n\t" // set carry flag to res
        "vzeroupper\n\t" // explained in readme.
        ".att_syntax prefix\n\t."
        : "+&r" (res)
        : "r" (str1), "r" (str2).
        : "ymm0", "cc".
    );

    return res;
}
```

When allocating memory, we use `aligned_alloc`, which aligns on a 32 byte boundary. To begin with, we align only the keys that are stored in my hash table - these are the ones used in the `vmovdqa` instruction. Before the measurements I checked that without using `aligned_alloc` my program crashes with an error, i.e. there is no alignment on the 32-byte boundary initially, so the tests make sense.

Measurements:

| | Run1 | Run2 | Run3 | Mean |
|--- |--- |--- |--- |--- |
|strcmp aligned only keys | 20497446244 | 21949195036 | 20672952570 | $(210 \pm 8) \cdot 10^8$ |

Now let's also equalize the keys that we feed to the input of the lookup function to find them in the table (that is, we will equalize str2 for the AsmStrcmp function). 

Measurements:

| | Run1 | Run2 | Run3 | Mean |
|--- |--- |--- |--- |--- |
|strcmp aligned only all | 21011997628 | 22021311548 | 20672952570 | $(219 \pm 9) \cdot 10^8$ |

The time has not improved, but perhaps even worsened (the values lie within the error). At the same time, the requirement of alignment strongly affects the stability of the program, because if it is not there, an error will occur. Let's refuse to use the instruction that requires alignment.

Performance gains:

| | Absolute acceleration | Relative acceleration|
|--- |--- |--- |
| Naive implementation | 1 | 1 |
|Intrinsic CRC32 | $1.80 \pm 0.08$ | $1.80 \pm 0.08$ |
|Strcmp inlined asm | $2.50 \pm 0.16$ | $1.39 \pm 0.11$ |

The gain is still satisfactory, we leave the optimization in place.

## Search for an item in the list

Let's run valgrind again:

![list opt](https://github.com/d3clane/HashTable/blob/master/ReadmeAssets/imgs/CallGrindListOpt.png)

You can see that now the "hardest" function is searching for an item in the list by key. Here is its code:

```
HtListErrors HtListFindElemByKey(HtListType* list, const char* key, size_t* elemPos)
{
    assert(list);
    assert(elemPos);

    size_t pos = list->end;

    do
    {
        const char* listKey = list->data[pos].value.key;
        
        if (listKey && AsmStrcmp(listKey, key))
        {
            *elemPos = pos;

            return HtListErrors::NO_ERR;
        }
        
        pos = list->data[pos].next;
    } while (pos != list->end);
    
    return HtListErrors::NO_ELEMENT_FOUND;
}
```

To understand the code, you also need to look at the structures used in the program:

```
struct HashTableElemType
{
    char* key;

    bool val;
};

struct HtListElemType
{
    HashTableElemType value;

    size_t prevPos;
    size_t nextPos;
};

struct HtListType
{
    HtListElemType* data;

    size_t end;
    size_t freeBlockHead;

    size_t size;
    size_t capacity;
};

enum class HtListErrors
{
    NO_ERR,

    MEMORY_ERR,

    DATA_IS_NULLPTR,
    OUT_OF_RANGE,

    INVALID_NULLPTR,
    INVALID_DATA,

    TRYING_TO_GET_NULL_ELEMENT,
    TRYING_TO_CHANGE_NULL_ELEMENT,

    NO_ELEMENT_FOUND,
};
```

Let's see what it compiles into:

```
HtListFindElemByKey(HtListType*, char const*, unsigned long*):
        mov r9, QWORD PTR [rdi+8] ; list end is written to r9
        mov r10, QWORD PTR [rdi] ; in r10 list data
        xor r11d, r11d ; r11d is reset to zero
        mov rcx, r9 ; in rcx list end
.L4:
        mov rax, rcx ; pos is written to rax
        sal rax, 5 ; multiply by 32 as sizeof(HtListElemType)
        add rax, r10 ; shift relative to list->data
        mov r8, QWORD PTR [rax] ; list->data[pos].value.key item is put in r8 
        test r8, r8 ; 
        je .L2 ; jump if r8 == nullptr
        mov edi, r11d ; edi = 0

        .intel_syntax noprefix ; \
        vmovdqu ymm0, YMMWORD PTR [r8] ; | |
        vptest ymm0, YMMWORD PTR [rsi] ; Asm strcmp inlined
        setc dil ; |
        vzeroupper ; /

        test edi, edi ; test asm strcmp res 
        jne .L13 ; if asm strcmp res != 0 -> jmp L13
.L2:
        mov rcx, QWORD PTR [rax+24] ; rcx = list->data[pos].nextPos
        cmp r9, rcx ; compares rcx and list->end
        jne .L4 ; loop if rcx != list->end
        mov eax, 8 ; return NO_ELEMENT_FOUND
        ret
.L13:
        mov QWORD PTR [rdx], rcx ; load response into *elemPos 
        xor eax, eax ; return NO_ERR
        ret 
```

Note the possible optimizations:
1. In my code list->end is always 0, that is, instead of accessing memory at the beginning, you can just net 0.
2. `vzeroupper` does not need to be called every time because there is no transition from AVX to SSE instructions inside the loop of this function - let's put it before the retern.
3. The line `vptest ymm0, YMMWORD PTR [rsi]` accesses the memory where the key is located each time, when in fact you could unload it into the ymm0 register, and access the memory in the hash table, since the key is not changed, and we are moving through the memory in the hash table. Also, it makes no sense to do `setc dil` and then `test edi, edi` since `vptest` already sets the CARRY FLAG in case the two lines are equal. 

Let's rewrite using our optimizations:

```
HtListFindElemByKey: ; (HtListType* list, const char* key, size_t* elemPos)
        mov r9, QWORD [rdi] ; r9 = list->data
        xor ecx, ecx ; ecx = 0 (start position)
        xor r10d, r10d ; r10d = 0

        vmovdqu ymm0, YWORD [rsi] ; loading key to ymm0
DO_WHILE_BODY:
        mov rax, rcx ; rax = pos
        sal rax, 5 ; rax * sizeof(HtListElemType)
        add rax, r9 ; rax = list->data + pos
        mov r8, QWORD [rax] ; r8 = list->data[pos].value.key
        test r8, r8 ;
        je DO_WHILE_CONDITION ; if r8 == null jmp to condition
        
        vptest ymm0, YWORD [r8] ;
        jc HT_LIST_RET ; compare r8 and key. If equal -> ret

DO_WHILE_CONDITION:
        mov rcx, QWORD [rax+24] ; rcx = list->data[pos].nextPos
        test rcx, rcx ;
        jne DO_WHILE_BODY ; if rcx != 0 -> continue
        mov eax, 8 ; return NO_ELEMENT_FOUND

        vzeroupper
        ret

HT_LIST_RET:
        mov QWORD [rdx], rcx ; *elemPos = pos
        xor eax, eax ; return NO_ERR

        vzeroupper
        ret
```

This code is written in [separate file](/Src/HashTable/HashTableList/HashTableListAsm.s) in assembly language and linked to my program. 

Measurements:

| | Run1 | Run2 | Run3 | Mean |
|--- |--- |--- |--- |--- |
|list find elem asm | 18265832736 | 18743839108 | 17652099914 | $(182 \pm 6) \cdot 10^8$ |

The characteristic running time of the program is about 10 seconds. 

| | Absolute acceleration | Relative acceleration|
|--- |--- |--- |
| Naive implementation | 1 | 1 | 1 |
|Intrinsic CRC32 | $1.80 \pm 0.08$ | $1.80 \pm 0.08$ |
|Strcmp inlined asm | $2.50 \pm 0.16$ | $1.39 \pm 0.11$ |
|list find elem asm | $2.79 \pm 0.14$ | $1.12 \pm 0.09$ |

The last optimization is still significant - as much as 12%, so it makes sense to try to optimize further. Initially, I thought that this was not a very high increase, but as I was told it was not, so I will do more profiling steps in the future, as long as the optimizations make sense. Generally speaking, having looked at the profiling results already now, it can be seen that the only function left "in the top" is the one responsible for hash table lookup, while the rest are related to input/output. Therefore, we can assume that there is no more than one optimization left to speed up hash table search.

## Optimize table size

Despite the fact that the table size for this task was fixed so that the load-factor was 7.5, we should not forget that real hash tables most often use a load-factor of about 0.7. Let's measure the running time of the hash table when its size is increased. This time the size will be $n = 528001$. Then $loadFactor = 0.70$. 

Measurements:

| | Run1 | Run2 | Run3 | Mean |
|--- |--- |--- |--- |--- |
|load-factor 0.7 | 12736916412 | 12519980688 | 13195608752 | $(128 \pm 3) \cdot 10^8$ |

The characteristic running time of the program is about 7 seconds. 

For the sake of purity of the experiment, it is also worth checking the speed of the hash table without any assembly optimizations but with an adequate load-factor. I will do this in the future when I return to this project.

## Synopsis

Summary table of results:

|                     |Absolute acceleration |Relative acceleration | Time in CPU ticks |
|---                |---                |---                  |---                      | 
|Naive implementation   | 1                   |  1                    | $(508 \pm 8) \cdot 10^8$  | 
|Intrinsic CRC32      | $1.80 \pm 0.08$     | $1.80 \pm 0.08$       | $(282 \pm 8) \cdot 10^8$  |
|Strcmp inlined asm   | $2.50 \pm 0.16$     | $1.39 \pm 0.11$       | $(203 \pm 10) \cdot 10^8$ |
|list find elem asm   | $2.79 \pm 0.14$     | $1.12 \pm 0.09$       | $(182 \pm 6) \cdot 10^8$  |
|load-factor 0.7      | $3.97 \pm 0.16$     | $1.42 \pm 0.08$       | $(128 \pm 3) \cdot 10^8$  |

It turns out that the optimal choice of load-factor can significantly improve the speed of the program. 

Let's calculate the $balance$ coefficient using the formula:

$$balance = \frac{\frac{T}{T_0}}{NumberOfOptimizingCodeLines} \cdot 1000$$

Let's calculate it for the cases of Strcmp inlined asm and list find elem asm. We won't calculate it for the case with load-factor reduction because this optimization didn't actually affect the assembler in any way. 

Let's count the number of lines of code written for the optimization. Each cell counts the number of lines written for previous optimizations + for this optimization.

| |NumberOfOptimizingCodeLines |
|--- |--- |
| Naive implementation | 0 |
|Intrinsic CRC32 | 4 |
|Strcmp inlined asm | 10 |
|List find elem asm | 30 |

Strcmp inlined asm:

$$balance = 250$$

List find elem asm:

$$balance = 93$$

Although the coefficient is rather strange, it still shows how good is the balance between the deterioration of code readability and maintainability and the resulting performance gain. And we can again conclude from it that optimization of list find elem asm is not critical and can be abandoned.

## Control experiment

Let's consider the part of the code from the benchmark that is responsible for testing the speed of the search operation:

```
uint64_t timeSpent = GetTimeStampCounter();

static const size_t numberOfTestsRun = 100;
for (size_t testId = 0; testId < numberOfTestsRun; ++testId)
{
    for (size_t wordId = 0; wordId < numberOfWords; ++wordId)
    {
        HashTableGetValue(hashTable, wordsAligned + wordId * HashTableElemKeyLen);
    }
}

timeSpent = GetTimeStampCounter() - timeSpent;
```

Here, the words used for the search operation are stored in wordsAligned. The words are stored in a row, with exactly 32 bytes allocated for each word, so that they can be separated from each other and everything is stored sequentially in memory. 

- numberOfWords - the number of words stored in wordsAligned.
- HashTableElemKeyLen = 32

It is easy to understand that, generally speaking, not only the time spent on the search operation is measured, but also the time spent on initializing loops and calculating the word address. Let's perform a control experiment - let's exclude HashTableGetValue from the experiment and leave only loops and calculation of the effective address. The problem appears - the compiler will throw out the whole loop because the data from it is not used. 

Solution:

```
uintptr_t ptrCalc = 0;
uint64_t timeSpent = GetTimeStampCounter();

static const size_t numberOfTestsRun = 100;
for (size_t testId = 0; testId < numberOfTestsRun; ++testId)
{
    for (size_t wordId = 0; wordId < numerOfWords; ++wordId)
    {
        ptrCalc += (uintptptr_t)(wordsAligned + wordId * HashTableElemKeyLen);
    }
}

timeSpent = GetTimeStampCounter() - timeSpent;
printf("res - %" PRIuPTR "\n", ptrCalc);
```

In this case, the compiler cannot throw out ptrCalc counting, because all operations are important for correct calculation of the value. Obviously, extra operations appear here - ptrCalc counting. That is, the time it will take to execute this piece of code is exactly greater than or equal to the time that would have been spent on the original loop without HashTableGetValue. Then, if this time turns out to be insignificant, we can disregard the time spent on the original loop without the additions.

The program will run 5 times, as the characteristic run time of the program is very small:

| | Run 1 | Run 2 | Run 3 | Run 4 | Run 5 | Mean |
|--- |--- |--- |--- |--- |--- |--- |
|baseline | 15683186 | 15741496 | 15766960 | 15962092 | 15638280 | $(157.6 \pm 1.2) \cdot 10^5$ |

Obviously, this time is within the margin of error for each of my measurements when making optimizations, and so it can be neglected, all measurements are correct.

## Conclusions

You should optimize a program when it is already perfectly debugged but you need more performance. Optimizations using intrinsics, assembler inserts and similar tools are associated with deterioration of code supportability and readability. Also, such code is hard to port because it can be tied to a specific architecture. 

Before we start optimizing, we should check how well the algorithm itself is implemented. In this case, overloading of the load-factor had a strong impact on the program performance. After that, you should evaluate the most bottlenecks in the program to understand what should be optimized first. Without such a search for bottlenecks, you may start optimizing something that has almost no effect on performance, spend a lot of time, spoil the code and still get about zero output. 

Sometimes, some optimizations already made should be abandoned in favor of code supportability and portability if they do not improve the program's performance much. 

Assembler optimizations are a powerful tool - using them alone, I was able to improve the program's performance by $2.79$ times. But you should still think several times about which optimizations to keep and which not to keep, and whether you need optimizations at all at the moment. 
