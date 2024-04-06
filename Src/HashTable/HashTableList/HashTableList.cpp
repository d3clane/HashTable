#include <assert.h>
#include <string.h>
#include <stdlib.h>

#include "Common/Log.h"
#include "HashTableList.h"

static const size_t MinCapacity       = 16;
static const HashTableElemType POISON = HashTableElemCtor();

static inline void HtListDataInit(HtListElemType* list, 
                                const size_t leftBorder, const size_t rightBorder,
                                const size_t listCapacity);

static inline void HtListElemDtor(HtListElemType* elem);

static void HtListElemInit(HtListElemType* elem, const HashTableElemType value, 
                                             const size_t prevPos, 
                                             const size_t nextPos);

static inline void DeleteFreeBlock(HtListType* list);
static inline void AddFreeBlock   (HtListType* list, const size_t newPos);

static inline HtListErrors HtListCapacityIncrease(HtListType* list);
static        HtListErrors HtListRebuild(HtListType* list);

//-------Graphic dump funcs---------

static inline void CreateImgInLogFile(const size_t imgIndex);
static inline void DotFileBegin(FILE* outDotFile);
static inline void DotFileEnd  (FILE* outDotFile);

static inline void DotFileCreateMainNode      (FILE* outDotFile, const HtListType* list, 
                                                                 const size_t nodeId);
static        void DotFileCreateMainNodes     (FILE* outDotFile, const HtListType* list);
static        void DotFileCreateMainEdges     (FILE* outDotFile, const HtListType* list);

static inline void DotFileCreateAuxiliaryInfo (FILE* outDotFile, const HtListType* list);
static        void DotFileCreateFictitiousEdges (FILE* outDotFile, const HtListType* list);

static inline HtListErrors GetPosForNewVal(HtListType* list, size_t* pos);

#ifndef NDEBUG

#define HT_LIST_CHECK(list)                    \
do                                          \
{                                           \
    HtListErrors listErr = HtListVerify(list);  \
                                            \
    if (listErr != HtListErrors::NO_ERR)      \
    {                                       \
        HT_LIST_TEXT_DUMP(list);               \
        HT_LIST_ERRORS_LOG_ERROR(listErr);     \
        return listErr;                     \
    }                                       \
} while (0)

#else

#define HT_LIST_CHECK(...)

#endif

HtListErrors HtListCtor(HtListType* list, const size_t listStandardCapacity)
{
    assert(list);

    size_t capacity = listStandardCapacity;
    if (capacity < MinCapacity)
        capacity = MinCapacity;
    
    list->size = 0;

    //fprintf(stderr, "List capacity ctoring\n");
    list->data = (HtListElemType*) calloc(capacity, sizeof(*list->data));
    //fprintf(stderr, "List capacity ctored\n");

    if (list->data == nullptr)
        return HtListErrors::MEMORY_ERR;

    list->capacity = capacity;

    //fprintf(stderr, "Initting 0 element\n");
    HtListElemInit(&list->data[0], POISON, 0, 0);
    // fprintf(stderr, "Initted 0 element\n");
    // fprintf(stderr, "Initting other data\n");
    HtListDataInit(list->data, 1, list->capacity, list->capacity);
    // fprintf(stderr, "Initted other data\n");

    list->end            = 0;
    list->freeBlockHead  = 1;

    HT_LIST_CHECK(list);

    return HtListErrors::NO_ERR;
}

HtListErrors HtListDtor(HtListType* list)
{
    assert(list);

    for (size_t i = 0; i < list->capacity; ++i)
        HtListElemDtor(list->data + i);

    free(list->data);

    list->end = list->freeBlockHead = 0;
    list->capacity = list->size = 0;

    list->data = nullptr;
    return HtListErrors::NO_ERR;
}

HtListErrors HtListCopy(const HtListType* source, HtListType* target)
{
    assert(source);
    assert(target);

    target->capacity      = source->capacity;
    target->freeBlockHead = source->freeBlockHead;
    target->data          = source->data;
    target->size          = source->size;
    
    return HtListErrors::NO_ERR;
}

#define LOG_ERR(error)            \
do                                \
{                                 \
    HT_LIST_ERRORS_LOG_ERROR(error); \
    HT_LIST_TEXT_DUMP(list);         \
    return error;                 \
} while (0)

HtListErrors HtListVerify(HtListType* list)
{
    assert(list);

    if (list->data == nullptr)
        LOG_ERR(HtListErrors::DATA_IS_NULLPTR);
    
    if (list->capacity < list->size)
        LOG_ERR(HtListErrors::OUT_OF_RANGE);

    //fprintf(stderr, "Veryfying POISON\n");
    if (!HashTableElemsCmp(list->data[0].value, POISON))
        LOG_ERR(HtListErrors::INVALID_NULLPTR);
    //fprintf(stderr, "Veryfied POISON\n");

    size_t freeBlockIndex = list->freeBlockHead;
    if (freeBlockIndex == 0)
        return HtListErrors::NO_ERR;

    if (list->data[list->freeBlockHead].prevPos != 0)
        LOG_ERR(HtListErrors::INVALID_DATA);

    while (list->data[freeBlockIndex].nextPos != 0)
    {
        if (!HashTableElemsCmp(list->data[freeBlockIndex].value, POISON))
            LOG_ERR(HtListErrors::INVALID_DATA);

        if (list->data[freeBlockIndex].nextPos > list->capacity)
            LOG_ERR(HtListErrors::OUT_OF_RANGE);

        if (list->data[freeBlockIndex].prevPos > list->capacity)
            LOG_ERR(HtListErrors::OUT_OF_RANGE);
        
        freeBlockIndex = list->data[freeBlockIndex].nextPos;
    }

    return HtListErrors::NO_ERR;
}

void HtListDump(const HtListType* list, const char* fileName,
                                    const char* funcName,
                                    const int line)
{
    assert(list);
    assert(fileName);
    assert(funcName);
    
    HtListTextDump(list, fileName, funcName, line);

    HtListGraphicDump(list);
}

void HtListTextDump(const HtListType* list, const char* fileName,
                                        const char* funcName,
                                        const int   line)
{
    assert(list);
    assert(fileName);
    assert(funcName);
    
    LogBegin(fileName, funcName, line);

    static const size_t numberOfElementsToPrint = 16;

    Log("HtList head: %zu, list tail: %zu\n", HtListGetHead(list), HtListGetTail(list));
    Log("Free blocks head: %zu\n", list->freeBlockHead);

    Log("HtList capacity: %zu\n", list->capacity);
    Log("HtList size    : %zu\n", list->size);

    //-----Print all data----

    Log("Data[%p]:\n", list->data);

    for (size_t i = 0; i < numberOfElementsToPrint && i < list->capacity; ++i)
    {
        Log("\tElement id: %zu, key: %s, value: %d, " 
            "previous position: %zu, next position: %zu\n",
            i, list->data[i].value.key, list->data[i].value.val,
               list->data[i].prevPos,   list->data[i].nextPos);
    }

    Log("\t...\n");

    //-----Print list-------

    Log("HtList:\n");

    size_t listTail = HtListGetTail(list);
    for (size_t i = HtListGetHead(list); i != listTail; i = list->data[i].nextPos)
    {
        Log("\tElement id: %zu, key: %s, value: %d, " 
            "previous position: %zu, next position: %zu\n",
            i, list->data[i].value.key, list->data[i].value.val,
               list->data[i].prevPos,   list->data[i].nextPos);
    }

    Log("\tHtList element: %zu, key: %s, value: %d, previous position: %zu, next position: %zu\n",
        listTail, list->data[listTail].value.key, list->data[listTail].value.val,
            list->data[listTail].prevPos, list->data[listTail].nextPos);
    
    LOG_END();
}

static inline void CreateImgInLogFile(const size_t imgIndex)
{
    static const size_t maxImgNameLength  = 64;
    static char imgName[maxImgNameLength] = "";
    snprintf(imgName, maxImgNameLength, "imgs/img_%zu_time_%s.png", imgIndex, __TIME__);

    static const size_t     maxCommandLength  = 128;
    static char commandName[maxCommandLength] = "";
    snprintf(commandName, maxCommandLength, "dot list.dot -T png -o %s", imgName);
    //TODO: fork + exec
    system(commandName);

    snprintf(commandName, maxCommandLength, "<img src = \"%s\">", imgName);    
    Log(commandName);
}

static inline void DotFileBegin(FILE* outDotFile)
{
    fprintf(outDotFile, "digraph G{\nrankdir=LR;\ngraph [bgcolor=\"#31353b\"];\n");
}

static inline void DotFileEnd(FILE* outDotFile)
{
    fprintf(outDotFile, "\n}\n");
}

static inline void DotFileCreateMainNode(FILE* outDotFile, const HtListType* list, const size_t nodeId)
{
    fprintf(outDotFile, "node%zu"
                        "[shape=Mrecord, style=filled, fillcolor=\"#7293ba\","
                        "label  =\"id: %zu   |"
                                "key: %s   |" 
                                "value  : %d   |"
                            "<f0> next: %zu  |"
                            "<f1> prev: %zu\","
                            "color = \"#008080\"];\n",
                        nodeId, nodeId, 
                        list->data[nodeId].value.key, 
                        list->data[nodeId].value.val,
                        list->data[nodeId].nextPos,
                        list->data[nodeId].prevPos);    
}

static void DotFileCreateMainNodes(FILE* outDotFile, const HtListType* list)
{
    for (size_t i = 0; i < list->capacity; ++i)
        DotFileCreateMainNode(outDotFile, list, i);  
}

static inline void DotFileCreateAuxiliaryInfo(FILE* outDotFile, const HtListType* list)
{
    fprintf(outDotFile, "node[shape = octagon, style = \"filled\", fillcolor = \"lightgray\"];\n");
    fprintf(outDotFile, "edge[color = \"lightgreen\"];\n");

    fprintf(outDotFile, "head->node%zu;\n", HtListGetHead(list));
    fprintf(outDotFile, "tail->node%zu;\n", HtListGetTail(list));
    fprintf(outDotFile, "end->node%zu;\n", 0lu);
    fprintf(outDotFile, "\"free block\"->node%zu;\n", list->freeBlockHead);
    fprintf(outDotFile, "nodeInfo[shape = Mrecord, style = filled, fillcolor=\"#19b2e6\","
                        "label=\"capacity: %zu | size : %zu\"];\n",
                        list->capacity, list->size);   
}

static void DotFileCreateFictitiousEdges(FILE* outDotFile, const HtListType* list)
{
    assert(outDotFile);
    assert(list);

    fprintf(outDotFile, "node0");
    for (size_t i = 1; i < list->capacity; ++i)
        fprintf(outDotFile, "->node%zu", i);
    fprintf(outDotFile, "[color=\"#31353b\", weight = 1, fontcolor=\"blue\",fontsize=78];\n");
}

static void DotFileCreateMainEdges(FILE* outDotFile, const HtListType* list)
{
    assert(outDotFile);
    assert(list);

    fprintf(outDotFile, "edge[color=\"red\", fontsize=12, constraint=false];\n");

    for (size_t i = 0; i < list->capacity; ++i)
        fprintf(outDotFile, "node%zu->node%zu;\n", i, list->data[i].nextPos);
}

void HtListGraphicDump(const HtListType* list)
{
    assert(list);

    static const char* dotFileName = "list.dot";
    FILE* outDotFile = fopen(dotFileName, "w");

    if (outDotFile == nullptr)
        return;

    DotFileBegin(outDotFile);

    DotFileCreateMainNodes     (outDotFile, list);
    DotFileCreateFictitiousEdges (outDotFile, list);
    DotFileCreateMainEdges     (outDotFile, list);
    DotFileCreateAuxiliaryInfo (outDotFile, list);

    DotFileEnd(outDotFile);

    fclose(outDotFile);

    static size_t imgIndex = 0;
    CreateImgInLogFile(imgIndex);
    imgIndex++;
}

HtListErrors HtListInsert(HtListType* list, const size_t anchorPos, const HashTableElemType value, 
                      size_t* insertedValPos)
{
    assert(list);
    assert(insertedValPos);
    assert(anchorPos < list->capacity);
    assert(anchorPos == list->end || !HashTableElemsCmp(list->data[anchorPos].value, POISON));

    HT_LIST_CHECK(list);
    
    size_t newValPos = 0;
    HtListErrors error = HtListErrors::NO_ERR;
               error = GetPosForNewVal(list, &newValPos);

    if (error != HtListErrors::NO_ERR)
        return error;

    HtListElemInit(&list->data[newValPos], 
                  value, list->data[anchorPos].prevPos, anchorPos);

    const size_t prevAnchor = list->data[anchorPos].prevPos;

    list->data[prevAnchor].nextPos = newValPos;
    list->data[anchorPos].prevPos  = newValPos;

    list->size++;

    HT_LIST_CHECK(list);

    *insertedValPos  = newValPos;

    return HtListErrors::NO_ERR;
}

HtListErrors HtListErase (HtListType* list, const size_t anchorPos)
{
    assert(list);
    assert(anchorPos < list->capacity);

    HT_LIST_CHECK(list);

    list->data[list->data[anchorPos].prevPos].nextPos = list->data[anchorPos].nextPos;
    list->data[list->data[anchorPos].nextPos].prevPos = list->data[anchorPos].prevPos;

    AddFreeBlock(list, anchorPos);

    list->size--;

    HT_LIST_CHECK(list);

    return HtListErrors::NO_ERR;
}

HtListErrors HtListGetNextElem(HtListType* list, size_t pos, size_t *nextElemPos)
{
    assert(list);
    assert(nextElemPos);

    HT_LIST_CHECK(list);

    *nextElemPos = list->data[pos].nextPos;

    return HtListErrors::NO_ERR;
}

HtListErrors HtListGetPrevElem(HtListType* list, size_t pos, size_t *prevElemPos)
{
    assert(list);
    assert(prevElemPos);

    HT_LIST_CHECK(list);

    *prevElemPos = list->data[pos].prevPos;

    return HtListErrors::NO_ERR;
}

HtListErrors HtListGetElem(HtListType* list, size_t pos, HashTableElemType* elemValue)
{
    assert(list);
    assert(elemValue);  

    if (pos == 0)
        return HtListErrors::TRYING_TO_GET_NULL_ELEMENT;

    HT_LIST_CHECK(list);

    *elemValue = HashTableElemCpy(list->data[pos].value);
    
    return HtListErrors::NO_ERR;
}

HtListErrors HtListSetElem(HtListType* list, size_t pos, const HashTableElemType newElemValue)
{
    assert(list);

    if (pos == 0)
        return HtListErrors::TRYING_TO_CHANGE_NULL_ELEMENT;

    HT_LIST_CHECK(list);

    size_t nextPos = list->data[pos].nextPos;
    size_t prevPos = list->data[pos].prevPos;

    HtListElemDtor(list->data + pos);

    HtListElemInit(list->data + pos, newElemValue, prevPos, nextPos);

    HT_LIST_CHECK(list);

    return HtListErrors::NO_ERR;
}

HtListErrors HtListFindElemByKey(HtListType* list, const char* key, size_t* elemPos)
{
    assert(list);
    assert(elemPos);

    HT_LIST_CHECK(list);

    size_t pos = list->end;

    do
    {
        HashTableElemType listVal = {};
        HtListGetElem(list, pos, &listVal);
        
        if (listVal.key && strcmp(listVal.key, key) == 0)
        {
            *elemPos = pos;

            HashTableElemDtor(&listVal);
            return HtListErrors::NO_ERR;
        }

        HashTableElemDtor(&listVal);

        HtListGetNextElem(list, pos, &pos);
    } while (pos != list->end);
    
    return HtListErrors::NO_ELEMENT_FOUND;
}

static inline void HtListDataInit(HtListElemType* list, 
                                const size_t leftBorder, const size_t rightBorder,
                                const size_t listCapacity)
{
    assert(list);
    assert(leftBorder  <= rightBorder);
    assert(rightBorder <= listCapacity);
    assert(leftBorder != 0);

    for (size_t i = leftBorder; i < rightBorder; ++i)
        HtListElemInit(&list[i], POISON, i - 1, i + 1);

    if (rightBorder == listCapacity)
    {
        HtListElemDtor(list + listCapacity - 1);
        HtListElemInit(&list[listCapacity - 1], POISON, listCapacity - 2, 0);  
    }
}

void HtListElemInit(HtListElemType* elem, const HashTableElemType value, 
                                          const size_t prevPos, 
                                          const size_t nextPos)
{
    assert(elem);
    
    elem->value   = value;
    elem->prevPos = prevPos;
    elem->nextPos = nextPos;
}

void HtListErrorsLogError(HtListErrors error, const char* fileName,
                                          const char* funcName,
                                          const int   line)
{
    assert(fileName);
    assert(funcName);

    Log(HTML_RED_HEAD_BEGIN
        "Logging errors called from file: %s, func: %s, line: %d"
        HTML_HEAD_END, fileName, funcName, line);

    switch(error)
    {
        case HtListErrors::DATA_IS_NULLPTR:
            Log("Data is nullptr\n");
            break;
        case HtListErrors::INVALID_DATA:
            Log("Invalid data\n");
            break;
        case HtListErrors::INVALID_NULLPTR:
            Log("Null address in list is invalid\n");
            break;
        case HtListErrors::MEMORY_ERR:
            Log("Memory error\n");
            break;
        case HtListErrors::OUT_OF_RANGE:
            Log("HtList element is out of range\n");
            break;
        
        case HtListErrors::NO_ERR:
        default:
            break;
    }
}                                          

static inline void DeleteFreeBlock(HtListType* list)
{
    assert(list);

    if (list->freeBlockHead == 0)
        HtListCapacityIncrease(list);

    assert(list->freeBlockHead != 0);

    size_t nextPos = list->data[list->freeBlockHead].nextPos;
    size_t prevPos = list->data[list->freeBlockHead].prevPos;
    HtListElemDtor(list->data + list->freeBlockHead);
    HtListElemInit(list->data + list->freeBlockHead, POISON, prevPos, nextPos);
    
    list->freeBlockHead        = list->data[list->freeBlockHead].nextPos;

    if (list->freeBlockHead != 0)
        list->data[list->freeBlockHead].prevPos = 0;
}

static inline void AddFreeBlock(HtListType* list, const size_t newPos)
{
    assert(list);
    assert(newPos < list->capacity);

    if (list->freeBlockHead == 0)
    {
        list->freeBlockHead = newPos;
        HtListElemDtor(list->data + list->freeBlockHead);
        HtListElemInit(&list->data[list->freeBlockHead], POISON, 0, 0);

        return;
    }

    //do not change order!
    list->data[list->freeBlockHead].prevPos = newPos;
    HtListElemDtor(list->data + newPos);
    HtListElemInit(&list->data[newPos], POISON, 0, list->freeBlockHead);
    list->freeBlockHead = newPos;
}

static inline HtListErrors HtListCapacityIncrease(HtListType* list)
{
    assert(list);
    
    if (list->freeBlockHead == 0)
        list->freeBlockHead = list->capacity;

    list->capacity *= 2;

    void* tmpPtr = realloc(list->data, list->capacity * sizeof(*list->data));

    if (tmpPtr == nullptr)
        return HtListErrors::MEMORY_ERR;
    
    list->data = (HtListElemType*)tmpPtr;
    
    HtListDataInit(list->data, list->capacity / 2, list->capacity, list->capacity);

    return HtListErrors::NO_ERR;
}

static HtListErrors HtListRebuild(HtListType* list)
{
    assert(list);

    HtListType newHtList = {};
    HtListCtor(&newHtList, list->capacity); 

    //-----rebuild used values-------

    size_t posInNewHtList = 1;

    size_t listTail = HtListGetTail(list);
    for (size_t i = HtListGetHead(list); i != listTail; i = list->data[i].nextPos)
    {
        HtListElemInit(&newHtList.data[posInNewHtList], list->data[i].value, 
                                                  posInNewHtList - 1, posInNewHtList + 1);
        ++posInNewHtList;
    }
    HtListElemInit(newHtList.data + posInNewHtList, list->data[listTail].value,
                                              posInNewHtList - 1, 0);
    HtListElemInit(newHtList.data, POISON, posInNewHtList, 1);

    newHtList.end  = 0;
    newHtList.freeBlockHead = (posInNewHtList + 1) % list->capacity;
    newHtList.size = list->size;

    HtListDtor(list);
    *list = newHtList;

    //NO newHtList Dtor because there is calloced memory used for list and nothing more dynamic
    return HtListErrors::NO_ERR;
}

HtListErrors HtListCapacityDecrease(HtListType* list)
{
    assert(list);

    HtListRebuild(list);
    assert(HtListGetTail(list) * 2 < list->capacity);

    list->capacity /= 2;

    void* tmpPtr = realloc(list->data, list->capacity * sizeof(*list->data));

    if (tmpPtr == nullptr)
        return HtListErrors::MEMORY_ERR;
    
    list->data = (HtListElemType*)tmpPtr;

    return HtListErrors::NO_ERR;
}

size_t HtListGetHead(const HtListType* list)
{
    assert(list);

    return list->data[list->end].nextPos;
}

size_t HtListGetTail(const HtListType* list)
{
    assert(list);

    return list->data[list->end].prevPos;
}

static inline HtListErrors GetPosForNewVal(HtListType* list, size_t* pos)
{
    assert(list);
    assert(pos);

    HtListErrors error = HtListErrors::NO_ERR;
    if (list->freeBlockHead == 0)
        error = HtListCapacityIncrease(list);

    if (error != HtListErrors::NO_ERR)
        return error;
    
    *pos = list->freeBlockHead;
    DeleteFreeBlock(list);

    return HtListErrors::NO_ERR;
}

static inline void HtListElemDtor(HtListElemType* elem)
{
    assert(elem);

    HashTableElemDtor(&elem->value);
    
    elem->nextPos = 0;
    elem->prevPos = 0;
}