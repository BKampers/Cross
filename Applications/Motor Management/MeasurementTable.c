/*
** Copyright 2014, Bart Kampers
*/

#include "MeasurementTable.h"

#include <stdlib.h>
#include <string.h>
#include <math.h>


/*
** Private
*/

#define UNINITIALIZED_TABLE { NULL_REFERENCE, 0, 0 }


MeasurementTable measurementTables[MAX_MEASUREMENT_TABLES];
int measurementTableCount = 0;


Status InitTableController(MeasurementTable* measurementTable, const char* name, byte columns, byte rows)
{
    Status status = FindTable(name, &(measurementTable->table));
    if (status != OK)
    {
        status = CreateTable(name, columns, rows, &(measurementTable->table));
    }
    measurementTable->name = name;
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
                *index = (byte) max(0, min(valueIndex, bound - 1));
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


Status GetActualTableControllerField(MeasurementTable* measurementTable, TableField* field)
{
    Status status = CalculateIndex(measurementTable->columnMeasurement, measurementTable->table.columns, &(measurementTable->columnIndex));
    if (status == OK)
    {
        status = CalculateIndex(measurementTable->rowMeasurement, measurementTable->table.rows, &(measurementTable->rowIndex));
        if (status == OK)
        {
            status = GetTableField(measurementTable->name, measurementTable->columnIndex, measurementTable->rowIndex, field);
        }
    }
    return status;
}


/*
** Interface
*/

Status CreateMeasurementTable(const char* name, const char* columnMeasurementName, const char* rowMeasurementName, byte columns, byte rows, MeasurementTable** measurementTable)
{
    if (measurementTableCount < MAX_MEASUREMENT_TABLES)
    {
        Status status;
        *measurementTable = &(measurementTables[measurementTableCount]);
        measurementTableCount++;
        status = InitTableController(*measurementTable, name, columns, rows);
        if (status == OK)
        {
            if (columnMeasurementName != NULL)
            {
                status = FindMeasurement(columnMeasurementName, &((*measurementTable)->columnMeasurement));
            }
            else
            {
                (*measurementTable)->columnMeasurement = NULL;
            }
            if (status == OK)
            {
                if (rowMeasurementName != NULL)
                {
                    status = FindMeasurement(rowMeasurementName, &((*measurementTable)->rowMeasurement));
                }
                else
                {
                    (*measurementTable)->rowMeasurement = NULL;
                }
            }
        }
        (*measurementTable)->precision = 1.0f;
        (*measurementTable)->minimum = 1.0f;
        (*measurementTable)->maximum = 100.0f;
        (*measurementTable)->decimals = 0;
        (*measurementTable)->columnIndex = 0;
        (*measurementTable)->rowIndex = 0;
        return status;
    }
    else
    {
        return "OutOfTableControllers";
    }
}


int GetMeasurementTableCount()
{
    return measurementTableCount;
}


const char* GetMeasurementTableName(int index)
{
    return measurementTables[index].name;
}


MeasurementTable* FindMeasurementTable(const char* name)
{
    int i;
    for (i = 0; i < measurementTableCount; ++i)
    {
        if (strcmp(name, measurementTables[i].name) == 0)
        {
            return &(measurementTables[i]);
        }
    }
    return NULL;
}


Status GetActualTableControllerFieldValue(MeasurementTable* measurementTable, float* fieldValue)
{
    TableField field;
    Status status = GetActualTableControllerField(measurementTable, &field);
    if (status == OK)
    {
        *fieldValue = field * measurementTable->precision;
    }
    return status;
}


Status GetTableControllerFieldValue(const MeasurementTable* measurementTable, byte column, byte row, float* fieldValue)
{
    TableField field;
    Status status = GetTableField(measurementTable->name, column, row, &field);
    if (status == OK)
    {
        *fieldValue = field * measurementTable->precision;
    }
    return status;
}


Status SetTableControllerFieldValue(const char*  name, int column, int row, float value)
{
    MeasurementTable* measurementTable = FindMeasurementTable(name);
    if (measurementTable != NULL)
    {
        int field = roundf(value / measurementTable->precision);
        return SetTableField(name, column, row, field);
    }
    else
    {
        return "NoSuchTableController";
    }
}
