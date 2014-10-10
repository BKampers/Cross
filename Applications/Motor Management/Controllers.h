#ifndef _CONTROLLERS_H_
#define _CONTROLLERS_H_

#include "ApiStatus.h"

#include "Table.h"
#include "Measurements.h"


extern char INJECTION[];
extern char IGNITION[];



typedef struct
{
    const char* name;
    Table table;
    Measurement* columnMeasurement;
    Measurement* rowMeasurement;
    byte columnIndex;
    byte rowIndex;
} TableController;


Status InitControllers();

TableController* FindTableController(const char* name);

Status GetTableControllerValue(TableController* tableController, TableField* value);

#endif /* _CONTROLLERS_H_ */
