#ifndef _TABLE_H_
#define _TABLE_H_


#include "PersistentElementManager.h"


typedef struct
{
    Reference nameReference;
    byte columns;
    byte rows;
} Table;


typedef int16_t TableField;


void RegisterTableTypes();

Status FindTable(const char* name, Table* table);

Status CreateTable(char* name, byte columns, byte rows, Table* table);
Status RemoveTable(const char* name);

Status SetTableField(const char* name, int column, int row, TableField value);
Status GetTableField(const char* name, int column, int row, TableField* value);

#endif /* _TABLE_H_ */
