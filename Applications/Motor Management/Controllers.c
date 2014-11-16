/*
** Copyright 2014, Bart Kampers
*/

#include "Controllers.h"

#include <stdlib.h>
#include <string.h>


/*
** Private
*/

#define UNINITIALIZED_TABLE { NULL_REFERENCE, 0, 0 }


TableController createdControllers[MAX_TABLE_CONTROLLERS];
int tableControllerCount = 0;


Status InitTableController(TableController* tableController, const char* name, byte columns, byte rows)
{
    Status status = FindTable(name, &(tableController->table));
    if (status != OK)
    {
        status = CreateTable(name, columns, rows, &(tableController->table));
    }
    tableController->name = name;
    return status;
}


Status CalculateIndex(Measurement* measurement, byte bound, byte* index)
{
    if (bound > 1)
    {
        if (measurement != NULL)
        {
            float value;
            Status status = GetMeasurementValue(measurement, &value);
            if (status == OK)
            {
                float range = GetMeasurementRange(measurement);
                int valueIndex = value / (range / bound);
                *index = (byte) max(0, min(valueIndex, bound));
            }
            return status;
        }
        else
        {
            return "NoMeasurementConfigured";
        }
    }
    else if (bound == 1)
    {
        *index = 0;
        return OK;
    }
    else
    {
        return "InvalidBound";
    }
}


Status GetActualTableControllerField(TableController* tableController, TableField* field)
{
    Status status = CalculateIndex(tableController->columnMeasurement, tableController->table.columns, &(tableController->columnIndex));
    if (status == OK)
    {
        status = CalculateIndex(tableController->rowMeasurement, tableController->table.rows, &(tableController->rowIndex));
        if (status == OK)
        {
            status = GetTableField(tableController->name, tableController->columnIndex, tableController->rowIndex, field);
        }
    }
    return status;
}


/*
** Interface
*/


Status InitControllers()
{
    return OK;
}


Status CreateTableController(const char* name, const char* columnMeasurementName, const char* rowMeasurementName, byte columns, byte rows, TableController** tableController)
{
    if (tableControllerCount < MAX_TABLE_CONTROLLERS)
    {
        Status status;
        *tableController = &(createdControllers[tableControllerCount]);
        tableControllerCount++;
        status = InitTableController(*tableController, name, columns, rows);
        if (status == OK)
        {
            if (columnMeasurementName != NULL)
            {
                status = FindMeasurement(columnMeasurementName, &((*tableController)->columnMeasurement));
            }
            else
            {
                (*tableController)->columnMeasurement = NULL;
            }
            if (status == OK)
            {
                if (rowMeasurementName != NULL)
                {
                    status = FindMeasurement(rowMeasurementName, &((*tableController)->rowMeasurement));
                }
                else
                {
                    (*tableController)->rowMeasurement = NULL;
                }
            }
        }
        (*tableController)->columnIndex = 0;
        (*tableController)->rowIndex = 0;
        return status;
    }
    else
    {
        return "OutOfTableControllers";
    }
}


TableController* FindTableController(const char* name)
{
    int i;
    for (i = 0; i < tableControllerCount; ++i)
    {
        if (strcmp(name, createdControllers[i].name) == 0)
        {
            return &(createdControllers[i]);
        }
    }
    return NULL;
}


Status GetActualTableControllerFieldValue(TableController* tableController, float* fieldValue)
{
    TableField field;
    Status status = GetActualTableControllerField(tableController, &field);
    if (status == OK)
    {
        *fieldValue = field * tableController->factor;
    }
    return status;
}


Status GetTableControllerFieldValue(const TableController* tableController, byte column, byte row, float* fieldValue)
{
    TableField field;
    Status status = GetTableField(tableController->name, column, row, &field);
    if (status == OK)
    {
        *fieldValue = field * tableController->factor;
    }
    return status;
}


Status SetTableControllerFieldValue(const char*  name, int column, int row, float value)
{
    TableController* tableController = FindTableController(name);
    if (tableController != NULL)
    {
        int field = (int) (value / tableController->factor + 0.5f);
        return SetTableField(name, column, row, field);
    }
    else
    {
        return "NoSuchTableController";
    }
}
