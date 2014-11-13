#ifndef _CONTROLLERS_H_
#define _CONTROLLERS_H_

#include "ApiStatus.h"

#include "Table.h"
#include "Measurements.h"


extern char INJECTION[];
extern char IGNITION[];

extern char WATER_TEMPERATURE_CORRECTION[];


typedef struct
{
    const char* name;
    Table table;
    Measurement* columnMeasurement;
    Measurement* rowMeasurement;
    float factor;
    float minimum;
    float maximum;
    byte decimals;
    byte columnIndex;
    byte rowIndex;
} TableController;


Status InitControllers();

TableController* FindTableController(const char* name);

Status GetActualTableControllerFieldValue(TableController* tableController, float* fieldValue);
Status GetTableControllerFieldValue(const TableController* tableController, byte column, byte row, float* fieldValue);

Status SetTableControllerFieldValue(const char*  name, int column, int row, float value);

#endif /* _CONTROLLERS_H_ */
