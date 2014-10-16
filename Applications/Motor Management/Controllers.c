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

char IGNITION[] = "Ignition";
char INJECTION[] = "Injection";


Status InitControllers()
{
    Measurement* loadMeasurement = NULL;
    Measurement* rpmMeasurement = NULL;
    Status status = FindMeasurement(LOAD, &loadMeasurement);
    if (status == OK)
    {
        status = FindMeasurement(RPM, &rpmMeasurement);
        if (status == OK)
        {
            status = InitTableController(&(tableControllers[0]), IGNITION, 20, 20);
            if (status == OK)
            {
                status = InitTableController(&(tableControllers[1]), INJECTION, 20, 20);
            }
        }
    }
    tableControllers[0].columnMeasurement = loadMeasurement;
    tableControllers[1].columnMeasurement = loadMeasurement;
    tableControllers[0].rowMeasurement = rpmMeasurement;
    tableControllers[1].rowMeasurement = rpmMeasurement;
    tableControllers[0].factor = 1.0f;
    tableControllers[1].factor = 0.1f;
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
