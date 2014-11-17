#ifndef _MEASUREMENTTABLE_H_
#define _MEASUREMENTTABLE_H_

#include "ApiStatus.h"

#include "Table.h"
#include "Measurements.h"


#define MAX_MEASUREMENT_TABLES 16


typedef struct
{
    const char* name;
    Table table;
    Measurement* columnMeasurement;
    Measurement* rowMeasurement;
    float precision;
    float minimum;
    float maximum;
    byte decimals;
    byte columnIndex;
    byte rowIndex;
} MeasurementTable;


Status CreateMeasurementTable(const char* name, const char* columnMeasurementName, const char* rowMeasurementName, byte columns, byte rows, MeasurementTable** measurementTable);

MeasurementTable* FindMeasurementTable(const char* name);

Status GetActualTableControllerFieldValue(MeasurementTable* measurementTable, float* fieldValue);
Status GetTableControllerFieldValue(const MeasurementTable* measurementTable, byte column, byte row, float* fieldValue);

Status SetTableControllerFieldValue(const char*  name, int column, int row, float value);

#endif /* _MEASUREMENTTABLE_H_ */
