/*
** Implementation of the Persistent Data Manager
*/

#include "PersistentElementManager.h"

#include <stdlib.h>


/*
** Private
*/

#define EMPTY   0x00
#define UNKNOWN 0xFF

char ELEMENT_ID_NOT_FOUND[] = "ElementIdNotFound";
char INVALID_REFERENCE[] = "InvalidReference";

TypeId lastId;


Status ReadType(Reference* reference, TypeId* id, ElementSize* size)
{
    Status status = ReadPersistentMemory(*reference, sizeof(TypeId), id);
    *reference += sizeof(TypeId);
    *size = 0;
    if (status == OK)
    {
        if (*id > lastId)
        {
            *id = UNKNOWN;
        }
        else if (*id != EMPTY)
        {
            status = ReadPersistentMemory(*reference, sizeof(ElementSize), (void*) size);
            *reference += sizeof(ElementSize);
        }
    }
    return status;
}


Status FindId(TypeId id, Reference* reference)
{
    Reference limit = PersistentMemoryLimit();
    Status status = ((id != EMPTY) && (id <= lastId)) ? ELEMENT_ID_NOT_FOUND : INVALID_ID;    
    while ((status == ELEMENT_ID_NOT_FOUND) && (*reference < limit))
    {
        Reference search = *reference;
        TypeId idRead;
        ElementSize sizeRead;
        Status readStatus = ReadType(&search, &idRead, &sizeRead);
        if (readStatus == OK)
        {
            if (id == idRead)
            {
                status = OK;
            }
            else
            {
                *reference = search + sizeRead;
            }
        }
        else
        {
            status = readStatus;
        }
    }
    if (status == ELEMENT_ID_NOT_FOUND)
    {
        *reference = NULL_REFERENCE;
    }
    return status;
}


Reference FindFreeSpace(ElementSize size)
{ 
    Reference found = NULL_REFERENCE;
    Reference emptySpace = 0x0000;
    Reference search = 0x0000;
    ElementSize spaceFound = 0;
    Reference limit = PersistentMemoryLimit();
    Status status = OK;
    while ((found == NULL_REFERENCE) && (search < limit) && (status == OK))
    {
        TypeId idRead;
        ElementSize sizeRead;
        status = ReadType(&search, &idRead, &sizeRead);
        switch (idRead)
        {
            case EMPTY:
                spaceFound++;
                if (spaceFound == size)
                {
                    found = emptySpace;
                }
                break;
            case UNKNOWN:
                emptySpace = search;
                spaceFound = 0;
                break;
            default:
                search += sizeRead;
                emptySpace = search;
                spaceFound = 0;
                break;
        }
    }
    return found;
}


/*
** Prepare an area to be cleared by EraseClearArea.
** Initialize if uninitialized, expand otherwise
*/
void PrepareClearArea(Reference* clearStart, Reference* clearEnd, Reference reference)
{
    if (*clearStart == NULL_REFERENCE)
    {
        *clearStart = reference;
    }
    *clearEnd = reference;
}


/*
** Erase bytes in the area if prepared by ExpandClearArea.
** Do nothing if no area was prepared.
*/
Status EraseClearArea(Reference* clearStart, Reference* clearEnd)
{
    Status status = OK;
    if (*clearStart != NULL_REFERENCE)
    {
        status = FillPersistentMemory(*clearStart, *clearEnd - *clearStart + 1, EMPTY);
        *clearStart = NULL_REFERENCE;
        *clearEnd = NULL_REFERENCE;
    }
    return status;
}


/*
** Interface
*/

void InitPersistentDataManager()
{
    lastId = 0;
}


void RegisterType(TypeId* newId)
{
    lastId++;
    *newId = lastId;
}


int GetTypeCount()
{
    return lastId;
}


Status AllocateElement(TypeId id, ElementSize size, Reference* reference)
{
    Status status = INVALID_ID;
    *reference = NULL_REFERENCE;
    if (id <= lastId)
    {
        *reference = FindFreeSpace(sizeof(id) + sizeof(size) + size);
        if (*reference != NULL_REFERENCE)
        {
            status = WritePersistentMemory(*reference, sizeof(id), &id);
            if (status == OK)
            {
                status = WritePersistentMemory(*reference + sizeof(id), sizeof(size), (byte*) &size);
            }
        }
        else
        {
            status = OUT_OF_MEMORY;
        }
    }
    return status;
}


Status StoreElement(void* buffer, Reference reference, ElementSize count)
{
    TypeId id;
    ElementSize size;
    Status status = ReadType(&reference, &id, &size);
    if (status == OK)
    {
        if ((id != EMPTY) && (id <= lastId))
        {
            if (size >= count)
            {
                status = WritePersistentMemory(reference, count, buffer);
            }
            else
            {
                status = BUFFER_OVERFLOW;
            }
        }
        else
        {
            status = INVALID_ID;
        }
    }
    return status;
}


Status StoreElementByte(Reference reference, ElementSize offset, byte buffer)
{
    TypeId id;
    ElementSize size;
    Status status = ReadType(&reference, &id, &size);
    if ((status == OK) && (offset < size))
    {
        status = WritePersistentMemory(reference + offset, 1, &buffer);
    }
    return status;
}


Status StoreElementBytes(Reference reference, ElementSize offset, ElementSize count, void* buffer)
{
    TypeId id;
    ElementSize size;
    Status status = ReadType(&reference, &id, &size);
    if (status == OK)
    {
        if (offset < size)
        {
            status = WritePersistentMemory(reference + offset, count, buffer);
        }
        else
        {
            status = BUFFER_OVERFLOW;
        }
    }
    return status;
}


Status GetElement(Reference reference, ElementSize count, void* buffer)
{
    TypeId id;
    ElementSize size;
    Status status = ReadType(&reference, &id, &size);
    if (status == OK)
    {
        if (size >= count)
        {
            status = ReadPersistentMemory(reference, count, buffer);
        }
        else
        {
            status = BUFFER_OVERFLOW;
        }
    }
    return status;
}


Status GetElementBytes(Reference reference, ElementSize offset, ElementSize count, void* buffer)
{
    TypeId id;
    ElementSize size;
    Status status = ReadType(&reference, &id, &size);
    if (status == OK)
    {
        if (offset + count <= size)
        {
            status = ReadPersistentMemory(reference + offset, count, buffer);
        }
        else
        {
            status = BUFFER_OVERFLOW;
        }
    }
    return status;
}


Status GetElementByte(Reference reference, ElementSize offset, byte* buffer)
{
    TypeId id;
    ElementSize size;
    Status status = ReadType(&reference, &id, &size);
    if (status == OK)
    {
        if (offset < size)
        {
            status = ReadPersistentMemory(reference + offset, 1, buffer);
        }
        else
        {
            status = BUFFER_OVERFLOW;
        }
    }
    return status;
}


Status RemoveElement(Reference reference)
{
    Reference dataReference = reference;
    TypeId id;
    ElementSize size;
    Status status = ReadType(&dataReference, &id, &size);
    if (status == OK)
    {
        status = FillPersistentMemory(reference, dataReference - reference + size, EMPTY);
    }
    return status;
}


Status FindFirst(TypeId id, Reference* reference)
{
    *reference = 0;
    return FindId(id, reference);
}


Status FindNext(TypeId id, Reference* reference)
{
    Status status = OK;
    if (*reference < PersistentMemoryLimit())
    {
        TypeId idRead;
        ElementSize sizeRead;
        status = ReadType(reference, &idRead, &sizeRead);
        if (status == OK)
        {
            *reference += sizeRead;
            status = FindId(id, reference);
        }
    }
    else
    {
        status = INVALID_REFERENCE;
    }
    return status;
}


Status GetType(Reference reference, TypeId* id)
{
    return ReadPersistentMemory(reference, sizeof(TypeId), id);
}


Status GetSize(Reference reference, ElementSize* size)
{
    TypeId id;
    return ReadType(&reference, &id, size);
}


Status CheckPersistentMemory(void (*notify)(Reference))
{
    Reference clearStart = NULL_REFERENCE;
    Reference clearEnd = NULL_REFERENCE;
    Status status = OK;
    Reference limit = PersistentMemoryLimit();
    Reference reference = 0;
    while ((reference < limit) && (status == OK))
    {
        TypeId id;
        ElementSize size;
        Reference check = reference;
        if (notify != NULL)
        {
            notify(check);
        }
        status = ReadType(&check, &id, &size);
        if (status == OK) 
        {
            if (size > 0)
            {
                check += size;
                if (check <= limit)
                {
                    /* Element with valid size. */
                    reference = check;
                    status = EraseClearArea(&clearStart, &clearEnd);
                }
                else 
                {
                    /* Size out of boundaries. Clear id to remove element. */
                    PrepareClearArea(&clearStart, &clearEnd, reference);
                    reference += sizeof(TypeId);
                }
            }
            else if (id != EMPTY)
            {
                /* Element with invalid size or unknown id. Clear id to remove element. */
                PrepareClearArea(&clearStart, &clearEnd, reference);
                reference = check;
                status = OK;
            }
            else
            {
                /* Available memory. */
                if (clearStart != NULL_REFERENCE)
                {
                    PrepareClearArea(&clearStart, &clearEnd, reference);
                }
                reference = check;
            }
        }
        else
        {
            PrepareClearArea(&clearStart, &clearEnd, reference);
            reference++;
            status = OK;
        }
    }
    if (status == OK)
    {
        status = EraseClearArea(&clearStart, &clearEnd);
    }
    return status;
}
