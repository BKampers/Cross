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


typedef struct
{
    const char* measurementName;
    MeasurementTable* table;
} CorrectionConfiguration;


Status CreateMeasurementTable(const char* name, const char* columnMeasurementName, const char* rowMeasurementName, byte columns, byte rows, MeasurementTable** measurementTable);
Status CreateCorrectionTable(const char* measurementName, MeasurementTable** correctionTable);

int GetMeasurementTableCount();
Status GetMeasurementTable(int index, MeasurementTable** table);
Status FindMeasurementTable(const char* name, MeasurementTable** table);

Status GetActualMeasurementTableField(MeasurementTable* measurementTable, float* fieldValue);
Status GetMeasurementTableField(const MeasurementTable* measurementTable, byte column, byte row, float* fieldValue);
Status SetMeasurementTableField(const char* name, int column, int row, float value);

Status GetMeasurementTableEnabled(const char* name, bool* enabled);
Status SetMeasurementTableEnabled(const char* name, bool enabled);

#endif /* _MEASUREMENTTABLE_H_ */
