/*
** Copyright © Bart Kampers
*/


#include <stdio.h>
#include <stdlib.h>

#include "Simple.h"

#include "stm32f10x_flash.h"
#include "PersistentMemoryDriver.h"


void* initializeMemory()
{
    initFlashMemory();
//    Reference limit = PersistentMemoryLimit();
    byte* memory = malloc(PersistentMemoryLimit());
//    for (Reference r = 0;r < limit; ++r) {
//        memory[r] = (byte) (r & 0xFF);
//    }
    return memory;
}


void testPersistentMemoryType() {
    ASSERT_NOT_NULL(PersistentMemoryType());
}


void testPersistentMemoryLimit() {
    Reference limit = PersistentMemoryLimit();
    ASSERT_TRUE(limit > 0);
}


void testReadPersistentMemoryBytes() {
    Reference limit = PersistentMemoryLimit();
    Reference reference;
    for (reference = 0; reference <= limit - 1; ++reference)
    {
        byte b;
        Status status = ReadPersistentMemory(reference, 1, &b);
        ASSERT_OK(status);
    }
}


void testReadPersistentMemoryWords() {
    Reference limit = PersistentMemoryLimit();
    Reference reference;
    for (reference = 0; reference <= limit - 2; reference += 2)
    {
        uint16_t w;
        Status status = ReadPersistentMemory(reference, 2, &w);
        ASSERT_OK(status);
    }
}


void testReadPersistentMemoryInts() {
    Reference limit = PersistentMemoryLimit();
    Reference reference;
    for (reference = 0; reference <= limit - 4; reference += 4)
    {
        uint32_t i;
        Status status = ReadPersistentMemory(reference, 4, &i);
        ASSERT_OK(status);
    }
}


int main(int argc, char** argv) {
    initFlashMemory();

    startSuite("FlashDriverTest");

    start("testPersistentMemoryType");
    testPersistentMemoryType();
    finish();
    
    start("testPersistentMemoryLimit");
    testPersistentMemoryLimit();
    finish();

    start("testReadPersistentMemoryBytes");
    testReadPersistentMemoryBytes();
    finish();

    start("testReadPersistentMemoryWords");
    testReadPersistentMemoryWords();
    finish();

    start("testReadPersistentMemoryInts");
    testReadPersistentMemoryInts();
    finish();

    finishSuite();
    return EXIT_SUCCESS;
}
