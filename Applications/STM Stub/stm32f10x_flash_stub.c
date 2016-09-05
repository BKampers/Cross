#include "stm32f10x_flash.h"
#include "Types.h"
#include <stdlib.h>
#include <string.h>


#define MEMORY_SIZE 0x1000

byte flashMemory[MEMORY_SIZE];


void ClearFlashMemory()
{
    memset(flashMemory, 0, MEMORY_SIZE);
}


uint32_t ReadFlashWord(uint32_t reference)
{
    return *((uint32_t*) (flashMemory + reference));
}


FLASH_Status WriteFlashWord(uint32_t reference, uint32_t data)
{
    if (reference < MEMORY_SIZE)
    {
        uint32_t* address = (uint32_t*) (flashMemory + reference);
        *address = data;
        return FLASH_COMPLETE;
    }
    else
    {
        return FLASH_ERROR_PG;
    }
}
