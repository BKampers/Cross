/*
** Implementtation of PersistentMemoryDriver saving memory tpo disk
*/


#include "PersistentMemoryDriver.h"

#include <stdio.h>
#include <string.h>
#include <sys/stat.h>


#define MEMORY_SIZE 0x1000

char* RUNTIME_DIRECTORY = "runtime_data";
char* FILE_PATH = "runtime_data/persistent_memory";

byte MEMORY[MEMORY_SIZE];
Status creationStatus = UNINITIALIZED;
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
            fread(MEMORY, sizeof(byte), sizeof(MEMORY), file);
            int closed = fclose(file);
            status = (closed == 0) ? OK : "LOAD_CLOSE_ERROR";
        }
        else
        {
            memset(MEMORY, 0, sizeof(MEMORY));
        }
        loaded = TRUE;
    }
    return status;
}


Status ensureRuntimeDirectory()
{
    if (creationStatus == UNINITIALIZED)
    {
        struct stat sb;
        if (stat(RUNTIME_DIRECTORY, &sb) == 0)
        {
            creationStatus = ((sb.st_mode & S_IFDIR) != 0) ? OK : "INVALID_RUNTIME_DIRECTORY";
        }
        else
        {
            int state = mkdir(RUNTIME_DIRECTORY, S_IRWXU | S_IRWXG | S_IRWXO);
            creationStatus = (state == 0) ? OK : "CANNOT_CREATE_RUNTIME_DIRECTORY";
        }
    }
    return creationStatus;
}


Status save()
{
    Status status = ensureRuntimeDirectory();
    if (status == OK)
    {
        FILE* file = fopen(FILE_PATH, "w+b");
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
