#ifndef HASH_TABLE_LIST_H
#define HASH_TABLE_LIST_H

#include <stddef.h>
#include <stdint.h>

#include "HashTable/HashTableElem.h"

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

HtListErrors HtListCtor  (HtListType* list, const size_t capacity = 0);
HtListErrors HtListCopy  (const HtListType* source, HtListType* target);
HtListErrors HtListDtor  (HtListType* list);
HtListErrors HtListVerify(HtListType* list);
HtListErrors HtListInsert(HtListType* list, const size_t anchorPos, const HashTableElemType value, 
                      size_t* insertedValPos);
HtListErrors HtListErase (HtListType* list, const size_t anchorPos);

HtListErrors HtListCapacityDecrease(HtListType* list);

HtListErrors HtListGetNextElem (HtListType* list, size_t pos, size_t *nextElemPos);
HtListErrors HtListGetPrevElem (HtListType* list, size_t pos, size_t *prevElemPos);
HtListErrors HtListGetElem     (HtListType* list, size_t pos, HashTableElemType* elemValue);
HtListErrors HtListGetElemNoCpy(HtListType* list, size_t pos, HashTableElemType* elemValue);
HtListErrors HtListSetElem     (HtListType* list, size_t pos, const HashTableElemType newElemValue);

extern "C" HtListErrors HtListFindElemByKey(HtListType* list, const char* key, size_t* elemPos);

size_t HtListGetHead(const HtListType* list);
size_t HtListGetTail(const HtListType* list);

#define HT_LIST_TEXT_DUMP(list) HtListTextDump((list), __FILE__, __func__, __LINE__)
void HtListTextDump(const HtListType* list, const char* fileName,
                                        const char* funcName,
                                        const int   line);
                                        
void HtListGraphicDump(const HtListType* list);

#define HT_LIST_DUMP(list) HtListDump((list), __FILE__, __func__, __LINE__)
void HtListDump(const HtListType* list, const char* fileName,
                                    const char* funcName,
                                    const int line);

#define HT_LIST_ERRORS_LOG_ERROR(error) HtListErrorsLogError((error), __FILE__, __func__, __LINE__)
void HtListErrorsLogError(HtListErrors error, const char* fileName,
                                          const char* funcName,
                                          const int   line);
#endif // HASH_TABLE_LIST_H