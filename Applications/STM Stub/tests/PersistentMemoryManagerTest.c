/*
** Copyright © Bart Kampers
*/

#include <stdlib.h>
#include <string.h>

#include "Simple.h"
#include "stm32f10x_flash.h"

#include "PersistentElementManager.h"


char* expected;
int callbackCount;

void checkCallback(Reference reference)
{
    callbackCount++;
}


void testInitPersistentDataManager()
{
    InitPersistentDataManager();
    ASSERT_EQUAL_INT(0, GetTypeCount());
}


void testRegisterType()
{
    TypeId id1, id2;
    InitPersistentDataManager();
    RegisterType(&id1);
    ASSERT_UNEQUAL_INT(INVALID_TYPE_ID, id1);
    ASSERT_EQUAL_INT(1, GetTypeCount());
    RegisterType(&id2);
    ASSERT_UNEQUAL_INT(INVALID_TYPE_ID, id2);
    ASSERT_UNEQUAL_INT(id1, id2);
    ASSERT_EQUAL_INT(2, GetTypeCount());
}


void testFindUnavailableId()
{
    Reference found;
    TypeId id;
    InitPersistentDataManager();
    ASSERT_STATUS(INVALID_ID, FindFirst(0, &found));
    ASSERT_STATUS(INVALID_ID, FindFirst(2, &found));
    found = 0;
    ASSERT_STATUS(INVALID_ID, FindNext(2, &found));
    RegisterType(&id);
    ASSERT_STATUS(INVALID_ID, FindFirst(id, &found));
    ASSERT_EQUAL_INT(NULL_REFERENCE, found);
    found = NULL_REFERENCE;
    ASSERT_STATUS("InvalidSearchReference", FindNext(id, &found));
    found = PersistentMemoryLimit();
    ASSERT_STATUS("InvalidSearchReference", FindNext(id, &found));
}


void testAllocate()
{
    TypeId id, read;
    Reference reference;
    ElementSize size;
    InitPersistentDataManager();
    RegisterType(&id);
    ASSERT_OK(AllocateElement(id, 1, &reference));
    ASSERT_OK(GetType(reference, &read));
    ASSERT_EQUAL_INT(id, read);
    ASSERT_OK(GetSize(reference, &size));
    ASSERT_EQUAL_INT(1, size);
}


void testFindRemove()
{
    TypeId id;
    Reference first, second, found;
    InitPersistentDataManager();
    RegisterType(&id);
    ASSERT_OK(AllocateElement(id, 2, &first));
    ASSERT_OK(AllocateElement(id, 2, &second));
    ASSERT_UNEQUAL_INT(NULL_REFERENCE, first);
    ASSERT_OK(FindFirst(id, &found));
    ASSERT_EQUAL_INT(first, found);
    ASSERT_OK(FindNext(id, &found));
    ASSERT_EQUAL_INT(second, found);
    ASSERT_STATUS(INVALID_ID, FindNext(id, &found));
    ASSERT_EQUAL_INT(NULL_REFERENCE, found);
    ASSERT_OK(RemoveElement(first));
    ASSERT_OK(FindFirst(id, &found));
    ASSERT_EQUAL_INT(second, found);
    ASSERT_OK(RemoveElement(second));
    ASSERT_STATUS(INVALID_ID, FindFirst(id, &found));
    ASSERT_EQUAL_INT(NULL_REFERENCE, found);
}


void testStoreGet()
{
    TypeId id;
    Reference reference;
    int value = 0x7E7E7E7E;
    int read;
    InitPersistentDataManager();
    RegisterType(&id);
    ASSERT_OK(AllocateElement(id, sizeof(int), &reference));
    ASSERT_OK(StoreElement(&value, reference, sizeof(int)));
    ASSERT_OK(GetElement(reference, sizeof(int), &read));
    ASSERT_EQUAL_INT(value, read);
}


void testStoreGetByte()
{
    TypeId id;
    Reference reference;
    uint16_t offset;
    InitPersistentDataManager();
    RegisterType(&id);
    ASSERT_OK(AllocateElement(id, sizeof(int), &reference));
    for (offset = 0; offset < sizeof(int); ++offset)
    {
        byte read;
        ASSERT_OK(StoreElementByte(reference, offset, (byte) offset));
        ASSERT_OK(GetElementByte(reference, offset, &read));
        ASSERT_EQUAL_INT((byte) offset, read);
    }
}


void testStoreGetBytes()
{
    TypeId id;
    Reference reference;
    int value = 0x5A5A5A5A;
    int read;
    InitPersistentDataManager();
    RegisterType(&id);
    ASSERT_OK(AllocateElement(id, sizeof(int), &reference));
    ASSERT_OK(StoreElementBytes(reference, 0, sizeof(int), &value));
    ASSERT_OK(GetElementBytes(reference, 0, sizeof(int), &read));
    ASSERT_EQUAL_INT(value, read);
}


void testCheckPersistentMemoryEmpty()
{
    Reference limit = PersistentMemoryLimit();
    memset(expected, 0, limit);
    InitPersistentDataManager();
    ASSERT_OK(CheckPersistentMemory(NULL));
    ASSERT_EQUAL_INT(0, memcmp(expected, flashMemory, limit));
    memset(flashMemory, 0xFF, limit);
    ASSERT_OK(CheckPersistentMemory(NULL));
    ASSERT_EQUAL_INT(0, memcmp(expected, flashMemory, limit));
}


void testCheckPersistentMemoryTwoTypes()
{
    TypeId id1, id2;
    ElementSize size = 10;
    Reference limit = PersistentMemoryLimit();
    callbackCount = 0;
    InitPersistentDataManager();
    RegisterType(&id1);
    RegisterType(&id2);
    memcpy(flashMemory, &id1, sizeof(TypeId));
    memcpy(flashMemory + sizeof(TypeId), &size, sizeof(ElementSize));
    memcpy(flashMemory + sizeof(TypeId) + sizeof(ElementSize) + size, &id2, sizeof(TypeId));
    memcpy(flashMemory + sizeof(TypeId) + sizeof(ElementSize) + size + sizeof(TypeId), &size, sizeof(ElementSize));
    memcpy(expected, flashMemory, limit);
    ASSERT_OK(CheckPersistentMemory(&checkCallback));
    ASSERT_EQUAL_INT(0, memcmp(expected, flashMemory, limit));
    ASSERT_UNEQUAL_INT(0, callbackCount);
}


void testCheckPersistentMemoryOutOfBounds()
{
    TypeId id1;
    Reference limit = PersistentMemoryLimit();
    InitPersistentDataManager();
    RegisterType(&id1);
    memset(flashMemory + limit - 2, id1, 2);
    ASSERT_OK(CheckPersistentMemory(NULL));
    memset(expected, 0, limit);
    ASSERT_EQUAL_INT(0, memcmp(expected, flashMemory, limit));
}


TestFunction testFunctions[] =
{
    { "testInitPersistentDataManager", &testInitPersistentDataManager },
    { "testRegisterType", &testRegisterType },
    { "testFindUnavailableId", &testFindUnavailableId },
    { "testAllocate", &testAllocate },
    { "testFindRemove", &testFindRemove },
    { "testStoreGet", &testStoreGet },
    { "testStoreGetByte", &testStoreGetByte },
    { "testStoreGetBytes", &testStoreGetBytes },
    { "testCheckPersistentMemoryEmpty", &testCheckPersistentMemoryEmpty },
    { "testCheckPersistentMemoryTwoTypes", &testCheckPersistentMemoryTwoTypes },
    { "testCheckPersistentMemoryOutOfBounds", &testCheckPersistentMemoryOutOfBounds }
};


int main(int argc, char** argv) 
{
    expected = malloc(PersistentMemoryLimit());
    setBeforeTest(&ClearFlashMemory);
    startSuite("Persistent Memory Manager test");
    testAll(testFunctions, (sizeof(testFunctions) / sizeof(TestFunction)));
    finishSuite();
    free(expected);
    return EXIT_SUCCESS;
}
