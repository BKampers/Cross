/*
** Implementation of Table Manager
** Copyright 2015, Bart Kampers
*/

#include "Table.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* INVALID_TABLE_VALUE = "InvalidTableValue";
char* INVALID_TABLE_INDEX = "InvalidTableIndex";
char* OUT_OF_BOUNDS = "TableOutOfBounds";


/*
** private
*/

TypeId tableTypeId;
TypeId tableNameTypeId;


Status ValidateTableValue(int value)
{
    return ((-32768 <= value) && (value <= 32767)) ? OK : INVALID_TABLE_VALUE;
}


Status FindTableReference(const char* name, Reference* reference)
{
    bool found = FALSE;
    Status status = FindFirst(tableTypeId, reference);
    while ((status == OK) && ! found)
    {
        Table table;
        status = GetElement(*reference, sizeof(Table), &table);
        if (status == OK)
        {
            ElementSize length;
            status = GetSize(table.nameReference, &length);
            if (status == OK)
            {
                char* tableName = malloc(length + 1);
                if (tableName != NULL)
                {
                    tableName[length] = '\0';
                    status = GetElement(table.nameReference, length, tableName);
                    if (status == OK)
                    {
                        if (strcmp(name, tableName) == 0)
                        {
                            found = TRUE;
                        }
                    }
                    free(tableName);
                }
            }
        }
        if (! found)
        {
            status = FindNext(tableTypeId, reference);
        }
    }
    return status;
}


Status LoadTable(const char* name, Reference* reference, Table* table)
{
    Status status = FindTableReference(name, reference);
    if (status == OK)
    {
        status = GetElement(*reference, sizeof(Table), table);
    }
    return status;
}


Status GetTableFieldByReference(Reference reference, int column, int row, TableField* value)
{
    Table table;
    Status status = GetElement(reference, sizeof(Table), &table);
    if (status == OK)
    {
        if ((column < table.columns) && (row < table.rows))
        {
            int offset = sizeof(Table) + (row * table.columns + column) * sizeof(TableField);
            status = GetElementBytes(reference, (ElementSize) offset, sizeof(TableField), value);
        }
        else
        {
            status = OUT_OF_BOUNDS;
        }
    }
    return status;
}


/*
** interface
*/

void RegisterTableTypes()
{
    RegisterType(&tableTypeId);
    RegisterType(&tableNameTypeId);
}


Status FindTable(const char* name, Table* table)
{
    Reference reference;
    Status status = FindTableReference(name, &reference);
    if (status == OK)
    {
        status = GetElement(reference, sizeof(Table), table);
    }
    return status;
}


Status CreateTable(const char* name, byte columns, byte rows, Table* table)
{
    Status status = OK;
    table->columns = columns;
    table->rows = rows;
    table->flags = 0x00;
    if (name != NULL)
    {
        ElementSize length = (ElementSize) strlen(name);
        status = AllocateElement(tableNameTypeId, length, &(table->nameReference));
        if (status == OK)
        {
            status = StoreElement((void*) name, table->nameReference, length);
        }
    }
    if (status == OK)
    {
        Reference reference;
        status = AllocateElement(tableTypeId, sizeof(Table) + columns * rows * sizeof(TableField), &reference);
        if (status == OK)
        {
            status = StoreElement(table, reference, sizeof(Table));
        }
    }
    return status;
}


Status RemoveTable(const char* name)
{
    Reference reference;
    Status status = FindTableReference(name, &reference);
    if (status == OK)
    {
        Table table;
        status = GetElement(reference, sizeof(Table), &table);
        if (status == OK)
        {
            status = RemoveElement(table.nameReference);
            if (status == OK)
            {
                status = RemoveElement(reference);
            }
        }
    }
    return status;
}


Status SetTableField(const char* name, int column, int row, int value)
{
    Status status = ValidateTableValue(value);
    if (status == OK)
    {
        Reference reference;
        status = FindTableReference(name, &reference);
        if (status == OK)
        {
            Table table;
            status = GetElement(reference, sizeof(Table), &table);
            if (status == OK)
            {
                if ((column < table.columns) && (row < table.rows))
                {
                    TableField field = (TableField) value;
                    int offset = sizeof(Table) + (row * table.columns + column) * sizeof(TableField);
                    status = StoreElementBytes(reference, (ElementSize) offset, sizeof(TableField), &field);
                }
                else
                {
                    status = OUT_OF_BOUNDS;
                }
            }
        }
    }
    return status;
}


Status GetTableField(const char* name, int column, int row, TableField* value)
{
    Reference reference;
    Status status = FindTableReference(name, &reference);
    if (status == OK)
    {
        status = GetTableFieldByReference(reference, column, row, value);
    }
    return status;
}


Status SetTableFlags(const char* name, byte mask)
{
    Reference reference;
    Table table;
    Status status = LoadTable(name, &reference, &table);
    if (status == OK)
    {
        table.flags |= mask;
        status = StoreElement(&table, reference, sizeof(Table));
    }
    return status;
}


Status ClearTableFlags(const char* name, byte mask)
{
    Reference reference;
    Table table;
    Status status = LoadTable(name, &reference, &table);
    if (status == OK)
    {
        table.flags &= (mask ^ 0xFF);
        status = StoreElement(&table, reference, sizeof(Table));
    }
    return status;
}


Status GetTableFlags(const char* name, byte mask, bool* set)
{
    Reference reference;
    Table table;
    Status status = LoadTable(name, &reference, &table);
    if (status == OK)
    {
        *set = (table.flags & mask) != 0;
    }
    return status;
}
