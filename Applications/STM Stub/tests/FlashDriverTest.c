/*
** Copyright © Bart Kampers
*/


#include <stdio.h>
#include <stdlib.h>

#include "Simple.h"

#include "stm32f10x_flash.h"
#include "PersistentMemoryDriver.h"


typedef struct
{
    uint64_t a;
    uint64_t b;
} S128;


void testPersistentMemoryType() {
    ASSERT_NOT_NULL(PersistentMemoryType());
}


void testPersistentMemoryLimit() {
    Reference limit = PersistentMemoryLimit();
    ASSERT_TRUE(limit > 0);
}


void testReadPersistentMemoryByte()
{
    Reference limit = PersistentMemoryLimit();
    Reference reference;
    for (reference = 0; reference < limit; ++reference)
    {
        byte b = (byte) (reference & 0xFF);
        flashMemory[reference] = b;
        ASSERT_OK(ReadPersistentMemory(reference, sizeof(byte), &b));
        ASSERT_EQUAL_INT(b, flashMemory[reference]);
    }
}


void testWritePersistentMemoryByte()
{
    Reference limit = PersistentMemoryLimit();
    Reference reference;
    for (reference = 0; reference < limit; ++reference)
    {
        byte b = (byte) (reference & 0xFF);
        ASSERT_OK(WritePersistentMemoryByte(reference, b));
        ASSERT_EQUAL_INT(b, flashMemory[reference]);
    }
}


void testReadPersistentMemoryInt()
{
    int initial = 0x41424344;
    Reference reference;
    for (reference = 0; reference < sizeof(int); ++reference)
    {
        int i;
        memcpy(flashMemory + reference, &initial, sizeof(int));
        ASSERT_OK(ReadPersistentMemory(reference, sizeof(int), &i));
        ASSERT_EQUAL_INT(initial, i);
    }    
}


void testWritePersistentMemoryInt()
{
    int initial = 0x41424344;
    Reference reference;
    for (reference = 0; reference < sizeof(int); ++reference)
    {
        ASSERT_OK(WritePersistentMemory(reference, sizeof(int), &initial));
        ASSERT_EQUAL_INT(0, memcmp(&initial, flashMemory + reference, sizeof(int)));
    }    
}


void testReadPersistentMemoryStruct()
{
    size_t size = sizeof(S128);
    S128 initial = {0xCCCC3333, 0x7777EEEE};
    Reference reference;
    for (reference = 0; reference < size; ++reference)
    {
        S128 s128;
        memcpy(flashMemory + reference, &initial, size);
        ASSERT_OK(ReadPersistentMemory(reference, size, &s128));
        ASSERT_EQUAL_INT(0, memcmp(&initial, &s128, size));
    }    
}


void testWritePersistentMemoryStruct()
{
    S128 initial = {0x0F0F0F0F, 0xF0F0F0F0};
    Reference reference;
    for (reference = 0; reference < sizeof(S128); ++reference)
    {
        ASSERT_OK(WritePersistentMemory(reference, sizeof(S128), &initial));
        ASSERT_EQUAL_INT(0, memcmp(&initial, flashMemory + reference, sizeof(S128)));
    }
}


void testFillPersistentMemory()
{
    Reference limit = PersistentMemoryLimit();
    byte value = 0x6A;
    int i;
    ASSERT_OK(FillPersistentMemory(0, limit, value));
    for (i = 0; i < limit; ++i)
    {
        ASSERT_EQUAL_INT(value, flashMemory[i]);
    }
}


void testInvalidReferences()
{
    S128 s128;
    Reference limit = PersistentMemoryLimit();
    Reference reference = limit - sizeof(S128) + 1;
    while (reference <= limit)
    {
        Status status = ReadPersistentMemory(reference, sizeof(S128), &s128);
        ASSERT_FALSE(status == OK);
        status = WritePersistentMemory(reference, sizeof(S128), &s128);
        ASSERT_FALSE(status == OK);
        reference++;
    }
}


int main(int argc, char** argv) {
    startSuite("FlashDriverTest");

    start("testPersistentMemoryType");
    testPersistentMemoryType();
    finish();
    
    start("testPersistentMemoryLimit");
    testPersistentMemoryLimit();
    finish();

    start("testReadPersistentMemoryByte");
    testReadPersistentMemoryByte();
    finish();

    start("testWritePersistentMemoryByte");
    testWritePersistentMemoryByte();
    finish();

    start("testReadPersistentMemoryInt");
    testReadPersistentMemoryInt();
    finish();

    start("testWritePersistentMemoryInt");
    testWritePersistentMemoryInt();
    finish();

    start("testReadPersistentMemoryStruct");
    testReadPersistentMemoryStruct();
    finish();
    
    start("testWritePersistentMemoryStruct");
    testWritePersistentMemoryStruct();
    finish();
    
    start("testFillPersistentMemory");
    testFillPersistentMemory();
    finish();
    
    start("testInvalidReferences");
    testInvalidReferences();
    finish();

    finishSuite();
    return EXIT_SUCCESS;
}
