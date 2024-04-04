# Исследование хештаблиц

## Описание

Проект разбит на две части - 

1. Исследование различных хеш-функций, чтобы понять, какая из них распределяет наиболее равномерно по хештаблице. 
2. Оптимизация производительности хештаблицы.

Опишем, как устроена хештаблица.

## Описание хештаблицы

Хештаблица написана методом цепочек, в качестве ключа выступает строчка, в качестве значения - булевская переменная. Если строчка есть в хештаблицы, то ее значение - true, как бы подтверждение факта существования этой строчки в хештаблице. Цепочки реализованы с помощью списка. Используя мою реализацию [cache-friendly двусвязного списка](https://github.com/d3clane/list), которая использует динамический массив для хранения блоков данных.

Схематичный вид хештаблицы:

![hash table](https://github.com/d3clane/HashTable/blob/main/ReadmeAssets/imgs/HashTable.png).

На картинке показано, что два входных значения с ключами "emberlizard" и "soggyturtle" на выходе с хеш-функции получают одно и то же значение, а значит попадают в одну ячейку хештаблицы. Здесь используется список, чтобы последовательно сохранить эти значения в одной ячейке - как бы цепочка.

Для исследования используется набор топ [370000 английский слов](https://github.com/dwyl/english-words?tab=readme-ov-file). Load-factor для моей хештаблицы равен $7.52$. Почему так много? Это необходимо, чтобы лучше видеть, насколько равномерно хеш-функции распределяют по хештаблице. Если бы load-factor был оптимальным и подбирался для ускорения работы хеш таблицы(в районе 1.5 - 2), то при исследовании распределения пики загруженности отдельных ячеек были бы не так видны и могло бы показаться, что распределение получается близким к равномерному, хотя на деле это не так. Сам же размер хеш таблицы равен 49157 - это простое число, так как при таком выборе распределение будет более вероятно равномерным. Объясню выбор размера, равным простому числу, подробнее:

Часто данные на выходе с хеш-функции могут иметь какой-то свой паттерн, то есть идти с каким-то шагом или же число элементов кратных какому-то $k$ больше, чем других. Пусть наша хеш-функция получила на вход какие-то данные, а на выходе дала ${k, 2k, 3k, 4k, ...}$ - все данные кратны $k$. Для того, чтобы данные попали в ячейки хештаблицы результаты берутся по модулю $hash_table_size$ - то есть по модулю количества ячеек в хештаблице. Тогда посмотрим, сколько различных ячеек в таком случае сможет заполнить хештаблица, в сколькие она чисто теоретически может положить эти данные. Данные начнут попадать в одни и те же ячейки, когда $k \equiv r \cdot k \pmod{hash_table_size}$, где $r - 1$ получается равным количеству различных остатков по делению, который можно получить. Решим это уравнение - его можно переписать в виде:

$k + hash_table_size \cdot \alpha = r \cdot k$, где $\alpha$ - какое-то целое число. Наименьшее $k$, когда это выполняется - $r \cdot k = $LCM(k, hash_table_size)$, то есть $r \cdot k = \frac{k \cdot hash_table_size}{GCD(k, hash_table_size)}$ или $r = \frac{hash_table_size}{GCD(k, hash_table_size)}$. 

Получается, что взяв hash_table_size простым числом мы с большей вероятностью получим $GCD(k, hash_table_size) = 1$, то есть сможем заполнить все клетки хештаблицы. 

## Исследование хеш-функций

Мною было написано 8 различных хеш-функций, все хеш-функции имеет сигнатуру вида `uint32_t HashFunc(const char* inString)`:

1. ConstantHash       - всегда возвращает число 42.
2. FirstCharASCIIHash - возвращает ASCII код первой буквы строки. 
3. StringLengthHash   - возвращает длину строки.
4. SumCharsASCIIHash  - суммирует ASCII коды букв строки.
5. RolHash            - пересчитывает hash по формуле $hash(n) = ROL(hash(n - 1)) \oplus str[n]$, где str - строчка, str[n] - $n$-я буква строки. ROL - битовая операция rotate left. Изначально hash(0) = str[0]. $\oplus$ - операция XOR. 
6. RorHash            - аналогично RolHash, только $hash(n) = ROR(hash(n - 1)) \oplus str[n]$. ROR - битовая операция rotate right.
7. MurMurHash         - объяснено на странице [википедии](https://en.wikipedia.org/wiki/MurmurHash).
8. CRC32Hash          - также объяснено на странице [википедии](https://en.wikipedia.org/wiki/Cyclic_redundancy_check).

Несложно увидеть, что некоторые из них заведомо плохие, но все равно необходимы для более полного исследования.

Исследование будет как визуальное, так и формульное - строятся графики, сраниваются, а также считается стандартное отклонение. Стандартное отклонение считается по формуле: $\sum_{0}^{n} {\frac{\sqrt{(x_i^2 - x_{mean}^2)}}{n}}$, где $x_i$ - длина i-й цепочки, $x_{mean}$ - средняя длина цепочки, а $n$ - количество таких цепочек. В моем случае $n = 49157$.

### Constant hash

```
uint32_t ConstantHash(const char* inString)
{
    assert(inString);

    return (uint32_t)42;
}
```

Как и ожидалось, все распределяется в один и тот же список:

![ConstantHash](https://github.com/d3clane/HashTable/blob/main/ReadmeAssets/imgs/ConstantHash.png). 

В увеличенном масштабе:

![ConstantHashClose](https://github.com/d3clane/HashTable/blob/main/ReadmeAssets/imgs/ConstantHashClose.png)

Стандартное отклонение - 1669.3.

Здесь проблема в методе хеширования очевидна - заполняется только одна ячейка.

### First char ASCII hash

```
uint32_t FirstCharASCIIHash(const char* inString)
{
    assert(inString);

    return (uint32_t)inString[0];
}
```

Ожидаемо, распределение должно быть в границах $[97, 122]$, как ASCII коды 'a'-'z'. То есть заполняется очень малое число ячеек. Гистограмма распределения:

![First char ASCII](https://github.com/d3clane/HashTable/blob/main/ReadmeAssets/imgs/FirstCharASCIIHash.png)

В увеличенном масштабе:

![First char ASCII close](https://github.com/d3clane/HashTable/blob/main/ReadmeAssets/imgs/FirstCharASCIIHashClose.png)

Стандартное отклонение - 403.4.

По гистограмме можно понять, что наиболее часто слова начинаются на буквы 'c', 'p', 's', так как на них максимальные пики загруженности. Наиболее редко на букву 'x'. 

Проблема данного метода хеширования в ограниченности количества использованных ячеек, а также в том, что есть буквы, с которых слова почти не начинаются, а значит эти ячейки почти не будут заполнены.

### String length hash

```
uint32_t StringLengthHash(const char* inString)
{
    assert(inString);

    return (uint32_t)strlen(inString);
}
```

Средняя длина английского слова - 4.7, считается что самое длинное слово - pneumonoultramicroscopicsilicovolcanoconiosis, длина которого 45. Получается, что на нашем датасете значения будут в границах [1, 45], причем ожидается нормальное распределение, так как фактически слов очень малой и очень большой длины сильно меньше, чем слов средней длины. Проверим на практике:

![string length hash](https://github.com/d3clane/HashTable/blob/main/ReadmeAssets/imgs/StringLengthHash.png)

В увеличенном масштабе:

![string length hash close](https://github.com/d3clane/HashTable/blob/main/ReadmeAssets/imgs/StringLengthHashClose.png)

Стандартное отклонение - 525.9.

Проблема такого метода хеширование в сильной ограниченности количества использованных ячеек, а также в нормальном распределении. 

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

Оценим, какой результат мне может вернуть подобная хеш-функция. Самое длинное слово - 45 букв, буква с самым большим ASCII кодом - 'z' =  122. Тогда результат $ \leq 45 * 122 = 5490$. Это, очевидно никогда не достигается (не существует слова 'zzzzzzz....'). При этом в среднем английских слов больше в районе от 4 до 15 букв, то есть значения в районе $ 'a' * 4 = 388 \leq hash_func_res \leq 'z' * 15 = 1830$. До этого размер хештаблицы был $49157$, сейчас, специально выбирем размер $389$. Зачем это было сделано - будет понятно позже. 

Полученный график:

![ASCII sum small cap](https://github.com/d3clane/HashTable/blob/main/ReadmeAssets/imgs/SumCharsASCIIHashSmallCap.png)

Стандартное отклонение - 156.5.

Казалось бы - неплохая хеш-функция - заполняет всю область определения, да, есть пики, но если бы load-factor был бы в разы меньше, такого бы не наблюдалось. Получается, если увеличить размер хеш-таблицы получится очень хорошее распредение, так ведь? Проверим.

Вернемся к прошлому размеру хештаблицы. Полученный график:

![ASCII sum](https://github.com/d3clane/HashTable/blob/main/ReadmeAssets/imgs/SumCharsASCIIHash.png)

В увеличенном масштабе:

![ASCII sum close](https://github.com/d3clane/HashTable/blob/main/ReadmeAssets/imgs/SumCharsASCIIHashClose.png)

Стандартное отклонение - 63.7.

Видно очень ярко выраженные пики, даже несмотря на то, что размер хеш таблицы увеличился, а ячейки с номером больше 2500 почти не заполнялись - экстраполировать результат не удалось. Сверху уже объяснялась причина такого поведения - это же не случайный набор букв, а полноценные английские слова. Для них есть закономерности в виде средней длины слова, самые часто встречающиеся буквы и тому подобное - отсюда и вытекает подобное распределение.

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

Помимо хеширования интересует следующий момент - на ассемблере существует инструкция `rol`, сможет ли компилятор увидеть это и заменить вызов моей функции `Rol` просто вызовом инструкции. Проверим это в godbolt'e. 

![No opt rol](https://github.com/d3clane/HashTable/blob/main/ReadmeAssets/imgs/NoOptRol.png)

Даже без оптимизаций компилятор смог заметить, что это инструкция rol и заменить ее! Правда, все равно оставил вызов моей функции Rol. А теперь с оптимизацией -O1:

![Opt rol](https://github.com/d3clane/HashTable/blob/main/ReadmeAssets/imgs/OptRol.png)

Компилятор полностью избавился от вызова функции Rol и заменил его инструкцией rol. 

Теперь график распределения:

![Rol hash](https://github.com/d3clane/HashTable/blob/main/ReadmeAssets/imgs/RolHash.png)

Стандартное отклонение - 8.3.

Заметно, что как-то чередуются размеры списков - подряд идет много сильно заполненных списков, затем много мало заполненных. При этом при больших значениях хеша распределение идет уже равномерное.

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

Результаты в godbolt абсолютно аналогичные, компилятор замечает, что это инструкция ror. График заполненности:

![Ror hash](https://github.com/d3clane/HashTable/blob/main/ReadmeAssets/imgs/RorHash.png)

Стандартное отклонение - 18.0

Распределение хуже, чем для RolHash, так как есть очень высокие пики. С чем это может быть связано. Средним размер слова будет в районе 5 - 16 симвлов. Получается около 5-16 rotate right моего hash, то есть очень сильно заполняются верхние 5 - 16 бит моего хеша. А потом этот хеш берется по модулю $49157$ и все эти биты становятс резко малозначимыми, как итог очень много хешей попадает в одну и ту же ячейку.

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
```

Распределение на графике:

![MurMurHash](https://github.com/d3clane/HashTable/blob/main/ReadmeAssets/imgs/MurMurHash.png)

Из-за особенностей масштаба на графике не видно, что на самом деле существуют списки с не очень большими размерами. Догадаться о их существовании легко - если судить по графику, то получится, что load-factor не соответствует заявленному (по графику он кажется явно больше 7.5). Как раз для таких случаев и нужен подсчет стандартного отклонения, чтобы дополнительно формульно оценить и сопоставить с визуальными результатами. MurMurHash в увеличенном масштабе:

![MurMurHash close](https://github.com/d3clane/HashTable/blob/main/ReadmeAssets/imgs/MurMurHashClose.png)

Стандартное отклонение - 2.74. 

Видно, что распределение получается достаточно равномерным. 

### CRC32 hash

```
uint32_t CRC32Hash(const char* inString)
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
```

Распределение на графике:

![CRC32 hash](https://github.com/d3clane/HashTable/blob/main/ReadmeAssets/imgs/CRC32Hash.png)

Здесь тоже из-за особенностей масштаба не видно, что на самом деле есть и списки с не очень большими размерами. В увеличенном масштабе получится:

![CRC32 hash close](https://github.com/d3clane/HashTable/blob/main/ReadmeAssets/imgs/CRC32HashClose.png)

Стандартное отклонение - 2.75.

Это тоже хорошо распределяющая хеш-функция.

### Промежуточный вывод

Сводная таблица стандартных отклонений для различных хеш-функций:

|                       |Стандартное отклонение |
|---                    |---                    |
|Сonstant hash          | 1669.3                |
|First char ASCII hash  | 403.4                 |
|String length hash     | 525.9                 |
|Sum chars ASCII hash   | 63.7                  |
|Rol hash               | 8.3                   |
|Ror hash               | 18.0                  |
|Murmur hash            | 2.74                  |
|CRC32 hash             | 2.75                  |

Очевидно, стоит выбирать между MurMurHash и CRC32 Hash, так как распределение получается наиболее равномерным, при этом особой разница между распределениями нет - и визуально, и формульно (стандартное отклонение) они максимально похожи. Следующим критерием выбора может служить скорость работы каждой из хеш-функций - они вызываются при операциях вставки / удаления / поиска в хештаблице, а значит вызываются достаточно часто. Для своей хештаблицы я выберу CRC32, потому что такой выбор даст мне больший простор для оптимизаций во второй части работы - существуют intrinsic-и, которые применяют алгоритм хеширования CRC32, а значит есть возможность для ускорения. 

## Оптимизация хештаблицы

