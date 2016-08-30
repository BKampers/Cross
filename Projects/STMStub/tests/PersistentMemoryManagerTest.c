/*
** Copyright © Bart Kampers
*/

#include <stdlib.h>

#include "Simple.h"

#include "PersistentElementManager.h"


typedef struct
{
    const char* name;
    void (*Call) ();
} TestFunction;


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


void testAllocateAndFind()
{
    TypeId id;
    Reference first, second, found;
    InitPersistentDataManager();
    RegisterType(&id);
    ASSERT_OK(AllocateElement(id, 1, &first));
    ASSERT_OK(AllocateElement(id, 1, &second));
    ASSERT_UNEQUAL_INT(NULL_REFERENCE, first);
    ASSERT_OK(FindFirst(id, &found));
    ASSERT_EQUAL_INT(first, found);
    ASSERT_OK(FindNext(id, &found));
    ASSERT_EQUAL_INT(second, found);
    ASSERT_STATUS(INVALID_ID, FindNext(id, &found));
    ASSERT_EQUAL_INT(NULL_REFERENCE, found);
}


TestFunction testFunctions[] =
{
    { "testInitPersistentDataManager", &testInitPersistentDataManager },
    { "testRegisterType", &testRegisterType },
    { "testAllocateAndFind", &testAllocateAndFind }
};

#define TEST_COUNT (sizeof(testFunctions) / sizeof(TestFunction))


int main(int argc, char** argv)
{
    int testId;
    startSuite("FlashDriverTest");
    for (testId = 0; testId < TEST_COUNT; ++testId)
    {
        start(testFunctions[testId].name);
        testFunctions[testId].Call();
        finish();
    }
    finishSuite();
    return EXIT_SUCCESS;
}
