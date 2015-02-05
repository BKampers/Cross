/*
** Copyright 2015, Bart Kampers
*/

#include "MeasurementTable.h"

#include <stdlib.h>
#include <string.h>
#include <math.h>


/*
** Private
*/


#define ENABLED_MASK 0x01


MeasurementTable measurementTables[MAX_MEASUREMENT_TABLES];
int measurementTableCount = 0;


Status InitMeasurementTable(MeasurementTable* measurementTable, const char* name, byte columns, byte rows)
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
        status = InitMeasurementTable(*measurementTable, name, columns, rows);
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


Status FindMeasurementTable(const char* name, MeasurementTable** table)
{
    int i;
    for (i = 0; i < measurementTableCount; ++i)
    {
        if (strcmp(name, measurementTables[i].name) == 0)
        {
            *table = &(measurementTables[i]);
            return OK;
        }
    }
    return "NoSuchMeasurementTable";
}


Status GetActualMeasurementTableField(MeasurementTable* measurementTable, float* fieldValue)
{
    TableField field;
    Status status = GetActualTableControllerField(measurementTable, &field);
    if (status == OK)
    {
        *fieldValue = field * measurementTable->precision;
    }
    return status;
}


Status GetMeasurementTableField(const MeasurementTable* measurementTable, byte column, byte row, float* fieldValue)
{
    TableField field;
    Status status = GetTableField(measurementTable->name, column, row, &field);
    if (status == OK)
    {
        *fieldValue = field * measurementTable->precision;
    }
    return status;
}


Status SetMeasurementTableField(const char* name, int column, int row, float value)
{
    MeasurementTable* measurementTable;
    Status status = FindMeasurementTable(name, &measurementTable);
    if (status == OK)
    {
        int field = roundf(value / measurementTable->precision);
        status = SetTableField(name, column, row, field);
    }
    return status;
}


Status GetMeasurementTableEnabled(const char* name, bool* enabled)
{
    return GetTableFlags(name, ENABLED_MASK, enabled);
}


Status SetMeasurementTableEnabled(const char* name, bool enabled)
{
    if (enabled)
    {
        return SetTableFlags(name, ENABLED_MASK);
    }
    else
    {
        return ClearTableFlags(name, ENABLED_MASK);
    }
}
