#ifndef _TABLE_H_
#define _TABLE_H_


#include "PersistentElementManager.h"


typedef struct
{
    Reference nameReference;
    byte columns;
    byte rows;
    byte flags;
} Table;


typedef int16_t TableField;


void RegisterTableTypes();

Status FindTable(const char* name, Table* table);

Status CreateTable(const char* name, byte columns, byte rows, Table* table);
Status RemoveTable(const char* name);

Status SetTableField(const char* name, int column, int row, int value);
Status GetTableField(const char* name, int column, int row, TableField* value);

Status SetTableFlags(const char* name, byte mask);
Status ClearTableFlags(const char* name, byte mask);
Status GetTableFlags(const char* name, byte mask, bool* set);

#endif /* _TABLE_H_ */
