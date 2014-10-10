/*
** Copyright 2014, Bart Kampers
*/

#include "Controllers.h"

#include <string.h>

/*
** Private
*/


#define TABLE_CONTROLLER_COUNT 2

TableController tableControllers[TABLE_CONTROLLER_COUNT];


Status InitTableController(TableController* tableController, char* name, byte columns, byte rows)
{
    Status status = FindTable(name, &(tableController->table));
    if (status != OK)
    {
        status = CreateTable(name, columns, rows, &(tableController->table));
    }
    tableController->name = name;
    tableController->columnIndex = 0;
    tableController->rowIndex = 0;
    return status;
}


Status CalculateIndex(Measurement* measurement, int bound, byte* index)
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


/*
** Interface
*/

char IGNITION[] = "Ignition";
char INJECTION[] = "Injection";


Status InitControllers()
{
    Status status = InitTableController(&(tableControllers[0]), IGNITION, 20, 20);
    if (status == OK)
    {
        status = InitTableController(&(tableControllers[1]), INJECTION, 20, 20);
    }
    return status;
}


TableController* FindTableController(const char* name)
{
    int i;
    for (i = 0; i < TABLE_CONTROLLER_COUNT; ++i)
    {
        if (strcmp(name, tableControllers[i].name) == 0)
        {
            return &(tableControllers[i]);
        }
    }
    return NULL;
}


Status GetTableControllerValue(TableController* tableControl, TableField* value)
{
    Status status = CalculateIndex(tableControl->columnMeasurement, tableControl->table.columns, &(tableControl->columnIndex));
    if (status == OK)
    {
        status = CalculateIndex(tableControl->rowMeasurement, tableControl->table.rows, &(tableControl->rowIndex));
        if (status == OK)
        {
            status = GetTableField(tableControl->name, tableControl->columnIndex, tableControl->rowIndex, value);
        }
    }
    return status;
}
