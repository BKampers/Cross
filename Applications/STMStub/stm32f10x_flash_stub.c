/*
 * Copyright © Bart Kampers
 */

#include "stm32f10x_flash.h"
#include "Types.h"
#include <stdlib.h>

#define MEMORY_SIZE 0x8000

void* flashMemory;


void initFlashMemory()
{
    int i;
    flashMemory = malloc(MEMORY_SIZE);
    for (i = 0; i < MEMORY_SIZE; ++i)
    {
        ((byte*) flashMemory)[i] = (byte) (i & 0xFF); 
    }
}


void FLASH_LockBank1()
{
}


void FLASH_UnlockBank1()
{
}


void FLASH_ClearFlag(int mask)
{
}


FLASH_Status FLASH_ErasePage(uint32_t page)
{
    return FLASH_COMPLETE;
}


FLASH_Status FLASH_ProgramWord(uint32_t flashAddress, uint32_t workAddress)
{
    return FLASH_COMPLETE;    
}
