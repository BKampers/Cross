/*
** Copyright © Bart Kampers
*/

#include <stdlib.h>

#include "Simple.h"
#include "stm32f10x_flash.h"

#include "PersistentElementManager.h"


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


TestFunction testFunctions[] =
{
    { "testInitPersistentDataManager", &testInitPersistentDataManager },
    { "testRegisterType", &testRegisterType },
    { "testAllocate", &testAllocate },
    { "testFindRemove", &testFindRemove },
    { "testStoreGet", &testStoreGet },
    { "testStoreGetByte", &testStoreGetByte },
    { "testStoreGetBytes", &testStoreGetBytes }
};

#define TEST_COUNT (sizeof(testFunctions) / sizeof(TestFunction))


int main(int argc, char** argv)
{
    int testId;
    startSuite("FlashDriverTest");
    for (testId = 0; testId < TEST_COUNT; ++testId)
    {
        ClearFlashMemory();
        start(testFunctions[testId].name);
        testFunctions[testId].Call();
        finish();
    }
    finishSuite();
    return EXIT_SUCCESS;
}
