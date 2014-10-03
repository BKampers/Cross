/*
** Implementation of Table Manager
** Copyright 2014, Bart Kampers
*/

#include "Table.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* INVALID_TABLE_INDEX = "Invalid table index";
char* OUT_OF_BOUNDS = "Table out of bounds";


/*
** private
*/

TypeId tableTypeId;
TypeId tableNameTypeId;


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


Status CreateTable(char* name, byte columns, byte rows)
{
    Status status = OK;
    Table table = { NULL_REFERENCE, columns, rows };
    if (name != NULL)
    {
        ElementSize length = (ElementSize) strlen(name);
        status = AllocateElement(tableNameTypeId, length, &table.nameReference);
        if (status == OK)
        {
            status = StoreElement(name, table.nameReference, length);
        }
    }
    if (status == OK)
    {
        Reference reference;
        status = AllocateElement(tableTypeId, sizeof(Table) + columns * rows * sizeof(TableField), &reference);
        if (status == OK)
        {
            status = StoreElement(&table, reference, sizeof(Table));
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


Status SetTableField(const char* name, int column, int row, TableField value)
{
    Reference reference;
    Status status = FindTableReference(name, &reference);
    if (status == OK)
    {
        Table table;
        status = GetElement(reference, sizeof(Table), &table);
        if (status == OK)
        {
            if ((column < table.columns) && (row < table.rows))
            {
                int offset = sizeof(Table) + (row * table.columns + column) * sizeof(TableField);
                status = StoreElementBytes(reference, (ElementSize) offset, sizeof(TableField), &value);
            }
            else
            {
                status = OUT_OF_BOUNDS;
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
        Table table;
        status = GetElement(reference, sizeof(Table), &table);
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
    }
return status;
}
