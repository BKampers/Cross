/*
 ** Interface for the Persistent Data Manager
 ** Copyright 2014, Bart Kampers
 */

#ifndef _PERSISTENTELEMENTMANAGER_H_
#define _PERSISTENTELEMENTMANAGER_H_

#include "Types.h"
#include "PersistentMemoryDriver.h"


#define INVALID_TYPE_ID 0xFF

typedef byte TypeId;
typedef uint16_t ElementSize;

void InitPersistentDataManager();

void RegisterType(TypeId* newId);
int GetTypeCount();

Status AllocateElement(TypeId id, ElementSize size, Reference* reference);

Status StoreElement(void* buffer, Reference reference, ElementSize count);
Status StoreElementByte(Reference reference, ElementSize offset, byte buffer);
Status StoreElementBytes(Reference reference, ElementSize offset, ElementSize count, void* buffer);

Status GetElement(Reference reference, ElementSize count, void* buffer);
Status GetElementByte(Reference, ElementSize offset, byte* buffer);
Status GetElementBytes(Reference reference, ElementSize offset, ElementSize count, void* buffer);

Status RemoveElement(Reference reference);

Status FindFirst(TypeId id, Reference* reference);
Status FindNext(TypeId id, Reference* reference);

Status GetType(Reference reference, TypeId* id);
Status GetSize(Reference reference, ElementSize* size);

Status CheckPersistentMemory(void (*notify)(Reference));

#endif /* __PERSISTENTELEMENTMANAGER_H__ */
