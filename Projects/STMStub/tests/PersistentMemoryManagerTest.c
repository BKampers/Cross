/*
** Copyright © Bart Kampers
*/

#include <stdlib.h>
#include <string.h>

#include "Simple.h"
#include "stm32f10x_flash.h"

#include "PersistentElementManager.h"


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
    byte* expected = malloc(limit);
    memset(expected, 0, limit);
    InitPersistentDataManager();
    ASSERT_OK(CheckPersistentMemory(NULL));
    ASSERT_EQUAL_INT(0, memcmp(expected, flashMemory, limit));
    memset(flashMemory, 0xFF, limit);
    ASSERT_OK(CheckPersistentMemory(NULL));
    ASSERT_EQUAL_INT(0, memcmp(expected, flashMemory, limit));
    free(expected);
}


void testCheckPersistentMemoryTwoTypes()
{
    TypeId id1, id2;
    ElementSize size = 10;
    Reference limit = PersistentMemoryLimit();
    byte* expected = malloc(limit);
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
    free(expected);
}


void testCheckPersistentMemoryOutOfBounds()
{
    TypeId id1;
    ElementSize size = 10;
    Reference limit = PersistentMemoryLimit();
    byte* expected = malloc(limit);
    callbackCount = 0;
    InitPersistentDataManager();
    RegisterType(&id1);
    void* base = flashMemory + limit - size - 5;
    memcpy(base, &id1, sizeof(TypeId));
    memcpy(base + sizeof(TypeId), &size, sizeof(ElementSize));
    memset(base + sizeof(TypeId) + sizeof(ElementSize), 1, 100);
    ASSERT_OK(CheckPersistentMemory(NULL));
    free(expected);
}


TestFunction testFunctions[] =
{
    { "testInitPersistentDataManager", &testInitPersistentDataManager },
    { "testRegisterType", &testRegisterType },
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
    setBeforeTest(&ClearFlashMemory);
    startSuite("FlashDriverTest");
    testAll(testFunctions, (sizeof(testFunctions) / sizeof(TestFunction)));
    finishSuite();
    return EXIT_SUCCESS;
}
