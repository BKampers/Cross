/*
** Copyright 2014, Bart Kampers
*/

#include "Controllers.h"

#include <string.h>

/*
** Private
*/


#define UNINITIALIZED_TABLE { NULL_REFERENCE, 0, 0 }

TableController tableControllers[] =
{/*   name,      table,               colMt, rowMt, factor, minimum, maximum, decimals, Indexes */
    { IGNITION,  UNINITIALIZED_TABLE, NULL,  NULL,    1.0f,    0.0f,   59.0f,        0,    0, 0 },
    { INJECTION, UNINITIALIZED_TABLE, NULL,  NULL,    0.1f,    0.0f,   22.0f,        1,    0, 0 }
};

#define TABLE_CONTROLLER_COUNT (sizeof(tableControllers) / sizeof(TableController))

//TableController tableControllers[TABLE_CONTROLLER_COUNT];


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
    TableController* ignitionController = &(tableControllers[0]);
    TableController* injectionController = &(tableControllers[1]);
    Measurement* loadMeasurement = NULL;
    Measurement* rpmMeasurement = NULL;
    Status status = FindMeasurement(LOAD, &loadMeasurement);
    if (status == OK)
    {
        status = FindMeasurement(RPM, &rpmMeasurement);
        if (status == OK)
        {
            status = InitTableController(ignitionController, IGNITION, 20, 20);
            ignitionController->columnMeasurement = loadMeasurement;
            ignitionController->rowMeasurement = rpmMeasurement;
            if (status == OK)
            {
                status = InitTableController(injectionController, INJECTION, 20, 20);
                injectionController->columnMeasurement = loadMeasurement;
                injectionController->rowMeasurement = rpmMeasurement;
            }
        }
    }
    /*
    tableControllers[0].factor = 1.0f;
    tableControllers[1].factor = 0.1f;
    tableControllers[0].minimum = 0.0f;
    tableControllers[1].minimum = 0.0f;
    tableControllers[0].maximum = 59.0f;
    tableControllers[1].maximum = 22.0f;
    tableControllers[0].decimals = 0;
    tableControllers[1].decimals = 1;
    */
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
