/*
** Implementtation of PersistentMemoryDriver saving memory tpo disk
*/


#include "PersistentMemoryDriver.h"

#include <stdio.h>
#include <string.h>


#define MEMORY_SIZE 0x800

char* FILE_PATH = "runtime_data/persistent_memory";

byte MEMORY[MEMORY_SIZE];
bool loaded = FALSE;


/*
** Private
*/

Status load()
{
    Status status = OK;
    if (! loaded)
    {
        FILE* file = fopen(FILE_PATH, "rb");
        if (file != NULL)
        {
            fread(MEMORY, sizeof(byte), MEMORY_SIZE * sizeof(byte), file);
            int closed = fclose(file);
            status = (closed == 0) ? OK : "LOAD_CLOSE_ERROR";
        }
        else
        {
            status = "LOAD_OPEN_ERROR";
        }
        loaded = (status == OK);
    }
    return status;
}


Status save()
{
    Status status = OK;
    FILE* file = fopen(FILE_PATH, "wb");
    if (file != NULL)
    {
        fwrite(MEMORY, sizeof(byte), MEMORY_SIZE * sizeof(byte), file);
        int closed = fclose(file);
        status = (closed == 0) ? OK : "SAVE_CLOSE_ERROR";
    }
    else
    {
        status = "SAVE_OPEN_ERROR";
    }
    return status;
}


/*
** Interface
*/

char* PersistentMemoryType()
{
    return "FiledMemory";
}


Reference PersistentMemoryLimit()
{
    return MEMORY_SIZE;
}


Status ReadPersistentMemory(Reference reference, int length, void* buffer)
{
    if ((length > 0) && (reference + length <= MEMORY_SIZE))
    {
        Status status = load();
        memcpy(buffer, &(MEMORY[reference]), length);
        return status;
    }
    else 
    {
        return INVALID_PARAMETER;
    }
}


Status WritePersistentMemory(Reference reference, int length, void* buffer)
{
    Status status = ((length > 0) && (reference + length <= MEMORY_SIZE)) ? OK : INVALID_PARAMETER;
    if (status == OK)
    {
        status = load();
        if (status == OK)
        {
            memcpy(&(MEMORY[reference]), buffer, length);
            status = save();
        }
   }
   return status;
}


Status WritePersistentMemoryByte(Reference reference, byte data)
{
    return WritePersistentMemory(reference, 1, &data);
}


Status FillPersistentMemory(Reference reference, int count, byte data)
{
    Status status = ((reference + count <= MEMORY_SIZE) && (count >= 0)) ? OK : INVALID_PARAMETER;
    if (status == OK)
    {
        status = load();
        if (status == OK)
        {
            memset(&(MEMORY[reference]), data, count);
            status = save();
        }
    }
    return status;
}
