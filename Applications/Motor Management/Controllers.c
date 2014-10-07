/*
** Copyright 2014, Bart Kampers
*/

#include "Controllers.h"

/*
** Private
*/

Status GetIndex(Measurement* measurement, int bound, byte* index)
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

Status GetTableControllerValue(TableController* tableControl, TableField* value)
{
    Status status = GetIndex(tableControl->columnMeasurement, tableControl->table.columns, &(tableControl->columnIndex));
    if (status == OK)
    {
        status = GetIndex(tableControl->rowMeasurement, tableControl->table.rows, &(tableControl->rowIndex));
        if (status == OK)
        {
            status = GetTableField(tableControl->name, tableControl->columnIndex, tableControl->rowIndex, value);
        }
    }
    return status;
}
