/*
** Implementation of Persistent Memory Driver for STM32F10x internal flash
*/

#include "PersistentMemoryDriver.h"

#include "FlashDefinitions.h"

#include <stdio.h>


/*
** private
*/

typedef uint32_t FlashWord;
typedef uint32_t Address;

#define FLASH_LIMIT 0x1000

//#define MEMORY_BASE ((FlashWord*) flashMemory)

//#define FLASH_BASE_ADDRESS ((Address) 0x0801F000)
//#define FLASH_END_ADDRESS  ((Address) FLASH_BASE_ADDRESS + FLASH_LIMIT)

#ifndef FLASH_PAGE_SIZE
#define FLASH_PAGE_SIZE 0x400
#endif

char statusText[32];
char workMemory[FLASH_PAGE_SIZE];
Reference loadedPage = NULL_REFERENCE;


void GenerateFlashStatusText(const char* message, FLASH_Status flashStatus, Address address)
{
    sprintf(statusText, "%-16s %6d %08X", message, flashStatus, (int) address);
}


void GenerateAddressStatusText(const char* message, Address address)
{
    sprintf(statusText, "%-8s %08X", message, (int) address);
}

void LoadPage(Reference pageBase)
{
    if (loadedPage != pageBase)
    {
        FlashWord* workAddress = (FlashWord*) workMemory;
        Reference reference = pageBase;
        LOCK_FLASH_BANK();
        while (reference < pageBase + FLASH_PAGE_SIZE)
        {
            *workAddress = READ_FLASH_WORD(reference);//(*(__IO uint32_t*) flashAddress);
            workAddress++;
            reference += sizeof(FlashWord);
        }
        loadedPage = pageBase;
     }
}


void ModifyPageData(int index, int length, void* buffer)
{
    int i;
    for (i = 0; i < length; ++i)
    {
        workMemory[index + i] = ((char*) buffer)[i];
    }
}


Status StorePage(Reference pageBase)
{
    UNLOCK_FLASH_BANK();
    /* Clear All pending flags */
    CLEAR_FLASH_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);

    FLASH_Status flashStatus = ERASE_FLASH_PAGE(pageBase);
    if (flashStatus == FLASH_COMPLETE)
    {
        /* status = FLASH_WaitForLastOperation(EraseTimeout); // Can cause eternal wait */
        FlashWord* workAddress = (FlashWord*) workMemory;
        Reference reference = pageBase;
        while ((flashStatus == FLASH_COMPLETE) && (reference < pageBase + FLASH_PAGE_SIZE))
        {
            flashStatus = WRITE_FLASH_WORD(reference, *workAddress);//FLASH_ProgramWord(flashAddress, *workAddress);
            workAddress++;
            reference += sizeof(FlashWord);
        }
        if (flashStatus != FLASH_COMPLETE)
        {
            GenerateFlashStatusText("FLASH_WRITE_ERR", flashStatus, reference);
        }
    }
    else
    {
        GenerateFlashStatusText("FLASH_ERASE_ERR", flashStatus, pageBase);
    }
    LOCK_FLASH_BANK();

    return (flashStatus == FLASH_COMPLETE) ? OK : statusText;
}


Status WritePage(Reference pageBase, int index, int length, void* buffer, int* written)
{
    LoadPage(pageBase);
    *written = (index + length <= FLASH_PAGE_SIZE) ? length : FLASH_PAGE_SIZE - index;
    ModifyPageData(index, *written, buffer);
    return StorePage(pageBase);
}


/*
** Interface
*/

char* PersistentMemoryType()
{
    sprintf(statusText, "Flash (page size = %X)", FLASH_PAGE_SIZE);
    return statusText;
}


Reference PersistentMemoryLimit()
{
    return (Reference) FLASH_LIMIT;
}


Status ReadPersistentMemory(Reference reference, int length, void* buffer)
{
    Status result = OK;
    byte* bufferAddress = buffer;
    //Address address = reference + FLASH_BASE_ADDRESS;
    UNLOCK_FLASH_BANK();
    while ((length > 0) && (result == OK))
    {
        int boundDistance = FLASH_LIMIT - reference;//FLASH_END_ADDRESS - address;
        if (boundDistance > 0)
        {
            FlashWord data;
            int i = 0;
            if (boundDistance < sizeof(FlashWord))
            {
                int delta = sizeof(FlashWord) - boundDistance;
                reference -= delta;
                i += delta;
            }
            data = READ_FLASH_WORD(reference);//(*(__IO FlashWord*) address);
            while ((i < sizeof(FlashWord)) && (length > 0))
            {
                *bufferAddress = ((byte*) &data)[i];
                bufferAddress++;
                i++;
                length--;
            }
            //address = address + sizeof(FlashWord);
            reference += sizeof(FlashWord);
        }
        else
        {
            GenerateAddressStatusText("FlsRdErr", reference);
            result = statusText;
        }
    }
    UNLOCK_FLASH_BANK();
    return result;
}


Status WritePersistentMemory(Reference reference, int length, void* buffer)
{
    Status result = OK;
    int pageIndex = reference / FLASH_PAGE_SIZE;
    int pageOffset = pageIndex * FLASH_PAGE_SIZE;
    Reference pageBase = /*FLASH_BASE_ADDRESS +*/ pageOffset;
    int workIndex = reference - pageOffset;
    while ((length > 0) && (result == OK))
    {
        if (pageBase < FLASH_LIMIT)
        {
            int written;
            result = WritePage(pageBase, workIndex, length, buffer, &written);
            pageBase += FLASH_PAGE_SIZE;
            workIndex = 0;
            buffer += written;
            length -= written;
        }
        else
        {
            GenerateAddressStatusText("FlsWrErr", pageBase);
            result = statusText;
        }
    }
    return result;
}


Status WritePersistentMemoryByte(Reference reference, byte data)
{
    return WritePersistentMemory(reference, 1, (void*) &data);
}


Status FillPersistentMemory(Reference reference, int count, byte data)
{
    Status result = OK;
    int pageIndex = reference / FLASH_PAGE_SIZE;
    int pageOffset = pageIndex * FLASH_PAGE_SIZE;
    Reference pageBase = pageOffset;
    int workIndex = reference - pageOffset;

    while ((result == OK) && (count > 0))
    {
        int i;
        int end = workIndex + count;
        if (end > FLASH_PAGE_SIZE)
        {
            end = FLASH_PAGE_SIZE;
        }
        LoadPage(pageBase);
        for (i = workIndex; i < end; ++i)
        {
            workMemory[i] = data;
        }
        result = StorePage(pageBase);
        workIndex = 0;
        pageBase += FLASH_PAGE_SIZE;
        count -= end - workIndex;
    }
    return result;
}
