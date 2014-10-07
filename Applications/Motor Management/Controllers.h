#ifndef _CONTROL_H_
#define _CONTROL_H_

#include "ApiStatus.h"

#include "Table.h"
#include "Measurements.h"


extern char INJECTION[];


typedef struct
{
    const char* name;
    Table table;
    Measurement* columnMeasurement;
    Measurement* rowMeasurement;
    byte columnIndex;
    byte rowIndex;
} TableController;


Status GetTableControllerValue(TableController* tableController, TableField* value);

#endif /* _CONTROL_H_ */
