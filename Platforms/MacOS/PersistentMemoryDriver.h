/*
** Interface for Persistent Memory Driver
** Copyright 2014, Bart Kampers
*/
#ifndef __PERSISTENMEMORYDRIVER_H__
#define __PERSISTENMEMORYDRIVER_H__


#include "Types.h"
#include "ApiStatus.h"


#define NULL_REFERENCE ((Reference) 0xFFFF)


typedef uint16_t Reference;

char* PersistentMemoryType();

Reference PersistentMemoryLimit();

Status ReadPersistentMemory(Reference refenece, int length, void* buffer);

Status WritePersistentMemory(Reference reference, int length, void* buffer);
Status WritePersistentMemoryByte(Reference reference, byte data);

Status FillPersistentMemory(Reference reference, int count, byte data);


#endif /* __PERSISTENMEMORYDRIVER_H__ */
